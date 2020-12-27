#pragma once

#include "spark/core/core.h"

#include "spark/renderer/drawables/drawable3d.h"
#include "spark/resource/texture.h"

#include <memory>

namespace Spark
{
class Sphere : public virtual Drawable3D
{
  public:
    SPARK_API virtual ~Sphere() = default;
    SPARK_API Sphere(const Sphere &other) = default;
    SPARK_API Sphere(Sphere &&other) noexcept = default;
    SPARK_API Sphere &operator=(const Sphere &other) = default;
    SPARK_API Sphere &operator=(Sphere &&other) noexcept = default;

  protected:
    SPARK_API Sphere(glm::vec3 position, int sectors, int stacks, glm::vec3 scale = glm::vec3(1.0));

  protected:
    int m_sectors;
    int m_stacks;
};

SPARK_API std::shared_ptr<Drawable3D> createSphere(glm::vec3 position, const Texture &texture,
                                                   const Texture &specularTexture, glm::vec3 scale = glm::vec3(1.0));
SPARK_API std::shared_ptr<Drawable3D> createSphere(glm::vec3 position, glm::vec3 color, int sectors = 36,
                                                   int stacks = 18, glm::vec3 scale = glm::vec3(1.0));
} // namespace Spark
