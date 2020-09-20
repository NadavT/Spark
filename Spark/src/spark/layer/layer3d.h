#pragma once

#include "layer.h"

#include "spark/renderer/camera.h"
#include "spark/renderer/layer_renderer.h"


#include <memory>

namespace Spark
{
class Layer3D : public Layer
{
  public:
    SPARK_API Layer3D(const std::string &name, Camera &camera);
    SPARK_API virtual ~Layer3D() = default;

    SPARK_API virtual void OnAttach();
    SPARK_API virtual void OnDetach();
    SPARK_API virtual void OnRender();

    SPARK_API virtual void addDrawable(std::shared_ptr<Drawable> &drawable);
    SPARK_API virtual void removeDrawable(Drawable *drawable);

  private:
    std::unique_ptr<LayerRenderer> m_layer_renderer;
    Camera &m_camera;
};
} // namespace Spark
