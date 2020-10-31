#pragma once

#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/vertex/vertex3d.h"

namespace Spark
{
struct Transformation3DLights
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct PushConsts
{
    alignas(4) int numberOfPointLights;
};

class VulkanPipeline3DLights : public VulkanPipeline
{
  public:
    VulkanPipeline3DLights(VulkanContext &context, VulkanFramebuffer &framebuffer);
    virtual ~VulkanPipeline3DLights();

    virtual void cleanup();
    virtual void recreate();

    virtual void bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet, VkDescriptorSet lightSet,
                      VkDescriptorSet textureSet, struct PushConsts pushConsts);

    VkDescriptorSetLayout getMVPDescriptorSetLayout();
    VkDescriptorSetLayout getTextureDescriptorSetLayout();

    void createTransformationDescriptorSets(unsigned int drawablesAmount,
                                            std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                            std::vector<std::vector<VkBuffer>> transformationUniforms);
    void createTextureDescriptorSets(unsigned int texturesAmount,
                                     std::vector<std::vector<VkDescriptorSet>> &texturesSets,
                                     std::vector<VkImageView> &textureImageViews,
                                     std::vector<VkSampler> &textureSamplers);

    void createSingleTransformationDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                                 std::vector<VkBuffer> transformationUniforms);
    void createSingleTextureDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &textureSets,
                                          VkImageView textureImageView, VkSampler textureSampler);

  private:
    void createDescriptorSetLayout();
    void createGraphicsPipeline();

  private:
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorSetLayout m_transformationDescriptorSetLayout;
    VkDescriptorSetLayout m_lightsDescriptorSetLayout;
    VkDescriptorSetLayout m_textureDescriptorSetLayout;
};
} // namespace Spark
