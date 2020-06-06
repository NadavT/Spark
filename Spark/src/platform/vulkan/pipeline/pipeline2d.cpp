#include "pipeline2d.h"

#include "platform/vulkan/vertex/vertex2d.h"
#include "spark/utils/file.h"
#include "spark/core/log.h"

namespace Spark
{
	const std::string VERTEX_2D_SHADER_PATH = "shaders/shader2d_vert.spv";
	const std::string FRAGMENT_2D_SHADER_PATH = "shaders/shader2d_frag.spv";

	VulkanPipeline2D::VulkanPipeline2D(VulkanContext& context, VulkanFramebuffer& framebuffer)
		: VulkanPipeline(context, framebuffer)
		, m_MVPDescriptorSetLayout(VK_NULL_HANDLE)
		, m_textureDescriptorSetLayout(VK_NULL_HANDLE)
	{
		//createDescriptorSetLayout();
		createGraphicsPipeline();
	}

	VulkanPipeline2D::~VulkanPipeline2D()
	{
		//vkDestroyDescriptorSetLayout(m_context.m_device, m_MVPDescriptorSetLayout, nullptr);
		//vkDestroyDescriptorSetLayout(m_context.m_device, m_textureDescriptorSetLayout, nullptr);
		cleanup();
	}

	void VulkanPipeline2D::cleanup()
	{
		//vkDestroyDescriptorSetLayout(m_context.m_device, m_MVPDescriptorSetLayout, nullptr);
		//vkDestroyDescriptorSetLayout(m_context.m_device, m_textureDescriptorSetLayout, nullptr);
		vkDestroyPipelineLayout(m_context.m_device, m_pipelineLayout, VK_NULL_HANDLE);
		m_pipelineLayout = VK_NULL_HANDLE;
		VulkanPipeline::cleanup();
	}

	void VulkanPipeline2D::recreate()
	{
		VulkanPipeline::recreate();
		//createDescriptorSetLayout();
		createGraphicsPipeline();
	}

	void VulkanPipeline2D::bind(VkCommandBuffer commandBuffer)
	{
		VulkanPipeline::bind(commandBuffer);
	}

	VkDescriptorSetLayout VulkanPipeline2D::getMVPDescriptorSetLayout()
	{
		return m_MVPDescriptorSetLayout;
	}

	VkDescriptorSetLayout VulkanPipeline2D::getTextureDescriptorSetLayout()
	{
		return m_textureDescriptorSetLayout;
	}

	void VulkanPipeline2D::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
	
		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 0;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;
		if (vkCreateDescriptorSetLayout(m_context.m_device, &layoutInfo, nullptr, &m_MVPDescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &samplerLayoutBinding;
		if (vkCreateDescriptorSetLayout(m_context.m_device, &layoutInfo, nullptr, &m_textureDescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void VulkanPipeline2D::createGraphicsPipeline()
	{
		auto vertexShaderCode = readFile(VERTEX_2D_SHADER_PATH);
		auto fragmentShaderCode = readFile(FRAGMENT_2D_SHADER_PATH);

		VkShaderModule vertexShader = createShaderModule(vertexShaderCode);
		VkShaderModule fragmentShader = createShaderModule(fragmentShaderCode);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		auto bindingDescription = Vertex2D::getBindingDescription();
		auto attributeDescriptions = Vertex2D::getAttributeDescriptions();

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		SPARK_CORE_ASSERT(vkCreatePipelineLayout(m_context.m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) == VK_SUCCESS, "failed to create pipeline layout!");

		VulkanPipeline::createGraphicsPipeline(vertexShader, fragmentShader, vertexInputInfo, m_pipelineLayout);

		vkDestroyShaderModule(m_context.m_device, vertexShader, VK_NULL_HANDLE);
		vkDestroyShaderModule(m_context.m_device, fragmentShader, VK_NULL_HANDLE);

	}
}
