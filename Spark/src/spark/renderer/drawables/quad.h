#pragma once

#include "spark/core/core.h"

#include "spark/renderer/drawables/drawable2d.h"
#include "spark/resource/texture.h"

#include <memory>

namespace Spark::Render
{
class Quad : public virtual Drawable2D
{
  public:
    SPARK_API virtual ~Quad() = default;
    SPARK_API Quad(const Quad &other) = default;
    SPARK_API Quad(Quad &&other) noexcept = default;
    SPARK_API Quad &operator=(const Quad &other) = default;
    SPARK_API Quad &operator=(Quad &&other) noexcept = default;

  protected:
    SPARK_API Quad(glm::vec2 position, glm::vec2 scale = glm::vec2(1.0));
};

SPARK_API std::unique_ptr<Drawable2D> createQuad(glm::vec2 position, const Texture &texture,
                                                 glm::vec2 scale = glm::vec2(1.0));
} // namespace Spark::Render
