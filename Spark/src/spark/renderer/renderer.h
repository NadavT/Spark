#pragma once

#include "spark/core/core.h"
#include "spark/core/window.h"

namespace Spark
{
	class Renderer
	{
	public:
		virtual ~Renderer() = default;

		virtual bool begin() { return true; }
		virtual void end() {};
		virtual void OnEvent(Event& e) {};

		virtual void waitForIdle() {};
	protected:
		Renderer() {};

	public:
		static std::unique_ptr<Renderer> Create(const Window& window);
	};
}
