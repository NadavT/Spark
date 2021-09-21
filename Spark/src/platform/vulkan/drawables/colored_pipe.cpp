#include "colored_pipe.h"

#include "spark/utils/math.h"

namespace Spark::Render
{
VulkanColoredPipe::VulkanColoredPipe(VulkanRenderer &renderer, std::vector<glm::vec3> positions, float radius,
                                     int sectors, glm::vec3 color, glm::vec3 scale)
    : Drawable3D(mid_range(positions), scale)
    , Pipe(positions, radius, sectors, scale)
    , VulkanPipe(renderer, positions, radius, sectors, scale)
    , VulkanColoredDrawable(color)
{
}
} // namespace Spark::Render
