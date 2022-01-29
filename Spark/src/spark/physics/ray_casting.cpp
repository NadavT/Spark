#include "ray_casting.h"

#include "spark/core/application.h"
#include "spark/core/input.h"
#include "spark/core/log.h"

#include "bounding/box_bounding.h"
#include "bounding/sphere_bounding.h"
#include "complex_object3d.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
bool isRayIntersects(Ray3D ray, const Object3DBounding &objectBound, glm::mat4 transformation)
{
    return getRayDistanceFromObject(ray, objectBound, transformation) > 0;
}

bool isRayIntersects(Ray3D ray, const Object3D &object)
{
    return getRayDistanceFromObject(ray, object) > 0;
}

bool approximatelyEquals(float a, float b)
{
    return glm::abs(a - b) < 1000 * glm::epsilon<float>();
}

bool isPointInTriangle(glm::vec3 point, glm::vec3 t0, glm::vec3 t1, glm::vec3 t2)
{
    glm::vec3 side0 = t1 - t0;
    glm::vec3 side1 = t2 - t0;
    point = point - t0;
    float a = 0;
    float b = 0;
    if (approximatelyEquals(side0.x, 0) && approximatelyEquals(side0.y, 0) && approximatelyEquals(side0.z, 0))
    {
        b = (!approximatelyEquals(side1.x, 0))
                ? point.x / side1.x
                : ((!approximatelyEquals(side1.y, 0)) ? point.y / side1.y
                                                      : ((!approximatelyEquals(side1.z, 0)) ? point.z / side1.z : 0));
    }
    else if (approximatelyEquals(side1.x, 0) && approximatelyEquals(side1.y, 0) && approximatelyEquals(side1.z, 0))
    {
        b = (!approximatelyEquals(side0.x, 0))
                ? point.x / side0.x
                : ((!approximatelyEquals(side0.y, 0)) ? point.y / side0.y
                                                      : ((!approximatelyEquals(side0.z, 0)) ? point.z / side0.z : 0));
    }
    else if (approximatelyEquals(side0.x, 0) && approximatelyEquals(side0.y, 0))
    {
        if (approximatelyEquals(side1.y, 0))
        {
            if (approximatelyEquals(point.y, 0))
            {
                b = (!approximatelyEquals(side1.x, 0)) ? (point.x / side1.x) : 0;
                a = (point.z - b * side1.z) / side0.z;
            }
        }
        else
        {
            b = point.y / side1.y;
            a = (point.z - b * side1.z) / side0.z;
        }
    }
    else if (approximatelyEquals(side1.x, 0) && approximatelyEquals(side1.y, 0))
    {
        if (approximatelyEquals(side0.y, 0))
        {
            if (approximatelyEquals(point.y, 0))
            {
                a = (!approximatelyEquals(side0.x, 0)) ? (point.x / side0.x) : 0;
                b = (point.z - a * side0.z) / side1.z;
            }
        }
        else
        {
            a = point.y / side0.y;
            b = (point.z - a * side0.z) / side1.z;
        }
    }
    else if (approximatelyEquals(side0.x, 0) && approximatelyEquals(side1.x, 0))
    {
        b = (side0.y * point.z - side0.z * point.y) / (side0.y * side1.z - side0.z * side1.y);
        a = (approximatelyEquals(side0.z, 0)) ? (point.z - b * side1.y) / side0.y : (point.z - b * side1.z) / side0.z;
    }
    else
    {
        if (approximatelyEquals((side0.x * side1.y - side0.y * side1.x), 0))
        {
            b = (side0.x * point.z - side0.z * point.x) / (side0.x * side1.z - side0.z * side1.x);
        }
        else
        {
            b = (side0.x * point.y - side0.y * point.x) / (side0.x * side1.y - side0.y * side1.x);
        }
        if (approximatelyEquals(side0.y, 0))
        {
            a = (point.z - b * side1.z) / side0.z;
        }
        else
        {
            a = (point.y - b * side1.y) / side0.y;
        }
    }
    return b >= 0 && b <= 1 && a >= 0 && a <= 1 && a + b <= 1 &&
           approximatelyEquals(point.x, a * side0.x + b * side1.x) &&
           approximatelyEquals(point.y, a * side0.y + b * side1.y) &&
           approximatelyEquals(point.z, a * side0.z + b * side1.z);
}

