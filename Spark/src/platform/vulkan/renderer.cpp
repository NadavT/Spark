#include "renderer.h"

namespace Spark
{
	VulkanRenderer::VulkanRenderer(const Window& window)
		: m_context(window)
	{

	}

	void VulkanRenderer::finalize()
	{
		vkDeviceWaitIdle(m_context.m_device);
	}
}