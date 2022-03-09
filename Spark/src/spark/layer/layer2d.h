#ifndef SPARK_LAYER2D_H
#define SPARK_LAYER2D_H

#include "layer.h"

#include "spark/renderer/layer_renderer.h"

#include <memory>

namespace Spark
{
class Layer2D : public Layer
{
  public:
    SPARK_API explicit Layer2D(const std::string &name);
    SPARK_API virtual ~Layer2D() = default;

    SPARK_API virtual void OnAttach();
    SPARK_API virtual void OnDetach();
    SPARK_API virtual void OnRender();

    SPARK_API virtual void addDrawable(std::shared_ptr<Render::Drawable> &drawable);
    SPARK_API virtual void removeDrawable(Render::Drawable *drawable);

  private:
    std::unique_ptr<Render::LayerRenderer> m_layer_renderer;
};
} // namespace Spark

#endif /* SPARK_LAYER2D_H */
