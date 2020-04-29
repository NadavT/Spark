#pragma once

#include "spark/renderer/renderer.h"
#include "spark/core/window.h"

#include "vulkan_context.h"

namespace Spark
{
	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(const Window& window);
		virtual ~VulkanRenderer() = default;

		virtual void finalize();

	public:
		VulkanContext m_context;
	};
}
