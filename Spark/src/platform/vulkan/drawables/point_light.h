#pragma once

#include "spark/renderer/drawables/point_light.h"

namespace Spark::Render
{
struct VulkanPointLight : PointLight
{
  public:
    VulkanPointLight(glm::vec3 position, glm::vec3 color, std::shared_ptr<Drawable3D> &drawable, bool isLit);
    virtual void turnOn();
    virtual void turnOff();
    virtual void setColor(glm::vec3 color);
};

} // namespace Spark::Render
