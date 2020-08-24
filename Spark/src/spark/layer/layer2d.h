#pragma once

#include "layer.h"

#include "spark/renderer/layer_renderer.h"

#include <memory>

namespace Spark
{
	class Layer2D : public Layer
	{
	public:
	    SPARK_API explicit Layer2D(const std::string& name);
		SPARK_API virtual ~Layer2D() = default;

		SPARK_API virtual void OnAttach();
		SPARK_API virtual void OnDetach();
		SPARK_API virtual void OnRender();

        SPARK_API virtual void addDrawable(std::shared_ptr<Drawable>& drawable);
		SPARK_API virtual void removeDrawable(Drawable* drawable);

	private:
		std::unique_ptr<LayerRenderer> m_layer_renderer;
	};
}
