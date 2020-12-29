#include "pipeline_triangle.h"

#include "spark/core/log.h"
#include "spark/utils/file.h"

namespace Spark::Render
{
const std::string TRIANGLE_VERTEX_SHADER_PATH = "shaders/triangle_vert.spv";
const std::string TRIANGLE_FRAGMENT_SHADER_PATH = "shaders/triangle_frag.spv";

VulkanPipelineTriangle::VulkanPipelineTriangle(VulkanContext &context, VulkanFramebuffer &framebuffer)
    : VulkanPipeline(context, framebuffer)
    , m_pipelineLayout(VK_NULL_HANDLE)
{
    recreate();
}

VulkanPipelineTriangle::~VulkanPipelineTriangle()
{
    cleanup();
}

void VulkanPipelineTriangle::cleanup()
{
    vkDestroyPipelineLayout(m_context.m_device, m_pipelineLayout, VK_NULL_HANDLE);
    m_pipelineLayout = VK_NULL_HANDLE;
    VulkanPipeline::cleanup();
}

void VulkanPipelineTriangle::recreate()
{
    VulkanPipeline::recreate();
    auto vertexShaderCode = readFile(TRIANGLE_VERTEX_SHADER_PATH);
    auto fragmentShaderCode = readFile(TRIANGLE_FRAGMENT_SHADER_PATH);

    VkShaderModule vertexShader = createShaderModule(vertexShaderCode);
    VkShaderModule fragmentShader = createShaderModule(fragmentShaderCode);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    SPARK_CORE_ASSERT(vkCreatePipelineLayout(m_context.m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) ==
                          VK_SUCCESS,
                      "failed to create pipeline layout!");

    createGraphicsPipeline(vertexShader, fragmentShader, vertexInputInfo, m_pipelineLayout);

    vkDestroyShaderModule(m_context.m_device, vertexShader, VK_NULL_HANDLE);
    vkDestroyShaderModule(m_context.m_device, fragmentShader, VK_NULL_HANDLE);
}
} // namespace Spark::Render
