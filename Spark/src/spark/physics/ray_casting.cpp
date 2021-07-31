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

float getRayDistanceFromObject(Ray3D ray, const SphereBounding &sphereBound, glm::mat4 transformation)
{
    const float a = 1.0f;
    glm::vec3 position = glm::vec3(transformation * glm::vec4(0, 0, 0, 1));
    float b = 2 * glm::dot(ray.direction, ray.source - position);
    float c =
        static_cast<float>(glm::pow(glm::length(ray.source - position), 2) - glm::pow(sphereBound.getRadius(), 2));
    float res = static_cast<float>(glm::pow(b, 2) - (4 * a * c));
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
    glm::vec3 side0 = t1 - t0;
    glm::vec3 side1 = t2 - t0;
    point = point - t0;
    float a = 0;
    float b = 0;
    if (side0.x == 0 && side0.y == 0 && side0.z == 0)
    {
        b = (side1.x != 0) ? point.x / side1.x
                           : ((side1.y != 0) ? point.y / side1.y : ((side1.z != 0) ? point.z / side1.z : 0));
    }
    else if (side1.x == 0 && side1.y == 0 && side1.z == 0)
    {
        b = (side0.x != 0) ? point.x / side0.x
                           : ((side0.y != 0) ? point.y / side0.y : ((side0.z != 0) ? point.z / side0.z : 0));
    }
    else if (side0.x == 0 && side0.y == 0)
    {
        if (side1.y == 0)
        {
            if (point.y == 0)
            {
                b = (side1.x != 0) ? (point.x / side1.x) : 0;
                a = (point.z - b * side1.z) / side0.z;
            }
        }
        else
        {
            b = point.y / side1.y;
            a = (point.z - b * side1.z) / side0.z;
        }
    }
    else if (side1.x == 0 && side1.y == 0)
    {
        if (side0.y == 0)
        {
            if (point.y == 0)
            {
                a = (side0.x != 0) ? (point.x / side0.x) : 0;
                b = (point.z - a * side0.z) / side1.z;
            }
        }
        else
        {
            a = point.y / side0.y;
            b = (point.z - a * side0.z) / side1.z;
        }
    }
    else if (side0.x == 0 && side1.x == 0)
    {
        b = (side0.y * point.z - side0.z * point.y) / (side0.y * side1.z - side0.z * side1.y);
        a = (point.z - b * side1.z) / side0.z;
    }
    else
    {
        b = (side0.x * point.y - side0.y * point.x) / (side0.x * side1.y - side0.y * side1.x);
        a = (point.y - b * side1.y) / side0.y;
    }
    return b >= 0 && b <= 1 && a >= 0 && a <= 1 && a + b <= 1 &&
           approximatelyEquals(point.x, a * side0.x + b * side1.x) &&
           approximatelyEquals(point.y, a * side0.y + b * side1.y) &&
           approximatelyEquals(point.z, a * side0.z + b * side1.z);
}

float getRayDistanceFromObject(Ray3D ray, const BoxBounding &boxBound, glm::mat4 transformation)
{
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

float getRayDistanceFromObject(Ray3D ray, const ComplexObject3D &object)
{
    glm::mat4 transformation = object.getTransformation();
    float distance = -1;
    for (auto &mesh : object.getMeshes())
    {
        for (auto &face : mesh->getFaces())
        {
            float curDistance = getRayDistanceFromFace(ray, face, transformation);
            if (curDistance != -1 && (distance == -1 || curDistance < distance))
            {
                distance = curDistance;
            }
        }
    }

    return distance;
}

float getRayDistanceFromObject(Ray3D ray, const Object3DBounding &objectBound, glm::mat4 transformation)
{
    switch (objectBound.getBoundingType())
    {
    case Object3DBoundingType::Sphere:
        return getRayDistanceFromObject(ray, static_cast<const SphereBounding &>(objectBound), transformation);
    case Object3DBoundingType::Box:
        return getRayDistanceFromObject(ray, static_cast<const BoxBounding &>(objectBound), transformation);
    default:
        SPARK_CORE_ASSERT(false, "Not supporting given bound object for ray casting");
        return -1;
    }
}

float getRayDistanceFromObject(Ray3D ray, const Object3D &object)
{
    float shortestDistance;
    switch (object.getObjectType())
    {
    case ObjectType::Simple:
        shortestDistance =
            getRayDistanceFromObject(ray, object.getBoundingObject(),
                                     object.getTransformation() * object.getBoundingObject().getTransformation());
        break;
    case ObjectType::Complex:
        shortestDistance = getRayDistanceFromObject(ray, reinterpret_cast<const ComplexObject3D &>(object));
        break;
    default:
        SPARK_CORE_ASSERT(false, "Not supporting given object for ray casting");
        return -1;
    }

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
