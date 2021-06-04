#include "pipeline3d_lights.h"

#include "spark/core/log.h"
#include "spark/utils/file.h"

namespace Spark::Render
{
const std::string VERTEX_3D_SHADER_PATH = "shaders/shader3dLights_vert.spv";
const std::string FRAGMENT_3D_COLOR_SHADER_PATH = "shaders/shader3dLights_frag_color.spv";
const std::string FRAGMENT_3D_TEXTURE_SHADER_PATH = "shaders/shader3dLights_frag_texture.spv";

VulkanPipeline3DLights::VulkanPipeline3DLights(VulkanContext &context, VulkanFramebuffer &framebuffer)
    : VulkanPipeline(context, framebuffer)
    , m_transformationDescriptorSetLayout(VK_NULL_HANDLE)
    , m_lightsDescriptorSetLayout(VK_NULL_HANDLE)
    , m_materialDescriptorSetLayout(VK_NULL_HANDLE)
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
    vkDestroyDescriptorSetLayout(m_context.m_device, m_materialDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(m_context.m_device, m_textureDescriptorSetLayout, nullptr);
    vkDestroyPipelineLayout(m_context.m_device, m_pipelineColorLayout, VK_NULL_HANDLE);
    m_pipelineColorLayout = VK_NULL_HANDLE;
    vkDestroyPipelineLayout(m_context.m_device, m_pipelineTextureLayout, VK_NULL_HANDLE);
    m_pipelineTextureLayout = VK_NULL_HANDLE;
    vkDestroyPipeline(m_context.m_device, m_pipelineColor, VK_NULL_HANDLE);
    m_pipelineColor = VK_NULL_HANDLE;
    vkDestroyPipeline(m_context.m_device, m_pipelineTexture, VK_NULL_HANDLE);
    m_pipelineTexture = VK_NULL_HANDLE;
    VulkanPipeline::cleanup();
}

void VulkanPipeline3DLights::recreate()
{
    VulkanPipeline::recreate();
    createDescriptorSetLayout();
    createGraphicsPipeline();
}

void VulkanPipeline3DLights::bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet,
                                  VkDescriptorSet lightSet, VkDescriptorSet materialSet, VkDescriptorSet textureSet,
                                  struct Vulkan3DLightsConsts pushConsts)
{
    const VkDescriptorSet descriptorSets[] = {transformationSet, lightSet, materialSet, textureSet};
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineTextureLayout, 0, 4,
                            descriptorSets, 0, nullptr);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineTexture);
    vkCmdPushConstants(commandBuffer, m_pipelineTextureLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(pushConsts),
                       &pushConsts);
}

void VulkanPipeline3DLights::bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet,
                                  VkDescriptorSet lightSet, VkDescriptorSet materialSet,
                                  struct Vulkan3DLightsConsts pushConsts)
{
    const VkDescriptorSet descriptorSets[] = {transformationSet, lightSet, materialSet};
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineColorLayout, 0, 3, descriptorSets,
                            0, nullptr);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineColor);
    vkCmdPushConstants(commandBuffer, m_pipelineColorLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(pushConsts),
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

VkDescriptorSetLayout VulkanPipeline3DLights::getMaterialDescriptorSetLayout()
{
    return m_materialDescriptorSetLayout;
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
    updateBufferDescriptorSets(drawablesAmount, transformationSets, transformationUniforms, sizeof(Transformation3D));
}

void VulkanPipeline3DLights::createLightDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &lightSets,
                                                       std::vector<VkBuffer> &dirLightUniform,
                                                       std::vector<VkBuffer> &pointLightsUniform,
                                                       std::vector<VkBuffer> &spotLightUniform)
{
    allocateDescriptorSets(1, m_lightsDescriptorSetLayout, lightSets);
    updateLightsDescriptorSets(lightSets, dirLightUniform, pointLightsUniform, spotLightUniform);
}

void VulkanPipeline3DLights::createMaterialDescriptorSets(unsigned int drawablesAmount,
                                                          std::vector<std::vector<VkDescriptorSet>> &materialSets,
                                                          std::vector<std::vector<VkBuffer>> materialUniform)
{
    allocateDescriptorSets(drawablesAmount, m_materialDescriptorSetLayout, materialSets);
    updateBufferDescriptorSets(drawablesAmount, materialSets, materialUniform, sizeof(Material));
}

void VulkanPipeline3DLights::createTextureDescriptorSets(unsigned int texturesAmount,
                                                         std::vector<std::vector<VkDescriptorSet>> &texturesSets,
                                                         std::vector<VkImageView> &textureImageViews,
                                                         std::vector<VkSampler> &textureSamplers,
                                                         std::vector<VkImageView> &specularImageViews,
                                                         std::vector<VkSampler> &specularSamplers)
{
    allocateDescriptorSets(texturesAmount, m_textureDescriptorSetLayout, texturesSets);
    updateTextureDescriptorSets(texturesAmount, texturesSets, textureImageViews, textureSamplers, specularImageViews,
                                specularSamplers);
}

void VulkanPipeline3DLights::createSingleTransformationDescriptorSet(
    std::vector<std::vector<VkDescriptorSet>> &transformationSets, std::vector<VkBuffer> transformationUniforms)
{
    addDescriptorSets(m_transformationDescriptorSetLayout, transformationSets);
    updateBufferDescriptorSets(1, transformationSets, {transformationUniforms}, sizeof(Transformation3D),
                               static_cast<unsigned int>(transformationSets.size() - 1));
}

void VulkanPipeline3DLights::createSingleMaterialDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &materialSets,
                                                               std::vector<VkBuffer> materialUniforms)
{
    addDescriptorSets(m_materialDescriptorSetLayout, materialSets);
    updateBufferDescriptorSets(1, materialSets, {materialUniforms}, sizeof(Material),
                               static_cast<unsigned int>(materialSets.size() - 1));
}

