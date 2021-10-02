#ifndef SPARK_PHYSICS_OBJECT3D_BOUNDING_H
#define SPARK_PHYSICS_OBJECT3D_BOUNDING_H

#include "spark/core/core.h"
#include "spark/vertex/vertex3d.h"

#include <glm/glm.hpp>

namespace Spark::Physics
{
class Object3DBounding
{
  public:
    SPARK_API virtual glm::mat4 getTransformation() const = 0;

    virtual float getRayDistanceFromObject(Ray3D ray, glm::mat4 transformation) const = 0;

  protected:
    SPARK_API Object3DBounding() = default;
};
} // namespace Spark::Physics

#endif /* SPARK_PHYSICS_OBJECT3D_BOUNDING_H */
