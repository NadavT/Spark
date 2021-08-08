#ifndef SPARK_PHYSICS_BOX_H
#define SPARK_PHYSICS_BOX_H

#include "bounding/box_bounding.h"
#include "object3d.h"

namespace Spark::Physics
{
class Box : public Object3D
{
  public:
    SPARK_API Box(glm::vec3 position, float width, float length, float height);
    SPARK_API virtual ~Box() = default;

    SPARK_API virtual const Object3DBounding &getBoundingObject() const;

    virtual ObjectType getObjectType() const;

  private:
    BoxBounding m_bounding;
};
} // namespace Spark::Physics

#endif /* SPARK_PHYSICS_BOX_H */
