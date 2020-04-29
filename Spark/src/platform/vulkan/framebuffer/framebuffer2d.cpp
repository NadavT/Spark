#include "framebuffer2D.h"

#include <array>

namespace Spark
{
	VulkanFramebuffer2D::VulkanFramebuffer2D(VulkanContext& context, bool firstLayer)
		: VulkanFramebuffer(context)
	{
		createRenderPass(firstLayer);
		createColorResources();
		createSwapchainFramebuffers();
	}

	VulkanFramebuffer2D::~VulkanFramebuffer2D()
	{
		vkDestroyImageView(m_context.m_device, m_colorImageView, nullptr);
		vkDestroyImage(m_context.m_device, m_colorImage, nullptr);
		vkFreeMemory(m_context.m_device, m_colorImageMemory, nullptr);
	}

	void VulkanFramebuffer2D::createRenderPass(bool firstLayer)
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = m_context.m_swapChainImageFormat;
		colorAttachment.samples = m_context.m_msaaSamples;
		colorAttachment.loadOp = (firstLayer) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if (m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		else
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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
		if (m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
			subpass.pResolveAttachments = &colorAttachmentResolveRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, colorAttachmentResolve };
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

	void VulkanFramebuffer2D::createColorResources() {
		VkFormat colorFormat = m_context.m_swapChainImageFormat;

		m_context.createImage(m_context.m_swapChainExtent.width, m_context.m_swapChainExtent.height,
			1, m_context.m_msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_colorImage, m_colorImageMemory);
		m_colorImageView = m_context.createImageView(m_colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	void VulkanFramebuffer2D::createSwapchainFramebuffers()
	{
		m_swapChainFramebuffers.resize(m_context.m_swapChainImageViews.size());

		for (size_t i = 0; i < m_context.m_swapChainImageViews.size(); i++) {
			std::array<VkImageView, 2> attachments = {
				m_colorImageView,
				m_context.m_swapChainImageViews[i],
			};

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
