#ifndef SPARK_VULKAN_PIPELINE3D_OUTLINE_H
#define SPARK_VULKAN_PIPELINE3D_OUTLINE_H

#include "platform/vulkan/pipeline/pipeline3d.h"
#include "platform/vulkan/vertex/vertex3d.h"

namespace Spark::Render
{
struct Vulkan3DOutlinePushConsts
{
    alignas(16) glm::vec3 color;
    alignas(4) float lineWidth;
};

class VulkanPipeline3DOutline : public VulkanPipeline
{
  public:
    VulkanPipeline3DOutline(VulkanContext &context, VulkanFramebuffer &framebuffer, bool clean = false,
                            bool xRay = true);
    virtual ~VulkanPipeline3DOutline();

    virtual void cleanup();
    virtual void recreate();

    virtual void bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet,
                      struct Vulkan3DOutlinePushConsts pushConsts);

    VkDescriptorSetLayout getMVPDescriptorSetLayout();

    void createTransformationDescriptorSets(unsigned int drawablesAmount,
                                            std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                            std::vector<std::vector<VkBuffer>> transformationUniforms);
    std::vector<VkDescriptorSet> createTransformationDescriptorSet(std::vector<VkBuffer> transformationUniforms);

    void createSingleTransformationDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                                 std::vector<VkBuffer> transformationUniforms);

    void setXray(bool xRay);

  private:
    void createDescriptorSetLayout();
    void createGraphicsPipeline();

  private:
    using VulkanPipeline::bind;
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorSetLayout m_transformationDescriptorSetLayout;
    bool m_clean;
    bool m_xRay;
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_PIPELINE3D_OUTLINE_H */
