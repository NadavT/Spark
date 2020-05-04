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
        SPARK_CORE_ASSERT(err == VK_SUCCESS, "Vulkan failed in imgui");
    }

    VulkanOverlay::VulkanOverlay(VulkanRenderer& renderer)
        : Overlay()
        , m_renderer(renderer)
        , m_framebuffer(nullptr)
        , m_showDemoWindow(true)
        , m_showAnotherWindow(false)
    {
        m_framebuffer = renderer.createFramebuffer(VulkanFramebufferType::Type2D, true);
        m_commandBuffers.resize(renderer.getImagesAmount());
        for (int i = 0; i < m_commandBuffers.size(); i++)
        {
            m_commandBuffers[i] = renderer.m_context.createCommandBuffer();
        }
    }

    VulkanOverlay::~VulkanOverlay()
    {
        for (int i = 0; i < m_commandBuffers.size(); i++)
        {
            m_renderer.m_context.destroyCommandBuffer(m_commandBuffers[i]);
        }
        m_renderer.destroyFramebuffer(m_framebuffer);
        m_framebuffer = nullptr;
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
        init_info.MinImageCount = m_renderer.getImagesAmount();
        init_info.ImageCount = m_renderer.getImagesAmount();
        init_info.MSAASamples = m_renderer.m_context.m_msaaSamples;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, m_framebuffer->getRenderPass());

        VkResult err = VK_SUCCESS;

        // Upload Fonts
        {
            VkCommandBuffer command_buffer = m_commandBuffers[0];

            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            m_renderer.beginCommandBuffer(command_buffer, &begin_info);

            ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

            VkSubmitInfo end_info = {};
            end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers = &command_buffer;
            m_renderer.endCommandBuffer(command_buffer);
            m_renderer.queueSubmit(&end_info, VK_NULL_HANDLE);

            m_renderer.waitForIdle();
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

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (m_showDemoWindow)
        //    ImGui::ShowDemoWindow(&m_showDemoWindow);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(350, 170), ImGuiCond_Always);
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
    }

    void VulkanOverlay::frameRender()
    {    
        VkCommandBuffer commandBuffer = m_commandBuffers[m_renderer.getCurrentImageIndex()];
        {
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            m_renderer.beginCommandBuffer(commandBuffer, &info);
            std::array<VkClearValue, 2> clearValues = {};
            clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };
            m_renderer.beginRenderPass(commandBuffer, m_framebuffer->getRenderPass(),
                m_framebuffer->getswapChainFramebuffers()[m_renderer.getCurrentImageIndex()], 2, clearValues.data());
        }

        // Record Imgui Draw Data and draw funcs into command buffer
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

        // Submit command buffer
        vkCmdEndRenderPass(commandBuffer);
        m_renderer.endCommandBuffer(commandBuffer);
        
        m_renderer.render(commandBuffer);
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
