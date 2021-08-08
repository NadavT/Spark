#ifndef SPARK_RENDER_RENDERER_H
#define SPARK_RENDER_RENDERER_H

#include "spark/core/core.h"
#include "spark/core/window.h"

namespace Spark::Render
{
class Renderer
{
  public:
    virtual ~Renderer() = default;

    virtual bool begin()
    {
        return true;
    }
    virtual void end(){};
    virtual void OnEvent(Event &e){};

    virtual void waitForIdle(){};

    SPARK_API virtual unsigned int getMSAA() const = 0;
    SPARK_API virtual void setMSAA(unsigned int sampleAmount) = 0;

  protected:
    Renderer(){};

  public:
    static std::unique_ptr<Renderer> Create(const Window &window);
};
} // namespace Spark::Render

#endif /* SPARK_RENDER_RENDERER_H */
