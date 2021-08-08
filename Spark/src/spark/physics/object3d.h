#ifndef SPARK_PHYSICS_OBJECT3D_H
#define SPARK_PHYSICS_OBJECT3D_H

#include "spark/core/core.h"

#include "bounding/object3d_bounding.h"

#include <glm/glm.hpp>

#include <vector>

namespace Spark::Physics
{
enum class ObjectType
{
    Simple,
    Complex
};

class Object3D
{
  public:
    SPARK_API virtual ~Object3D() = default;

    SPARK_API virtual glm::vec3 getPosition() const;

    SPARK_API virtual void move(glm::vec3 position);
    SPARK_API virtual void scale(glm::vec3 scale);
    SPARK_API virtual void rotate(float angle, glm::vec3 axis);
    SPARK_API virtual void setPosition(glm::vec3 position);
    SPARK_API virtual void setScale(glm::vec3 scale);
    SPARK_API virtual void setRotation(float angle, glm::vec3 axis);

    SPARK_API virtual void setAsRelativeTransform();

    SPARK_API virtual glm::mat4 getTransformation() const;

    SPARK_API virtual const Object3DBounding &getBoundingObject() const = 0;

    virtual ObjectType getObjectType() const = 0;

    virtual void setParent(Object3D *parent);
    virtual void removeParent();

    virtual Object3D *getParent() const;
    virtual const std::vector<Object3D *> &getChilds() const;

  protected:
    SPARK_API Object3D(glm::vec3 position);
    virtual void addChild(Object3D *child);
    virtual void removeChild(Object3D *child);
    virtual void clearChilds();

  protected:
    glm::vec3 m_position;
    glm::mat4 m_scaleMatrix;
    glm::mat4 m_rotationMatrix;
    glm::mat4 m_relTransform;
    Object3D *m_parent;
    std::vector<Object3D *> m_childs;
};
} // namespace Spark::Physics

#endif /* SPARK_PHYSICS_OBJECT3D_H */
