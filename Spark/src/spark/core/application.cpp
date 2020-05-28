#include "application.h"
#include "spark/event/application_event.h"
#include "spark/event/keyboard_event.h"
#include "spark/event/mouse_event.h"
#include "spark/core/log.h"
#include "spark/core/time.h"
#include "spark/core/input.h"

#include "platform/vulkan/test/test_layer.h"

namespace Spark
{
	Application::Application()
	{
		loggerInit();

		m_window = Window::Create();
		m_window->SetEventCallback(SPARK_BIND_EVENT_FN(Application::OnEvent));
		
		m_renderer = Renderer::Create(GetWindow());

		Input::Init(*this);

		m_testLayer = std::make_unique<VulkanTestLayer>(reinterpret_cast<VulkanRenderer&>(*m_renderer));
		m_testLayer->OnAttach();

		m_overlay = Overlay::Create(*m_renderer);
		m_overlay->OnAttach();
	}

	Application::~Application()
	{
		m_overlay->OnDetach();
		m_overlay.reset(nullptr);
		m_testLayer->OnDetach();
		m_testLayer.reset(nullptr);
		Input::Destroy();
		m_renderer.reset(nullptr);
		m_window.reset(nullptr);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(SPARK_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(SPARK_BIND_EVENT_FN(Application::OnWindowResize));

		m_renderer->OnEvent(e);
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
			if (timestep.GetSeconds() < 0)
			{
				timestep = Time(1.0f / 120);
			}
			if (1.0f / 120 - timestep.GetSeconds() > 0.001)
			{
				sleep(Time(1.0f / 120) - timestep.GetSeconds() - Time(0.001));
			}
			while (timestep.GetSeconds() < 1.0f / 120)
			{
				currTime = getCurrentTime();
				timestep = currTime - m_lastFrameTime;

			}
			m_lastFrameTime = currTime;

			//SPARK_CORE_TRACE("timestep: {0}ms", timestep.GetMilliSeconds());

			for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
				(*it)->OnUpdate(timestep);
			m_overlay->OnUpdate(timestep);
			m_window->OnUpdate(timestep);

			Render();

			Time frameTime = getCurrentTime() - m_lastFrameTime;
			//SPARK_CORE_TRACE("frame time: {0}ms", (getCurrentTime() - currTime).GetMilliSeconds());
		}

		m_renderer->waitForIdle();
	}

	const Window& Application::GetWindow() const
	{
		return *m_window;
	}

	void Application::Render()
	{
		if (!m_minimized && m_renderer->begin())
		{
			for (Layer* layer : m_layerStack)
				layer->OnRender();
			m_testLayer->OnRender();
			m_overlay->OnRender();
			m_renderer->end();
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
			m_minimized = true;
			return false;
		}
		m_minimized = false;

		return false;
	}
}
