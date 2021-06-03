#include "pipeline3d_model.h"

#include "spark/core/log.h"
#include "spark/utils/file.h"

namespace Spark::Render
{
const std::string VERTEX_3D_SHADER_PATH = "shaders/shader3dModel_vert.spv";
const std::string FRAGMENT_3D_MODEL_SHADER_PATH = "shaders/shader3dModel_frag.spv";
#define MAX_TEXTURES 25

VulkanPipeline3DModel::VulkanPipeline3DModel(VulkanContext &context, VulkanFramebuffer &framebuffer)
    : VulkanPipeline(context, framebuffer)
    , m_transformationDescriptorSetLayout(VK_NULL_HANDLE)
    , m_lightsDescriptorSetLayout(VK_NULL_HANDLE)
    , m_materialDescriptorSetLayout(VK_NULL_HANDLE)
    , m_textureDescriptorSetLayout(VK_NULL_HANDLE)
{
    createDescriptorSetLayout();
    createGraphicsPipeline();
}

VulkanPipeline3DModel::~VulkanPipeline3DModel()
{
    cleanup();
}

void VulkanPipeline3DModel::cleanup()
{
    vkDestroyDescriptorSetLayout(m_context.m_device, m_transformationDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(m_context.m_device, m_lightsDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(m_context.m_device, m_materialDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(m_context.m_device, m_textureDescriptorSetLayout, nullptr);
    vkDestroyPipelineLayout(m_context.m_device, m_pipelineLayout, VK_NULL_HANDLE);
    m_pipelineLayout = VK_NULL_HANDLE;
    VulkanPipeline::cleanup();
}

void VulkanPipeline3DModel::recreate()
{
    VulkanPipeline::recreate();
    createDescriptorSetLayout();
    createGraphicsPipeline();
}

void VulkanPipeline3DModel::bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet,
                                 VkDescriptorSet lightSet, VkDescriptorSet materialSet, VkDescriptorSet textureSet,
                                 struct Vulkan3DModelConsts pushConsts)
{
    const VkDescriptorSet descriptorSets[] = {transformationSet, lightSet, materialSet, textureSet};
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 4, descriptorSets, 0,
                            nullptr);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(pushConsts),
                       &pushConsts);
}

VkDescriptorSetLayout VulkanPipeline3DModel::getMVPDescriptorSetLayout()
{
    return m_transformationDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanPipeline3DModel::getLightsDescriptorSetLayout()
{
    return m_lightsDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanPipeline3DModel::getMaterialDescriptorSetLayout()
{
    return m_materialDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanPipeline3DModel::getTextureDescriptorSetLayout()
{
    return m_textureDescriptorSetLayout;
}

void VulkanPipeline3DModel::createTransformationDescriptorSets(
    unsigned int drawablesAmount, std::vector<std::vector<VkDescriptorSet>> &transformationSets,
    std::vector<std::vector<VkBuffer>> transformationUniforms)
{
    allocateDescriptorSets(drawablesAmount, m_transformationDescriptorSetLayout, transformationSets);
    updateBufferDescriptorSets(drawablesAmount, transformationSets, transformationUniforms,
                               sizeof(Transformation3DModel));
}

void VulkanPipeline3DModel::createLightDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &lightSets,
                                                      std::vector<VkBuffer> &dirLightUniform,
                                                      std::vector<VkBuffer> &pointLightsUniform,
                                                      std::vector<VkBuffer> &spotLightUniform)
{
    allocateDescriptorSets(1, m_lightsDescriptorSetLayout, lightSets);
    updateLightsDescriptorSets(lightSets, dirLightUniform, pointLightsUniform, spotLightUniform);
}

void VulkanPipeline3DModel::createMaterialDescriptorSets(unsigned int drawablesAmount,
                                                         std::vector<std::vector<VkDescriptorSet>> &materialSets,
                                                         std::vector<std::vector<VkBuffer>> materialUniform)
{
    allocateDescriptorSets(drawablesAmount, m_materialDescriptorSetLayout, materialSets);
    updateBufferDescriptorSets(drawablesAmount, materialSets, materialUniform, sizeof(MaterialModel));
}

void VulkanPipeline3DModel::createTextureDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &texturesSets,
                                                        std::vector<std::vector<VkImageView>> &textureImageViews,
                                                        std::vector<std::vector<VkSampler>> &textureSamplers,
                                                        std::vector<std::vector<VkImageView>> &specularImageViews,
                                                        std::vector<std::vector<VkSampler>> &specularSamplers)
{
    allocateDescriptorSets(static_cast<unsigned int>(textureImageViews.size()), m_textureDescriptorSetLayout,
                           texturesSets);
    updateTextureDescriptorSets(texturesSets, textureImageViews, textureSamplers, specularImageViews, specularSamplers);
}

void VulkanPipeline3DModel::createSingleTransformationDescriptorSet(
    std::vector<std::vector<VkDescriptorSet>> &transformationSets, std::vector<VkBuffer> transformationUniforms)
{
    addDescriptorSets(m_transformationDescriptorSetLayout, transformationSets);
    updateBufferDescriptorSets(1, transformationSets, {transformationUniforms}, sizeof(Transformation3DModel),
                               static_cast<unsigned int>(transformationSets.size() - 1));
}

void VulkanPipeline3DModel::createSingleMaterialDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &materialSets,
                                                              std::vector<VkBuffer> materialUniforms)
{
    addDescriptorSets(m_materialDescriptorSetLayout, materialSets);
    updateBufferDescriptorSets(1, materialSets, {materialUniforms}, sizeof(MaterialModel),
                               static_cast<unsigned int>(materialSets.size() - 1));
}

void VulkanPipeline3DModel::createSingleTextureDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &textureSets,
                                                             std::vector<VkImageView> textureImageView,
                                                             std::vector<VkSampler> textureSampler,
                                                             std::vector<VkImageView> specularImageView,
                                                             std::vector<VkSampler> specularSampler)
{
    std::vector<std::vector<VkImageView>> imageViews = {textureImageView};
    std::vector<std::vector<VkSampler>> samplers = {textureSampler};
    std::vector<std::vector<VkImageView>> specularViews = {specularImageView};
    std::vector<std::vector<VkSampler>> specularSamplers = {specularSampler};
    addDescriptorSets(m_textureDescriptorSetLayout, textureSets);
    updateTextureDescriptorSets(textureSets, imageViews, samplers, specularViews, specularSamplers,
                                static_cast<unsigned int>(textureSets.size() - 1));
}

