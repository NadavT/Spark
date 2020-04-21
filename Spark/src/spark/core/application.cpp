#include "application.h"
#include "spark/event/application_event.h"
#include "spark/event/keyboard_event.h"
#include "spark/event/mouse_event.h"
#include "spark/core/log.h"

Spark::Application::Application()
{
	WindowResizeEvent resizeEvent(100, 200);
	SPARK_CORE_INFO("Got event: {0}", ((Event&)resizeEvent).ToString());
	MouseMovedEvent mouseMovedEvent(100, 200);
	SPARK_CORE_INFO("Got event: {0}", ((Event&)mouseMovedEvent).ToString());

}

Spark::Application::~Application()
{
}

void Spark::Application::Run()
{
	while (true)
	{

	}
}
