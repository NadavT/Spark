#ifndef SPARK_RENDER_QUAD_H
#define SPARK_RENDER_QUAD_H

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

  protected:
    Quad(glm::vec2 position, glm::vec2 scale = glm::vec2(1.0));
    Quad(const Quad &other) = default;
    Quad(Quad &&other) noexcept = default;
    Quad &operator=(const Quad &other) = default;
    Quad &operator=(Quad &&other) noexcept = default;
};

SPARK_API std::unique_ptr<Drawable2D> createQuad(glm::vec2 position, const Texture &texture,
                                                 glm::vec2 scale = glm::vec2(1.0));
} // namespace Spark::Render

#endif /* SPARK_RENDER_QUAD_H */
