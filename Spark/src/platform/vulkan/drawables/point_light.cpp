#include "point_light.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark
{
VulkanPointLight::VulkanPointLight(glm::vec3 position, glm::vec3 color, std::shared_ptr<Drawable> &drawable, bool isLit)
    : PointLight(position, color, drawable, isLit)
{
}

void VulkanPointLight::turnOn()
{
    PointLight::turnOn();
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    renderer.m_recreationNeeded = true;
}

void VulkanPointLight::turnOff()
{
    PointLight::turnOff();
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    renderer.m_recreationNeeded = true;
}
} // namespace Spark
