#pragma once

#include "spark/core/application.h"
#include "spark/core/core.h"
#include "spark/core/log.h"

#ifdef SPARK_PLATFORM_WINDOWS
extern Spark::Application *Spark::CreateApplication();

int main(int argc, char **argv)
{
    Spark::Application *app = Spark::CreateApplication();
    SPARK_CORE_TRACE("Initialized app");
    app->Run();
    delete app;
}
#endif
