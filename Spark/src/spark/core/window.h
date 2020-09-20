#pragma once

#include "spark/core/core.h"
#include "spark/core/time.h"
#include "spark/event/event.h"
#include <functional>

namespace Spark
{
struct SPARK_API WindowProperties
{
    const char *title;
    unsigned int width = 0;
    unsigned int height = 0;
    ;

    WindowProperties(const char *title = "Spark Engine app", unsigned int width = 1280, unsigned int height = 720)
        : title(title)
        , width(width)
        , height(height)
    {
    }
};

using EventCallbackFn = std::function<void(Event &)>;

class Window
{
  public:
    virtual ~Window()
    {
    }

    virtual void OnUpdate(Time &diffTime) = 0;

    SPARK_API virtual unsigned int GetWidth() const = 0;
    SPARK_API virtual unsigned int GetHeight() const = 0;

    SPARK_API virtual void SetEventCallback(const EventCallbackFn &callback) = 0;

    virtual void *getPlatformWindow() const = 0;

    static std::unique_ptr<Window> Create(const WindowProperties &properties = WindowProperties());
};
} // namespace Spark