#pragma once

#include "object.h"

#include "spark/resource/texture.h"

namespace Spark
{
class Box : public Object3D
{
  public:
    SPARK_API Box(glm::vec3 position, float width, float length, float height, glm::vec3 color);
    SPARK_API Box(glm::vec3 position, float width, float length, float height, const Texture &texture,
                  const Texture &specularTexture);
    SPARK_API virtual ~Box() = default;
};
} // namespace Spark
