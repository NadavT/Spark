project(spark)

if (NOT CMAKE_GENERATOR STREQUAL "NMake Makefiles" AND NOT DEFINED CACHE{CMAKE_CXX_CLANG_TIDY})
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-warnings-as-errors=*;-extra-arg=/EHsc" CACHE)
endif
# Declare spark library
add_library(spark SHARED)

# Add sources to spark
add_subdirectory(spark)
add_subdirectory(platform)

# Set spark includes as a library
target_include_directories(spark PUBLIC
                           ${PROJECT_SOURCE_DIR}
                          )

# Compiler definitions
set_property(TARGET spark PROPERTY CXX_STANDARD 17)
target_compile_definitions(spark PUBLIC SPARK_DYNAMIC_LINK)
target_compile_definitions(spark PRIVATE SPARK_BUILD_DLL)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(spark PUBLIC SPARK_DEBUG)
endif()
if(MSVC)
    target_compile_options(spark PRIVATE /w44250)
    target_compile_options(spark PRIVATE /W3 /WX)
else()
    MESSAGE(FATAL_ERROR "Compiler isn't supported")
endif()

# Copy spark library to app (like sandbox)
add_custom_command(TARGET spark POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:spark> ${APP_EXECUTABLE_PATH}
                  )
