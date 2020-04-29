#pragma once

#include "spark/overlay/overlay.h"
#include "spark/event/mouse_event.h"
#include "spark/event/keyboard_event.h"
#include "platform/Windows/window.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/framebuffer/framebuffer2D.h"

#include <vector>

namespace Spark
{
	class VulkanOverlay : public Overlay
	{
	public:
		VulkanOverlay(VulkanRenderer& renderer);
		virtual ~VulkanOverlay();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate(Time& diffTime);
		virtual void OnEvent(Event& e);

	private:
		void frameRender();
		void framePresent();

		bool onMouseButtonPressed(MouseButtonPressedEvent& e);
		bool onMouseButtonReleased(MouseButtonReleasedEvent& e);
		bool onMouseMoved(MouseMovedEvent& e);
		bool onMouseScroll(MouseScrolledEvent& e);
		bool onKeyPressed(KeyPressedEvent& e);
		bool onKeyReleased(KeyReleasedEvent& e);
		bool onKeyTyped(KeyTypedEvent& e);

	private:
		VulkanRenderer& m_renderer;
		VulkanFramebuffer2D m_framebuffer;

		std::vector<VkSemaphore> m_imageAvailableSemaphores;
		std::vector<VkSemaphore> m_renderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences;
		VkCommandBuffer m_commandBuffer;
		uint32_t m_currentFrame = 0;

		bool m_showDemoWindow;
		bool m_showAnotherWindow;
	};
}
