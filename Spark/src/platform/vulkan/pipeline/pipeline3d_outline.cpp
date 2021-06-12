#include "pipeline3d_outline.h"

#include "spark/core/log.h"
#include "spark/utils/file.h"

namespace Spark::Render
{
const std::string VERTEX_3D_OUTLINE_SHADER_PATH = "shaders/shader3dOutline_vert.spv";
const std::string FRAGMENT_3D_OUTLINE_SHADER_PATH = "shaders/shader3dOutline_frag.spv";

VulkanPipeline3DOutline::VulkanPipeline3DOutline(VulkanContext &context, VulkanFramebuffer &framebuffer, bool clean)
    : VulkanPipeline(context, framebuffer)
    , m_transformationDescriptorSetLayout(VK_NULL_HANDLE)
    , m_clean(clean)
{
    createDescriptorSetLayout();
    createGraphicsPipeline();
}

VulkanPipeline3DOutline::~VulkanPipeline3DOutline()
{
    cleanup();
}

void VulkanPipeline3DOutline::cleanup()
{
    vkDestroyDescriptorSetLayout(m_context.m_device, m_transformationDescriptorSetLayout, nullptr);
    vkDestroyPipelineLayout(m_context.m_device, m_pipelineLayout, VK_NULL_HANDLE);
    m_pipelineLayout = VK_NULL_HANDLE;
    VulkanPipeline::cleanup();
}

void VulkanPipeline3DOutline::recreate()
{
    VulkanPipeline::recreate();
    createDescriptorSetLayout();
    createGraphicsPipeline();
}

void VulkanPipeline3DOutline::bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet,
                                   struct Vulkan3DOutlinePushConsts pushConsts)
{
    const VkDescriptorSet descriptorSets[] = {transformationSet};
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, descriptorSets, 0,
                            nullptr);
    VulkanPipeline::bind(commandBuffer);
    vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(pushConsts),
                       &pushConsts);
}

VkDescriptorSetLayout VulkanPipeline3DOutline::getMVPDescriptorSetLayout()
{
    return m_transformationDescriptorSetLayout;
}

void VulkanPipeline3DOutline::createTransformationDescriptorSets(
    unsigned int drawablesAmount, std::vector<std::vector<VkDescriptorSet>> &transformationSets,
    std::vector<std::vector<VkBuffer>> transformationUniforms)
{
    allocateDescriptorSets(drawablesAmount, m_transformationDescriptorSetLayout, transformationSets);
    updateBufferDescriptorSets(drawablesAmount, transformationSets, transformationUniforms, sizeof(Transformation3D));
}

std::vector<VkDescriptorSet> VulkanPipeline3DOutline::createTransformationDescriptorSet(
    std::vector<VkBuffer> transformationUniforms)
{
    std::vector<VkDescriptorSet> transformationSet = createDescriptorSet(m_transformationDescriptorSetLayout);

    updateBufferDescriptorSet(transformationSet, transformationUniforms, sizeof(Transformation3D));

    return transformationSet;
}

void VulkanPipeline3DOutline::createSingleTransformationDescriptorSet(
    std::vector<std::vector<VkDescriptorSet>> &transformationSets, std::vector<VkBuffer> transformationUniforms)
{
    addDescriptorSets(m_transformationDescriptorSetLayout, transformationSets);
    updateBufferDescriptorSets(1, transformationSets, {transformationUniforms}, sizeof(Transformation3D),
                               static_cast<unsigned int>(transformationSets.size() - 1));
}

void VulkanPipeline3DOutline::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding transformationLayoutBinding = {};
    transformationLayoutBinding.binding = 0;
    transformationLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    transformationLayoutBinding.descriptorCount = 1;
    transformationLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    transformationLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &transformationLayoutBinding;
    SPARK_CORE_ASSERT(vkCreateDescriptorSetLayout(m_context.m_device, &layoutInfo, nullptr,
                                                  &m_transformationDescriptorSetLayout) == VK_SUCCESS,
                      "failed to create descriptor set layout!");
}

void VulkanPipeline3DOutline::createGraphicsPipeline()
{
    auto vertexShaderCode = readFile(VERTEX_3D_OUTLINE_SHADER_PATH);
    auto fragmentShaderCode = readFile(FRAGMENT_3D_OUTLINE_SHADER_PATH);

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

    std::array<VkDescriptorSetLayout, 1> descriptorSets = {m_transformationDescriptorSetLayout};

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(struct Vulkan3DOutlinePushConsts);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSets.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSets.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    SPARK_CORE_ASSERT(vkCreatePipelineLayout(m_context.m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) ==
                          VK_SUCCESS,
                      "failed to create pipeline layout!");

    VkPipelineRasterizationStateCreateInfo rasterizationState = {};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.polygonMode = (m_clean) ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
    rasterizationState.lineWidth = (m_clean) ? 1.0f : 2.0f;
    rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
    rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    setRasterizationState(rasterizationState);

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_TRUE;
    depthStencil.front.compareOp = (m_clean) ? VK_COMPARE_OP_NEVER : VK_COMPARE_OP_NOT_EQUAL;
    depthStencil.front.failOp = (m_clean) ? VK_STENCIL_OP_ZERO : VK_STENCIL_OP_KEEP;
    depthStencil.front.passOp = (m_clean) ? VK_STENCIL_OP_ZERO : VK_STENCIL_OP_REPLACE;
    depthStencil.front.compareMask = 0x02;
    depthStencil.front.writeMask = (m_clean) ? 0x03 : 0x01;
    depthStencil.front.reference = 3;
    depthStencil.back = depthStencil.front;
    setDepthStencilState(depthStencil);

    VulkanPipeline::createGraphicsPipeline(vertexShader, fragmentShader, vertexInputInfo, m_pipelineLayout, true);

    vkDestroyShaderModule(m_context.m_device, vertexShader, VK_NULL_HANDLE);
    vkDestroyShaderModule(m_context.m_device, fragmentShader, VK_NULL_HANDLE);
}

} // namespace Spark::Render
