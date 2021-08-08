#ifndef SPARK_CYLINDER_H
#define SPARK_CYLINDER_H

#include "object.h"

#include "spark/resource/texture.h"

namespace Spark
{
class Cylinder : public Object3D
{
  public:
    SPARK_API Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height, glm::vec3 color,
                       int sectors = 36, int stacks = 1);
    SPARK_API Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height, const Texture &texture,
                       const Texture &specularTexture, int sectors = 36, int stacks = 1);
    SPARK_API virtual ~Cylinder() = default;
};

SPARK_API std::unique_ptr<Object3D> createCylinder(glm::vec3 position, float baseRadius, float topRadius, float height,
                                                   glm::vec3 color, int sectors = 36, int stacks = 1);
SPARK_API std::unique_ptr<Object3D> createCylinder(glm::vec3 position, float baseRadius, float topRadius, float height,
                                                   const Texture &texture, const Texture &specularTexture,
                                                   int sectors = 36, int stacks = 1);
} // namespace Spark

#endif /* SPARK_CYLINDER_H */
