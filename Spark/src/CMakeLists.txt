add_definitions(-DSPARK_DYNAMIC_LINK -DSPARK_BUILD_DLL)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DSPARK_DEBUG)
endif()

set(SPARK_SOURCE
    "spark/core/application.cpp"
    "spark/core/log.cpp"
    "spark/core/window.cpp"
    )

set(SPARK_WINDOWS_SOURCE
    "platform/Windows/window.cpp"
    "platform/Windows/time.cpp"
    )

add_library(spark SHARED ${SPARK_SOURCE} ${SPARK_WINDOWS_SOURCE})
target_include_directories(spark PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/vendor/spdlog/include)
target_include_directories(spark PRIVATE ${PROJECT_SOURCE_DIR}/vendor/glfw/include)
target_link_libraries(spark PRIVATE glfw)

add_custom_command(TARGET spark POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:spark> ${APP_EXECUTABLE_PATH}
)
