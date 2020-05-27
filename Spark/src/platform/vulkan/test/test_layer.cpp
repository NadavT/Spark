#include "test_layer.h"

#include "platform/vulkan/pipeline/pipeline2d.h"

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
		m_framebuffer = renderer.createFramebuffer(VulkanFramebufferType::Type2D, true, true);
		m_pipeline = reinterpret_cast<VulkanPipeline2D*>(renderer.createPipeline(VulkanPipelineType::Type2D, *m_framebuffer));
		m_commandBuffers.resize(renderer.getImagesAmount());
		for (int i = 0; i < m_commandBuffers.size(); i++)
		{
			m_commandBuffers[i] = renderer.m_context.createCommandBuffer();
		}
		renderer.createUniformBuffers(sizeof(MVP), m_uniformBuffers, m_uniformBuffersMemory);
		createDescriptorSets();
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
		createCommandBuffers();
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
		if (m_renderer.isRecreationNeeded())
		{
			for (VkCommandBuffer commandBuffer : m_commandBuffers) {
				m_renderer.resetCommandBuffer(commandBuffer);
			}
			createCommandBuffers();
		}

		VkCommandBuffer commandBuffer = m_commandBuffers[m_renderer.getCurrentImageIndex()];
		m_renderer.render(commandBuffer);
	}

	void VulkanTestLayer::createDescriptorSets()
	{
		m_MVPDescriptorSets.resize(m_renderer.m_context.m_swapChainImages.size());

		std::vector<VkDescriptorSetLayout> MVPLayouts(m_renderer.m_context.m_swapChainImages.size(), m_pipeline->getMVPDescriptorSetLayout());
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_renderer.m_context.m_descriptorPool;
		allocInfo.descriptorSetCount = m_MVPDescriptorSets.size();
		allocInfo.pSetLayouts = MVPLayouts.data();

		if (vkAllocateDescriptorSets(m_renderer.m_context.m_device, &allocInfo, m_MVPDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < m_renderer.m_context.m_swapChainImages.size(); i++) {
			std::vector<VkWriteDescriptorSet> descriptorWrites = {};

			VkWriteDescriptorSet writeDescripotrSet = {};
			VkDescriptorBufferInfo bufferInfo;

			bufferInfo.buffer = m_uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(MVP);
			writeDescripotrSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescripotrSet.dstSet = m_MVPDescriptorSets[i];
			writeDescripotrSet.dstBinding = 0;
			writeDescripotrSet.dstArrayElement = 0;
			writeDescripotrSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescripotrSet.descriptorCount = 1;
			writeDescripotrSet.pBufferInfo = &bufferInfo;
			descriptorWrites.push_back(writeDescripotrSet);

			vkUpdateDescriptorSets(m_renderer.m_context.m_device, static_cast<uint32_t>(descriptorWrites.size()),
				descriptorWrites.data(), 0, nullptr);
		}
	}

	void VulkanTestLayer::createCommandBuffers()
	{
		void* data;

		MVP mvp = {};
		mvp.model = m_quad->getModelMatrix();

		int i = 0;
		for (VkCommandBuffer commandBuffer : m_commandBuffers) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			m_renderer.beginRenderPass(commandBuffer, m_framebuffer->getRenderPass(),
				m_framebuffer->getswapChainFramebuffers()[i], 2, clearValues.data());

			const VkDescriptorSet descriptorSets[] = { m_MVPDescriptorSets[m_renderer.getCurrentImageIndex()] };
			//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getLayout(), 0, 1, descriptorSets, 0, nullptr);
			m_pipeline->bind(commandBuffer);

			vkCmdDraw(commandBuffer, 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffer);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}

			//vkMapMemory(m_renderer.m_context.m_device, m_uniformBuffersMemory[i], 0, sizeof(mvp), 0, &data);
			//memcpy(data, &mvp, sizeof(mvp));
			//vkUnmapMemory(m_renderer.m_context.m_device, m_uniformBuffersMemory[i]);

			i++;
		}

	}
}
