#pragma once

#include "spark/layer/layer.h"
#include "spark/renderer/renderer.h"

namespace Spark
{
	class Overlay : public Layer
	{
	public:
		virtual ~Overlay() = default;
	protected:
		Overlay()
			:Layer("Overlay")
		{

		};
	public:
		static std::unique_ptr<Overlay> Create(Renderer& renderer);
	};
}