void VulkanPipeline3DModel::createDescriptorSetLayout()
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
    samplerLayoutBinding.descriptorCount = MAX_TEXTURES;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding specularLayoutBinding = {};
    specularLayoutBinding.binding = 1;
    specularLayoutBinding.descriptorCount = MAX_TEXTURES;
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

void VulkanPipeline3DModel::createGraphicsPipeline()
{
    auto vertexShaderCode = readFile(VERTEX_3D_SHADER_PATH);
    auto fragmentShaderCode = readFile(FRAGMENT_3D_MODEL_SHADER_PATH);

    VkShaderModule vertexShader = createShaderModule(vertexShaderCode);
    VkShaderModule fragmentShader = createShaderModule(fragmentShaderCode);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = vertex3DGetBindingDescription();
    auto attributeDescriptions = vertex3DGetAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    std::array<VkDescriptorSetLayout, 4> descriptorSets = {m_transformationDescriptorSetLayout,
                                                           m_lightsDescriptorSetLayout, m_materialDescriptorSetLayout,
                                                           m_textureDescriptorSetLayout};

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(struct Vulkan3DModelConsts);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSets.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSets.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    SPARK_CORE_ASSERT(vkCreatePipelineLayout(m_context.m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) ==
                          VK_SUCCESS,
                      "failed to create pipeline layout!");

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_TRUE;
    depthStencil.front.compareOp = VK_COMPARE_OP_NOT_EQUAL;
    depthStencil.front.failOp = VK_STENCIL_OP_KEEP;
    depthStencil.front.depthFailOp = VK_STENCIL_OP_REPLACE;
    depthStencil.front.passOp = VK_STENCIL_OP_REPLACE;
    depthStencil.front.compareMask = 0x01;
    depthStencil.front.writeMask = 0x01;
    depthStencil.front.reference = 1;
    depthStencil.back = depthStencil.front;
    setDepthStencilState(depthStencil);

    VulkanPipeline::createGraphicsPipeline(vertexShader, fragmentShader, vertexInputInfo, m_pipelineLayout, true);

    vkDestroyShaderModule(m_context.m_device, vertexShader, VK_NULL_HANDLE);
    vkDestroyShaderModule(m_context.m_device, fragmentShader, VK_NULL_HANDLE);
}

