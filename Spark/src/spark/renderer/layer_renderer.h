#pragma once

#include "spark/core/core.h"
#include "drawable.h"

#include <vector>

namespace Spark
{
	enum class LayerRendererType
	{
		Layer2D,
		Layer3D
	};

	class LayerRenderer
	{
	public:
		SPARK_API virtual ~LayerRenderer() = default;

		SPARK_API virtual void OnAttach() = 0;
		SPARK_API virtual void OnDetach() = 0;
		SPARK_API virtual void OnRender() = 0;

		SPARK_API virtual void addDrawable(Drawable* drawable);
		SPARK_API virtual void removeDrawable(Drawable* drawable);

	protected:
		SPARK_API LayerRenderer();

	protected:
		std::vector<Drawable*> m_drawables;
	};
}
