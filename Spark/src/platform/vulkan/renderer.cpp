#include "renderer.h"

#include "overlay/overlay.h"

namespace Spark
{
	VulkanRenderer::VulkanRenderer(const Window& window)
		: m_context(window)
	{
		VulkanOverlay o(*this, reinterpret_cast<const WindowsWindow &>(window));
	}
}