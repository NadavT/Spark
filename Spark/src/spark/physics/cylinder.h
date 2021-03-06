#ifndef SPARK_PHYSICS_CYLINDER_H
#define SPARK_PHYSICS_CYLINDER_H

#include "bounding/box_bounding.h"
#include "object3d.h"

namespace Spark::Physics
{
class Cylinder : public Object3D
{
  public:
    SPARK_API Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height);
    SPARK_API virtual ~Cylinder() = default;

    SPARK_API virtual const Object3DBounding &getBoundingObject() const;

  private:
    BoxBounding m_bounding;
};
} // namespace Spark::Physics

#endif /* SPARK_PHYSICS_CYLINDER_H */
