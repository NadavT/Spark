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

    SPARK_API virtual std::shared_ptr<Render::Drawable3D> getDrawable();
    SPARK_API virtual Physics::Object3D &getPhysicsObject() const;

    SPARK_API virtual void move(glm::vec3 position);
    SPARK_API virtual void scale(glm::vec3 scale);
    SPARK_API virtual void rotate(float angle, glm::vec3 axis);
    SPARK_API virtual void setPosition(glm::vec3 position);
    SPARK_API virtual void setScale(glm::vec3 scale);
    SPARK_API virtual void setRotation(float angle, glm::vec3 axis);

    SPARK_API virtual void setAsRelativeTransform();

    SPARK_API virtual void setParent(Object3D *parent);
    SPARK_API virtual void removeParent();

    SPARK_API virtual Object3D *getParent() const;
    SPARK_API virtual const std::vector<Object3D *> &getChilds() const;

  protected:
    SPARK_API Object3D(std::shared_ptr<Render::Drawable3D> drawable, std::unique_ptr<Physics::Object3D> physicObject);
    SPARK_API Object3D() = default;
    virtual void addChild(Object3D *child);
    virtual void removeChild(Object3D *child);
    virtual void clearChilds();

  private:
    std::shared_ptr<Render::Drawable3D> m_drawable;
    std::unique_ptr<Physics::Object3D> m_physicsObject;
    Object3D *m_parent;
    std::vector<Object3D *> m_childs;
};
} // namespace Spark
