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
    , m_lightsDescriptorSetLayout(VK_NULL_HANDLE)
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
    vkDestroyDescriptorSetLayout(m_context.m_device, m_lightsDescriptorSetLayout, nullptr);
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
                                  VkDescriptorSet lightSet, VkDescriptorSet textureSet, struct PushConsts pushConsts)
{
    const VkDescriptorSet descriptorSets[] = {transformationSet, lightSet, textureSet};
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 3, descriptorSets, 0,
                            nullptr);
    VulkanPipeline::bind(commandBuffer);
    vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(pushConsts),
                       &pushConsts);
}

void VulkanPipeline3DLights::bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet,
                                  VkDescriptorSet lightSet, struct PushConsts pushConsts)
{
    const VkDescriptorSet descriptorSets[] = {transformationSet, lightSet};
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 2, descriptorSets, 0,
                            nullptr);
    VulkanPipeline::bind(commandBuffer);
    vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(pushConsts),
                       &pushConsts);
}

VkDescriptorSetLayout VulkanPipeline3DLights::getMVPDescriptorSetLayout()
{
    return m_transformationDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanPipeline3DLights::getLightsDescriptorSetLayout()
{
    return m_lightsDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanPipeline3DLights::getTextureDescriptorSetLayout()
{
    return m_textureDescriptorSetLayout;
}

void VulkanPipeline3DLights::createTransformationDescriptorSets(
    unsigned int drawablesAmount, std::vector<std::vector<VkDescriptorSet>> &transformationSets,
    std::vector<std::vector<VkBuffer>> transformationUniforms)
{
    allocateDescriptorSets(drawablesAmount, m_transformationDescriptorSetLayout, transformationSets);
    updateBufferDescriptorSets(drawablesAmount, transformationSets, transformationUniforms,
                               sizeof(Transformation3DLights));
}

void VulkanPipeline3DLights::createLightDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &lightSets,
                                                       std::vector<VkBuffer> &dirLightUniform,
                                                       std::vector<VkBuffer> &pointLightsUniform,
                                                       std::vector<VkBuffer> &spotLightUniform)
{
    allocateDescriptorSets(1, m_lightsDescriptorSetLayout, lightSets);
    updateLightsDescriptorSets(lightSets, dirLightUniform, pointLightsUniform, spotLightUniform);
}

void VulkanPipeline3DLights::createTextureDescriptorSets(unsigned int texturesAmount,
                                                         std::vector<std::vector<VkDescriptorSet>> &texturesSets,
                                                         std::vector<VkImageView> &textureImageViews,
                                                         std::vector<VkSampler> &textureSamplers,
                                                         std::vector<VkImageView> &specularImageViews,
                                                         std::vector<VkSampler> &specularSamplers,
                                                         std::vector<std::vector<VkBuffer>> materialUniform)
{
    allocateDescriptorSets(texturesAmount, m_textureDescriptorSetLayout, texturesSets);
    updateTextureDescriptorSets(texturesAmount, texturesSets, textureImageViews, textureSamplers, specularImageViews,
                                specularSamplers, materialUniform);
}

void VulkanPipeline3DLights::createSingleTransformationDescriptorSet(
    std::vector<std::vector<VkDescriptorSet>> &transformationSets, std::vector<VkBuffer> transformationUniforms)
{
    addDescriptorSets(m_transformationDescriptorSetLayout, transformationSets);
    updateBufferDescriptorSets(1, transformationSets, {transformationUniforms}, sizeof(Transformation3DLights),
                               static_cast<unsigned int>(transformationSets.size() - 1));
}

void VulkanPipeline3DLights::createSingleTextureDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &textureSets,
                                                              VkImageView textureImageView, VkSampler textureSampler,
                                                              VkImageView specularImageView, VkSampler specularSampler,
                                                              std::vector<VkBuffer> materialUniform)
{
    std::vector<VkImageView> imageViews = {textureImageView};
    std::vector<VkSampler> samplers = {textureSampler};
    std::vector<VkImageView> specularViews = {specularImageView};
    std::vector<VkSampler> specularSamplers = {specularSampler};
    addDescriptorSets(m_textureDescriptorSetLayout, textureSets);
    updateTextureDescriptorSets(1, textureSets, imageViews, samplers, specularViews, specularSamplers,
                                {materialUniform}, static_cast<unsigned int>(textureSets.size() - 1));
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

    VkDescriptorSetLayoutBinding specularLayoutBinding = {};
    specularLayoutBinding.binding = 1;
    specularLayoutBinding.descriptorCount = 1;
    specularLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    specularLayoutBinding.pImmutableSamplers = nullptr;
    specularLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding materialBinding = {};
    materialBinding.binding = 2;
    materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    materialBinding.descriptorCount = 1;
    materialBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    materialBinding.pImmutableSamplers = nullptr;

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

    std::array<VkDescriptorSetLayoutBinding, 3> samplerBindings = {samplerLayoutBinding, specularLayoutBinding,
                                                                   materialBinding};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(samplerBindings.size());
    layoutInfo.pBindings = samplerBindings.data();
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

void VulkanPipeline3DLights::updateLightsDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &lightSets,
                                                        std::vector<VkBuffer> &dirLightUniform,
                                                        std::vector<VkBuffer> &pointLightsUniform,
                                                        std::vector<VkBuffer> &spotLightUniform)
{
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    bufferInfos.resize(m_context.m_swapChainImages.size() * 3);

    std::vector<VkWriteDescriptorSet> descriptorWrites = {};

    for (size_t i = 0; i < m_context.m_swapChainImages.size(); i++)
    {
        VkWriteDescriptorSet writeDescripotrSet = {};

        bufferInfos[i * 3].buffer = dirLightUniform[i];
        bufferInfos[i * 3].offset = 0;
        bufferInfos[i * 3].range = sizeof(VulkanShaderDirectionalLight);
        writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescripotrSet.dstSet = lightSets[0][i];
        writeDescripotrSet.dstBinding = 0;
        writeDescripotrSet.dstArrayElement = 0;
        writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescripotrSet.descriptorCount = 1;
        writeDescripotrSet.pBufferInfo = &bufferInfos[i * 3];
        descriptorWrites.push_back(writeDescripotrSet);

        bufferInfos[i * 3 + 1].buffer = pointLightsUniform[i];
        bufferInfos[i * 3 + 1].offset = 0;
        bufferInfos[i * 3 + 1].range = sizeof(VulkanShaderPointLight) * MAX_POINT_LIGHTS;
        writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescripotrSet.dstSet = lightSets[0][i];
        writeDescripotrSet.dstBinding = 1;
        writeDescripotrSet.dstArrayElement = 0;
        writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescripotrSet.descriptorCount = 1;
        writeDescripotrSet.pBufferInfo = &bufferInfos[i * 3 + 1];
        descriptorWrites.push_back(writeDescripotrSet);

        bufferInfos[i * 3 + 2].buffer = spotLightUniform[i];
        bufferInfos[i * 3 + 2].offset = 0;
        bufferInfos[i * 3 + 2].range = sizeof(VulkanShaderSpotLight);
        writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescripotrSet.dstSet = lightSets[0][i];
        writeDescripotrSet.dstBinding = 2;
        writeDescripotrSet.dstArrayElement = 0;
        writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescripotrSet.descriptorCount = 1;
        writeDescripotrSet.pBufferInfo = &bufferInfos[i * 3 + 2];
        descriptorWrites.push_back(writeDescripotrSet);
    }

    vkUpdateDescriptorSets(m_context.m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
                           0, nullptr);
}

