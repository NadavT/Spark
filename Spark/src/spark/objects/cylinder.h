#pragma once

#include "object.h"

#include "spark/resource/texture.h"

namespace Spark
{
class Cylinder : public Object3D
{
  public:
    SPARK_API Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height, glm::vec3 color);
    SPARK_API Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height, const Texture &texture,
                       const Texture &specularTexture);
    SPARK_API virtual ~Cylinder() = default;
};

SPARK_API std::unique_ptr<Object3D> createCylinder(glm::vec3 position, float baseRadius, float topRadius, float height,
                                                   glm::vec3 color);
SPARK_API std::unique_ptr<Object3D> createCylinder(glm::vec3 position, float baseRadius, float topRadius, float height,
                                                   const Texture &texture, const Texture &specularTexture);
} // namespace Spark
