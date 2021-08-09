#ifndef SPARK_VULKAN_PIPELINE3D_H
#define SPARK_VULKAN_PIPELINE3D_H

#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/vertex/vertex3d.h"

namespace Spark::Render
{
struct Transformation3D
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

class VulkanPipeline3D : public VulkanPipeline
{
  public:
    VulkanPipeline3D(VulkanContext &context, VulkanFramebuffer &framebuffer);
    virtual ~VulkanPipeline3D();

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
                                     std::vector<VkImageView> &textureImageViews,
                                     std::vector<VkSampler> &textureSamplers);

    void createSingleTransformationDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                                 std::vector<VkBuffer> transformationUniforms);
    void createSingleTextureDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &textureSets,
                                          VkImageView textureImageView, VkSampler textureSampler);

  private:
    using VulkanPipeline::bind;
    void createDescriptorSetLayout();
    void createGraphicsPipeline();

  private:
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorSetLayout m_transformationDescriptorSetLayout;
    VkDescriptorSetLayout m_textureDescriptorSetLayout;
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_PIPELINE3D_H */
