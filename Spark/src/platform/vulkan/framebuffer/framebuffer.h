#pragma once

#include "platform/vulkan/vulkan_context.h"

namespace Spark
{
	enum class VulkanFramebufferType
	{
		Type2D,
		Type3D
	};

	class VulkanFramebuffer
	{
	public:
		virtual ~VulkanFramebuffer();
		VulkanFramebuffer(const VulkanFramebuffer& other) = delete;
		VulkanFramebuffer& operator=(const VulkanFramebuffer& other) = delete;

		virtual void cleanup();
		virtual void recreate(VkImageView renderTarget);

		VkRenderPass getRenderPass() const;
		const std::vector<VkFramebuffer> getswapChainFramebuffers() const;
	protected:
		VulkanFramebuffer(VulkanContext& context, VkImageView renderTarget, bool clear = false, bool resolve = false);

		VkSubpassDescription getBasicSubpass(std::vector<VkAttachmentDescription>& attachments, std::vector<VkAttachmentReference>& attachmentsRefs);
		VkSubpassDependency getBasicSubpassDependency();
		std::vector<VkImageView> getBasicFramebufferAttachments(VkImageView currentImageView);

	protected:
		bool m_clear;
		bool m_resolve;
		VulkanContext& m_context;
		VkRenderPass m_renderPass;
		std::vector<VkFramebuffer> m_swapChainFramebuffers;
		VkImageView m_renderTarget;
	};
}
