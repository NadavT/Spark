#pragma once

#include "drawable.h"

#include "spark/resource/model.h"

namespace Spark::Render
{
class Drawable3D : public virtual Drawable
{
  public:
    SPARK_API virtual ~Drawable3D() = default;

    SPARK_API virtual void move(glm::vec3 position);
    SPARK_API virtual void scale(glm::vec3 scale);
    SPARK_API virtual void rotate(float angle, glm::vec3 axis);
    SPARK_API virtual void setPosition(glm::vec3 position);
    SPARK_API virtual void setScale(glm::vec3 scale);
    SPARK_API virtual void setRotation(float angle, glm::vec3 axis);

    SPARK_API virtual glm::mat4 getTransformation();

  protected:
    SPARK_API Drawable3D(glm::vec3 position, glm::vec3 scale, float rotationAngle = 0,
                         glm::vec3 rotationAxis = {1, 0, 0});

  private:
    glm::mat4 m_translationMatrix;
    glm::mat4 m_scaleMatrix;
    glm::mat4 m_rotationMatrix;
};

SPARK_API std::unique_ptr<Drawable3D> createModelDrawable(const Model &model, glm::vec3 position, glm::vec3 scale);
} // namespace Spark::Render