void VulkanPipeline3DModel::updateLightsDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &lightSets,
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
        bufferInfos[i * 3].range = sizeof(VulkanShaderDirectionalLightModel);
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
        bufferInfos[i * 3 + 1].range = sizeof(VulkanShaderPointLightModel) * MAX_POINT_LIGHTS;
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
        bufferInfos[i * 3 + 2].range = sizeof(VulkanShaderSpotLightModel);
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

void VulkanPipeline3DModel::updateTextureDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &sets,
                                                        std::vector<std::vector<VkImageView>> &textureImageViews,
                                                        std::vector<std::vector<VkSampler>> &textureSamplers,
                                                        std::vector<std::vector<VkImageView>> &specularImageViews,
                                                        std::vector<std::vector<VkSampler>> &specularSamplers,
                                                        unsigned int offset)
{
    std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;

    std::vector<VkWriteDescriptorSet> descriptorWrites = {};

    for (size_t i = 0; i < textureImageViews.size(); i++)
    {
        for (size_t j = 0; j < m_context.m_swapChainImages.size(); j++)
        {
            std::vector<VkDescriptorImageInfo> info(MAX_TEXTURES);
            VkWriteDescriptorSet writeDescripotrSet = {};
            if (textureImageViews[i].size() > 0)
            {
                for (size_t k = 0; k < textureImageViews[i].size(); k++)
                {
                    info[k].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    info[k].imageView = textureImageViews[i][k];
                    info[k].sampler = textureSamplers[i][k];
                }
                for (size_t k = 0; k < MAX_TEXTURES - textureImageViews[i].size(); k++)
                {
                    info[textureImageViews[i].size() + k].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    info[textureImageViews[i].size() + k].imageView = textureImageViews[i][0];
                    info[textureImageViews[i].size() + k].sampler = textureSamplers[i][0];
                }
                imageInfos.push_back(info);
                writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescripotrSet.dstSet = sets[offset + i][j];
                writeDescripotrSet.dstBinding = 0;
                writeDescripotrSet.dstArrayElement = 0;
                writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                writeDescripotrSet.descriptorCount = MAX_TEXTURES;
                writeDescripotrSet.pImageInfo = imageInfos.back().data();
                descriptorWrites.push_back(writeDescripotrSet);
            }

            if (specularImageViews[i].size() > 0)
            {
                for (size_t k = 0; k < specularImageViews[i].size(); k++)
                {
                    info[k].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    info[k].imageView = specularImageViews[i][k];
                    info[k].sampler = specularSamplers[i][k];
                }
                for (size_t k = 0; k < MAX_TEXTURES - specularImageViews[i].size(); k++)
                {
                    info[specularImageViews[i].size() + k].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    info[specularImageViews[i].size() + k].imageView = specularImageViews[i][0];
                    info[specularImageViews[i].size() + k].sampler = specularSamplers[i][0];
                }
                imageInfos.push_back(info);
                writeDescripotrSet.dstBinding = 1;
                writeDescripotrSet.pImageInfo = imageInfos.back().data();
                descriptorWrites.push_back(writeDescripotrSet);
            }
        }
    }

    vkUpdateDescriptorSets(m_context.m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
                           0, nullptr);
}
} // namespace Spark::Render
