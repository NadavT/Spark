#ifndef SPARK_VULKAN_LAYER2D_H
#define SPARK_VULKAN_LAYER2D_H

#include "platform/vulkan/drawables/quad.h"
#include "platform/vulkan/layers/layer_renderer_2d.h"
#include "platform/vulkan/pipeline/pipeline2d.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/resource/texture.h"
#include "platform/vulkan/resource/texture_sampler.h"
#include "spark/layer/layer.h"

#include "spark/event/keyboard_event.h"

namespace Spark::Render
{
class VulkanLayer2D : public Layer
{
  public:
    VulkanLayer2D(VulkanRenderer &renderer);
    virtual ~VulkanLayer2D();

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnUpdate(Time &diffTime);
    virtual void OnEvent(Event &e);
    virtual void OnRender();

  private:
    bool handleKeyPressed(KeyPressedEvent &e);

  private:
    VulkanRenderer &m_renderer;
    VulkanLayerRenderer2D m_layer_renderer;

    std::unique_ptr<VulkanQuad> m_quad;
    std::unique_ptr<VulkanQuad> m_quad2;
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_LAYER2D_H */
