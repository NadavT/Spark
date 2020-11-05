#pragma once

#include "platform/vulkan/pipeline/pipeline3d_lights.h"
#include "platform/vulkan/renderer.h"
#include "spark/renderer/camera.h"
#include "spark/renderer/layer_renderer.h"


#include <unordered_map>

namespace Spark
{
class VulkanLayerRenderer3DLights : public LayerRenderer
{
  public:
    VulkanLayerRenderer3DLights(VulkanRenderer &renderer, Camera &camera);
    virtual ~VulkanLayerRenderer3DLights();

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
    VulkanPipeline3DLights *m_pipeline;

    std::vector<std::vector<VkBuffer>> m_uniformTransformations;
    std::vector<std::vector<VkDeviceMemory>> m_uniformTransformationsMemory;
    std::vector<VkBuffer> m_uniformDirectionalLightBuffers;
    std::vector<VkDeviceMemory> m_uniformDirectionalLightBuffersMemory;
    std::vector<VkBuffer> m_uniformPointLightBuffers;
    std::vector<VkDeviceMemory> m_uniformPointLightBuffersMemory;
    std::vector<VkBuffer> m_uniformSpotLightBuffers;
    std::vector<VkDeviceMemory> m_uniformSpotLightBuffersMemory;
    std::vector<std::vector<VkBuffer>> m_uniformMaterialBuffers;
    std::vector<std::vector<VkDeviceMemory>> m_uniformMaterialBuffersMemory;
    std::vector<std::vector<VkDescriptorSet>> m_transformationDescriptorSets;
    std::vector<std::vector<VkDescriptorSet>> m_lightsDescriptorSets;
    std::vector<std::vector<VkDescriptorSet>> m_textureDescriptorSets;
    std::unordered_map<std::string, unsigned int> m_textureDescriptorOffset;

    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<Drawable *> m_toBeRemoved;

    bool m_isAttached;
    bool m_isRecreationNeeded;

    Camera &m_camera;
};
} // namespace Spark
