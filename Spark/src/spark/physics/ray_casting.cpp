#include "ray_casting.h"

#include "spark/core/application.h"
#include "spark/core/input.h"
#include "spark/core/log.h"

#include "bounding/box_bounding.h"
#include "bounding/sphere_bounding.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
bool isRayIntersects(Ray3D ray, const Object3DBounding &objectBound)
{
    return getRayDistanceFromObject(ray, objectBound) > 0;
}

float getRayDistanceFromObject(Ray3D ray, const SphereBounding &sphereBound)
{
    const float a = 1.0f;
    float b = 2 * glm::dot(ray.direction, ray.source - sphereBound.getPosition());
    float c = glm::pow(glm::length(ray.source - sphereBound.getPosition()), 2) - glm::pow(sphereBound.getRadius(), 2);
    float res = glm::pow(b, 2) - (4 * a * c);
    if (res < 0)
    {
        return -1;
    }

    float sol1 = (-b + glm::sqrt(res)) / (2 * a);
    float sol2 = (-b - glm::sqrt(res)) / (2 * a);

    if (sol1 < 0 && sol2 < 0)
    {
        return -1;
    }
    else if (sol1 < 0)
    {
        return glm::length(sol2 * ray.direction);
    }
    else
    {
        float distance1 = glm::length(sol1 * ray.direction);
        float distance2 = glm::length(sol2 * ray.direction);
        return (distance1 < distance2) ? distance1 : distance2;
    }
}

bool approximatelyEquals(float a, float b)
{
    return glm::abs(a - b) < 100 * glm::epsilon<float>();
}

bool isPointInTriangle(glm::vec3 point, glm::vec3 t0, glm::vec3 t1, glm::vec3 t2)
{
    glm::vec3 T21 = t1 - t0;
    glm::vec3 T31 = t2 - t0;
    point = point - t0;
    float a = 0;
    float b = 0;
    if (T21.x == 0 && T21.y == 0 && T21.z == 0)
    {
        b = (T31.x != 0) ? point.x / T31.x : ((T31.y != 0) ? point.y / T31.y : ((T31.z != 0) ? point.z / T31.z : 0));
    }
    else if (T31.x == 0 && T31.y == 0 && T31.z == 0)
    {
        b = (T21.x != 0) ? point.x / T21.x : ((T21.y != 0) ? point.y / T21.y : ((T21.z != 0) ? point.z / T21.z : 0));
    }
    else if (T21.x == 0 && T21.y == 0)
    {
        if (T31.y == 0)
        {
            if (point.y == 0)
            {
                b = (T31.x != 0) ? (point.x / T31.x) : 0;
                a = (point.z - b * T31.z) / T21.z;
            }
        }
        else
        {
            b = point.y / T31.y;
            a = (point.z - b * T31.z) / T21.z;
        }
    }
    else if (T31.x == 0 && T31.y == 0)
    {
        if (T21.y == 0)
        {
            if (point.y == 0)
            {
                a = (T21.x != 0) ? (point.x / T21.x) : 0;
                b = (point.z - a * T21.z) / T31.z;
            }
        }
        else
        {
            a = point.y / T21.y;
            b = (point.z - a * T21.z) / T31.z;
        }
    }
    else if (T21.x == 0 && T31.x == 0)
    {
        b = (T21.y * point.z - T21.z * point.y) / (T21.y * T31.z - T21.z * T31.y);
        a = (point.z - b * T31.z) / T21.z;
    }
    else
    {
        b = (T21.x * point.y - T21.y * point.x) / (T21.x * T31.y - T21.y * T31.x);
        a = (point.y - b * T31.y) / T21.y;
    }
    return b >= 0 && b <= 1 && a >= 0 && a <= 1 && a + b <= 1 && approximatelyEquals(point.x, a * T21.x + b * T31.x) &&
           approximatelyEquals(point.y, a * T21.y + b * T31.y) && approximatelyEquals(point.z, a * T21.z + b * T31.z);
}

float getRayDistanceFromObject(Ray3D ray, const BoxBounding &boxBound)
{
    glm::mat4 transformation = boxBound.getTranslation() * boxBound.getRotation() * boxBound.getScale();
    glm::vec3 v0 = transformation * glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
    glm::vec3 v1 = transformation * glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);
    glm::vec3 v2 = transformation * glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
    glm::vec3 v3 = transformation * glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
    glm::vec3 v4 = transformation * glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
    glm::vec3 v5 = transformation * glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec3 v6 = transformation * glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec3 v7 = transformation * glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);

    glm::vec3 faces[6][4] = {
        {v0, v3, v2, v1}, {v4, v7, v6, v5}, {v0, v4, v5, v1}, {v1, v2, v6, v5}, {v2, v6, v7, v3}, {v3, v0, v4, v7},
    };

    float distance = -1;
    for (int i = 0; i < 6; ++i)
    {
        glm::vec3 planeNormal = glm::normalize(glm::cross(faces[i][1] - faces[i][0], faces[i][3] - faces[i][0]));
        float planeDistance = -glm::dot(faces[i][0], planeNormal);
        if (glm::dot(ray.direction, planeNormal) == 0)
        {
            continue;
        }
        float intersection =
            -(glm::dot(ray.source, planeNormal) + planeDistance) / (glm::dot(ray.direction, planeNormal));
        if (intersection < 0)
        {
            continue;
        }
        glm::vec3 intersectionPoint = ray.source + intersection * ray.direction;

        float curDistance = glm::distance(intersectionPoint, ray.source);
        if (isPointInTriangle(intersectionPoint, faces[i][0], faces[i][3], faces[i][1]) ||
            isPointInTriangle(intersectionPoint, faces[i][2], faces[i][1], faces[i][3]))
        {
            if (distance == -1 || curDistance < distance)
            {
                distance = curDistance;
            }
        }
    }
    return distance;
}

float getRayDistanceFromObject(Ray3D ray, const Object3DBounding &objectBound)
{
    switch (objectBound.getBoundingType())
    {
    case Object3DBoundingType::Sphere:
        return getRayDistanceFromObject(ray, static_cast<const SphereBounding &>(objectBound));
    case Object3DBoundingType::Box:
        return getRayDistanceFromObject(ray, static_cast<const BoxBounding &>(objectBound));
    default:
        SPARK_CORE_ASSERT(false, "Not supporting given bound object for ray casting");
        return -1;
    }
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
} // namespace Spark::Physics