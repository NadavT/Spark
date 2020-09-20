#include "spark/core/time.h"

#include <GLFW/glfw3.h>
#include <Windows.h>

namespace Spark
{
Time getCurrentTime()
{
    return Time((float)glfwGetTime());
}

void sleep(const Time &time)
{
    Sleep(static_cast<int>(time.GetMilliSeconds()));
}
} // namespace Spark
