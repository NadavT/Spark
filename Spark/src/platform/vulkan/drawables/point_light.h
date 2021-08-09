#ifndef SPARK_VULKAN_POINT_LIGHT_H
#define SPARK_VULKAN_POINT_LIGHT_H

#include "spark/renderer/drawables/point_light.h"

namespace Spark::Render
{
struct VulkanPointLight : PointLight
{
  public:
    VulkanPointLight(glm::vec3 position, glm::vec3 color, std::unique_ptr<Object3D> object, bool isLit);
    virtual void turnOn();
    virtual void turnOff();
    virtual void setColor(glm::vec3 color);
};

} // namespace Spark::Render

#endif /* SPARK_VULKAN_POINT_LIGHT_H */
