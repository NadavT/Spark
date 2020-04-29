#include "application.h"
#include "spark/event/application_event.h"
#include "spark/event/keyboard_event.h"
#include "spark/event/mouse_event.h"
#include "spark/core/log.h"
#include "spark/core/time.h"
#include "spark/core/input.h"

namespace Spark
{
	Application::Application()
	{
		loggerInit();

		m_window = Window::Create();
		m_window->SetEventCallback(SPARK_BIND_EVENT_FN(Application::OnEvent));
		
		m_renderer = Renderer::Create(GetWindow());

		Input::Init(*this);

		m_overlay = Overlay::Create(*m_renderer);
		m_overlay->OnAttach();
	}

	Application::~Application()
	{
		m_overlay->OnDetach();
		m_overlay.reset(nullptr);
		Input::Destroy();
		m_renderer.reset(nullptr);
		m_window.reset(nullptr);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(SPARK_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(SPARK_BIND_EVENT_FN(Application::OnWindowResize));

		m_overlay->OnEvent(e);
		if (!e.handled)
		{
			for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
			{
				(*it)->OnEvent(e);
				if (e.handled)
					break;
			}
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_layerStack.PushOverlayLayer(layer);
		layer->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		m_layerStack.PopLayer(layer);
		layer->OnDetach();
	}

	void Application::PopOverlay(Layer* layer)
	{
		m_layerStack.PopOverlayLayer(layer);
		layer->OnDetach();
	}

	void Application::Run()
	{
		while (!m_shouldClose)
		{
			Time currTime = getCurrentTime();
			Time timestep = currTime - m_lastFrameTime;
			m_lastFrameTime = currTime;

			if (!m_minimized)
			{
				for (Layer* layer : m_layerStack)
					layer->OnUpdate(timestep);
				m_overlay->OnUpdate(timestep);
			}
			
			m_renderer->finalize();
			m_window->OnUpdate(timestep);

			timestep = getCurrentTime() - m_lastFrameTime;
			if (timestep.GetSeconds() < 1.0 / 120)
				sleep(Time(1.0f / 120) - timestep);
		}
	}

	const Window& Application::GetWindow() const
	{
		return *m_window;
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
			m_minimized = true;
			return false;
		}
		m_minimized = false;

		return false;
	}
}
