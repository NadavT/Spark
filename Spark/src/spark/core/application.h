#pragma once

#include "spark/core/core.h"
#include "spark/core/window.h"
#include "spark/event/application_event.h"
#include "spark/core/layer_stack.h"

namespace Spark
{
	class Application
	{
	public:
		SPARK_API Application();
		SPARK_API virtual ~Application();

		SPARK_API void OnEvent(Event& e);

		SPARK_API void PushLayer(Layer* layer);
		SPARK_API void PushOverlay(Layer* layer);
		SPARK_API void PopLayer(Layer* layer);
		SPARK_API void PopOverlay(Layer* layer);

		SPARK_API void Run();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_window;
		bool m_shouldClose = false;
		bool m_minimized = false;
		LayerStack m_layerStack;
		Time m_lastFrameTime;
	};

	Application* CreateApplication();
}