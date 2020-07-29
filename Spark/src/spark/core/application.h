#pragma once

#include "spark/core/core.h"
#include "spark/core/window.h"
#include "spark/event/application_event.h"
#include "spark/core/layer_stack.h"
#include "spark/renderer/renderer.h"
#include "spark/overlay/overlay.h"

namespace Spark
{
	class Application
	{
	public:
		SPARK_API Application();
		SPARK_API virtual ~Application();

		SPARK_API void OnEvent(Event& e);
		SPARK_API virtual void handleEvent(Event& e);

		SPARK_API void PushLayer(Layer* layer);
		SPARK_API void PushOverlay(Layer* layer);
		SPARK_API void PopLayer(Layer* layer);
		SPARK_API void PopOverlay(Layer* layer);

		SPARK_API void Run();

		SPARK_API static const Application& GetApp();

		const Window& GetWindow() const;
		Renderer& GetRenderer() const;
	private:
		void Render();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		static Application* m_app;

		std::unique_ptr<Window> m_window;
		std::unique_ptr<Renderer> m_renderer;
		bool m_shouldClose = false;
		bool m_minimized = false;
		LayerStack m_layerStack;
		Time m_lastFrameTime;

		std::unique_ptr<Overlay> m_overlay;
		std::unique_ptr<Layer> m_testLayer;
	};

	Application* CreateApplication();
}