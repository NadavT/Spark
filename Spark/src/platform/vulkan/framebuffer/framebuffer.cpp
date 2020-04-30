#include "framebuffer.h"

namespace Spark
{
	VulkanFramebuffer::VulkanFramebuffer(VulkanContext& context)
		: m_context(context)
		, m_renderPass(VK_NULL_HANDLE)
		, m_swapChainFramebuffers()
	{

	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		cleanup();
	}

	void VulkanFramebuffer::cleanup()
	{
		for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++) {
			vkDestroyFramebuffer(m_context.m_device, m_swapChainFramebuffers[i], nullptr);
		}

		vkDestroyRenderPass(m_context.m_device, m_renderPass, nullptr);
	}

	void VulkanFramebuffer::recreate()
	{
		m_renderPass = VK_NULL_HANDLE;
		m_swapChainFramebuffers.resize(0);
	}

	VkRenderPass VulkanFramebuffer::getRenderPass() const
	{
		return m_renderPass;
	}

	const std::vector<VkFramebuffer> VulkanFramebuffer::getswapChainFramebuffers() const
	{
		return m_swapChainFramebuffers;
	}
}
