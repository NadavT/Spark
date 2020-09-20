#include "input.h"

#include <GLFW/glfw3.h>

namespace Spark
{
WindowsInput::WindowsInput(const WindowsWindow &window)
    : m_window(window)
{
}

bool WindowsInput::IsKeyPressedImpl(KeyCode key)
{
    int state = glfwGetKey(static_cast<GLFWwindow *>(m_window.GetNativeWindow()), static_cast<int>(key));
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::IsMouseButtonPressedImpl(MouseCode button)
{
    int state = glfwGetMouseButton(static_cast<GLFWwindow *>(m_window.GetNativeWindow()), static_cast<int>(button));
    return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::GetMousePositionImpl()
{
    double x, y;
    glfwGetCursorPos(static_cast<GLFWwindow *>(m_window.GetNativeWindow()), &x, &y);
    return std::pair<float, float>(static_cast<float>(x), static_cast<float>(y));
}

float WindowsInput::GetMouseXImpl()
{
    auto pos = GetMousePositionImpl();
    return pos.first;
}

float WindowsInput::GetMouseYImpl()
{
    auto pos = GetMousePositionImpl();
    return pos.second;
}

void WindowsInput::HideMouseImpl()
{
    glfwSetInputMode(static_cast<GLFWwindow *>(m_window.GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowsInput::UnHideMouseImpl()
{
    glfwSetInputMode(static_cast<GLFWwindow *>(m_window.GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
} // namespace Spark
