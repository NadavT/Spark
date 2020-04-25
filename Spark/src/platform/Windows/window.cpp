#include "window.h"

#include "spark/core/log.h"
#include "spark/event/application_event.h"
#include "spark/event/keyboard_event.h"
#include "spark/event/mouse_event.h"

namespace Spark
{
	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		SPARK_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	static void windowSizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		SPARK_CORE_ASSERT(data != NULL, "Got NULL as glfw user pointer");
		data->width = width;
		data->height = height;

		WindowResizeEvent event(width, height);
		data->eventCallback(event);
	}

	static void windowCloseCallback(GLFWwindow* window)
	{
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		SPARK_CORE_ASSERT(data != NULL, "Got NULL as glfw user pointer");
		WindowCloseEvent event;
		data->eventCallback(event);
	}

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		SPARK_CORE_ASSERT(data != NULL, "Got NULL as glfw user pointer");

		switch (action)
		{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(static_cast<KeyCode>(key), 0);
				data->eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(static_cast<KeyCode>(key));
				data->eventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(static_cast<KeyCode>(key), 1);
				data->eventCallback(event);
				break;
			}
		}
	}

	static void charCallback(GLFWwindow* window, unsigned int keycode)
	{
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		SPARK_CORE_ASSERT(data != NULL, "Got NULL as glfw user pointer");

		KeyTypedEvent event(static_cast<KeyCode>(keycode));
		data->eventCallback(event);
	}

	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		SPARK_CORE_ASSERT(data != NULL, "Got NULL as glfw user pointer");

		switch (action)
		{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(static_cast<MouseCode>(button));
				data->eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
				data->eventCallback(event);
				break;
			}
		}
	}

	static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		SPARK_CORE_ASSERT(data != NULL, "Got NULL as glfw user pointer");

		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		data->eventCallback(event);
	}

	static void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
	{
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		SPARK_CORE_ASSERT(data != NULL, "Got NULL as glfw user pointer");

		MouseMovedEvent event((float)xPos, (float)yPos);
		data->eventCallback(event);
	}


	WindowsWindow::WindowsWindow(const WindowProperties& properties)
	{
		Init(properties);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProperties& properties)
	{
		m_data.title = properties.title;
		m_data.width = properties.width;
		m_data.height = properties.height;

		SPARK_CORE_INFO("Creating window {0} ({1}, {2})",
			properties.title, properties.width, properties.height);

		if (s_GLFWWindowCount == 0)
		{
			int success = glfwInit();
			SPARK_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_window = glfwCreateWindow((int)properties.width, (int)properties.height, m_data.title.c_str(), nullptr, nullptr);
		s_GLFWWindowCount++;

		glfwSetWindowUserPointer(m_window, &m_data);

		glfwSetWindowSizeCallback(m_window, windowSizeCallback);
		glfwSetWindowCloseCallback(m_window, windowCloseCallback);
		glfwSetKeyCallback(m_window, keyCallback);
		glfwSetCharCallback(m_window, charCallback);
		glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
		glfwSetScrollCallback(m_window, scrollCallback);
		glfwSetCursorPosCallback(m_window, cursorPosCallback);
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_window);
		s_GLFWWindowCount--;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
	}
}