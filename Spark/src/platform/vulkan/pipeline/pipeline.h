#pragma once

#include "platform/vulkan/framebuffer/framebuffer.h"
#include "platform/vulkan/vulkan_context.h"

namespace Spark
{
enum class VulkanPipelineType
{
    Type2D,
    Type3D,
    Type3DLights,
    Type3DOutline,
    TypeTriangle
};

class VulkanPipeline
{
  public:
    virtual ~VulkanPipeline();
    VulkanPipeline(const VulkanPipeline &other) = delete;
    VulkanPipeline &operator=(const VulkanPipeline &other) = delete;

    virtual void cleanup();
    virtual void recreate();

    virtual void bind(VkCommandBuffer commandBuffer);

  protected:
    VulkanPipeline(VulkanContext &context, VulkanFramebuffer &framebuffer);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    void createGraphicsPipeline(VkShaderModule vertexShader, VkShaderModule fragmentShader,
                                VkPipelineVertexInputStateCreateInfo vertexInputInfo, VkPipelineLayout layout,
                                bool depthTesting = false,
                                VkPipelineDepthStencilStateCreateInfo *depthStencilState = VK_NULL_HANDLE,
                                VkPipeline *pipeline = VK_NULL_HANDLE);
    void allocateDescriptorSets(unsigned int amount, VkDescriptorSetLayout layout,
                                std::vector<std::vector<VkDescriptorSet>> &sets);
    void addDescriptorSets(VkDescriptorSetLayout layout, std::vector<std::vector<VkDescriptorSet>> &sets,
                           unsigned int amount = 1);
    void updateBufferDescriptorSets(unsigned int amount, std::vector<std::vector<VkDescriptorSet>> &sets,
                                    std::vector<std::vector<VkBuffer>> uniforms, VkDeviceSize range,
                                    unsigned int offset = 0);
    void updateTextureDescriptorSets(unsigned int amount, std::vector<std::vector<VkDescriptorSet>> &sets,
                                     std::vector<VkImageView> &textureImageViews,
                                     std::vector<VkSampler> &textureSamplers, unsigned int offset = 0);

  protected:
    VulkanContext &m_context;
    VulkanFramebuffer &m_framebuffer;
    VkPipeline m_pipeline;
};
} // namespace Spark
