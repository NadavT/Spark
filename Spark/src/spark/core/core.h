#pragma once

#include <memory>

// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
    #ifdef _WIN64
	    /* Windows x64  */
        #define SPARK_PLATFORM_WINDOWS
    #else
	    /* Windows x86 */
        #error "x86 Builds are not supported!"
    #endif
#else
    #error "only windows is currently supported!"
#endif

#ifdef SPARK_PLATFORM_WINDOWS
    #ifdef SPARK_DYNAMIC_LINK
        #ifdef SPARK_BUILD_DLL
            #define SPARK_API __declspec(dllexport)
        #else
            #define SPARK_API __declspec(dllimport)
        #endif
    #else
        #define SPARK_API
    #endif
#else
    #error "only windows is currently supported!""
#endif // End of DLL support

#ifdef SPARK_DEBUG
    #if defined SPARK_PLATFORM_WINDOWS
        #define SPARK_DEBUG_BREAK() __debugbreak()
    #else
        #error "Platform doesn't support debugbreak yet!"
    #endif
    #define SPARK_ENABLE_ASSERTS
#else
    #define SPARK_DEBUG_BREAK()
#endif

#ifdef SPARK_ENABLE_ASSERTS
    #define SPARK_ASSERT(x, ...) { if(!(x)) { SPARK_ERROR("Assertion Failed: {0}", __VA_ARGS__); SPARK_DEBUG_BREAK(); } }
    #define SPARK_CORE_ASSERT(x, ...) { if(!(x)) { SPARK_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); SPARK_DEBUG_BREAK(); } }
#else
    #define SPARK_ASSERT(x, ...)
    #define SPARK_CORE_ASSERT(x, ...)
#endif

#define BIT(n) (1 << n)
