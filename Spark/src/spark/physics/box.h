#pragma once

#include "bounding/box_bounding.h"
#include "object3d.h"

namespace Spark::Physics
{
class Box : public Object3D
{
  public:
    SPARK_API Box(glm::vec3 position, float width, float length, float height);
    SPARK_API virtual ~Box() = default;

    SPARK_API virtual void move(glm::vec3 position);
    SPARK_API virtual void scale(glm::vec3 scale);
    SPARK_API virtual void rotate(float angle, glm::vec3 axis);
    SPARK_API virtual void setPosition(glm::vec3 position);
    SPARK_API virtual void setScale(glm::vec3 scale);
    SPARK_API virtual void setRotation(float angle, glm::vec3 axis);

    SPARK_API virtual const Object3DBounding &getBoundingObject() const;

  private:
    glm::mat4 m_scaleMatrix;
    BoxBounding m_bounding;
};
} // namespace Spark::Physics