float getRayDistanceFromFace(Ray3D ray, std::array<Vertex3D, 3> face, glm::mat4 transformation)
{
    glm::vec3 v0 = transformation * glm::vec4(face[0].pos.x, face[0].pos.y, face[0].pos.z, 1.0f);
    glm::vec3 v1 = transformation * glm::vec4(face[1].pos.x, face[1].pos.y, face[1].pos.z, 1.0f);
    glm::vec3 v2 = transformation * glm::vec4(face[2].pos.x, face[2].pos.y, face[2].pos.z, 1.0f);
    glm::vec3 planeNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    float planeDistance = -glm::dot(v0, planeNormal);
    if (glm::dot(ray.direction, planeNormal) == 0)
    {
        return -1;
    }
    float intersection = -(glm::dot(ray.source, planeNormal) + planeDistance) / (glm::dot(ray.direction, planeNormal));
    if (intersection < 0)
    {
        return -1;
    }
    glm::vec3 intersectionPoint = ray.source + intersection * ray.direction;

    if (isPointInTriangle(intersectionPoint, v0, v1, v2))
    {
        return glm::distance(intersectionPoint, ray.source);
    }
    else
    {
        return -1;
    }
}

float getRayDistanceFromObject(Ray3D ray, const Object3DBounding &objectBound, glm::mat4 transformation)
{
    return objectBound.getRayDistanceFromObject(ray, transformation);
}

float getRayDistanceFromObject(Ray3D ray, const Object3D &object)
{
    float shortestDistance;
    shortestDistance = object.getRayDistanceFromObject(ray);

    for (auto &child : object.getChilds())
    {
        SPARK_CORE_ASSERT(child != nullptr, "Broken object {0}:Got a null child from object",
                          static_cast<const void *>(&object));
        float childDistance = getRayDistanceFromObject(ray, *child);
        if (childDistance != -1)
        {
            if (shortestDistance == -1 || childDistance < shortestDistance)
            {
                shortestDistance = childDistance;
            }
        }
    }

    return shortestDistance;
}

Ray3D getMouseRay(const Render::Camera &camera)
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

glm::vec3 getClosestPointToRayFromRay(Ray3D fromRay, Ray3D toRay)
{
    glm::vec3 normal = glm::normalize(glm::cross(fromRay.direction, toRay.direction));
    if (approximatelyEquals(normal.x, 0) && approximatelyEquals(normal.y, 0) && approximatelyEquals(normal.z, 0))
    {
        return fromRay.source;
    }

    float distance = glm::abs(glm::dot((fromRay.source - toRay.source), normal));

    float X = toRay.source.x - fromRay.source.x - distance * normal.x;
    float Y = toRay.source.y - fromRay.source.y - distance * normal.y;
    float Z = toRay.source.z - fromRay.source.z - distance * normal.z;
    float t1 = 0;

    if (!approximatelyEquals(fromRay.direction.x * toRay.direction.y - fromRay.direction.y * toRay.direction.x, 0))
    {
        t1 = (toRay.direction.y * X - toRay.direction.x * Y) /
             (fromRay.direction.x * toRay.direction.y - fromRay.direction.y * toRay.direction.x);
    }
    else if (!approximatelyEquals(fromRay.direction.x * toRay.direction.z - fromRay.direction.z * toRay.direction.x, 0))
    {
        t1 = (toRay.direction.z * X - toRay.direction.x * Z) /
             (fromRay.direction.x * toRay.direction.z - fromRay.direction.z * toRay.direction.x);
    }
    else if (!approximatelyEquals(fromRay.direction.y * toRay.direction.z - fromRay.direction.z * toRay.direction.y, 0))
    {
        t1 = (toRay.direction.z * Y - toRay.direction.y * Z) /
             (fromRay.direction.y * toRay.direction.z - fromRay.direction.z * toRay.direction.y);
    }
    else
    {
        SPARK_CORE_ERROR("Can't calculate getClosestPointToRayFromRay");
        SPARK_DEBUG_BREAK();
    }

    return fromRay.source + fromRay.direction * t1;
}
} // namespace Spark::Physics