void VulkanPipeline3DLights::createSingleTextureDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &textureSets,
                                                              VkImageView textureImageView, VkSampler textureSampler,
                                                              VkImageView specularImageView, VkSampler specularSampler)
{
    std::vector<VkImageView> imageViews = {textureImageView};
    std::vector<VkSampler> samplers = {textureSampler};
    std::vector<VkImageView> specularViews = {specularImageView};
    std::vector<VkSampler> specularSamplers = {specularSampler};
    addDescriptorSets(m_textureDescriptorSetLayout, textureSets);
    updateTextureDescriptorSets(1, textureSets, imageViews, samplers, specularViews, specularSamplers,
                                static_cast<unsigned int>(textureSets.size() - 1));
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

    VkDescriptorSetLayoutBinding materialBinding = {};
    materialBinding.binding = 0;
    materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    materialBinding.descriptorCount = 1;
    materialBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    materialBinding.pImmutableSamplers = nullptr;

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
    layoutInfo.pBindings = &materialBinding;
    SPARK_CORE_ASSERT(vkCreateDescriptorSetLayout(m_context.m_device, &layoutInfo, nullptr,
                                                  &m_materialDescriptorSetLayout) == VK_SUCCESS,
                      "failed to create descriptor set layout!");

    std::array<VkDescriptorSetLayoutBinding, 2> samplerBindings = {samplerLayoutBinding, specularLayoutBinding};
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
    auto fragmentShaderColorCode = readFile(FRAGMENT_3D_COLOR_SHADER_PATH);
    auto fragmentShaderTextureCode = readFile(FRAGMENT_3D_TEXTURE_SHADER_PATH);

    VkShaderModule vertexShader = createShaderModule(vertexShaderCode);
    VkShaderModule fragmentColorShader = createShaderModule(fragmentShaderColorCode);
    VkShaderModule fragmentTextureShader = createShaderModule(fragmentShaderTextureCode);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = vertex3DGetBindingDescription();
    auto attributeDescriptions = vertex3DGetAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    std::array<VkDescriptorSetLayout, 3> descriptorSetsColor = {
        m_transformationDescriptorSetLayout, m_lightsDescriptorSetLayout, m_materialDescriptorSetLayout};
    std::array<VkDescriptorSetLayout, 4> descriptorSetsTexture = {
        m_transformationDescriptorSetLayout, m_lightsDescriptorSetLayout, m_materialDescriptorSetLayout,
        m_textureDescriptorSetLayout};

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(struct Vulkan3DLightsConsts);

    VkPipelineLayoutCreateInfo pipelineColorLayoutInfo{};
    pipelineColorLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineColorLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetsColor.size());
    pipelineColorLayoutInfo.pSetLayouts = descriptorSetsColor.data();
    pipelineColorLayoutInfo.pushConstantRangeCount = 1;
    pipelineColorLayoutInfo.pPushConstantRanges = &pushConstantRange;
    SPARK_CORE_ASSERT(vkCreatePipelineLayout(m_context.m_device, &pipelineColorLayoutInfo, nullptr,
                                             &m_pipelineColorLayout) == VK_SUCCESS,
                      "failed to create pipeline layout!");

    VkPipelineLayoutCreateInfo pipelineTextureLayoutInfo{};
    pipelineTextureLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineTextureLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetsTexture.size());
    pipelineTextureLayoutInfo.pSetLayouts = descriptorSetsTexture.data();
    pipelineTextureLayoutInfo.pushConstantRangeCount = 1;
    pipelineTextureLayoutInfo.pPushConstantRanges = &pushConstantRange;
    SPARK_CORE_ASSERT(vkCreatePipelineLayout(m_context.m_device, &pipelineTextureLayoutInfo, nullptr,
                                             &m_pipelineTextureLayout) == VK_SUCCESS,
                      "failed to create pipeline layout!");

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_TRUE;
    depthStencil.front.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencil.front.failOp = VK_STENCIL_OP_KEEP;
    depthStencil.front.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencil.front.passOp = VK_STENCIL_OP_REPLACE;
    depthStencil.front.compareMask = 0xff;
    depthStencil.front.writeMask = 0xff;
    depthStencil.front.reference = 1;
    depthStencil.back = depthStencil.front;
    setDepthStencilState(depthStencil);

    VulkanPipeline::createGraphicsPipeline(vertexShader, fragmentColorShader, vertexInputInfo, m_pipelineColorLayout,
                                           true, &m_pipelineColor);

    VulkanPipeline::createGraphicsPipeline(vertexShader, fragmentTextureShader, vertexInputInfo,
                                           m_pipelineTextureLayout, true, &m_pipelineTexture);

    vkDestroyShaderModule(m_context.m_device, vertexShader, VK_NULL_HANDLE);
    vkDestroyShaderModule(m_context.m_device, fragmentColorShader, VK_NULL_HANDLE);
    vkDestroyShaderModule(m_context.m_device, fragmentTextureShader, VK_NULL_HANDLE);
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

void VulkanPipeline3DLights::updateTextureDescriptorSets(unsigned int amount,
                                                         std::vector<std::vector<VkDescriptorSet>> &sets,
                                                         std::vector<VkImageView> &textureImageViews,
                                                         std::vector<VkSampler> &textureSamplers,
                                                         std::vector<VkImageView> &specularImageViews,
                                                         std::vector<VkSampler> &specularSamplers, unsigned int offset)
{
    std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;
    imageInfos.resize(amount * 2, std::vector<VkDescriptorImageInfo>(m_context.m_swapChainImages.size()));

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
        }
    }

    vkUpdateDescriptorSets(m_context.m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
                           0, nullptr);
}

} // namespace Spark::Render
