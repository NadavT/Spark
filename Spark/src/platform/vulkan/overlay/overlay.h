#pragma once

#include "spark/overlay/overlay.h"
#include "platform/Windows/window.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/framebuffer/framebuffer2D.h"

#include <vector>

namespace Spark
{
	class VulkanOverlay : public Overlay
	{
	public:
		VulkanOverlay(VulkanRenderer& renderer, const WindowsWindow& window);
		virtual ~VulkanOverlay() = default;

	private:
		void frameRender();
		void framePresent();

	private:
		VulkanRenderer& m_renderer;
		VulkanFramebuffer2D m_framebuffer;

		std::vector<VkSemaphore> m_imageAvailableSemaphores;
		std::vector<VkSemaphore> m_renderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences;
		VkCommandBuffer m_commandBuffer;
		uint32_t m_currentFrame = 0;
		VkClearValue m_clearValue;
	};
}
