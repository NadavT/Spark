#pragma once

#include "spark/overlay/overlay.h"
#include "examples/imgui_impl_vulkan.h"

namespace Spark
{
	class VulkanOverlay : public Overlay
	{
	public:
		VulkanOverlay();
		virtual ~VulkanOverlay() = default;
	};
}
