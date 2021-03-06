#ifndef SPARK_PHYSICS_SPHERE_BOUNDING_H
#define SPARK_PHYSICS_SPHERE_BOUNDING_H

#include "object3d_bounding.h"

#include <glm/glm.hpp>

namespace Spark::Physics
{
class SphereBounding : public Object3DBounding
{
  public:
    SphereBounding(glm::vec3 position, float radius);
    virtual ~SphereBounding() = default;

    inline glm::vec3 getPosition() const
    {
        return m_position;
    };

    inline void setPosition(glm::vec3 position)
    {
        m_position = position;
    };

    inline float getRadius() const
    {
        return m_radius;
    };

    inline void setRadius(float radius)
    {
        m_radius = radius;
    };
    SPARK_API virtual glm::mat4 getTransformation() const;

    virtual float getRayDistanceFromObject(Ray3D ray, glm::mat4 transformation) const;

  private:
    glm::vec3 m_position;
    float m_radius;
};

} // namespace Spark::Physics

#endif /* SPARK_SPHERE_BOUNDING_H */
