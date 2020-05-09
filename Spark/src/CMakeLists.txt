set(SPARK_SOURCE
    "spark/core/application.cpp"
    "spark/core/log.cpp"
    "spark/core/window.cpp"
    "spark/core/layer.cpp"
    "spark/core/layer_stack.cpp"
    "spark/core/input.cpp"
    "spark/renderer/renderer.cpp"
    "spark/overlay/overlay.cpp"
    "spark/utils/file.cpp")

set(SPARK_WINDOWS_SOURCE
    "platform/Windows/window.cpp"
    "platform/Windows/time.cpp"
    "platform/Windows/input.cpp"
    )

set(SPARK_VULKAN_SOURCE
    "platform/vulkan/renderer.cpp"
    "platform/vulkan/vulkan_context.cpp"
    "platform/vulkan/framebuffer/framebuffer.cpp"
    "platform/vulkan/framebuffer/framebuffer2d.cpp"
    "platform/vulkan/overlay/overlay.cpp"
    "platform/vulkan/pipeline/pipeline.cpp"
    "platform/vulkan/pipeline/pipeline2d.cpp"
    "platform/vulkan/vertex/vertex2d.cpp"
    "platform/vulkan/quad.cpp")

set(SPARK_IMGUI_SOURCE
    ${PROJECT_SOURCE_DIR}/vendor/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/vendor/imgui/examples/imgui_impl_vulkan.cpp
    ${PROJECT_SOURCE_DIR}/vendor/imgui/examples/imgui_impl_glfw.cpp
    )

find_package(Vulkan REQUIRED)

add_library(spark SHARED ${SPARK_SOURCE} ${SPARK_WINDOWS_SOURCE} ${SPARK_VULKAN_SOURCE} ${SPARK_IMGUI_SOURCE})

set_property(TARGET spark PROPERTY CXX_STANDARD 17)

target_include_directories(spark PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/vendor/spdlog/include)
target_include_directories(spark PRIVATE ${PROJECT_SOURCE_DIR}/vendor/glfw/include Vulkan::Vulkan ${PROJECT_SOURCE_DIR}/vendor/imgui ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/vendor/glm)

target_link_libraries(spark PRIVATE glfw Vulkan::Vulkan)

target_compile_definitions(spark PUBLIC SPARK_DYNAMIC_LINK)
target_compile_definitions(spark PRIVATE SPARK_BUILD_DLL SPARK_PLATFORM_VULKAN VK_USE_PLATFORM_WIN32_KHR NOMINMAX)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(spark PRIVATE SPARK_DEBUG)
endif()
target_compile_options(spark PRIVATE /W3)

add_custom_command(OUTPUT ${APP_EXECUTABLE_PATH}/shaders/texture.vert
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/shaders
                   COMMAND %VULKAN_SDK%/Bin/glslc.exe ${PROJECT_SOURCE_DIR}/src/platform/vulkan/shaders/texture.vert -o ${PROJECT_BINARY_DIR}/shaders/texture_vert.spv
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_EXECUTABLE_PATH}/shaders
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/shaders/texture_vert.spv ${APP_EXECUTABLE_PATH}/shaders
                   DEPENDS ${PROJECT_SOURCE_DIR}/src/platform/vulkan/shaders/texture.vert
)

add_custom_command(OUTPUT ${APP_EXECUTABLE_PATH}/shaders/texture.frag
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/shaders
                   COMMAND %VULKAN_SDK%/Bin/glslc.exe ${PROJECT_SOURCE_DIR}/src/platform/vulkan/shaders/texture.frag -o ${PROJECT_BINARY_DIR}/shaders/texture_frag.spv
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_EXECUTABLE_PATH}/shaders
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/shaders/texture_frag.spv ${APP_EXECUTABLE_PATH}/shaders
                   DEPENDS ${PROJECT_SOURCE_DIR}/src/platform/vulkan/shaders/texture.frag
)

add_custom_target(shaders 
    DEPENDS ${APP_EXECUTABLE_PATH}/shaders/texture.vert ${APP_EXECUTABLE_PATH}/shaders/texture.frag
)

add_dependencies(spark shaders)

add_custom_command(TARGET spark POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:spark> ${APP_EXECUTABLE_PATH}
)
