#pragma once

#include "spark/core/core.h"

#include "spark/physics/object3d.h"
#include "spark/renderer/drawables/drawable3d.h"

namespace Spark
{
class Object3D
{
  public:
    SPARK_API virtual ~Object3D() = default;

    inline virtual std::shared_ptr<Render::Drawable3D> getDrawable()
    {
        return m_drawable;
    };
    inline virtual Physics::Object3D &getPhysicsObject() const
    {
        return *m_physicsObject;
    };

    SPARK_API virtual void move(glm::vec3 position);
    SPARK_API virtual void scale(glm::vec3 scale);
    SPARK_API virtual void rotate(float angle, glm::vec3 axis);
    SPARK_API virtual void setPosition(glm::vec3 position);
    SPARK_API virtual void setScale(glm::vec3 scale);
    SPARK_API virtual void setRotation(float angle, glm::vec3 axis);

  protected:
    SPARK_API Object3D(std::shared_ptr<Render::Drawable3D> &drawable, std::unique_ptr<Physics::Object3D> physicObject);

  private:
    std::shared_ptr<Render::Drawable3D> m_drawable;
    std::unique_ptr<Physics::Object3D> m_physicsObject;
};
} // namespace Spark
