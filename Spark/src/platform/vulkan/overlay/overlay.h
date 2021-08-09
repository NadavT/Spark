#ifndef SPARK_VULKAN_OVERLAY_H
#define SPARK_VULKAN_OVERLAY_H

#include "platform/Windows/window.h"
#include "platform/vulkan/framebuffer/framebuffer.h"
#include "platform/vulkan/renderer.h"
#include "spark/event/keyboard_event.h"
#include "spark/event/mouse_event.h"
#include "spark/overlay/overlay.h"

#include <vector>

namespace Spark::Render
{
class VulkanOverlay : public Overlay
{
  public:
    VulkanOverlay(VulkanRenderer &renderer);
    virtual ~VulkanOverlay();

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnUpdate(Time &diffTime);
    virtual void OnEvent(Event &e);
    virtual void OnRender();
    virtual void prepareOverlay();

  private:
    void frameRender();

    bool onMouseButtonPressed(MouseButtonPressedEvent &e);
    bool onMouseButtonReleased(MouseButtonReleasedEvent &e);
    bool onMouseMoved(MouseMovedEvent &e);
    bool onMouseScroll(MouseScrolledEvent &e);
    bool onKeyPressed(KeyPressedEvent &e);
    bool onKeyReleased(KeyReleasedEvent &e);
    bool onKeyTyped(KeyTypedEvent &e);

  private:
    VulkanRenderer &m_renderer;
    VulkanFramebuffer *m_framebuffer;

    std::vector<VkCommandBuffer> m_commandBuffers;
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_OVERLAY_H */
