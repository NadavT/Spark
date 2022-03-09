#ifndef SPARK_VULKAN_VULKAN_DRAWABLE_H
#define SPARK_VULKAN_VULKAN_DRAWABLE_H

#include "platform/vulkan/drawables/render_primitive.h"
#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"
#include "spark/renderer/drawables/drawable.h"

namespace Spark::Render
{
enum class VulkanDrawableType
{
    Colored,
    Textured,
    Model
};
class VulkanDrawable : public virtual Drawable
{
  public:
    virtual ~VulkanDrawable() = default;

    virtual VulkanDrawableType getDrawableType() const = 0;
    virtual std::vector<const VulkanRenderPrimitive *> getRenderPrimitives() const = 0;

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

class VulkanDrawablePrimitive : public virtual VulkanDrawable, public VulkanRenderPrimitive
{
  public:
    virtual ~VulkanDrawablePrimitive() = default;

    virtual std::vector<const VulkanRenderPrimitive *> getRenderPrimitives() const;

  protected:
    VulkanDrawablePrimitive() = default;
};

} // namespace Spark::Render

#endif /* SPARK_VULKAN_VULKAN_DRAWABLE_H */
