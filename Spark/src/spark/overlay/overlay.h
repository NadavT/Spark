#pragma once

#include "spark/layer/layer.h"
#include "spark/renderer/renderer.h"

#include <imgui.h>

namespace Spark
{
	class Overlay : public Layer
	{
	public:
		virtual ~Overlay() = default;

		virtual void prepareOverlay() {};

	protected:
		Overlay();
	public:
		static std::unique_ptr<Overlay> Create(Renderer& renderer);
	};
}
