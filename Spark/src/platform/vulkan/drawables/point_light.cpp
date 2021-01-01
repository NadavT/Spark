#include "point_light.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark::Render
{
VulkanPointLight::VulkanPointLight(glm::vec3 position, glm::vec3 color, std::unique_ptr<Object3D> object, bool isLit)
    : PointLight(position, color, std::move(object), isLit)
{
}

void VulkanPointLight::turnOn()
{
    PointLight::turnOn();
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    renderer.signalRecreationNeeded();
}

void VulkanPointLight::turnOff()
{
    PointLight::turnOff();
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    renderer.signalRecreationNeeded();
}

void VulkanPointLight::setColor(glm::vec3 color)
{
    PointLight::setColor(color);
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    renderer.signalRecreationNeeded();
}
} // namespace Spark::Render
