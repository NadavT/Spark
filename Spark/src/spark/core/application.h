#pragma once

#include "spark/core/core.h"

namespace Spark
{
	class SPARK_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}