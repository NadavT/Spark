#pragma once

#include "spark/core/core.h"
#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_WINDOWS
extern Spark::Application* Spark::CreateApplication();

int main(int argc, char** argv)
{
	Spark::Application* app = Spark::CreateApplication();
	app->Run();
	delete app;
}
#endif