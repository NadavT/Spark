#ifndef SPARK_RENDER_DRAWABLE2D_H
#define SPARK_RENDER_DRAWABLE2D_H

#include "drawable.h"

namespace Spark::Render
{
class Drawable2D : public virtual Drawable
{
  public:
    SPARK_API virtual ~Drawable2D() = default;

    SPARK_API virtual void move(glm::vec2 position);
    SPARK_API virtual void scale(glm::vec2 scale);
    SPARK_API virtual glm::mat4 getTransformation() const;

  protected:
    Drawable2D(glm::vec2 position, glm::vec2 scale);
    Drawable2D(const Drawable2D &other) = default;
    Drawable2D(Drawable2D &&other) noexcept = default;
    Drawable2D &operator=(const Drawable2D &other) = default;
    Drawable2D &operator=(Drawable2D &&other) noexcept = default;

  private:
    glm::vec2 m_position;
    glm::vec2 m_scale;
};
} // namespace Spark::Render

#endif /* SPARK_RENDER_DRAWABLE2D_H */
