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
		, m_transformationDescriptorSetLayout(VK_NULL_HANDLE)
		, m_textureDescriptorSetLayout(VK_NULL_HANDLE)
	{
		createDescriptorSetLayout();
		createGraphicsPipeline();
	}

	VulkanPipeline2D::~VulkanPipeline2D()
	{
		cleanup();
	}

	void VulkanPipeline2D::cleanup()
	{
		vkDestroyDescriptorSetLayout(m_context.m_device, m_transformationDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_context.m_device, m_textureDescriptorSetLayout, nullptr);
		vkDestroyPipelineLayout(m_context.m_device, m_pipelineLayout, VK_NULL_HANDLE);
		m_pipelineLayout = VK_NULL_HANDLE;
		VulkanPipeline::cleanup();
	}

	void VulkanPipeline2D::recreate()
	{
		VulkanPipeline::recreate();
		createDescriptorSetLayout();
		createGraphicsPipeline();
	}

	void VulkanPipeline2D::bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet, VkDescriptorSet textureSet)
	{
		const VkDescriptorSet descriptorSets[] = { transformationSet, textureSet };
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 2, descriptorSets, 0, nullptr);
		VulkanPipeline::bind(commandBuffer);
	}

	VkDescriptorSetLayout VulkanPipeline2D::getMVPDescriptorSetLayout()
	{
		return m_transformationDescriptorSetLayout;
	}

	VkDescriptorSetLayout VulkanPipeline2D::getTextureDescriptorSetLayout()
	{
		return m_textureDescriptorSetLayout;
	}

	void VulkanPipeline2D::createDescriptorSets(std::vector<VkDescriptorSet>& transformationSets, std::vector<VkBuffer> transformationUniforms,
		std::vector<VkDescriptorSet>& textureSets, VkImageView textureImageView, VkSampler textureSampler)
	{
		std::vector<VkDescriptorSetLayout> transformationLayouts(m_context.m_swapChainImages.size(), m_transformationDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_context.m_descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(m_context.m_swapChainImages.size());
		allocInfo.pSetLayouts = transformationLayouts.data();
		transformationSets.resize(m_context.m_swapChainImages.size());
		SPARK_CORE_ASSERT(vkAllocateDescriptorSets(m_context.m_device, &allocInfo, transformationSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets");

		std::vector<VkDescriptorSetLayout> textureLayouts(m_context.m_swapChainImages.size(), m_textureDescriptorSetLayout);
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_context.m_descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(m_context.m_swapChainImages.size());
		allocInfo.pSetLayouts = textureLayouts.data();
		textureSets.resize(m_context.m_swapChainImages.size());
		SPARK_CORE_ASSERT(vkAllocateDescriptorSets(m_context.m_device, &allocInfo, textureSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets");

		std::vector<VkDescriptorBufferInfo> bufferInfos;
		bufferInfos.resize(m_context.m_swapChainImages.size());

		for (size_t i = 0; i < m_context.m_swapChainImages.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = transformationUniforms[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(Transformation2D);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = textureImageView;
			imageInfo.sampler = textureSampler;

			std::vector<VkWriteDescriptorSet> descriptorWrites = {};
			VkWriteDescriptorSet writeDescripotrSet = {};

			writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescripotrSet.dstSet = transformationSets[i];
			writeDescripotrSet.dstBinding = 0;
			writeDescripotrSet.dstArrayElement = 0;
			writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescripotrSet.descriptorCount = 1;
			writeDescripotrSet.pBufferInfo = &bufferInfo;
			descriptorWrites.push_back(writeDescripotrSet);

			writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescripotrSet.dstSet = textureSets[i];
			writeDescripotrSet.dstBinding = 0;
			writeDescripotrSet.dstArrayElement = 0;
			writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescripotrSet.descriptorCount = 1;
			writeDescripotrSet.pImageInfo = &imageInfo;
			descriptorWrites.push_back(writeDescripotrSet);

			vkUpdateDescriptorSets(m_context.m_device, static_cast<uint32_t>(descriptorWrites.size()),
				descriptorWrites.data(), 0, nullptr);
		}
	}

	void VulkanPipeline2D::createDescriptorSetLayout()
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
		SPARK_CORE_ASSERT(vkCreateDescriptorSetLayout(m_context.m_device, &layoutInfo, nullptr, &m_transformationDescriptorSetLayout) == VK_SUCCESS, "failed to create descriptor set layout!");

		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &samplerLayoutBinding;
		SPARK_CORE_ASSERT(vkCreateDescriptorSetLayout(m_context.m_device, &layoutInfo, nullptr, &m_textureDescriptorSetLayout) == VK_SUCCESS, "failed to create descriptor set layout!");
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

		std::array<VkDescriptorSetLayout, 2> descriptorSets = { m_transformationDescriptorSetLayout, m_textureDescriptorSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSets.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSets.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		SPARK_CORE_ASSERT(vkCreatePipelineLayout(m_context.m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) == VK_SUCCESS, "failed to create pipeline layout!");

		VulkanPipeline::createGraphicsPipeline(vertexShader, fragmentShader, vertexInputInfo, m_pipelineLayout);

		vkDestroyShaderModule(m_context.m_device, vertexShader, VK_NULL_HANDLE);
		vkDestroyShaderModule(m_context.m_device, fragmentShader, VK_NULL_HANDLE);

	}
}
