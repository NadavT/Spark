#pragma once

#include "spark/core/core.h"

namespace Spark
{
	class Drawable
	{
	public:
		SPARK_API virtual ~Drawable() = default;
	protected:
		SPARK_API Drawable() = default;
	};
}