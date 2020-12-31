#include "ray_casting.h"

#include "spark/core/application.h"
#include "spark/core/input.h"
#include "spark/core/log.h"

#include "bounding/sphere_bounding.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
bool isRayIntersects(Ray3D ray, const Object3DBounding &objectBound)
{
    SPARK_CORE_ASSERT(objectBound.getBoundingType() == Object3DBoundingType::Sphere, "Only sphere is supported");
    const SphereBounding &sphereBound = static_cast<const SphereBounding &>(objectBound);
    float b = glm::dot(ray.direction, ray.source - sphereBound.getPosition());
    float c = glm::pow(glm::length(ray.source - sphereBound.getPosition()), 2) - glm::pow(sphereBound.getRadius(), 2);
    return glm::pow(b, 2) - c >= 0;
}

SPARK_API Ray3D getMouseRay(const Render::Camera &camera)
{
    const Application &app = Application::GetApp();
    float windowWidth = static_cast<float>(app.GetWindow().GetWidth());
    float windowHeight = static_cast<float>(app.GetWindow().GetHeight());
    glm::vec2 mousePosition = Input::GetMousePosition();
    float x = (2.0f * mousePosition.x) / windowWidth - 1.0f;
    float y = 1.0f - (2.0f * mousePosition.y) / windowHeight;
    glm::mat4 projection = glm::perspective(camera.getZoom(), windowWidth / windowHeight, 0.1f, 100.0f);
    glm::vec4 eyeRay = glm::inverse(projection) * glm::vec4(x, y, -1.0f, 1.0f);
    eyeRay = glm::vec4(eyeRay.x, eyeRay.y, -1.0f, 0.0f);
    glm::vec3 worldRay = glm::inverse(camera.getViewMatrix()) * eyeRay;
    worldRay = glm::normalize(worldRay);
    return {worldRay, camera.getPosition()};
}
} // namespace Spark::Physics