#ifndef SPARK_PHYSICS_RAY_CASTING_H
#define SPARK_PHYSICS_RAY_CASTING_H

#include "spark/core/core.h"
#include "spark/renderer/camera.h"

#include "bounding/object3d_bounding.h"
#include "object3d.h"

namespace Spark::Physics
{
struct Ray3D
{
    glm::vec3 direction;
    glm::vec3 source;
};

SPARK_API bool isRayIntersects(Ray3D ray, const Object3DBounding &objectBound, glm::mat4 transformation);
SPARK_API bool isRayIntersects(Ray3D ray, const Object3D &object);
SPARK_API float getRayDistanceFromObject(Ray3D ray, const Object3DBounding &objectBound, glm::mat4 transformation);
SPARK_API float getRayDistanceFromObject(Ray3D ray, const Object3D &object);
SPARK_API glm::vec3 getClosestPointToRayFromRay(Ray3D fromRay, Ray3D toRay);
SPARK_API Ray3D getMouseRay(const Render::Camera &camera);
} // namespace Spark::Physics

#endif /* SPARK_PHYSICS_RAY_CASTING_H */
