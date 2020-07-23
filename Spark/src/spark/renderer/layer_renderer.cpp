#include "layer_renderer.h"

#include "spark/core/log.h"

namespace Spark
{
	LayerRenderer::LayerRenderer()
		: m_drawables()
	{
	}

	void LayerRenderer::addDrawable(Drawable* drawable)
	{
		m_drawables.push_back(drawable);
	}

	void LayerRenderer::removeDrawable(Drawable* drawable)
	{
		auto found_it = std::find(m_drawables.begin(), m_drawables.end(), drawable);
		SPARK_CORE_ASSERT(found_it != m_drawables.end(), "Tried to remove a drawable which isn't in the layer");
		if (found_it != m_drawables.end())
		{
			m_drawables.erase(found_it);
		}
	}
}
