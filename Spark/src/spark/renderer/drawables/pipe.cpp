#include "pipe.h"

#include "spark/core/application.h"
#include "spark/utils/math.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/colored_pipe.h"
    #include "platform/vulkan/drawables/textured_pipe.h"
    #include "platform/vulkan/renderer.h"
    #include "platform/vulkan/resource/texture.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark::Render
{

Pipe::Pipe(std::vector<glm::vec3> positions, float radius, int sectors, glm::vec3 scale)
    : Drawable3D(mid_range(positions), scale)
    , m_positions(positions)
    , m_radius(radius)
    , m_sectors(sectors)
{
}

std::unique_ptr<Drawable3D> createPipe(std::vector<glm::vec3> positions, float radius, const Texture &texture,
                                       const Texture &specularTexture, int sectors, glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    const VulkanTexture &vulkanTexture = reinterpret_cast<const VulkanTexture &>(texture);
    const VulkanTexture &vulkanSpecularTexture = reinterpret_cast<const VulkanTexture &>(specularTexture);
    return std::make_unique<VulkanTexturedPipe>(renderer, positions, radius, sectors, vulkanTexture,
                                                vulkanSpecularTexture, scale);
#endif // SPARK_PLATFORM_VULKAN
}

std::unique_ptr<Drawable3D> createPipe(std::vector<glm::vec3> positions, float radius, glm::vec3 color, int sectors,
                                       glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    return std::make_unique<VulkanColoredPipe>(renderer, positions, radius, sectors, color, scale);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark::Render
