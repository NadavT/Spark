#include "overlay.h"

#ifdef SPARK_PLATFORM_VULKAN
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/overlay/overlay.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
	Overlay::Overlay() 
		:Layer("Overlay")
	{

	};

	std::unique_ptr<Overlay> Spark::Overlay::Create(Renderer& renderer)
	{
#ifdef SPARK_PLATFORM_VULKAN
		return std::make_unique<VulkanOverlay>(reinterpret_cast<VulkanRenderer&>(renderer));
#else
#error "Currently only supporting Vulkan."
#endif // SPARK_PLATFORM_VULKAN
	}
}
