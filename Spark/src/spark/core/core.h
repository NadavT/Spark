#pragma once

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

#define BIT(n) (1 << n)
