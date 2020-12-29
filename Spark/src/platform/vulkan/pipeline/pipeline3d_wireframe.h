#pragma once

#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/vertex/vertex3d.h"

namespace Spark::Render
{
struct Transformation3DWireframe
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct Vulkan3DWireframePushConsts
{
    alignas(16) glm::vec3 color;
};

class VulkanPipeline3DWireframe : public VulkanPipeline
{
  public:
    VulkanPipeline3DWireframe(VulkanContext &context, VulkanFramebuffer &framebuffer);
    virtual ~VulkanPipeline3DWireframe();

    virtual void cleanup();
    virtual void recreate();

    virtual void bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet,
                      struct Vulkan3DWireframePushConsts pushConsts);

    VkDescriptorSetLayout getMVPDescriptorSetLayout();

    void createTransformationDescriptorSets(unsigned int drawablesAmount,
                                            std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                            std::vector<std::vector<VkBuffer>> transformationUniforms);

    void createSingleTransformationDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                                 std::vector<VkBuffer> transformationUniforms);

  private:
    void createDescriptorSetLayout();
    void createGraphicsPipeline();

  private:
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorSetLayout m_transformationDescriptorSetLayout;
};
} // namespace Spark::Render
