#pragma once

#include "object.h"

#include "spark/resource/texture.h"

namespace Spark
{
class Sphere : public Object3D
{
  public:
    SPARK_API Sphere(glm::vec3 position, float radius, glm::vec3 color, int sectors = 36, int stacks = 18);
    SPARK_API Sphere(glm::vec3 position, float radius, const Texture &texture, const Texture &specularTexture,
                     int sectors = 36, int stacks = 18);
    SPARK_API virtual ~Sphere() = default;
};
} // namespace Spark
