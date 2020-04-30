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
		virtual void recreate();

		VkRenderPass getRenderPass() const;
		const std::vector<VkFramebuffer> getswapChainFramebuffers() const;
	protected:
		VulkanFramebuffer(VulkanContext& context);
		void createColorResources();

	protected:
		VulkanContext& m_context;
		VkRenderPass m_renderPass;
		std::vector<VkFramebuffer> m_swapChainFramebuffers;
	};
}
