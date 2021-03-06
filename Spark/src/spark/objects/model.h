#pragma once

#include "object.h"

#include "spark/resource/model.h"

namespace Spark
{
class ModelObject : public Object3D
{
  public:
    SPARK_API ModelObject(const Spark::Model &model, glm::vec3 position, glm::vec3 scale);
    SPARK_API virtual ~ModelObject() = default;
};

SPARK_API std::unique_ptr<Object3D> createModelObject(const Spark::Model &model, glm::vec3 position, glm::vec3 scale);
} // namespace Spark
