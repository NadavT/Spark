#include "overlay.h"

#include "spark/core/log.h"

#include <imgui.h>
#include <examples/imgui_impl_vulkan.h>
#include <examples/imgui_impl_glfw.h>

namespace Spark
{
    static void check_vk_result(VkResult err)
    {
        if (err == 0) return;
        SPARK_CORE_ERROR("Imgui vkResult: {}", err);
        if (err < 0)
            abort();
    }

    void VulkanOverlay::frameRender()
    {
        VkResult err;

        err = vkAcquireNextImageKHR(m_renderer.m_context.m_device, m_renderer.m_context.m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_currentFrame);
        check_vk_result(err);

        {
            err = vkWaitForFences(m_renderer.m_context.m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
            check_vk_result(err);

            err = vkResetFences(m_renderer.m_context.m_device, 1, &m_inFlightFences[m_currentFrame]);
            check_vk_result(err);
        }
        {
            vkResetCommandBuffer(m_commandBuffer, 0);
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(m_commandBuffer, &info);
            check_vk_result(err);
        }
        {
            VkRenderPassBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass = m_framebuffer.getRenderPass();
            info.framebuffer = m_framebuffer.getswapChainFramebuffers()[m_currentFrame];
            info.renderArea.extent.width = m_renderer.m_context.m_swapChainExtent.width;
            info.renderArea.extent.height = m_renderer.m_context.m_swapChainExtent.height;

            info.clearValueCount = 1;
            info.pClearValues = &m_clearValue;
            vkCmdBeginRenderPass(m_commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        // Record Imgui Draw Data and draw funcs into command buffer
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_commandBuffer);

        // Submit command buffer
        vkCmdEndRenderPass(m_commandBuffer);
        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount = 1;
            info.pWaitSemaphores = &m_imageAvailableSemaphores[m_currentFrame];
            info.pWaitDstStageMask = &wait_stage;
            info.commandBufferCount = 1;
            info.pCommandBuffers = &m_commandBuffer;
            info.signalSemaphoreCount = 1;
            info.pSignalSemaphores = &m_renderFinishedSemaphores[m_currentFrame];

            err = vkEndCommandBuffer(m_commandBuffer);
            check_vk_result(err);
            err = vkQueueSubmit(m_renderer.m_context.m_graphicsQueue, 1, &info, m_inFlightFences[m_currentFrame]);
            check_vk_result(err);
        }

    }

    void VulkanOverlay::framePresent()
    {
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &m_renderFinishedSemaphores[m_currentFrame];
        info.swapchainCount = 1;
        info.pSwapchains = &m_renderer.m_context.m_swapChain;
        info.pImageIndices = &m_currentFrame;
        VkResult err = vkQueuePresentKHR(m_renderer.m_context.m_graphicsQueue, &info);
        check_vk_result(err);
        m_currentFrame = (m_currentFrame + 1) % m_renderer.m_context.m_swapChainImages.size(); // Now we can use the next set of semaphores
    }

	VulkanOverlay::VulkanOverlay(VulkanRenderer& renderer, const WindowsWindow& window)
		: Overlay()
        , m_renderer(renderer)
        , m_framebuffer(renderer.m_context)
        , m_currentFrame(0)
        , m_clearValue()
	{
        m_imageAvailableSemaphores.resize(renderer.m_context.m_swapChainImages.size());
        m_renderFinishedSemaphores.resize(renderer.m_context.m_swapChainImages.size());
        m_inFlightFences.resize(renderer.m_context.m_swapChainImages.size());
        m_commandBuffer = renderer.m_context.createCommandBuffer();
        

        for (size_t i = 0; i < renderer.m_context.m_swapChainImages.size(); i++) {
            m_imageAvailableSemaphores[i] = renderer.m_context.createSemaphore();
            m_renderFinishedSemaphores[i] = renderer.m_context.createSemaphore();
            m_inFlightFences[i] = renderer.m_context.createFence();
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForVulkan(window.GetNativeWindow(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = renderer.m_context.m_instance;
        init_info.PhysicalDevice = renderer.m_context.m_physicalDevice;
        init_info.Device = renderer.m_context.m_device;
        init_info.QueueFamily = renderer.m_context.m_queueFamilyIndices.graphicsFamily.value();
        init_info.Queue = renderer.m_context.m_graphicsQueue;
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = renderer.m_context.m_descriptorPool;
        init_info.Allocator = VK_NULL_HANDLE;
        init_info.MinImageCount = renderer.m_context.m_swapChainImages.size();
        init_info.ImageCount = renderer.m_context.m_swapChainImages.size();
        init_info.MSAASamples = renderer.m_context.m_msaaSamples;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, m_framebuffer.getRenderPass());

        VkResult err = VK_SUCCESS;

        // Upload Fonts
        {
            VkCommandBuffer command_buffer = m_commandBuffer;

            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(command_buffer, &begin_info);
            check_vk_result(err);

            ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

            VkSubmitInfo end_info = {};
            end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers = &command_buffer;
            err = vkEndCommandBuffer(command_buffer);
            check_vk_result(err);
            err = vkQueueSubmit(renderer.m_context.m_graphicsQueue, 1, &end_info, VK_NULL_HANDLE);
            check_vk_result(err);

            err = vkDeviceWaitIdle(renderer.m_context.m_device);
            check_vk_result(err);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }


        // Our state
        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Main loop
        while (!glfwWindowShouldClose(window.GetNativeWindow()))
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
            if (show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);

            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
            {
                static float f = 0.0f;
                static int counter = 0;

                ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
                ImGui::Begin("Hello, world!", NULL, ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoBackground);                          // Create a window called "Hello, world!" and append into it.

                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
                ImGui::Checkbox("Another Window", &show_another_window);

                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

                if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                    counter++;
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            // 3. Show another simple window.
            if (show_another_window)
            {
                ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    show_another_window = false;
                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            memcpy(&m_clearValue.color.float32[0], &clear_color, 4 * sizeof(float));
            frameRender();

            framePresent();

            err = vkDeviceWaitIdle(renderer.m_context.m_device);
            check_vk_result(err);
        }

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        for (size_t i = 0; i < renderer.m_context.m_swapChainImages.size(); i++) {
            renderer.m_context.destroySemaphore(m_imageAvailableSemaphores[i]);
            renderer.m_context.destroySemaphore(m_renderFinishedSemaphores[i]);
            renderer.m_context.destroyFence(m_inFlightFences[i]);
        }
        renderer.m_context.destroyCommandBuffer(m_commandBuffer);
    }
}
