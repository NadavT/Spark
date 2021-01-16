#pragma once

#include "platform/vulkan/drawables/vulkan_drawable.h"
#include "platform/vulkan/renderer.h"
#include "spark/resource/model.h"

#include "spark/renderer/drawables/box.h"

namespace Spark::Render
{
class VulkanDrawableModel : public virtual Drawable3D, public virtual VulkanDrawable
{
  public:
    VulkanDrawableModel(const Model &model, glm::vec3 position, glm::vec3 scale);
    virtual ~VulkanDrawableModel() = default;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer);
    virtual VulkanDrawableType getDrawableType() const;
    virtual const Model &getModel() const;

  private:
    const Model &m_model;
};
} // namespace Spark::Render
