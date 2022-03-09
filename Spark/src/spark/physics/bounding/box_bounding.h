#ifndef SPARK_PHYSICS_BOX_BOUNDING_H
#define SPARK_PHYSICS_BOX_BOUNDING_H

#include "object3d_bounding.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
class BoxBounding : public Object3DBounding
{
  public:
    BoxBounding(glm::mat4 translation, glm::mat4 scale, glm::mat4 rotation);
    virtual ~BoxBounding() = default;

    inline glm::mat4 getTranslation() const
    {
        return m_translationMatrix;
    };
    inline void setTranslation(glm::mat4 translation)
    {
        m_translationMatrix = translation;
    };
    inline glm::mat4 getScale() const
    {
        return m_scaleMatrix;
    };
    inline void setScale(glm::mat4 scale)
    {
        m_scaleMatrix = scale;
    };
    inline glm::mat4 getRotation() const
    {
        return m_rotationMatrix;
    };
    inline void setRotation(glm::mat4 rotation)
    {
        m_rotationMatrix = rotation;
    };
    SPARK_API virtual glm::mat4 getTransformation() const;

    virtual float getRayDistanceFromObject(Ray3D ray, glm::mat4 transformation) const;

  private:
    glm::mat4 m_translationMatrix;
    glm::mat4 m_scaleMatrix;
    glm::mat4 m_rotationMatrix;
};

} // namespace Spark::Physics

#endif /* SPARK_PHYSICS_BOX_BOUNDING_H */
