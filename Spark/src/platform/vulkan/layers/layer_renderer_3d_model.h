#pragma once

#include "platform/vulkan/drawables/model.h"
#include "platform/vulkan/drawables/point_light.h"
#include "platform/vulkan/drawables/textured_drawable.h"
#include "platform/vulkan/framebuffer/framebuffer3d.h"
#include "platform/vulkan/pipeline/pipeline3d_model.h"
#include "platform/vulkan/pipeline/pipeline3d_outline.h"
#include "platform/vulkan/pipeline/pipeline3d_wireframe.h"
#include "platform/vulkan/renderer.h"
#include "spark/renderer/camera.h"
#include "spark/renderer/layer_renderer.h"

#include <unordered_map>

namespace Spark::Render
{
class VulkanLayerRenderer3DModel : public LayerRenderer
{
  public:
    VulkanLayerRenderer3DModel(VulkanRenderer &renderer, Camera &camera);
    virtual ~VulkanLayerRenderer3DModel();

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnRender();

    virtual void addDrawable(std::shared_ptr<Drawable> &drawable);
    virtual void removeDrawable(Drawable *drawable);

    virtual void setDirLight(glm::vec3 direction, glm::vec3 color);
    virtual void addPointLight(VulkanPointLight &pointLight);
    virtual void removePointLight(VulkanPointLight &pointLight);
    virtual void setSpotLight(glm::vec3 color);

    virtual void setWireframe(WireframeState state, glm::vec3 color);

  private:
    void createCommandBuffers();
    void createResourcesForDrawables(std::vector<std::shared_ptr<Drawable>> &drawables);
    unsigned int getResourcesForTexutredDrawable(VulkanTexturedDrawable &drawable,
                                                 std::vector<std::vector<VkImageView>> &textures,
                                                 std::vector<std::vector<VkSampler>> &samplers,
                                                 std::vector<std::vector<VkImageView>> &specularTextures,
                                                 std::vector<std::vector<VkSampler>> &specularSamplers);
    unsigned int getResourcesForModelDrawable(VulkanDrawableModel &drawable,
                                              std::vector<std::vector<VkImageView>> &textures,
                                              std::vector<std::vector<VkSampler>> &samplers,
                                              std::vector<std::vector<VkImageView>> &specularTextures,
                                              std::vector<std::vector<VkSampler>> &specularSamplers);

  private:
    VulkanRenderer &m_renderer;
    VulkanFramebuffer3D *m_framebuffer;
    VulkanPipeline3DModel *m_pipeline;
    VulkanPipeline3DOutline *m_outlinePipeline;
    VulkanPipeline3DOutline *m_cleanOutlinePipeline;
    VulkanPipeline3DWireframe *m_wireframePipeline;

    std::vector<std::vector<VkBuffer>> m_uniformTransformations;
    std::vector<std::vector<VkDeviceMemory>> m_uniformTransformationsMemory;
    std::vector<VkBuffer> m_uniformDirectionalLightBuffers;
    std::vector<VkDeviceMemory> m_uniformDirectionalLightBuffersMemory;
    std::vector<VkBuffer> m_uniformPointLightBuffers;
    std::vector<VkDeviceMemory> m_uniformPointLightBuffersMemory;
    std::vector<VkBuffer> m_uniformSpotLightBuffers;
    std::vector<VkDeviceMemory> m_uniformSpotLightBuffersMemory;
    std::vector<std::vector<std::vector<VkBuffer>>> m_uniformMaterialBuffers;
    std::vector<std::vector<std::vector<VkDeviceMemory>>> m_uniformMaterialBuffersMemory;
    std::vector<std::vector<VkDescriptorSet>> m_transformationDescriptorSets;
    std::vector<std::vector<VkDescriptorSet>> m_outlineTransformationDescriptorSets;
    std::vector<std::vector<VkDescriptorSet>> m_lightsDescriptorSets;
    std::vector<std::vector<std::vector<VkDescriptorSet>>> m_materialDescriptorSets;
    std::vector<std::vector<VkDescriptorSet>> m_textureDescriptorSets;
    std::unordered_map<std::string, unsigned int> m_textureDescriptorOffset;

    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<Drawable *> m_toBeRemoved;

    bool m_isAttached;
    bool m_isRecreationNeeded;

    Camera &m_camera;
    glm::vec3 m_dirLightDirection;
    glm::vec3 m_dirLightColor;
    glm::vec3 m_spotLightColor;
    std::vector<VulkanPointLight *> m_pointLights;

    WireframeState m_wireframe;
    glm::vec3 m_wireframeColor;
};
} // namespace Spark::Render
