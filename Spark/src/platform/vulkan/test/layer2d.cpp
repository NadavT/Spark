#include "layer2d.h"

#include "platform/vulkan/pipeline/pipeline2d.h"

namespace Spark
{
	VulkanLayer2D::VulkanLayer2D(VulkanRenderer& renderer)
		: Layer("2d layer")
		, m_renderer(renderer)
		, m_framebuffer(nullptr)
		, m_pipeline(nullptr)
	{
		m_framebuffer = renderer.createFramebuffer(VulkanFramebufferType::Type2D);
		m_pipeline = renderer.createPipeline(VulkanPipelineType::Type2D, *m_framebuffer);
		m_commandBuffers.resize(renderer.getImagesAmount());
		m_quad = std::make_unique<Quad>(m_renderer.m_context, glm::vec2(5,5));
		for (int i = 0; i < m_commandBuffers.size(); i++)
		{
			m_commandBuffers[i] = renderer.m_context.createCommandBuffer();
		}
	}

	VulkanLayer2D::~VulkanLayer2D()
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

	void VulkanLayer2D::OnAttach()
	{
		createCommandBuffers();
	}

	void VulkanLayer2D::OnDetach()
	{
		for (VkCommandBuffer commandBuffer : m_commandBuffers) {
			m_renderer.resetCommandBuffer(commandBuffer);
		}
	}

	void VulkanLayer2D::OnRender()
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

	void VulkanLayer2D::createCommandBuffers()
	{
		int i = 0;
		for (VkCommandBuffer commandBuffer : m_commandBuffers) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			m_renderer.beginRenderPass(commandBuffer, m_framebuffer->getRenderPass(),
				m_framebuffer->getswapChainFramebuffers()[i]);

			m_pipeline->bind(commandBuffer);

			m_quad->fillCommandBuffer(commandBuffer);

			vkCmdEndRenderPass(commandBuffer);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
			i++;
		}

	}
}
