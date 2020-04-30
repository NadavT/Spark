#include "spark/core/time.h"

#include <Windows.h>
#include <GLFW/glfw3.h>

namespace Spark
{
	Time getCurrentTime()
	{
		return Time((float)glfwGetTime());
	}

	void sleep(const Time& time)
	{
		Sleep(static_cast<int>(time.GetMilliSeconds()));
	}
}
