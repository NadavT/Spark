#pragma once

#include "drawables/drawable.h"
#include "spark/core/core.h"

#include <vector>

namespace Spark::Render
{
enum class LayerRendererType
{
    Layer2D,
    Layer3D
};

enum class WireframeState
{
    None,
    Only,
    Both
};

class LayerRenderer
{
  public:
    virtual ~LayerRenderer() = default;

    virtual void OnAttach() = 0;
    virtual void OnDetach() = 0;
    virtual void OnRender() = 0;

    virtual void addDrawable(std::shared_ptr<Drawable> &drawable);
    virtual void removeDrawable(Drawable *drawable);

  protected:
    LayerRenderer();

  protected:
    std::vector<std::shared_ptr<Drawable>> m_drawables;
};
} // namespace Spark::Render
