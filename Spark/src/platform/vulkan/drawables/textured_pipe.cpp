#include "textured_pipe.h"

#include "spark/utils/math.h"

namespace Spark::Render
{
VulkanTexturedPipe::VulkanTexturedPipe(VulkanRenderer &renderer, std::vector<glm::vec3> positions, float radius,
                                       bool closed, int sectors, const VulkanTexture &texture,
                                       const VulkanTexture &specularTexture, glm::vec3 scale)
    : Drawable3D({0, 0, 0}, scale)
    , Pipe(positions, radius, closed, sectors, scale)
    , VulkanPipe(renderer, positions, radius, closed, sectors, scale)
    , VulkanTexturedDrawable(texture, specularTexture)
{
}
} // namespace Spark::Render
