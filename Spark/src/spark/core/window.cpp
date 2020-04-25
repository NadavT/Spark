#include "window.h"

#ifdef SPARK_PLATFORM_WINDOWS
#include "platform/Windows/window.h"
#endif

namespace Spark
{
	std::unique_ptr<Window> Window::Create(const WindowProperties& properties)
	{
		#ifdef SPARK_PLATFORM_WINDOWS
		return std::make_unique<WindowsWindow>(properties);
		#endif
	}
}
