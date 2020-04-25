#pragma once

#include "spark/core/core.h"
#include "spark/core/application.h"
#include "spark/core/log.h"

#ifdef SPARK_PLATFORM_WINDOWS
extern Spark::Application* Spark::CreateApplication();

int main(int argc, char** argv)
{
	Spark::loggerInit();
	SPARK_CORE_TRACE("Initializing app");
	Spark::Application* app = Spark::CreateApplication();
	app->Run();
	delete app;
}
#endif
