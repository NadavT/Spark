set(SPARK_SOURCE
    "spark/core/application.cpp"
    "spark/core/log.cpp"
    "spark/core/window.cpp"
    "spark/core/layer.cpp"
    "spark/core/layer_stack.cpp"
    "spark/core/input.cpp"
    "spark/renderer/renderer.cpp"
    "spark/overlay/overlay.cpp"
    )

set(SPARK_WINDOWS_SOURCE
    "platform/Windows/window.cpp"
    "platform/Windows/time.cpp"
    "platform/Windows/input.cpp"
    )

set(SPARK_VULKAN_SOURCE
    "platform/Vulkan/renderer.cpp"
    "platform/Vulkan/vulkan_context.cpp"
    "platform/Vulkan/overlay/overlay.cpp"
    )

find_package(Vulkan REQUIRED)

add_library(spark SHARED ${SPARK_SOURCE} ${SPARK_WINDOWS_SOURCE} ${SPARK_VULKAN_SOURCE})

set_property(TARGET spark PROPERTY CXX_STANDARD 17)

target_include_directories(spark PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/vendor/spdlog/include)
target_include_directories(spark PRIVATE ${PROJECT_SOURCE_DIR}/vendor/glfw/include Vulkan::Vulkan ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/vendor/imgui)

target_link_libraries(spark PRIVATE glfw Vulkan::Vulkan)

target_compile_definitions(spark PUBLIC SPARK_DYNAMIC_LINK)
target_compile_definitions(spark PRIVATE SPARK_BUILD_DLL SPARK_PLATFORM_VULKAN VK_USE_PLATFORM_WIN32_KHR NOMINMAX)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(spark PRIVATE SPARK_DEBUG)
endif()


add_custom_command(TARGET spark POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:spark> ${APP_EXECUTABLE_PATH}
)
