#pragma once

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"
#include "spark/renderer/drawable.h"

namespace Spark
{

enum class VulkanDrawableType
{
    Colored,
    Textured
};
class VulkanDrawable : public virtual Drawable
{
  public:
    virtual ~VulkanDrawable() = default;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer) = 0;
    virtual VulkanDrawableType getDrawableType() const = 0;

    virtual void highlight()
    {
        Drawable::highlight();
        VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
        renderer.signalRecreationNeeded();
    };

    virtual void unhighlight()
    {
        Drawable::unhighlight();
        VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
        renderer.signalRecreationNeeded();
    };

  protected:
    VulkanDrawable() = default;
};
} // namespace Spark