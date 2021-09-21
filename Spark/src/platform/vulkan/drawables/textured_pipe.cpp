#include "textured_pipe.h"

#include "spark/utils/math.h"

namespace Spark::Render
{
VulkanTexturedPipe::VulkanTexturedPipe(VulkanRenderer &renderer, std::vector<glm::vec3> positions, float radius,
                                       int sectors, const VulkanTexture &texture, const VulkanTexture &specularTexture,
                                       glm::vec3 scale)
    : Drawable3D(mid_range(positions), scale)
    , Pipe(positions, radius, sectors, scale)
    , VulkanPipe(renderer, positions, radius, sectors, scale)
    , VulkanTexturedDrawable(texture, specularTexture)
{
}
} // namespace Spark::Render
