#pragma once

#include "platform/vulkan/framebuffer/framebuffer.h"

namespace Spark
{
	class VulkanFramebuffer2D : public VulkanFramebuffer
	{
	public:
		VulkanFramebuffer2D(VulkanContext& context, bool firstLayer = false, bool lastLayer = false, VkImageView multisampleImageView = VK_NULL_HANDLE);
		virtual ~VulkanFramebuffer2D();

		virtual void cleanup();
		virtual void recreate(VkImageView multisampleImageView = VK_NULL_HANDLE);

	private:
		void createRenderPass(bool firstLayer, bool lastLayer);
		void createSwapchainFramebuffers();
	};
}
