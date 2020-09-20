#pragma once

#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/vertex/vertex2d.h"

namespace Spark
{
struct Transformation2D
{
    glm::mat4 transformMatrix;
};

class VulkanPipeline2D : public VulkanPipeline
{
  public:
    VulkanPipeline2D(VulkanContext &context, VulkanFramebuffer &framebuffer);
    virtual ~VulkanPipeline2D();

    virtual void cleanup();
    virtual void recreate();

    virtual void bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet, VkDescriptorSet textureSet);

    VkDescriptorSetLayout getMVPDescriptorSetLayout();
    VkDescriptorSetLayout getTextureDescriptorSetLayout();

    void createTransformationDescriptorSets(unsigned int drawablesAmount,
                                            std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                            std::vector<std::vector<VkBuffer>> transformationUniforms);
    void createTextureDescriptorSets(unsigned int texturesAmount,
                                     std::vector<std::vector<VkDescriptorSet>> &texturesSets,
                                     std::vector<VkImageView> &textureImageView,
                                     std::vector<VkSampler> &textureSampler);

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
    VkDescriptorSetLayout m_textureDescriptorSetLayout;
};
} // namespace Spark
