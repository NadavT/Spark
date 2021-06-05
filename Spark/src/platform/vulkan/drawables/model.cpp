#include "model.h"

#include "platform/vulkan/resource/mesh.h"

#include "spark/core/log.h"

namespace Spark::Render
{
VulkanDrawableModel::VulkanDrawableModel(const Model &model, glm::vec3 position, glm::vec3 scale)
    : Drawable3D(position, scale)
    , m_model(model)
{
}

std::vector<const VulkanRenderPrimitive *> VulkanDrawableModel::getRenderPrimitives() const
{
    std::vector<const VulkanRenderPrimitive *> primitives;
    for (auto &mesh : m_model.getMeshes())
    {
        SPARK_CORE_ASSERT(mesh != nullptr, "Model has an invalid(null) mesh");
        primitives.push_back(reinterpret_cast<VulkanMesh *>(mesh.get()));
    }
    return primitives;
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
