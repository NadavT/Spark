#pragma once

#include "spark/core/core.h"
#include "spark/event/event.h"
#include <functional>

namespace Spark
{
	struct WindowProperties
	{
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowProperties(const std::string& title = "Spark Engine app",
			unsigned int width = 1280,
			unsigned int height = 720)
			: title(title), width(width), height(height)
		{
		}
	};

	using EventCallbackFn = std::function<void(Event&)>;

	class SPARK_API Window
	{
	public:
		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		static std::unique_ptr<Window> Create(const WindowProperties& properties = WindowProperties());
	};
}