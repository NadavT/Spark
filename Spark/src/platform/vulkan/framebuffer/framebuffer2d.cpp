#include "framebuffer2D.h"

#include <array>

namespace Spark
{
	VulkanFramebuffer2D::VulkanFramebuffer2D(VulkanContext& context, bool firstLayer, bool lastLayer, VkImageView multisampleImageView)
		: VulkanFramebuffer(context, firstLayer, lastLayer, multisampleImageView)
	{
		createRenderPass(firstLayer, lastLayer);
		createSwapchainFramebuffers();
	}

	VulkanFramebuffer2D::~VulkanFramebuffer2D()
	{

	}

	void VulkanFramebuffer2D::cleanup()
	{
		VulkanFramebuffer::cleanup();
	}

	void VulkanFramebuffer2D::recreate(VkImageView multisampleImageView)
	{
		VulkanFramebuffer::recreate(multisampleImageView);
;		createRenderPass(m_firstLayer, m_lastLayer);
		createSwapchainFramebuffers();
	}

	void VulkanFramebuffer2D::createRenderPass(bool firstLayer, bool lastLayer)
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = m_context.m_swapChainImageFormat;
		colorAttachment.samples = m_context.m_msaaSamples;
		colorAttachment.loadOp = (firstLayer) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		if (m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
		{
			colorAttachment.initialLayout = (firstLayer) ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		else
		{
			colorAttachment.initialLayout = (firstLayer) ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}


		VkAttachmentDescription colorAttachmentResolve = {};
		colorAttachmentResolve.format = m_context.m_swapChainImageFormat;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentResolveRef = {};
		colorAttachmentResolveRef.attachment = 1;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		if (m_lastLayer && m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
		{
			subpass.pResolveAttachments = &colorAttachmentResolveRef;
		}

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, colorAttachmentResolve };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		if (m_lastLayer && m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
		{
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderPassInfo.pAttachments = attachments.data();
		}
		else
		{
			renderPassInfo.attachmentCount = 1;
			renderPassInfo.pAttachments = &colorAttachment;
		}
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
			std::vector<VkImageView> attachments;
			if (m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
			{
				attachments.push_back(m_multisampleImageView);
				if (m_lastLayer)
				{
					attachments.push_back(m_context.m_swapChainImageViews[i]);
				}
			}
			else
			{
				attachments.push_back(m_context.m_swapChainImageViews[i]);
			}

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
