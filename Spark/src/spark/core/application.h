#pragma once

#include "spark/core/core.h"
#include "spark/core/window.h"
#include "spark/event/application_event.h"

namespace Spark
{
	class SPARK_API Application
	{
	public:
		Application();
		virtual ~Application();

		void OnEvent(Event& e);

		void Run();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_window;
		bool m_shouldClose = false;
		bool m_Minimized = false;
	};

	Application* CreateApplication();
}