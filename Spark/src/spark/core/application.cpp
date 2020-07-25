#include "application.h"
#include "spark/event/application_event.h"
#include "spark/event/keyboard_event.h"
#include "spark/event/mouse_event.h"
#include "spark/core/log.h"
#include "spark/core/time.h"
#include "spark/core/input.h"
#include "spark/resource/resource_manager.h"

#include "platform/vulkan/test/triangle_layer.h"
#include "platform/vulkan/test/layer2d.h"

double FPS_LIMIT = 1.0 / 120;

namespace Spark
{
	Application* Application::m_app = nullptr;

	Application::Application()
	{
		SPARK_CORE_ASSERT(m_app == nullptr, "App was already created");
		m_app = this;
		loggerInit();

		m_window = Window::Create();
		m_window->SetEventCallback(SPARK_BIND_EVENT_FN(Application::OnEvent));
		
		m_renderer = Renderer::Create(GetWindow());

		Input::Init(*this);
		ResourceManager::Init();

		m_testLayer = std::make_unique<VulkanTriangleLayer>(reinterpret_cast<VulkanRenderer&>(*m_renderer));
		PushLayer(m_testLayer.get());

		m_overlay = Overlay::Create(*m_renderer);
		m_overlay->OnAttach();
	}

	Application::~Application()
	{
		PopLayer(m_testLayer.get());
		m_overlay->OnDetach();
		m_overlay.reset(nullptr);
		m_testLayer.reset(nullptr);
		ResourceManager::Cleanup();
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
				timestep = Time(FPS_LIMIT);
			}
			if (FPS_LIMIT - timestep.GetSeconds() > FPS_LIMIT / 10)
			{
				sleep(Time(FPS_LIMIT) - timestep.GetSeconds() - Time(FPS_LIMIT / 10));
			}
			while (timestep.GetSeconds() < FPS_LIMIT)
			{
				currTime = getCurrentTime();
				timestep = currTime - m_lastFrameTime;

			}
			m_lastFrameTime = currTime;

			for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
				(*it)->OnUpdate(timestep);
			m_overlay->OnUpdate(timestep);
			m_window->OnUpdate(timestep);

			Render();

			Time frameTime = getCurrentTime() - m_lastFrameTime;
		}

		m_renderer->waitForIdle();
	}

	const Application& Application::GetApp()
	{
		SPARK_CORE_ASSERT(m_app != nullptr, "App wasn't initialized");
		return *m_app;
	}

	const Window& Application::GetWindow() const
	{
		SPARK_CORE_ASSERT(m_app != nullptr, "Window wasn't initialized");
		return *m_window;
	}

	Renderer& Application::GetRenderer() const
	{
		SPARK_CORE_ASSERT(m_app != nullptr, "Renderer wasn't initialized");
		return *m_renderer;
	}

	void Application::Render()
	{
		if (!m_minimized && m_renderer->begin())
		{
			for (Layer* layer : m_layerStack)
				layer->OnRender();
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
