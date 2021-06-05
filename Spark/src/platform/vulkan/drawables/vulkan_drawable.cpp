#include "vulkan_drawable.h"

namespace Spark::Render
{
std::vector<const VulkanRenderPrimitive *> VulkanDrawablePrimitive::getRenderPrimitives() const
{
    std::vector<const VulkanRenderPrimitive *> primitives;
    primitives.push_back(this);
    return primitives;
}
} // namespace Spark::Render
