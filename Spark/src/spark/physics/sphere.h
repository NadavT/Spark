#pragma once

#include "bounding/sphere_bounding.h"
#include "object3d.h"

namespace Spark::Physics
{
class Sphere : public Object3D
{
  public:
    SPARK_API Sphere(glm::vec3 position, float radius);
    SPARK_API virtual ~Sphere() = default;

    SPARK_API virtual void move(glm::vec3 position);
    SPARK_API virtual void scale(glm::vec3 scale);
    SPARK_API virtual void setPosition(glm::vec3 position);
    SPARK_API virtual void setScale(glm::vec3 scale);

    SPARK_API virtual const Object3DBounding &getBoundingObject() const;

  private:
    float m_radius;
    SphereBounding m_bounding;
};
} // namespace Spark::Physics
