#include "framebuffer2D.h"

#include <array>

namespace Spark
{
	VulkanFramebuffer2D::VulkanFramebuffer2D(VulkanContext& context, VkImageView renderTarget, bool clear, bool resolve)
		: VulkanFramebuffer(context, renderTarget, clear, resolve)
	{
		createRenderPass();
		createSwapchainFramebuffers();
	}

	VulkanFramebuffer2D::~VulkanFramebuffer2D()
	{

	}

	void VulkanFramebuffer2D::cleanup()
	{
		VulkanFramebuffer::cleanup();
	}

	void VulkanFramebuffer2D::recreate(VkImageView renderTarget)
	{
		VulkanFramebuffer::recreate(renderTarget);
;		createRenderPass();
		createSwapchainFramebuffers();
	}

	void VulkanFramebuffer2D::createRenderPass()
	{
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkAttachmentReference> attachmentsRefs;
		VkSubpassDescription subpass = getBasicSubpass(attachments, attachmentsRefs);

		VkSubpassDependency dependency = getBasicSubpassDependency();

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_context.m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void VulkanFramebuffer2D::createSwapchainFramebuffers()
	{
		m_swapChainFramebuffers.resize(m_context.m_swapChainImageViews.size());

		for (size_t i = 0; i < m_context.m_swapChainImageViews.size(); i++) {
			std::vector<VkImageView> attachments = getBasicFramebufferAttachments(m_context.m_swapChainImageViews[i]);

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_context.m_swapChainExtent.width;
			framebufferInfo.height = m_context.m_swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_context.m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}
}
