#include "pipeline3d_lights.h"

#include "platform/vulkan/vertex/vertex2d.h"
#include "spark/core/log.h"
#include "spark/utils/file.h"

namespace Spark
{
const std::string VERTEX_3D_SHADER_PATH = "shaders/shader3dLights_vert.spv";
const std::string FRAGMENT_3D_SHADER_PATH = "shaders/shader3dLights_frag.spv";

VulkanPipeline3DLights::VulkanPipeline3DLights(VulkanContext &context, VulkanFramebuffer &framebuffer)
    : VulkanPipeline(context, framebuffer)
    , m_transformationDescriptorSetLayout(VK_NULL_HANDLE)
    , m_textureDescriptorSetLayout(VK_NULL_HANDLE)
{
    createDescriptorSetLayout();
    createGraphicsPipeline();
}

VulkanPipeline3DLights::~VulkanPipeline3DLights()
{
    cleanup();
}

void VulkanPipeline3DLights::cleanup()
{
    vkDestroyDescriptorSetLayout(m_context.m_device, m_transformationDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(m_context.m_device, m_textureDescriptorSetLayout, nullptr);
    vkDestroyPipelineLayout(m_context.m_device, m_pipelineLayout, VK_NULL_HANDLE);
    m_pipelineLayout = VK_NULL_HANDLE;
    VulkanPipeline::cleanup();
}

void VulkanPipeline3DLights::recreate()
{
    VulkanPipeline::recreate();
    createDescriptorSetLayout();
    createGraphicsPipeline();
}

void VulkanPipeline3DLights::bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet,
                                  VkDescriptorSet textureSet)
{
    const VkDescriptorSet descriptorSets[] = {transformationSet, textureSet};
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 2, descriptorSets, 0,
                            nullptr);
    VulkanPipeline::bind(commandBuffer);
}

