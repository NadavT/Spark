#pragma once

#include "drawable.h"

namespace Spark::Render
{
class Drawable2D : public virtual Drawable
{
  public:
    SPARK_API virtual ~Drawable2D() = default;

    SPARK_API virtual void move(glm::vec2 position);
    SPARK_API virtual void scale(glm::vec2 scale);
    SPARK_API virtual glm::mat4 getTransformation();

  protected:
    SPARK_API Drawable2D(glm::vec2 position, glm::vec2 scale);

  private:
    glm::vec2 m_position;
    glm::vec2 m_scale;
};
} // namespace Spark::Render