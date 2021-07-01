#pragma once

#include "bounding/box_bounding.h"
#include "object3d.h"

#include "spark/resource/mesh.h"

#include <memory>
#include <vector>

namespace Spark::Physics
{
class ComplexObject3D : public Object3D
{
  public:
    SPARK_API ComplexObject3D(const std::vector<std::unique_ptr<Mesh>> &meshes, glm::vec3 position, glm::vec3 scale);
    SPARK_API ComplexObject3D(const std::vector<std::unique_ptr<Mesh>> &meshes, const BoxBounding &boxBound);
    SPARK_API virtual ~ComplexObject3D() = default;

    const std::vector<std::unique_ptr<Mesh>> &getMeshes() const;
    SPARK_API virtual const Object3DBounding &getBoundingObject() const;

    virtual ObjectType getObjectType() const;

  private:
    const std::vector<std::unique_ptr<Mesh>> &m_meshes;
    BoxBounding m_bounding;
};
} // namespace Spark::Physics
