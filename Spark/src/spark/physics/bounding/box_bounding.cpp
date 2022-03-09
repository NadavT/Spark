#include "box_bounding.h"

#include "spark/physics/ray_casting.h"

namespace Spark::Physics
{
BoxBounding::BoxBounding(glm::mat4 translation, glm::mat4 scale, glm::mat4 rotation)
    : Object3DBounding()
    , m_translationMatrix(translation)
    , m_scaleMatrix(scale)
    , m_rotationMatrix(rotation)
{
}

glm::mat4 BoxBounding::getTransformation() const
{
    return m_translationMatrix * m_rotationMatrix * m_scaleMatrix;
}

float BoxBounding::getRayDistanceFromObject(Ray3D ray, glm::mat4 transformation) const
{
    glm::vec3 v0 = transformation * getTransformation() * glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
    glm::vec3 v1 = transformation * getTransformation() * glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);
    glm::vec3 v2 = transformation * getTransformation() * glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
    glm::vec3 v3 = transformation * getTransformation() * glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
    glm::vec3 v4 = transformation * getTransformation() * glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
    glm::vec3 v5 = transformation * getTransformation() * glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec3 v6 = transformation * getTransformation() * glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec3 v7 = transformation * getTransformation() * glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);

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
} // namespace Spark::Physics
