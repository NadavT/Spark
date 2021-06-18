#pragma once

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

SPARK_API bool isRayIntersects(Ray3D ray, const Object3DBounding &objectBound);
SPARK_API bool isRayIntersects(Ray3D ray, const Object3D &object);
SPARK_API float getRayDistanceFromObject(Ray3D ray, const Object3DBounding &objectBound);
SPARK_API float getRayDistanceFromObject(Ray3D ray, const Object3D &object);
SPARK_API Ray3D getMouseRay(const Render::Camera &camera);
} // namespace Spark::Physics