void VulkanPipeline3DLights::updateTextureDescriptorSets(
    unsigned int amount, std::vector<std::vector<VkDescriptorSet>> &sets, std::vector<VkImageView> &textureImageViews,
    std::vector<VkSampler> &textureSamplers, std::vector<VkImageView> &specularImageViews,
    std::vector<VkSampler> &specularSamplers, std::vector<std::vector<VkBuffer>> materialUniform, unsigned int offset)
{
    std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;
    imageInfos.resize(amount * 2, std::vector<VkDescriptorImageInfo>(m_context.m_swapChainImages.size()));
    std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos;
    bufferInfos.resize(amount, std::vector<VkDescriptorBufferInfo>(m_context.m_swapChainImages.size()));

    std::vector<VkWriteDescriptorSet> descriptorWrites = {};

    for (size_t i = 0; i < amount; i++)
    {
        for (size_t j = 0; j < m_context.m_swapChainImages.size(); j++)
        {
            VkWriteDescriptorSet writeDescripotrSet = {};

            imageInfos[i * 2][j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i * 2][j].imageView = textureImageViews[i];
            imageInfos[i * 2][j].sampler = textureSamplers[i];
            writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescripotrSet.dstSet = sets[offset + i][j];
            writeDescripotrSet.dstBinding = 0;
            writeDescripotrSet.dstArrayElement = 0;
            writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writeDescripotrSet.descriptorCount = 1;
            writeDescripotrSet.pImageInfo = &imageInfos[i * 3][j];
            descriptorWrites.push_back(writeDescripotrSet);

            imageInfos[i * 2 + 1][j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i * 2 + 1][j].imageView = specularImageViews[i];
            imageInfos[i * 2 + 1][j].sampler = specularSamplers[i];
            writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescripotrSet.dstSet = sets[offset + i][j];
            writeDescripotrSet.dstBinding = 1;
            writeDescripotrSet.dstArrayElement = 0;
            writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writeDescripotrSet.descriptorCount = 1;
            writeDescripotrSet.pImageInfo = &imageInfos[i * 3 + 1][j];
            descriptorWrites.push_back(writeDescripotrSet);

            bufferInfos[i][j].buffer = materialUniform[i][j];
            bufferInfos[i][j].offset = 0;
            bufferInfos[i][j].range = sizeof(Material);
            writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescripotrSet.dstSet = sets[offset + i][j];
            writeDescripotrSet.dstBinding = 2;
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

} // namespace Spark
