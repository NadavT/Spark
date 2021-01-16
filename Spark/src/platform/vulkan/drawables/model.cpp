#include "model.h"

#include "spark/core/log.h"

namespace Spark::Render
{
VulkanDrawableModel::VulkanDrawableModel(const Model &model, glm::vec3 position, glm::vec3 scale)
    : Drawable3D(position, scale)
    , m_model(model)
{
}

void VulkanDrawableModel::fillCommandBuffer(VkCommandBuffer commandBuffer)
{
    SPARK_CORE_ASSERT(false, "fillCommandBuffer should be called for model meshes and not model");
}

VulkanDrawableType VulkanDrawableModel::getDrawableType() const
{
    return VulkanDrawableType::Model;
}

const Model &VulkanDrawableModel::getModel() const
{
    return m_model;
}
} // namespace Spark::Render
