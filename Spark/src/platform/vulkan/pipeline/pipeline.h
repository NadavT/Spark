#pragma once

#include "platform/vulkan/framebuffer/framebuffer.h"
#include "platform/vulkan/vulkan_context.h"

namespace Spark::Render
{
enum class VulkanPipelineType
{
    Type2D,
    Type3D,
    Type3DLights,
    Type3DModel,
    Type3DOutline,
    Type3DCleanOutline,
    Type3DWireframe,
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

    void destroyDescriptorSet(std::vector<VkDescriptorSet> &set);

  protected:
    VulkanPipeline(VulkanContext &context, VulkanFramebuffer &framebuffer);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    void createGraphicsPipeline(VkShaderModule vertexShader, VkShaderModule fragmentShader,
                                VkPipelineVertexInputStateCreateInfo vertexInputInfo, VkPipelineLayout layout,
                                bool depthTesting = false, VkPipeline *pipeline = VK_NULL_HANDLE);
    void allocateDescriptorSets(unsigned int amount, VkDescriptorSetLayout layout,
                                std::vector<std::vector<VkDescriptorSet>> &sets);
    void allocateDescriptorSet(VkDescriptorSetLayout layout, std::vector<VkDescriptorSet> &set);
    std::vector<VkDescriptorSet> createDescriptorSet(VkDescriptorSetLayout layout);
    void addDescriptorSets(VkDescriptorSetLayout layout, std::vector<std::vector<VkDescriptorSet>> &sets,
                           unsigned int amount = 1);
    void updateBufferDescriptorSets(unsigned int amount, std::vector<std::vector<VkDescriptorSet>> &sets,
                                    std::vector<std::vector<VkBuffer>> uniforms, VkDeviceSize range,
                                    unsigned int offset = 0);
    void updateBufferDescriptorSet(std::vector<VkDescriptorSet> &set, std::vector<VkBuffer> uniform,
                                   VkDeviceSize range);
    void updateTextureDescriptorSets(unsigned int amount, std::vector<std::vector<VkDescriptorSet>> &sets,
                                     std::vector<VkImageView> &textureImageViews,
                                     std::vector<VkSampler> &textureSamplers, unsigned int offset = 0);

    void setInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo info);
    void setviewportState(VkPipelineViewportStateCreateInfo info);
    void setRasterizationState(VkPipelineRasterizationStateCreateInfo info);
    void setMultiSampleState(VkPipelineMultisampleStateCreateInfo info);
    void setDepthStencilState(VkPipelineDepthStencilStateCreateInfo info);
    void setColorBlendState(VkPipelineColorBlendStateCreateInfo info);

  protected:
    VulkanContext &m_context;
    VulkanFramebuffer &m_framebuffer;
    VkPipeline m_pipeline;
    VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyState;
    VkViewport m_viewport;
    VkRect2D m_scissor;
    VkPipelineViewportStateCreateInfo m_viewportState;
    VkPipelineRasterizationStateCreateInfo m_rasterizationState;
    VkPipelineMultisampleStateCreateInfo m_multiSampleState;
    VkPipelineDepthStencilStateCreateInfo m_depthStencilState;
    VkPipelineColorBlendAttachmentState m_colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo m_colorBlendState;
};
} // namespace Spark::Render
