#include "spark/core/log.h"

#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace Spark
{
class Log
{
  public:
    static void Init()
    {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Spark.log", true));

        logSinks[0]->set_pattern("%^[%T] %n: %v%$");
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

        s_CoreLogger = std::make_shared<spdlog::logger>("SPARK", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_CoreLogger);
        s_CoreLogger->set_level(spdlog::level::trace);
        s_CoreLogger->flush_on(spdlog::level::trace);

        s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_ClientLogger);
        s_ClientLogger->set_level(spdlog::level::trace);
        s_ClientLogger->flush_on(spdlog::level::trace);
    }

    static spdlog::logger *GetCoreLogger()
    {
        return s_CoreLogger.get();
    }
    static spdlog::logger *GetClientLogger()
    {
        return s_ClientLogger.get();
    }

  private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void loggerInit()
{
    Log::Init();
}

spdlog::logger *getCoreLogger()
{
    return Log::GetCoreLogger();
}

spdlog::logger *getClientLogger()
{
    return Log::GetClientLogger();
}
} // namespace Spark
