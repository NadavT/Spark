#include "renderer.h"

#ifdef SPARK_PLATFORM_VULKAN
#include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN


namespace Spark
{
	std::unique_ptr<Renderer> Renderer::Create(const Window& window)
	{
#ifdef SPARK_PLATFORM_VULKAN
		return std::make_unique<VulkanRenderer>(window);
#endif // SPARK_PLATFORM_VULKAN
	}
}
