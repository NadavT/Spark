#ifndef SPARK_PHYSICS_PIPE_H
#define SPARK_PHYSICS_PIPE_H

#include "bounding/box_bounding.h"
#include "object3d.h"

namespace Spark::Physics
{
class Pipe : public Object3D
{
  public:
    SPARK_API Pipe(std::vector<glm::vec3> positions, float radius);
    SPARK_API virtual ~Pipe() = default;

    SPARK_API virtual void scale(glm::vec3 scale);
    SPARK_API virtual void setScale(glm::vec3 scale);

    SPARK_API virtual const Object3DBounding &getBoundingObject() const;

    virtual float getRayDistanceFromObject(Ray3D ray) const;

  private:
    std::vector<glm::vec3> m_positions;
    float m_radius;
    float m_originalRadius;
    BoxBounding m_bounding;
};
} // namespace Spark::Physics

#endif /* SPARK_PHYSICS_PIPE_H */
