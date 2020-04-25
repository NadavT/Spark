#include "spark/core/time.h"

#include <GLFW/glfw3.h>

namespace Spark
{
	Time getCurrentTime()
	{
		return Time((float)glfwGetTime());
	}
}
