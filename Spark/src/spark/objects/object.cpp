#include "object.h"

#include "spark/core/log.h"

namespace Spark
{
std::shared_ptr<Render::Drawable3D> Object3D::getDrawable()
{
    return m_drawable;
}

Physics::Object3D &Object3D::getPhysicsObject() const
{
    return *m_physicsObject;
}

Object3D::Object3D(std::shared_ptr<Render::Drawable3D> drawable, std::unique_ptr<Physics::Object3D> physicObject)
    : m_drawable(drawable)
    , m_physicsObject(std::move(physicObject))
    , m_parent(nullptr)
    , m_childs()
{
}

void Object3D::move(glm::vec3 position)
{
    m_drawable->move(position);
    m_physicsObject->move(position);
}

void Object3D::scale(glm::vec3 scale)
{
    m_drawable->scale(scale);
    m_physicsObject->scale(scale);
}

void Object3D::rotate(float angle, glm::vec3 axis)
{
    m_drawable->rotate(angle, axis);
    m_physicsObject->rotate(angle, axis);
}

void Object3D::setPosition(glm::vec3 position)
{
    m_drawable->setPosition(position);
    m_physicsObject->setPosition(position);
}

void Object3D::setScale(glm::vec3 scale)
{
    m_drawable->setScale(scale);
    m_physicsObject->setScale(scale);
}

void Object3D::setRotation(float angle, glm::vec3 axis)
{
    m_drawable->setRotation(angle, axis);
    m_physicsObject->setRotation(angle, axis);
}

void Object3D::setAsRelativeTransform()
{
    m_drawable->setAsRelativeTransform();
    m_physicsObject->setAsRelativeTransform();
}

void Object3D::setParent(Object3D *parent)
{
    if (m_parent)
    {
        removeParent();
    }

    m_parent = parent;
    if (parent)
    {
        parent->addChild(this);
        m_drawable->setParent(parent->getDrawable().get());
        m_physicsObject->setParent(&parent->getPhysicsObject());
    }
    else
    {
        m_drawable->setParent(nullptr);
        m_physicsObject->setParent(nullptr);
    }
}

void Object3D::removeParent()
{
    if (m_parent)
    {
        m_drawable->removeParent();
        m_physicsObject->removeParent();
        m_parent->removeChild(this);
        m_parent = nullptr;
    }
}

Object3D *Object3D::getParent() const
{
    return m_parent;
}

const std::vector<Object3D *> &Object3D::getChilds() const
{
    return m_childs;
}

void Object3D::addChild(Object3D *child)
{
    m_childs.push_back(child);
}

void Object3D::removeChild(Object3D *child)
{
    auto found_it = std::find(m_childs.begin(), m_childs.end(), child);

    SPARK_CORE_ASSERT(found_it != m_childs.end(), "Tried to remove a child which isn't a child of this parent");
    if (found_it != m_childs.end())
    {
        m_childs.erase(found_it);
    }
}

void Object3D::clearChilds()
{
    m_childs.clear();
}
} // namespace Spark
