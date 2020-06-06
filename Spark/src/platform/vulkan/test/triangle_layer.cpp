#include "triangle_layer.h"

#include "platform/vulkan/pipeline/pipeline2d.h"

namespace Spark
{
	VulkanTriangleLayer::VulkanTriangleLayer(VulkanRenderer& renderer)
		: Layer("triangle layer")
		, m_renderer(renderer)
		, m_framebuffer(nullptr)
		, m_pipeline(nullptr)
	{
		m_framebuffer = renderer.createFramebuffer(VulkanFramebufferType::Type2D, true, true);
		m_pipeline = renderer.createPipeline(VulkanPipelineType::TypeTriangle, *m_framebuffer);
		m_commandBuffers.resize(renderer.getImagesAmount());
		for (int i = 0; i < m_commandBuffers.size(); i++)
		{
			m_commandBuffers[i] = renderer.m_context.createCommandBuffer();
		}
	}

	VulkanTriangleLayer::~VulkanTriangleLayer()
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

	void VulkanTriangleLayer::OnAttach()
	{
		createCommandBuffers();
	}

	void VulkanTriangleLayer::OnDetach()
	{
		for (VkCommandBuffer commandBuffer : m_commandBuffers) {
			m_renderer.resetCommandBuffer(commandBuffer);
		}
	}

	void VulkanTriangleLayer::OnRender()
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

	void VulkanTriangleLayer::createCommandBuffers()
	{
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

			m_pipeline->bind(commandBuffer);

			vkCmdDraw(commandBuffer, 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffer);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
			i++;
		}

	}
}
