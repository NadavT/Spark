#pragma once

#include "spark/renderer/renderer.h"
#include "spark/core/window.h"
#include "spark/event/application_event.h"

#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/framebuffer/framebuffer.h"

#include <vector>

namespace Spark
{
	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(const Window& window);
		virtual ~VulkanRenderer() = default;

		virtual void end();
		virtual void OnEvent(Event& e);
		VulkanFramebuffer* createFramebuffer(VulkanFramebufferType type);
		void destroyFramebuffer(VulkanFramebuffer* framebuffer);
		void recreateSwapchain();

		void waitForIdle();

		void beginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferBeginInfo* info);
		void endCommandBuffer(VkCommandBuffer commandBuffer);
		void queueSubmit(VkSubmitInfo* info, VkFence fence);
		bool accuireNextImage(VkSemaphore semaphore, VkFence fence, uint32_t* imageIndex);
		void waitForFence(const VkFence* fence);
		void resetFence(const VkFence* fence);
		void resetCommandBuffer(VkCommandBuffer commandBuffer);
		void beginRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, uint32_t clearValueCount, const VkClearValue* clearValues = VK_NULL_HANDLE);
		void endRenderPass(VkCommandBuffer commandBuffer);
		void queuePresent(const VkPresentInfoKHR* info);

	private:
		bool onWindowResize(WindowResizeEvent& e);

	public:
		VulkanContext m_context;
		std::vector<std::unique_ptr<VulkanFramebuffer>> m_framebuffers;
		int m_width;
		int m_height;
	};
}
