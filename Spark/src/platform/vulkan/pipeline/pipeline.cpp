#include "pipeline.h"

#include "spark/core/log.h"

namespace Spark::Render
{
VulkanPipeline::VulkanPipeline(VulkanContext &context, VulkanFramebuffer &framebuffer)
    : m_context(context)
    , m_framebuffer(framebuffer)
    , m_pipeline(VK_NULL_HANDLE)
    , m_inputAssemblyState{}
    , m_viewport{}
    , m_scissor{}
    , m_viewportState{}
    , m_rasterizationState{}
    , m_multiSampleState{}
    , m_depthStencilState{}
    , m_colorBlendAttachment{}
    , m_colorBlendState{}
{
    m_inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    m_inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    m_inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    m_rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    m_rasterizationState.depthClampEnable = VK_FALSE;
    m_rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    m_rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    m_rasterizationState.lineWidth = 1.0f;
    m_rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
    m_rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    m_rasterizationState.depthBiasEnable = VK_FALSE;

    m_depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_depthStencilState.depthTestEnable = VK_TRUE;
    m_depthStencilState.depthWriteEnable = VK_TRUE;
    m_depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    m_depthStencilState.depthBoundsTestEnable = VK_FALSE;
    m_depthStencilState.stencilTestEnable = VK_FALSE;

    m_colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    m_colorBlendAttachment.blendEnable = VK_TRUE;
    m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;

    m_colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    m_colorBlendState.logicOpEnable = VK_FALSE;
    m_colorBlendState.logicOp = VK_LOGIC_OP_COPY;
    m_colorBlendState.attachmentCount = 1;
    m_colorBlendState.pAttachments = &m_colorBlendAttachment;
    m_colorBlendState.blendConstants[0] = 0.0f;
    m_colorBlendState.blendConstants[1] = 0.0f;
    m_colorBlendState.blendConstants[2] = 0.0f;
    m_colorBlendState.blendConstants[3] = 0.0f;

    recreate();
}

VulkanPipeline::~VulkanPipeline()
{
    cleanup();
}

void VulkanPipeline::cleanup()
{
    vkDestroyPipeline(m_context.m_device, m_pipeline, nullptr);
    m_pipeline = VK_NULL_HANDLE;
}

void VulkanPipeline::recreate()
{
    m_pipeline = VK_NULL_HANDLE;

    m_multiSampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    m_multiSampleState.sampleShadingEnable = (m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT) ? VK_TRUE : VK_FALSE;
    m_multiSampleState.rasterizationSamples = m_context.m_msaaSamples;

    m_viewport.x = 0.0f;
    m_viewport.y = 0.0f;
    m_viewport.width = (float)m_context.m_swapChainExtent.width;
    m_viewport.height = (float)m_context.m_swapChainExtent.height;
    m_viewport.minDepth = 0.0f;
    m_viewport.maxDepth = 1.0f;

    m_scissor.offset = {0, 0};
    m_scissor.extent = m_context.m_swapChainExtent;

    m_viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    m_viewportState.viewportCount = 1;
    m_viewportState.pViewports = &m_viewport;
    m_viewportState.scissorCount = 1;
    m_viewportState.pScissors = &m_scissor;
}

void VulkanPipeline::bind(VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
}

VkShaderModule VulkanPipeline::createShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_context.m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void VulkanPipeline::createGraphicsPipeline(VkShaderModule vertexShader, VkShaderModule fragmentShader,
                                            VkPipelineVertexInputStateCreateInfo vertexInputInfo,
                                            VkPipelineLayout layout, bool depthTesting, VkPipeline *pipeline)
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertexShader;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragmentShader;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_STENCIL_WRITE_MASK};

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.pDynamicStates = dynamicStates;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &m_inputAssemblyState;
    pipelineInfo.pViewportState = &m_viewportState;
    pipelineInfo.pRasterizationState = &m_rasterizationState;
    pipelineInfo.pMultisampleState = &m_multiSampleState;
    pipelineInfo.pDepthStencilState = (depthTesting) ? &m_depthStencilState : VK_NULL_HANDLE;
    pipelineInfo.pColorBlendState = &m_colorBlendState;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.layout = layout;
    pipelineInfo.renderPass = m_framebuffer.getRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (pipeline == VK_NULL_HANDLE)
    {
        pipeline = &m_pipeline;
    }

    if (vkCreateGraphicsPipelines(m_context.m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

void VulkanPipeline::allocateDescriptorSets(unsigned int amount, VkDescriptorSetLayout layout,
                                            std::vector<std::vector<VkDescriptorSet>> &sets)
{
    std::vector<VkDescriptorSetLayout> layouts(m_context.m_swapChainImages.size(), layout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    sets.resize(amount, std::vector<VkDescriptorSet>(m_context.m_swapChainImages.size()));
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_context.m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_context.m_swapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();
    for (size_t i = 0; i < amount; i++)
    {
        SPARK_CORE_ASSERT(vkAllocateDescriptorSets(m_context.m_device, &allocInfo, sets[i].data()) == VK_SUCCESS,
                          "Failed to allocate descriptor sets");
    }
}

void VulkanPipeline::addDescriptorSets(VkDescriptorSetLayout layout, std::vector<std::vector<VkDescriptorSet>> &sets,
                                       unsigned int amount)
{
    size_t originalAmount = sets.size();
    for (size_t i = 0; i < amount; i++)
    {
        sets.push_back(std::vector<VkDescriptorSet>(m_context.m_swapChainImages.size()));
    }

    std::vector<VkDescriptorSetLayout> layouts(m_context.m_swapChainImages.size(), layout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_context.m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_context.m_swapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();
    for (size_t i = 0; i < amount; i++)
    {
        SPARK_CORE_ASSERT(vkAllocateDescriptorSets(m_context.m_device, &allocInfo, sets[originalAmount + i].data()) ==
                              VK_SUCCESS,
                          "Failed to allocate descriptor sets");
    }
}

void VulkanPipeline::updateBufferDescriptorSets(unsigned int amount, std::vector<std::vector<VkDescriptorSet>> &sets,
                                                std::vector<std::vector<VkBuffer>> uniforms, VkDeviceSize range,
                                                unsigned int offset)
{
    std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos;
    bufferInfos.resize(amount, std::vector<VkDescriptorBufferInfo>(m_context.m_swapChainImages.size()));

    std::vector<VkWriteDescriptorSet> descriptorWrites = {};

    for (size_t i = 0; i < amount; i++)
    {
        for (size_t j = 0; j < m_context.m_swapChainImages.size(); j++)
        {
            bufferInfos[i][j].buffer = uniforms[i][j];
            bufferInfos[i][j].offset = 0;
            bufferInfos[i][j].range = range;

            VkWriteDescriptorSet writeDescripotrSet = {};

            writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescripotrSet.dstSet = sets[offset + i][j];
            writeDescripotrSet.dstBinding = 0;
            writeDescripotrSet.dstArrayElement = 0;
            writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescripotrSet.descriptorCount = 1;
            writeDescripotrSet.pBufferInfo = &bufferInfos[i][j];
            descriptorWrites.push_back(writeDescripotrSet);
        }
    }

    vkUpdateDescriptorSets(m_context.m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
                           0, nullptr);
}

void VulkanPipeline::updateTextureDescriptorSets(unsigned int amount, std::vector<std::vector<VkDescriptorSet>> &sets,
                                                 std::vector<VkImageView> &textureImageViews,
                                                 std::vector<VkSampler> &textureSamplers, unsigned int offset)
{
    std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;
    imageInfos.resize(amount, std::vector<VkDescriptorImageInfo>(m_context.m_swapChainImages.size()));

    std::vector<VkWriteDescriptorSet> descriptorWrites = {};

    for (size_t i = 0; i < amount; i++)
    {
        for (size_t j = 0; j < m_context.m_swapChainImages.size(); j++)
        {
            imageInfos[i][j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i][j].imageView = textureImageViews[i];
            imageInfos[i][j].sampler = textureSamplers[i];

            VkWriteDescriptorSet writeDescripotrSet = {};

            writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescripotrSet.dstSet = sets[offset + i][j];
            writeDescripotrSet.dstBinding = 0;
            writeDescripotrSet.dstArrayElement = 0;
            writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writeDescripotrSet.descriptorCount = 1;
            writeDescripotrSet.pImageInfo = &imageInfos[i][j];
            descriptorWrites.push_back(writeDescripotrSet);
        }
    }

    vkUpdateDescriptorSets(m_context.m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
                           0, nullptr);
}

void VulkanPipeline::setInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo info)
{
    m_inputAssemblyState = info;
}

void VulkanPipeline::setviewportState(VkPipelineViewportStateCreateInfo info)
{
    m_viewportState = info;
}

void VulkanPipeline::setRasterizationState(VkPipelineRasterizationStateCreateInfo info)
{
    m_rasterizationState = info;
}

void VulkanPipeline::setMultiSampleState(VkPipelineMultisampleStateCreateInfo info)
{
    m_multiSampleState = info;
}

void VulkanPipeline::setDepthStencilState(VkPipelineDepthStencilStateCreateInfo info)
{
    m_depthStencilState = info;
}

void VulkanPipeline::setColorBlendState(VkPipelineColorBlendStateCreateInfo info)
{
    m_colorBlendState = info;
}
} // namespace Spark::Render
