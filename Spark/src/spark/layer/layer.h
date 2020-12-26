#pragma once

#include "spark/core/core.h"
#include "spark/core/time.h"
#include "spark/event/event.h"

namespace Spark
{
class Layer
{
  public:
    SPARK_API explicit Layer(const std::string &name);
    SPARK_API virtual ~Layer() = default;

    SPARK_API virtual void OnAttach()
    {
    }
    SPARK_API virtual void OnDetach()
    {
    }
    SPARK_API virtual void OnUpdate(Time &diffTime)
    {
    }
    SPARK_API virtual void OnEvent(Event &e)
    {
    }
    SPARK_API virtual void OnRender()
    {
    }
    SPARK_API virtual void generateOverlay()
    {
    }

    SPARK_API const char *getName() const;

  protected:
    std::string m_name;
};
} // namespace Spark