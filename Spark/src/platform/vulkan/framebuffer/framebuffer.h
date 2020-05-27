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
		virtual void recreate(VkImageView multisampleImageView = VK_NULL_HANDLE);

		VkRenderPass getRenderPass() const;
		const std::vector<VkFramebuffer> getswapChainFramebuffers() const;

		bool isFirstLayer() const;
		bool isLastLayer() const;
	protected:
		VulkanFramebuffer(VulkanContext& context, bool firstLayer = false, bool lastLayer = false, VkImageView multisampleImageView = VK_NULL_HANDLE);

	protected:
		bool m_firstLayer;
		bool m_lastLayer;
		VulkanContext& m_context;
		VkRenderPass m_renderPass;
		std::vector<VkFramebuffer> m_swapChainFramebuffers;
		VkImageView m_multisampleImageView;
	};
}
