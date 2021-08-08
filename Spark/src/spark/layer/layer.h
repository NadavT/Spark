#ifndef SPARK_LAYER_H
#define SPARK_LAYER_H

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
        m_isAttached = true;
    }
    SPARK_API virtual void OnDetach()
    {
        m_isAttached = false;
    }
    SPARK_API bool isAttached()
    {
        return m_isAttached;
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

  private:
    bool m_isAttached;
};
} // namespace Spark

#endif /* SPARK_LAYER_H */
