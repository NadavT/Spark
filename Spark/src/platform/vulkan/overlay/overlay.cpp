#include "overlay.h"

#include "spark/core/log.h"
#include "spark/event/event.h"
#include "spark/core/input.h"

#include <imgui.h>
#include <examples/imgui_impl_vulkan.h>

namespace Spark
{
    static void check_vk_result(VkResult err)
    {
        if (err == 0) return;
        SPARK_CORE_ERROR("Imgui vkResult: {}", err);
        if (err < 0)
            abort();
    }

    VulkanOverlay::VulkanOverlay(VulkanRenderer& renderer)
        : Overlay()
        , m_renderer(renderer)
        , m_framebuffer(renderer.m_context)
        , m_currentFrame(0)
        , m_showDemoWindow(true)
        , m_showAnotherWindow(false)
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
    }

    VulkanOverlay::~VulkanOverlay()
    {
        for (size_t i = 0; i < m_renderer.m_context.m_swapChainImages.size(); i++) {
            m_renderer.m_context.destroySemaphore(m_imageAvailableSemaphores[i]);
            m_renderer.m_context.destroySemaphore(m_renderFinishedSemaphores[i]);
            m_renderer.m_context.destroyFence(m_inFlightFences[i]);
        }
        m_renderer.m_context.destroyCommandBuffer(m_commandBuffer);
    }

    void VulkanOverlay::OnAttach()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.KeyMap[ImGuiKey_Tab] = static_cast<int>(KeyCode::Tab);
        io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int>(KeyCode::Left);
        io.KeyMap[ImGuiKey_RightArrow] = static_cast<int>(KeyCode::Right);
        io.KeyMap[ImGuiKey_UpArrow] = static_cast<int>(KeyCode::Up);
        io.KeyMap[ImGuiKey_DownArrow] = static_cast<int>(KeyCode::Down);
        io.KeyMap[ImGuiKey_PageUp] = static_cast<int>(KeyCode::PageUp);
        io.KeyMap[ImGuiKey_PageDown] = static_cast<int>(KeyCode::PageDown);
        io.KeyMap[ImGuiKey_Home] = static_cast<int>(KeyCode::Home);
        io.KeyMap[ImGuiKey_End] = static_cast<int>(KeyCode::End);
        io.KeyMap[ImGuiKey_Insert] = static_cast<int>(KeyCode::Insert);
        io.KeyMap[ImGuiKey_Delete] = static_cast<int>(KeyCode::Delete);
        io.KeyMap[ImGuiKey_Backspace] = static_cast<int>(KeyCode::Backspace);
        io.KeyMap[ImGuiKey_Space] = static_cast<int>(KeyCode::Space);
        io.KeyMap[ImGuiKey_Enter] = static_cast<int>(KeyCode::Enter);
        io.KeyMap[ImGuiKey_Escape] = static_cast<int>(KeyCode::Escape);
        io.KeyMap[ImGuiKey_KeyPadEnter] = static_cast<int>(KeyCode::KPEnter);
        io.KeyMap[ImGuiKey_A] = static_cast<int>(KeyCode::A);
        io.KeyMap[ImGuiKey_C] = static_cast<int>(KeyCode::C);
        io.KeyMap[ImGuiKey_V] = static_cast<int>(KeyCode::V);
        io.KeyMap[ImGuiKey_X] = static_cast<int>(KeyCode::X);
        io.KeyMap[ImGuiKey_Y] = static_cast<int>(KeyCode::Y);
        io.KeyMap[ImGuiKey_Z] = static_cast<int>(KeyCode::Z);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_renderer.m_context.m_instance;
        init_info.PhysicalDevice = m_renderer.m_context.m_physicalDevice;
        init_info.Device = m_renderer.m_context.m_device;
        init_info.QueueFamily = m_renderer.m_context.m_queueFamilyIndices.graphicsFamily.value();
        init_info.Queue = m_renderer.m_context.m_graphicsQueue;
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = m_renderer.m_context.m_descriptorPool;
        init_info.Allocator = VK_NULL_HANDLE;
        init_info.MinImageCount = static_cast<uint32_t>(m_renderer.m_context.m_swapChainImages.size());
        init_info.ImageCount = static_cast<uint32_t>(m_renderer.m_context.m_swapChainImages.size());
        init_info.MSAASamples = m_renderer.m_context.m_msaaSamples;
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
            err = vkQueueSubmit(m_renderer.m_context.m_graphicsQueue, 1, &end_info, VK_NULL_HANDLE);
            check_vk_result(err);

            err = vkDeviceWaitIdle(m_renderer.m_context.m_device);
            check_vk_result(err);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
    }

    void VulkanOverlay::OnDetach()
    {
        ImGui_ImplVulkan_Shutdown();
    }

    void VulkanOverlay::OnUpdate(Time& diffTime)
    {
        ImGuiIO& io = ImGui::GetIO();

        io.DeltaTime = diffTime.GetSeconds();
        io.DisplaySize = ImVec2((float)m_renderer.m_context.m_swapChainExtent.width, (float)m_renderer.m_context.m_swapChainExtent.height);
        io.DisplayFramebufferScale = ImVec2(1, 1);
        io.MousePos = ImVec2(Input::GetMouseX(), Input::GetMouseY());
    }

    void VulkanOverlay::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseButtonPressedEvent>(SPARK_BIND_EVENT_FN(VulkanOverlay::onMouseButtonPressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(SPARK_BIND_EVENT_FN(VulkanOverlay::onMouseButtonReleased));
        dispatcher.Dispatch<MouseMovedEvent>(SPARK_BIND_EVENT_FN(VulkanOverlay::onMouseMoved));
        dispatcher.Dispatch<MouseScrolledEvent>(SPARK_BIND_EVENT_FN(VulkanOverlay::onMouseScroll));
        dispatcher.Dispatch<KeyPressedEvent>(SPARK_BIND_EVENT_FN(VulkanOverlay::onKeyPressed));
        dispatcher.Dispatch<KeyReleasedEvent>(SPARK_BIND_EVENT_FN(VulkanOverlay::onKeyReleased));
        dispatcher.Dispatch<KeyTypedEvent>(SPARK_BIND_EVENT_FN(VulkanOverlay::onKeyTyped));
    }

    void VulkanOverlay::OnRender()
    {
        VkResult err = VK_SUCCESS;
        ImGuiIO& io = ImGui::GetIO();

        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui::NewFrame();

        //// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (m_showDemoWindow)
        //    ImGui::ShowDemoWindow(&m_showDemoWindow);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::Begin("Hello, world!", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Text("Current mouse position: %f, %f", io.MousePos.x, io.MousePos.y);               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &m_showDemoWindow);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &m_showAnotherWindow);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (m_showAnotherWindow)
        {
            ImGui::Begin("Another Window", &m_showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                m_showAnotherWindow = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        frameRender();

        framePresent();

        check_vk_result(err);
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
            info.clearValueCount = 0;
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

    bool VulkanOverlay::onMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        int mouseButtonCode = static_cast<int>(e.GetMouseButton());

        if (mouseButtonCode >= 0 && mouseButtonCode < IM_ARRAYSIZE(io.MouseDown))
        {
            io.MouseDown[mouseButtonCode] = true;
            return io.WantCaptureMouse;
        }

        return false;
    }

    bool VulkanOverlay::onMouseButtonReleased(MouseButtonReleasedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        int mouseButtonCode = static_cast<int>(e.GetMouseButton());

        if (mouseButtonCode >= 0 && mouseButtonCode < IM_ARRAYSIZE(io.MouseDown))
        {
            io.MouseDown[mouseButtonCode] = false;
            return io.WantCaptureMouse;
        }

        return false;
    }

    bool VulkanOverlay::onMouseMoved(MouseMovedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        return io.WantCaptureMouse;
    }

    bool VulkanOverlay::onMouseScroll(MouseScrolledEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheelH += e.GetXOffset();
        io.MouseWheel += e.GetYOffset();

        return io.WantCaptureMouse;
    }

    bool VulkanOverlay::onKeyPressed(KeyPressedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        int key = static_cast<int>(e.GetKeyCode());
        io.KeysDown[key] = true;
        io.KeyCtrl = io.KeysDown[static_cast<int>(KeyCode::LeftControl)] || io.KeysDown[static_cast<int>(KeyCode::RightControl)];
        io.KeyShift = io.KeysDown[static_cast<int>(KeyCode::LeftShift)] || io.KeysDown[static_cast<int>(KeyCode::RightShift)];
        io.KeyAlt = io.KeysDown[static_cast<int>(KeyCode::LeftAlt)] || io.KeysDown[static_cast<int>(KeyCode::RightAlt)];

        return io.WantCaptureKeyboard;
    }

    bool VulkanOverlay::onKeyReleased(KeyReleasedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        int key = static_cast<int>(e.GetKeyCode());
        io.KeysDown[key] = false;
        io.KeyCtrl = io.KeysDown[static_cast<int>(KeyCode::LeftControl)] || io.KeysDown[static_cast<int>(KeyCode::RightControl)];
        io.KeyShift = io.KeysDown[static_cast<int>(KeyCode::LeftShift)] || io.KeysDown[static_cast<int>(KeyCode::RightShift)];
        io.KeyAlt = io.KeysDown[static_cast<int>(KeyCode::LeftAlt)] || io.KeysDown[static_cast<int>(KeyCode::RightAlt)];

        return io.WantCaptureKeyboard;
    }

    bool VulkanOverlay::onKeyTyped(KeyTypedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(static_cast<int>(e.GetKeyCode()));

        return io.WantCaptureKeyboard;
    }
}
