#include "texture.h"
#include "resource_manager.h"

#include "spark/core/application.h"
#include "spark/core/log.h"

#ifdef SPARK_PLATFORM_VULKAN
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/resource/texture.h"
#include "platform/vulkan/resource/texture_image.h"
#include "platform/vulkan/resource/texture_sampler.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Texture::Texture(const std::string &name)
    : Resource(name, ResourceType::texture)
{
}

std::unique_ptr<Texture> createTexture(const std::string &name, const std::string &path)
{
#ifdef SPARK_PLATFORM_VULKAN
    SPARK_CORE_ASSERT(ResourceManager::getResource(name) == nullptr,
                      "A resource with the name \"" + name + "\" is already loaded");
    VulkanContext &context = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer()).getContext();

    if (ResourceManager::getResource("SPARK_CORE:VulkanImageSampler") == nullptr)
    {
        ResourceManager::loadResource(std::make_unique<VulkanTextureSampler>(context, "SPARK_CORE:VulkanImageSampler"));
    }

    SPARK_CORE_ASSERT(ResourceManager::getResource("SPARK_CORE:VulkanImage:" + name) == nullptr,
                      "A resource with the name \"" + name + "\" is already loaded");
    ResourceManager::loadResource(
        std::make_unique<VulkanTextureImage>(context, "SPARK_CORE:VulkanImage:" + name, path));

    VulkanTextureImage *texture_image =
        reinterpret_cast<VulkanTextureImage *>(ResourceManager::getResource("SPARK_CORE:VulkanImage:" + name));
    VulkanTextureSampler *sampler =
        reinterpret_cast<VulkanTextureSampler *>(ResourceManager::getResource("SPARK_CORE:VulkanImageSampler"));
    return std::make_unique<VulkanTexture>(name, *texture_image, *sampler);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark
