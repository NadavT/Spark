add_definitions(-DSPARK_DYNAMIC_LINK -DSPARK_BUILD_DLL)
add_library(spark SHARED "spark/core/application.cpp" "spark/core/log.cpp")
target_include_directories(spark PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/vendor/spdlog/include)

add_custom_command(TARGET spark POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:spark> ${APP_EXECUTABLE_PATH}
)
