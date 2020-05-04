#pragma once

#include "spark/renderer/renderer.h"
#include "spark/core/window.h"
#include "spark/event/application_event.h"

#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/framebuffer/framebuffer.h"

#include <vector>
#include <array>

namespace Spark
{
	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(const Window& window);
		virtual ~VulkanRenderer();

		virtual bool begin();
		virtual void end();

		void render(VkCommandBuffer commandBuffer);
		uint32_t getCurrentImageIndex() const;
		uint32_t getImagesAmount() const;

		virtual void OnEvent(Event& e);
		VulkanFramebuffer* createFramebuffer(VulkanFramebufferType type, bool first_layer=false);
		void destroyFramebuffer(VulkanFramebuffer* framebuffer);
		void recreateSwapchain();

		virtual void waitForIdle();

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
		std::array<std::vector<VkSemaphore>, 2> m_semaphores;
		std::array<VkFence, 2> m_inFlightFences;
		uint32_t m_currentFrame;
		uint32_t m_currentImageIndex;
		bool m_framebufferResized;
		VkCommandBuffer m_lastCommandBuffer;
		VkSemaphore m_lastSemaphore;
		uint32_t m_currentCommandsDrawCount;
	};
}
