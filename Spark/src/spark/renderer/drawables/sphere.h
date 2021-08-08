#ifndef SPARK_RENDER_SPHERE_H
#define SPARK_RENDER_SPHERE_H

#include "spark/core/core.h"

#include "spark/renderer/drawables/drawable3d.h"
#include "spark/resource/texture.h"

#include <memory>

namespace Spark::Render
{
class Sphere : public virtual Drawable3D
{
  public:
    SPARK_API virtual ~Sphere() = default;
    SPARK_API Sphere(const Sphere &other) = default;
    SPARK_API Sphere(Sphere &&other) noexcept = default;

  protected:
    SPARK_API Sphere(glm::vec3 position, int sectors, int stacks, glm::vec3 scale = glm::vec3(1.0));

  protected:
    int m_sectors;
    int m_stacks;
};

SPARK_API std::unique_ptr<Drawable3D> createSphere(glm::vec3 position, const Texture &texture,
                                                   const Texture &specularTexture, int sectors = 36, int stacks = 18,
                                                   glm::vec3 scale = glm::vec3(1.0));
SPARK_API std::unique_ptr<Drawable3D> createSphere(glm::vec3 position, glm::vec3 color, int sectors = 36,
                                                   int stacks = 18, glm::vec3 scale = glm::vec3(1.0));
} // namespace Spark::Render

#endif /* SPARK_RENDER_SPHERE_H */
