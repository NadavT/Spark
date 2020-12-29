#pragma once

#include "drawable.h"

namespace Spark::Render
{
class Drawable3D : public virtual Drawable
{
  public:
    SPARK_API virtual ~Drawable3D() = default;

    SPARK_API virtual void move(glm::vec3 position);
    SPARK_API virtual void scale(glm::vec3 scale);
    SPARK_API virtual void setPosition(glm::vec3 position);
    SPARK_API virtual void setScale(glm::vec3 scale);

    SPARK_API virtual glm::mat4 getTransformation();

  protected:
    SPARK_API Drawable3D(glm::vec3 position, glm::vec3 scale);

  private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
};
} // namespace Spark::Render