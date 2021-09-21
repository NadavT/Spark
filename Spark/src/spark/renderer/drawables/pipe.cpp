#include "pipe.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/renderer.h"
    #include "platform/vulkan/resource/texture.h"

#endif // SPARK_PLATFORM_VULKAN

#include <algorithm>

namespace Spark::Render
{
static inline glm::vec3 mid_range(std::vector<glm::vec3> vectors)
{
    std::vector<float> x(vectors.size(), 0), y(vectors.size(), 0), z(vectors.size(), 0);
    std::transform(vectors.begin(), vectors.end(), x.begin(), [](glm::vec3 &v) { return v.x; });
    std::transform(vectors.begin(), vectors.end(), y.begin(), [](glm::vec3 &v) { return v.y; });
    std::transform(vectors.begin(), vectors.end(), z.begin(), [](glm::vec3 &v) { return v.z; });
    auto [x_min, x_max] = std::minmax_element(x.begin(), x.end());
    auto [y_min, y_max] = std::minmax_element(y.begin(), y.end());
    auto [z_min, z_max] = std::minmax_element(z.begin(), z.end());
    return {(*x_min + *x_max) / 2.0f, (*y_min + *y_max) / 2.0f, (*z_min + *z_max) / 2.0f};
}

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
    // return std::make_unique<VulkanTexturedPipe>(renderer, position, sectors, stacks, vulkanTexture,
    //                                             vulkanSpecularTexture, scale);
#endif // SPARK_PLATFORM_VULKAN
}

std::unique_ptr<Drawable3D> createPipe(std::vector<glm::vec3> positions, float radius, glm::vec3 color, int sectors,
                                       glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    // return std::make_unique<VulkanColoredPipe>(renderer, position, sectors, stacks, color, scale);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark::Render
