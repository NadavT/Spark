#ifndef SPARK_OVERLAY_H
#define SPARK_OVERLAY_H

#include "spark/layer/layer.h"
#include "spark/renderer/renderer.h"

#ifndef IMGUI_API
    #define IMGUI_API SPARK_API
#endif
#include <imgui.h>

namespace Spark
{
class Overlay : public Layer
{
  public:
    virtual ~Overlay() = default;

    virtual void prepareOverlay(){};

  protected:
    Overlay();

  public:
    static std::unique_ptr<Overlay> Create(Render::Renderer &renderer);
};
} // namespace Spark

#endif /* SPARK_OVERLAY_H */
