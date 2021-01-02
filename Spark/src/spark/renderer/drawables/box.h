#pragma once

#include "spark/core/core.h"

#include "spark/renderer/drawables/drawable3d.h"
#include "spark/resource/texture.h"

#include <memory>

namespace Spark::Render
{
class Box : public virtual Drawable3D
{
  public:
    SPARK_API virtual ~Box() = default;
    SPARK_API Box(const Box &other) = default;
    SPARK_API Box(Box &&other) noexcept = default;
    SPARK_API Box &operator=(const Box &other) = default;
    SPARK_API Box &operator=(Box &&other) noexcept = default;

  protected:
    SPARK_API Box(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0));
};

SPARK_API std::shared_ptr<Drawable3D> createCube(glm::vec3 position, const Texture &texture,
                                                 const Texture &specularTexture, glm::vec3 scale = glm::vec3(1.0));
SPARK_API std::shared_ptr<Drawable3D> createCube(glm::vec3 position, glm::vec3, glm::vec3 scale = glm::vec3(1.0));
} // namespace Spark::Render
