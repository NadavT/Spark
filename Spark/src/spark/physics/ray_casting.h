#ifndef SPARK_PHYSICS_RAY_CASTING_H
#define SPARK_PHYSICS_RAY_CASTING_H

#include "spark/core/core.h"
#include "spark/renderer/camera.h"
#include "spark/vertex/vertex3d.h"

#include "object3d.h"

namespace Spark::Physics
{
SPARK_API bool isRayIntersects(Ray3D ray, const Object3DBounding &objectBound, glm::mat4 transformation);
SPARK_API bool isRayIntersects(Ray3D ray, const Object3D &object);
SPARK_API float getRayDistanceFromObject(Ray3D ray, const Object3DBounding &objectBound, glm::mat4 transformation);
SPARK_API float getRayDistanceFromObject(Ray3D ray, const Object3D &object);
SPARK_API glm::vec3 getClosestPointToRayFromRay(Ray3D fromRay, Ray3D toRay);
SPARK_API Ray3D getMouseRay(const Render::Camera &camera);

bool isPointInTriangle(glm::vec3 point, glm::vec3 t0, glm::vec3 t1, glm::vec3 t2);
float getRayDistanceFromFace(Ray3D ray, std::array<Vertex3D, 3> face, glm::mat4 transformation);

} // namespace Spark::Physics

#endif /* SPARK_PHYSICS_RAY_CASTING_H */
