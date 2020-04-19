#pragma once

#include "spark/core/core.h"

#include <spdlog/spdlog.h>

namespace Spark
{
	SPARK_API void loggerInit();
	SPARK_API spdlog::logger* getCoreLogger();
	SPARK_API spdlog::logger* getClientLogger();

	template<typename... Args>
	SPARK_API void logTrace(spdlog::logger* logger, const Args&... args)
	{
		logger->trace(args...);
	}

	template<typename... Args>
	SPARK_API void logInfo(spdlog::logger* logger, const Args&... args)
	{
		logger->info(args...);
	}

	template<typename... Args>
	SPARK_API void logWarn(spdlog::logger* logger, const Args&... args)
	{
		logger->warn(args...);
	}

	template<typename... Args>
	SPARK_API void logError(spdlog::logger* logger, const Args&... args)
	{
		logger->error(args...);
	}

	template<typename... Args>
	SPARK_API void logCritical(spdlog::logger* logger, const Args&... args)
	{
		logger->critical(args...);
	}
};


// Core log macros
#define SPARK_CORE_TRACE(...)    ::Spark::logTrace(::Spark::getCoreLogger(), __VA_ARGS__)
#define SPARK_CORE_INFO(...)     ::Spark::logInfo(::Spark::getCoreLogger(), __VA_ARGS__)
#define SPARK_CORE_WARN(...)     ::Spark::logWarn(::Spark::getCoreLogger(), __VA_ARGS__)
#define SPARK_CORE_ERROR(...)    ::Spark::logError(::Spark::getCoreLogger(), __VA_ARGS__)
#define SPARK_CORE_CRITICAL(...) ::Spark::logCritical(::Spark::getCoreLogger(), __VA_ARGS__)

// Client log macros			   
#define SPARK_TRACE(...)         ::Spark::logTrace(::Spark::getClientLogger(), __VA_ARGS__)
#define SPARK_INFO(...)          ::Spark::logInfo(::Spark::getClientLogger(), __VA_ARGS__)
#define SPARK_WARN(...)          ::Spark::logWarn(::Spark::getClientLogger(), __VA_ARGS__)
#define SPARK_ERROR(...)         ::Spark::logError(::Spark::getClientLogger(), __VA_ARGS__)
#define SPARK_CRITICAL(...)      ::Spark::logCritical(::Spark::getClientLogger(), __VA_ARGS__)
