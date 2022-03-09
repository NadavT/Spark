#ifndef SPARK_RENDER_CYLINDER_H
#define SPARK_RENDER_CYLINDER_H

#include "spark/core/core.h"

#include "spark/renderer/drawables/drawable3d.h"
#include "spark/resource/texture.h"

#include <memory>

namespace Spark::Render
{
class Cylinder : public virtual Drawable3D
{
  public:
    SPARK_API virtual ~Cylinder() = default;
    SPARK_API Cylinder(const Cylinder &other) = default;
    SPARK_API Cylinder(Cylinder &&other) noexcept = default;
    SPARK_API Cylinder &operator=(const Cylinder &other) = default;
    SPARK_API Cylinder &operator=(Cylinder &&other) noexcept = default;

  protected:
    SPARK_API Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height, int sectors, int stacks,
                       glm::vec3 scale = glm::vec3(1.0));

  protected:
    float m_baseRadius;
    float m_topRadius;
    float m_height;
    int m_sectors;
    int m_stacks;
};

SPARK_API std::unique_ptr<Drawable3D> createCylinder(glm::vec3 position, const Texture &texture,
                                                     const Texture &specularTexture, float baseRadius, float topRadius,
                                                     float height, int sectors = 36, int stacks = 1,
                                                     glm::vec3 scale = glm::vec3(1.0));
SPARK_API std::unique_ptr<Drawable3D> createCylinder(glm::vec3 position, glm::vec3 color, float baseRadius,
                                                     float topRadius, float height, int sectors = 36, int stacks = 1,
                                                     glm::vec3 scale = glm::vec3(1.0));
} // namespace Spark::Render

#endif /* SPARK_RENDER_CYLINDER_H */