VkDescriptorSetLayout VulkanPipeline3DLights::getMVPDescriptorSetLayout()
{
    return m_transformationDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanPipeline3DLights::getTextureDescriptorSetLayout()
{
    return m_textureDescriptorSetLayout;
}

void VulkanPipeline3DLights::createTransformationDescriptorSets(
    unsigned int drawablesAmount, std::vector<std::vector<VkDescriptorSet>> &transformationSets,
    std::vector<std::vector<VkBuffer>> transformationUniforms)
{
    std::vector<VkDescriptorSetLayout> transformationLayouts(m_context.m_swapChainImages.size(),
                                                             m_transformationDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    transformationSets.resize(drawablesAmount, std::vector<VkDescriptorSet>(m_context.m_swapChainImages.size()));
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_context.m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_context.m_swapChainImages.size());
    allocInfo.pSetLayouts = transformationLayouts.data();
    for (size_t i = 0; i < drawablesAmount; i++)
    {
        SPARK_CORE_ASSERT(vkAllocateDescriptorSets(m_context.m_device, &allocInfo, transformationSets[i].data()) ==
                              VK_SUCCESS,
                          "Failed to allocate descriptor sets");
    }

    std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos;
    bufferInfos.resize(drawablesAmount, std::vector<VkDescriptorBufferInfo>(m_context.m_swapChainImages.size()));

    std::vector<VkWriteDescriptorSet> descriptorWrites = {};

    for (size_t i = 0; i < drawablesAmount; i++)
    {
        for (size_t j = 0; j < m_context.m_swapChainImages.size(); j++)
        {
            bufferInfos[i][j].buffer = transformationUniforms[i][j];
            bufferInfos[i][j].offset = 0;
            bufferInfos[i][j].range = sizeof(Transformation3D);

            VkWriteDescriptorSet writeDescripotrSet = {};

            writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescripotrSet.dstSet = transformationSets[i][j];
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

void VulkanPipeline3DLights::createTextureDescriptorSets(unsigned int texturesAmount,
                                                         std::vector<std::vector<VkDescriptorSet>> &texturesSets,
                                                         std::vector<VkImageView> &textureImageView,
                                                         std::vector<VkSampler> &textureSampler)
{
    std::vector<VkDescriptorSetLayout> textureLayouts(m_context.m_swapChainImages.size(), m_textureDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    texturesSets.resize(texturesAmount, std::vector<VkDescriptorSet>(m_context.m_swapChainImages.size()));
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_context.m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_context.m_swapChainImages.size());
    allocInfo.pSetLayouts = textureLayouts.data();
    for (size_t i = 0; i < texturesAmount; i++)
    {
        SPARK_CORE_ASSERT(vkAllocateDescriptorSets(m_context.m_device, &allocInfo, texturesSets[i].data()) ==
                              VK_SUCCESS,
                          "Failed to allocate descriptor sets");
    }

    std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;
    imageInfos.resize(texturesAmount, std::vector<VkDescriptorImageInfo>(m_context.m_swapChainImages.size()));

    std::vector<VkWriteDescriptorSet> descriptorWrites = {};

    for (size_t i = 0; i < texturesAmount; i++)
    {
        for (size_t j = 0; j < m_context.m_swapChainImages.size(); j++)
        {
            imageInfos[i][j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i][j].imageView = textureImageView[i];
            imageInfos[i][j].sampler = textureSampler[i];

            VkWriteDescriptorSet writeDescripotrSet = {};

            writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescripotrSet.dstSet = texturesSets[i][j];
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

void VulkanPipeline3DLights::createSingleTransformationDescriptorSet(
    std::vector<std::vector<VkDescriptorSet>> &transformationSets, std::vector<VkBuffer> transformationUniforms)
{
    transformationSets.push_back(std::vector<VkDescriptorSet>(m_context.m_swapChainImages.size()));
    std::vector<VkDescriptorSetLayout> transformationLayouts(m_context.m_swapChainImages.size(),
                                                             m_transformationDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_context.m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_context.m_swapChainImages.size());
    allocInfo.pSetLayouts = transformationLayouts.data();
    SPARK_CORE_ASSERT(vkAllocateDescriptorSets(m_context.m_device, &allocInfo, transformationSets.back().data()) ==
                          VK_SUCCESS,
                      "Failed to allocate descriptor sets");

    std::vector<VkDescriptorBufferInfo> bufferInfos;
    bufferInfos.resize(m_context.m_swapChainImages.size());

    std::vector<VkWriteDescriptorSet> descriptorWrites = {};

    for (size_t i = 0; i < m_context.m_swapChainImages.size(); i++)
    {
        bufferInfos[i].buffer = transformationUniforms[i];
        bufferInfos[i].offset = 0;
        bufferInfos[i].range = sizeof(Transformation3D);

        VkWriteDescriptorSet writeDescripotrSet = {};

        writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescripotrSet.dstSet = transformationSets.back()[i];
        writeDescripotrSet.dstBinding = 0;
        writeDescripotrSet.dstArrayElement = 0;
        writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescripotrSet.descriptorCount = 1;
        writeDescripotrSet.pBufferInfo = &bufferInfos[i];
        descriptorWrites.push_back(writeDescripotrSet);
    }

    vkUpdateDescriptorSets(m_context.m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
                           0, nullptr);
}

void VulkanPipeline3DLights::createSingleTextureDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &textureSets,
                                                              VkImageView textureImageView, VkSampler textureSampler)
{
    textureSets.push_back(std::vector<VkDescriptorSet>(m_context.m_swapChainImages.size()));
    std::vector<VkDescriptorSetLayout> textureLayouts(m_context.m_swapChainImages.size(), m_textureDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_context.m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_context.m_swapChainImages.size());
    allocInfo.pSetLayouts = textureLayouts.data();
    SPARK_CORE_ASSERT(vkAllocateDescriptorSets(m_context.m_device, &allocInfo, textureSets.back().data()) == VK_SUCCESS,
                      "Failed to allocate descriptor sets");

    std::vector<VkDescriptorImageInfo> imageInfos;
    imageInfos.resize(m_context.m_swapChainImages.size());

    std::vector<VkWriteDescriptorSet> descriptorWrites = {};

    for (size_t i = 0; i < m_context.m_swapChainImages.size(); i++)
    {
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView = textureImageView;
        imageInfos[i].sampler = textureSampler;

        VkWriteDescriptorSet writeDescripotrSet = {};

        writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescripotrSet.dstSet = textureSets.back()[i];
        writeDescripotrSet.dstBinding = 0;
        writeDescripotrSet.dstArrayElement = 0;
        writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescripotrSet.descriptorCount = 1;
        writeDescripotrSet.pImageInfo = &imageInfos[i];
        descriptorWrites.push_back(writeDescripotrSet);
    }

    vkUpdateDescriptorSets(m_context.m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
                           0, nullptr);
}

void VulkanPipeline3DLights::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding transformationLayoutBinding = {};
    transformationLayoutBinding.binding = 0;
    transformationLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    transformationLayoutBinding.descriptorCount = 1;
    transformationLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    transformationLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding dirLightLayoutBinding = {};
    dirLightLayoutBinding.binding = 0;
    dirLightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    dirLightLayoutBinding.descriptorCount = 1;
    dirLightLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    dirLightLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding pointLightLayoutBinding = {};
    pointLightLayoutBinding.binding = 1;
    pointLightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pointLightLayoutBinding.descriptorCount = 1;
    pointLightLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pointLightLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding spotLightLayoutBinding = {};
    spotLightLayoutBinding.binding = 2;
    spotLightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    spotLightLayoutBinding.descriptorCount = 1;
    spotLightLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    spotLightLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &transformationLayoutBinding;
    SPARK_CORE_ASSERT(vkCreateDescriptorSetLayout(m_context.m_device, &layoutInfo, nullptr,
                                                  &m_transformationDescriptorSetLayout) == VK_SUCCESS,
                      "failed to create descriptor set layout!");

    std::array<VkDescriptorSetLayoutBinding, 3> lightBindings = {dirLightLayoutBinding, pointLightLayoutBinding,
                                                                 spotLightLayoutBinding};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(lightBindings.size());
    layoutInfo.pBindings = lightBindings.data();
    SPARK_CORE_ASSERT(vkCreateDescriptorSetLayout(m_context.m_device, &layoutInfo, nullptr,
                                                  &m_lightsDescriptorSetLayout) == VK_SUCCESS,
                      "failed to create descriptor set layout!");

    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &samplerLayoutBinding;
    SPARK_CORE_ASSERT(vkCreateDescriptorSetLayout(m_context.m_device, &layoutInfo, nullptr,
                                                  &m_textureDescriptorSetLayout) == VK_SUCCESS,
                      "failed to create descriptor set layout!");
}

void VulkanPipeline3DLights::createGraphicsPipeline()
{
    auto vertexShaderCode = readFile(VERTEX_3D_SHADER_PATH);
    auto fragmentShaderCode = readFile(FRAGMENT_3D_SHADER_PATH);

    VkShaderModule vertexShader = createShaderModule(vertexShaderCode);
    VkShaderModule fragmentShader = createShaderModule(fragmentShaderCode);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex3D::getBindingDescription();
    auto attributeDescriptions = Vertex3D::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    std::array<VkDescriptorSetLayout, 3> descriptorSets = {m_transformationDescriptorSetLayout,
                                                           m_lightsDescriptorSetLayout, m_textureDescriptorSetLayout};

	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(struct PushConsts);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSets.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSets.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    SPARK_CORE_ASSERT(vkCreatePipelineLayout(m_context.m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) ==
                          VK_SUCCESS,
                      "failed to create pipeline layout!");

    VulkanPipeline::createGraphicsPipeline(vertexShader, fragmentShader, vertexInputInfo, m_pipelineLayout, true);

    vkDestroyShaderModule(m_context.m_device, vertexShader, VK_NULL_HANDLE);
    vkDestroyShaderModule(m_context.m_device, fragmentShader, VK_NULL_HANDLE);
}
} // namespace Spark
