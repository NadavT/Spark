#include "framebuffer.h"

namespace Spark
{
	VulkanFramebuffer::VulkanFramebuffer(VulkanContext& context, bool firstLayer, bool lastLayer, VkImageView multisampleImageView)
		: m_context(context)
		, m_renderPass(VK_NULL_HANDLE)
		, m_swapChainFramebuffers()
		, m_firstLayer(firstLayer)
		, m_lastLayer(lastLayer)
		, m_multisampleImageView(multisampleImageView)
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

	void VulkanFramebuffer::recreate(VkImageView multisampleImageView)
	{
		m_multisampleImageView = multisampleImageView;
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
	bool VulkanFramebuffer::isFirstLayer() const
	{
		return m_firstLayer;
	}
	bool VulkanFramebuffer::isLastLayer() const
	{
		return m_lastLayer;
	}
}
