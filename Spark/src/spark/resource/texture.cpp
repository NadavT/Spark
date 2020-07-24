#include "texture.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/resource/texture.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
    Texture::Texture(const std::string& name)
        : Resource(name)
    {

    }

//     std::unique_ptr<Texture> createTexture(const std::string& name, const std::string& path)
//     {
// #ifdef SPARK_PLATFORM_VULKAN
//         VulkanContext& context = reinterpret_cast<VulkanRenderer&>(Application::GetApp().GetRenderer()).getContext();
// 		return std::make_unique<VulkanTexture>(context, name, path);
// #endif // SPARK_PLATFORM_VULKAN
//     }
}
