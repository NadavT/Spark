#include "colored_pipe.h"

#include "spark/utils/math.h"

namespace Spark::Render
{
VulkanColoredPipe::VulkanColoredPipe(VulkanRenderer &renderer, std::vector<glm::vec3> positions, float radius,
                                     bool closed, int sectors, glm::vec3 color, glm::vec3 scale)
    : Drawable3D({0, 0, 0}, scale)
    , Pipe(positions, radius, closed, sectors, scale)
    , VulkanPipe(renderer, positions, radius, closed, sectors, scale)
    , VulkanColoredDrawable(color)
{
}
} // namespace Spark::Render
