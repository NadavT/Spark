#ifndef SPARK_RENDER_DRAWABLE3D_H
#define SPARK_RENDER_DRAWABLE3D_H

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

    SPARK_API virtual void setAsRelativeTransform();

    SPARK_API virtual void setCalculateLight(bool calcLight);
    SPARK_API virtual bool isCalculateLight() const;

    SPARK_API virtual glm::mat4 getTransformation() const;

  protected:
    Drawable3D(glm::vec3 position, glm::vec3 scale, float rotationAngle = 0, glm::vec3 rotationAxis = {1, 0, 0});
    Drawable3D(const Drawable3D &other) = default;
    Drawable3D(Drawable3D &&other) noexcept = default;
    Drawable3D &operator=(const Drawable3D &other) = default;
    Drawable3D &operator=(Drawable3D &&other) noexcept = default;

  private:
    glm::mat4 m_translationMatrix;
    glm::mat4 m_scaleMatrix;
    glm::mat4 m_rotationMatrix;

    glm::mat4 m_relTransform;

    bool m_calcLight;
};

SPARK_API std::unique_ptr<Drawable3D> createModelDrawable(const Model &model, glm::vec3 position, glm::vec3 scale);
} // namespace Spark::Render

#endif /* SPARK_RENDER_DRAWABLE3D_H */
