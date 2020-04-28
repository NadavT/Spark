#pragma once

#include "spark/core/layer.h"

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
	};
}
