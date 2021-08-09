#pragma once

#include "spark/core/core.h"
#include "spark/core/window.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Spark
{
struct WindowData
{
    std::string title;
    unsigned int width = 0;
    unsigned int height = 0;

    float lastX = 0.0f;
    float lastY = 0.0f;
    bool firstMove = true;

    EventCallbackFn eventCallback;
};

class WindowsWindow : public Window
{
  public:
    WindowsWindow(const WindowProperties &properties);
    virtual ~WindowsWindow();

    void OnUpdate(Time &diffTime) override;

    unsigned int GetWidth() const override
    {
        return m_data.width;
    }
    unsigned int GetHeight() const override
    {
        return m_data.height;
    }

    // Window attributes
    void SetEventCallback(const EventCallbackFn &callback) override
    {
        m_data.eventCallback = callback;
    }

    virtual void *getPlatformWindow() const override;

    virtual GLFWwindow *GetNativeWindow() const
    {
        return m_window;
    }

  private:
    void Init(const WindowProperties &properties);
    void Shutdown();

  private:
    GLFWwindow *m_window;

    WindowData m_data;
};
} // namespace Spark
