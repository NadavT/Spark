#include "test_layer.h"

namespace Spark
{
	struct MVP {
		glm::mat3 model;
	};

	VulkanTestLayer::VulkanTestLayer(VulkanRenderer& renderer)
		: Layer("test layer")
		, m_renderer(renderer)
		, m_framebuffer(nullptr)
		, m_pipeline(nullptr)
		, m_quad(nullptr)
	{
		m_framebuffer = renderer.createFramebuffer(VulkanFramebufferType::Type2D, true);
		m_pipeline = renderer.createPipeline(VulkanPipelineType::Type2D, *m_framebuffer);
		m_commandBuffers.resize(renderer.getImagesAmount());
		for (int i = 0; i < m_commandBuffers.size(); i++)
		{
			m_commandBuffers[i] = renderer.m_context.createCommandBuffer();
		}
		renderer.createUniformBuffers(sizeof(MVP), m_uniformBuffers, m_uniformBuffersMemory);
	}

	VulkanTestLayer::~VulkanTestLayer()
	{
		for (size_t i = 0; i < m_renderer.m_context.m_swapChainImages.size(); i++) {
			vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformBuffers[i], nullptr);
			vkFreeMemory(m_renderer.m_context.m_device, m_uniformBuffersMemory[i], nullptr);
		}

		for (int i = 0; i < m_commandBuffers.size(); i++)
		{
			m_renderer.m_context.destroyCommandBuffer(m_commandBuffers[i]);
		}
		m_renderer.destroyPipeline(m_pipeline);
		m_pipeline = nullptr;
		m_renderer.destroyFramebuffer(m_framebuffer);
		m_framebuffer = nullptr;
	}

	void VulkanTestLayer::OnAttach()
	{
		m_quad = std::make_unique<Quad>(m_renderer.m_context, glm::vec2(100.0f, 100.0f));

		for (VkCommandBuffer commandBuffer : m_commandBuffers) {
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			m_renderer.beginCommandBuffer(commandBuffer, &info);
			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			m_renderer.beginRenderPass(commandBuffer, m_framebuffer->getRenderPass(),
				m_framebuffer->getswapChainFramebuffers()[m_renderer.getCurrentImageIndex()], 2, clearValues.data());

			int mvpDescriptorSetOffset = 0;
			const VkDescriptorSet descriptorSets[] = { MVPDescriptorSets[mvpDescriptorSetOffset][i], lightDescriptorSets[i], textureDescriptorSets[i] };
			vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext->pipelineLayout[0], 0, 3, descriptorSets, 0, nullptr);
			m_pipeline->bind(commandBuffer);

			// Record Imgui Draw Data and draw funcs into command buffer
			m_quad->fillCommandBuffer(commandBuffer);

			// Submit command buffer
			vkCmdEndRenderPass(commandBuffer);
			m_renderer.endCommandBuffer(commandBuffer);
		}
	}

	void VulkanTestLayer::OnDetach()
	{
		for (VkCommandBuffer commandBuffer : m_commandBuffers) {
			m_renderer.resetCommandBuffer(commandBuffer);
		}
		m_quad.reset(nullptr);
	}

	void VulkanTestLayer::OnRender()
	{
		m_renderer.render(m_commandBuffers[m_renderer.getCurrentImageIndex()]);
	}

	void VulkanTestLayer::createDescriptorSets()
	{
		VkDescriptorSetLayout MVPLayout;
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_renderer.m_context.m_descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &MVPLayout;

		if (vkAllocateDescriptorSets(m_renderer.m_context.m_device, &allocInfo, &m_MVPDescriptorSet) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		VkDescriptorSetLayout textureLayout;
		allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_renderer.m_context.m_descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &textureLayout;

		if (vkAllocateDescriptorSets(m_renderer.m_context.m_device, &allocInfo, &m_textureDescriptorSet) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < m_renderer.m_context.m_swapChainImages.size(); i++) {
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = textureImageView;
			imageInfo.sampler = textureSampler;

			std::vector<VkWriteDescriptorSet> descriptorWrites = {};

			VkWriteDescriptorSet writeDescripotrSet = {};
			VkDescriptorBufferInfo bufferInfo;

			bufferInfo.buffer = m_uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(MVP);
			writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescripotrSet.dstSet = m_MVPDescriptorSet;
			writeDescripotrSet.dstBinding = 0;
			writeDescripotrSet.dstArrayElement = 0;
			writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescripotrSet.descriptorCount = 1;
			writeDescripotrSet.pBufferInfo = &bufferInfo[j];
			descriptorWrites.push_back(writeDescripotrSet);

			writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescripotrSet.dstSet = m_textureDescriptorSet;
			writeDescripotrSet.dstBinding = 0;
			writeDescripotrSet.dstArrayElement = 0;
			writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescripotrSet.descriptorCount = 1;
			writeDescripotrSet.pImageInfo = &imageInfo;
			descriptorWrites.push_back(writeDescripotrSet);

			vkUpdateDescriptorSets(m_renderer.m_context.m_device, static_cast<uint32_t>(descriptorWrites.size()),
				descriptorWrites.data(), 0, nullptr);
		}
	}
}
