#pragma once

#include "spark/core/core.h"

namespace Spark::Physics
{
enum class Object3DBoundingType
{
    Box,
    Sphere
};

class Object3DBounding
{
  public:
    inline Object3DBoundingType getBoundingType() const
    {
        return m_type;
    }

  protected:
    SPARK_API Object3DBounding(Object3DBoundingType type);

  private:
    Object3DBoundingType m_type;
};
} // namespace Spark::Physics
