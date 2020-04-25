#include "application.h"
#include "spark/event/application_event.h"
#include "spark/event/keyboard_event.h"
#include "spark/event/mouse_event.h"
#include "spark/core/log.h"

namespace Spark
{
	Application::Application()
	{
		m_window = Window::Create();
		m_window->SetEventCallback(SPARK_BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		SPARK_CORE_TRACE("Got event: {0}", e.ToString());
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(SPARK_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(SPARK_BIND_EVENT_FN(Application::OnWindowResize));
	}

	void Application::Run()
	{
		while (!m_shouldClose)
		{
			m_window->OnUpdate();
		}
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_shouldClose = true;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;

		return false;
	}
}
