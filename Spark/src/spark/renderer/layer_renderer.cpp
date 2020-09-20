#include "layer_renderer.h"

#include "spark/core/log.h"

namespace Spark
{
LayerRenderer::LayerRenderer()
    : m_drawables()
{
}

void LayerRenderer::addDrawable(std::shared_ptr<Drawable> &drawable)
{
    m_drawables.push_back(drawable);
}

void LayerRenderer::removeDrawable(Drawable *drawable)
{
    auto found_it = std::find_if(m_drawables.begin(), m_drawables.end(),
                                 [&](std::shared_ptr<Drawable> const &p) { return p.get() == drawable; });

    SPARK_CORE_ASSERT(found_it != m_drawables.end(), "Tried to remove a drawable which isn't in the layer");
    if (found_it != m_drawables.end())
    {
        m_drawables.erase(found_it);
    }
}
} // namespace Spark
