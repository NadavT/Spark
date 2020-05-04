#pragma once

#include "spark/overlay/overlay.h"
#include "spark/event/mouse_event.h"
#include "spark/event/keyboard_event.h"
#include "platform/Windows/window.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/framebuffer/framebuffer.h"

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
		virtual void OnRender();

	private:
		void frameRender();

		bool onMouseButtonPressed(MouseButtonPressedEvent& e);
		bool onMouseButtonReleased(MouseButtonReleasedEvent& e);
		bool onMouseMoved(MouseMovedEvent& e);
		bool onMouseScroll(MouseScrolledEvent& e);
		bool onKeyPressed(KeyPressedEvent& e);
		bool onKeyReleased(KeyReleasedEvent& e);
		bool onKeyTyped(KeyTypedEvent& e);

	private:
		VulkanRenderer& m_renderer;
		VulkanFramebuffer* m_framebuffer;

		std::vector<VkCommandBuffer> m_commandBuffers;

		bool m_showDemoWindow;
		bool m_showAnotherWindow;
	};
}
