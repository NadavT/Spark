#include "pipeline3d.h"

#include "spark/core/log.h"
#include "spark/utils/file.h"

namespace Spark
{
const std::string VERTEX_3D_SHADER_PATH = "shaders/shader3d_vert.spv";
const std::string FRAGMENT_3D_SHADER_PATH = "shaders/shader3d_frag.spv";

VulkanPipeline3D::VulkanPipeline3D(VulkanContext &context, VulkanFramebuffer &framebuffer)
    : VulkanPipeline(context, framebuffer)
    , m_transformationDescriptorSetLayout(VK_NULL_HANDLE)
    , m_textureDescriptorSetLayout(VK_NULL_HANDLE)
{
    createDescriptorSetLayout();
    createGraphicsPipeline();
}

VulkanPipeline3D::~VulkanPipeline3D()
{
    cleanup();
}

void VulkanPipeline3D::cleanup()
{
    vkDestroyDescriptorSetLayout(m_context.m_device, m_transformationDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(m_context.m_device, m_textureDescriptorSetLayout, nullptr);
    vkDestroyPipelineLayout(m_context.m_device, m_pipelineLayout, VK_NULL_HANDLE);
    m_pipelineLayout = VK_NULL_HANDLE;
    VulkanPipeline::cleanup();
}

void VulkanPipeline3D::recreate()
{
    VulkanPipeline::recreate();
    createDescriptorSetLayout();
    createGraphicsPipeline();
}

void VulkanPipeline3D::bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet,
                            VkDescriptorSet textureSet)
{
    const VkDescriptorSet descriptorSets[] = {transformationSet, textureSet};
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 2, descriptorSets, 0,
                            nullptr);
    VulkanPipeline::bind(commandBuffer);
}

VkDescriptorSetLayout VulkanPipeline3D::getMVPDescriptorSetLayout()
{
    return m_transformationDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanPipeline3D::getTextureDescriptorSetLayout()
{
    return m_textureDescriptorSetLayout;
}

void VulkanPipeline3D::createTransformationDescriptorSets(unsigned int drawablesAmount,
                                                          std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                                          std::vector<std::vector<VkBuffer>> transformationUniforms)
{
    allocateDescriptorSets(drawablesAmount, m_transformationDescriptorSetLayout, transformationSets);
    updateBufferDescriptorSets(drawablesAmount, transformationSets, transformationUniforms, sizeof(Transformation3D));
}

void VulkanPipeline3D::createTextureDescriptorSets(unsigned int texturesAmount,
                                                   std::vector<std::vector<VkDescriptorSet>> &texturesSets,
                                                   std::vector<VkImageView> &textureImageViews,
                                                   std::vector<VkSampler> &textureSamplers)
{
    allocateDescriptorSets(texturesAmount, m_textureDescriptorSetLayout, texturesSets);
    updateTextureDescriptorSets(texturesAmount, texturesSets, textureImageViews, textureSamplers);
}

void VulkanPipeline3D::createSingleTransformationDescriptorSet(
    std::vector<std::vector<VkDescriptorSet>> &transformationSets, std::vector<VkBuffer> transformationUniforms)
{
    addDescriptorSets(m_transformationDescriptorSetLayout, transformationSets);
    updateBufferDescriptorSets(1, transformationSets, {transformationUniforms}, sizeof(Transformation3D),
                               static_cast<unsigned int>(transformationSets.size() - 1));
}

void VulkanPipeline3D::createSingleTextureDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &textureSets,
                                                        VkImageView textureImageView, VkSampler textureSampler)
{
    std::vector<VkImageView> imageViews = {textureImageView};
    std::vector<VkSampler> samplers = {textureSampler};
    addDescriptorSets(m_textureDescriptorSetLayout, textureSets);
    updateTextureDescriptorSets(1, textureSets, imageViews, samplers,
                                static_cast<unsigned int>(textureSets.size() - 1));
}

void VulkanPipeline3D::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding transformationLayoutBinding = {};
    transformationLayoutBinding.binding = 0;
    transformationLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    transformationLayoutBinding.descriptorCount = 1;
    transformationLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    transformationLayoutBinding.pImmutableSamplers = nullptr;

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

    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &samplerLayoutBinding;
    SPARK_CORE_ASSERT(vkCreateDescriptorSetLayout(m_context.m_device, &layoutInfo, nullptr,
                                                  &m_textureDescriptorSetLayout) == VK_SUCCESS,
                      "failed to create descriptor set layout!");
}

void VulkanPipeline3D::createGraphicsPipeline()
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

    std::array<VkDescriptorSetLayout, 2> descriptorSets = {m_transformationDescriptorSetLayout,
                                                           m_textureDescriptorSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSets.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSets.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    SPARK_CORE_ASSERT(vkCreatePipelineLayout(m_context.m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) ==
                          VK_SUCCESS,
                      "failed to create pipeline layout!");

    VulkanPipeline::createGraphicsPipeline(vertexShader, fragmentShader, vertexInputInfo, m_pipelineLayout, true);

    vkDestroyShaderModule(m_context.m_device, vertexShader, VK_NULL_HANDLE);
    vkDestroyShaderModule(m_context.m_device, fragmentShader, VK_NULL_HANDLE);
}
} // namespace Spark
