#include "test_layer.h"

namespace Spark
{
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
	}

	VulkanTestLayer::~VulkanTestLayer()
	{
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
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext->graphicsPipelines[0]);

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
}
