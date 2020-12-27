#include "quad.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/quad.h"
    #include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Quad::Quad(glm::vec2 position, glm::vec2 scale)
    : Drawable2D(position, scale)
{
}

std::shared_ptr<Drawable> createQuad(glm::vec2 position, const Texture &texture, glm::vec2 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    const VulkanTexture &vulkanTexture = reinterpret_cast<const VulkanTexture &>(texture);
    return std::make_shared<VulkanQuad>(renderer, position, vulkanTexture, scale);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark
