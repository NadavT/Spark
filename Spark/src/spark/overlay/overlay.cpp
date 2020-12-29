#include "overlay.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/overlay/overlay.h"
    #include "platform/vulkan/renderer.h"

#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Overlay::Overlay()
    : Layer("Overlay"){

      };

std::unique_ptr<Overlay> Spark::Overlay::Create(Render::Renderer &renderer)
{
#ifdef SPARK_PLATFORM_VULKAN
    return std::make_unique<Render::VulkanOverlay>(reinterpret_cast<Render::VulkanRenderer &>(renderer));
#else
    #error "Currently only supporting Vulkan."
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark
