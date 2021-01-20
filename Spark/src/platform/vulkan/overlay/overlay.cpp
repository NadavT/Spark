#include "overlay.h"

#include "spark/core/input.h"
#include "spark/core/log.h"
#include "spark/event/event.h"

#include <examples/imgui_impl_vulkan.h>

namespace Spark::Render
{
static void check_vk_result(VkResult err)
{
    SPARK_CORE_ASSERT(err == VK_SUCCESS, "Vulkan failed in imgui");
}

VulkanOverlay::VulkanOverlay(VulkanRenderer &renderer)
    : Overlay()
    , m_renderer(renderer)
    , m_framebuffer(nullptr)
{
    m_framebuffer = renderer.createFramebuffer(VulkanFramebufferType::Type2D);
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
    ImGuiIO &io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
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
    // ImGui::StyleColorsClassic();

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
    init_info.MSAASamples = m_renderer.getVulkanMSAA();
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
        m_renderer.queueSubmits(1, &end_info, VK_NULL_HANDLE);

        m_renderer.waitForIdle();
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}

void VulkanOverlay::OnDetach()
{
    ImGui_ImplVulkan_Shutdown();
}

void VulkanOverlay::OnUpdate(Time &diffTime)
{
    ImGuiIO &io = ImGui::GetIO();

    io.DeltaTime = static_cast<float>(diffTime.GetSeconds());
    io.DisplaySize = ImVec2((float)m_renderer.m_context.m_swapChainExtent.width,
                            (float)m_renderer.m_context.m_swapChainExtent.height);
    io.DisplayFramebufferScale = ImVec2(1, 1);
    io.MousePos = ImVec2(Input::GetMouseX(), Input::GetMouseY());
}

void VulkanOverlay::OnEvent(Event &e)
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
    if (m_renderer.isRecreationNeeded())
    {
        OnDetach();
        OnAttach();
    }
    ImGui::Render();
    frameRender();
}

void VulkanOverlay::prepareOverlay()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();
}

void VulkanOverlay::frameRender()
{
    VkCommandBuffer commandBuffer = m_commandBuffers[m_renderer.getCurrentImageIndex()];
    {
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        m_renderer.beginCommandBuffer(commandBuffer, &info);
        m_renderer.beginRenderPass(commandBuffer, m_framebuffer->getRenderPass(),
                                   m_framebuffer->getswapChainFramebuffers()[m_renderer.getCurrentImageIndex()]);
    }

    // Record Imgui Draw Data and draw funcs into command buffer
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

    // Submit command buffer
    vkCmdEndRenderPass(commandBuffer);
    m_renderer.endCommandBuffer(commandBuffer);

    m_renderer.render(commandBuffer);
}

bool VulkanOverlay::onMouseButtonPressed(MouseButtonPressedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    int mouseButtonCode = static_cast<int>(e.GetMouseButton());

    if (mouseButtonCode >= 0 && mouseButtonCode < IM_ARRAYSIZE(io.MouseDown))
    {
        io.MouseDown[mouseButtonCode] = true;
        return io.WantCaptureMouse;
    }

    return false;
}

bool VulkanOverlay::onMouseButtonReleased(MouseButtonReleasedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    int mouseButtonCode = static_cast<int>(e.GetMouseButton());

    if (mouseButtonCode >= 0 && mouseButtonCode < IM_ARRAYSIZE(io.MouseDown))
    {
        io.MouseDown[mouseButtonCode] = false;
        return io.WantCaptureMouse;
    }

    return false;
}

bool VulkanOverlay::onMouseMoved(MouseMovedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool VulkanOverlay::onMouseScroll(MouseScrolledEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheelH += e.GetXOffset();
    io.MouseWheel += e.GetYOffset();

    return io.WantCaptureMouse;
}

bool VulkanOverlay::onKeyPressed(KeyPressedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    int key = static_cast<int>(e.GetKeyCode());
    io.KeysDown[key] = true;
    io.KeyCtrl =
        io.KeysDown[static_cast<int>(KeyCode::LeftControl)] || io.KeysDown[static_cast<int>(KeyCode::RightControl)];
    io.KeyShift =
        io.KeysDown[static_cast<int>(KeyCode::LeftShift)] || io.KeysDown[static_cast<int>(KeyCode::RightShift)];
    io.KeyAlt = io.KeysDown[static_cast<int>(KeyCode::LeftAlt)] || io.KeysDown[static_cast<int>(KeyCode::RightAlt)];

    if (io.WantCaptureKeyboard)
    {
        Input::BlockInput();
    }
    else
    {
        Input::UnBlockInput();
    }

    return io.WantCaptureKeyboard;
}

bool VulkanOverlay::onKeyReleased(KeyReleasedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    int key = static_cast<int>(e.GetKeyCode());
    io.KeysDown[key] = false;
    io.KeyCtrl =
        io.KeysDown[static_cast<int>(KeyCode::LeftControl)] || io.KeysDown[static_cast<int>(KeyCode::RightControl)];
    io.KeyShift =
        io.KeysDown[static_cast<int>(KeyCode::LeftShift)] || io.KeysDown[static_cast<int>(KeyCode::RightShift)];
    io.KeyAlt = io.KeysDown[static_cast<int>(KeyCode::LeftAlt)] || io.KeysDown[static_cast<int>(KeyCode::RightAlt)];

    return io.WantCaptureKeyboard;
}

bool VulkanOverlay::onKeyTyped(KeyTypedEvent &e)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(static_cast<int>(e.GetKeyCode()));

    return io.WantCaptureKeyboard;
}
} // namespace Spark::Render
