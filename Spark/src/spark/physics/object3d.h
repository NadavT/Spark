#pragma once

#include "spark/core/core.h"

#include "bounding/object3d_bounding.h"

#include <glm/glm.hpp>

namespace Spark::Physics
{
class Object3D
{
  public:
    SPARK_API virtual ~Object3D() = default;

    SPARK_API virtual glm::vec3 getPosition() const;

    SPARK_API virtual void move(glm::vec3 position);
    SPARK_API virtual void scale(glm::vec3 scale) = 0;
    SPARK_API virtual void setPosition(glm::vec3 position);
    SPARK_API virtual void setScale(glm::vec3 scale) = 0;

    SPARK_API virtual const Object3DBounding &getBoundingObject() const = 0;

  protected:
    SPARK_API Object3D(glm::vec3 position);

  protected:
    glm::vec3 m_position;
};
} // namespace Spark::Physics
