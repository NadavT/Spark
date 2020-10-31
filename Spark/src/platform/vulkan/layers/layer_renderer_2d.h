#pragma once

#include "platform/vulkan/pipeline/pipeline2d.h"
#include "platform/vulkan/renderer.h"
#include "spark/renderer/layer_renderer.h"


#include <unordered_map>

namespace Spark
{
class VulkanLayerRenderer2D : public LayerRenderer
{
  public:
    explicit VulkanLayerRenderer2D(VulkanRenderer &renderer);
    virtual ~VulkanLayerRenderer2D();

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnRender();

    virtual void addDrawable(std::shared_ptr<Drawable> &drawable);
    virtual void removeDrawable(Drawable *drawable);

  private:
    void createCommandBuffers();

  private:
    VulkanRenderer &m_renderer;
    VulkanFramebuffer *m_framebuffer;
    VulkanPipeline2D *m_pipeline;

    std::vector<std::vector<VkBuffer>> m_uniformTransformations;
    std::vector<std::vector<VkDeviceMemory>> m_uniformTransformationsMemory;
    std::vector<std::vector<VkDescriptorSet>> m_transformationDescriptorSets;
    std::vector<std::vector<VkDescriptorSet>> m_textureDescriptorSets;
    std::unordered_map<std::string, unsigned int> m_textureDescriptorOffset;

    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<Drawable *> m_toBeRemoved;

    bool m_isAttached;
    bool m_isRecreationNeeded;
};
} // namespace Spark
