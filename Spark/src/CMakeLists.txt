set(SPARK_SOURCE
    "spark/core/application.cpp"
    "spark/core/log.cpp"
    "spark/core/window.cpp"
    "spark/core/layer_stack.cpp"
    "spark/core/input.cpp"
    "spark/layer/layer.cpp"
    "spark/layer/layer2d.cpp"
    "spark/layer/layer3d.cpp"
    "spark/physics/object3d.cpp"
    "spark/physics/ray_casting.cpp"
    "spark/physics/sphere.cpp"
    "spark/physics/box.cpp"
    "spark/physics/bounding/box_bounding.cpp"
    "spark/physics/bounding/object3d_bounding.cpp"
    "spark/physics/bounding/sphere_bounding.cpp"
    "spark/renderer/renderer.cpp"
    "spark/renderer/layer_renderer.cpp"
    "spark/renderer/camera.cpp"
    "spark/renderer/drawables/drawable.cpp"
    "spark/renderer/drawables/drawable2d.cpp"
    "spark/renderer/drawables/drawable3d.cpp"
    "spark/renderer/drawables/quad.cpp"
    "spark/renderer/drawables/box.cpp"
    "spark/renderer/drawables/point_light.cpp"
    "spark/renderer/drawables/sphere.cpp"
    "spark/overlay/overlay.cpp"
    "spark/objects/object.cpp"
    "spark/objects/sphere.cpp"
    "spark/objects/box.cpp"
    "spark/utils/file.cpp"
    "spark/resource/mesh.cpp"
    "spark/resource/model.cpp"
    "spark/resource/texture.cpp"
    "spark/resource/resource_manager.cpp"
     )

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
    "platform/vulkan/framebuffer/framebuffer3d.cpp"
    "platform/vulkan/overlay/overlay.cpp"
    "platform/vulkan/pipeline/pipeline.cpp"
    "platform/vulkan/pipeline/pipeline2d.cpp"
    "platform/vulkan/pipeline/pipeline3d.cpp"
    "platform/vulkan/pipeline/pipeline3d_lights.cpp"
    "platform/vulkan/pipeline/pipeline3d_outline.cpp"
    "platform/vulkan/pipeline/pipeline3d_wireframe.cpp"
    "platform/vulkan/pipeline/pipeline3d_model.cpp"
    "platform/vulkan/pipeline/pipeline_triangle.cpp"
    "platform/vulkan/vertex/vertex2d.cpp"
    "platform/vulkan/vertex/vertex3d.cpp"
    "platform/vulkan/test/triangle_layer.cpp"
    "platform/vulkan/test/layer2d.cpp"
    "platform/vulkan/resource/mesh.cpp"
    "platform/vulkan/resource/texture.cpp"
    "platform/vulkan/resource/texture_sampler.cpp"
    "platform/vulkan/resource/texture_image.cpp"
    "platform/vulkan/layers/layer_renderer_2d.cpp"
    "platform/vulkan/layers/layer_renderer_3d.cpp"
    "platform/vulkan/layers/layer_renderer_3d_lights.cpp"
    "platform/vulkan/layers/layer_renderer_3d_model.cpp"
    "platform/vulkan/drawables/quad.cpp"
    "platform/vulkan/drawables/colored_drawable.cpp"
    "platform/vulkan/drawables/textured_drawable.cpp"
    "platform/vulkan/drawables/textured_box.cpp"
   "platform/vulkan/drawables/textured_sphere.cpp"
    "platform/vulkan/drawables/colored_box.cpp"
    "platform/vulkan/drawables/colored_sphere.cpp"
    "platform/vulkan/drawables/box.cpp"
    "platform/vulkan/drawables/sphere.cpp"
    "platform/vulkan/drawables/point_light.cpp"
    "platform/vulkan/drawables/model.cpp"
    )

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

target_include_directories(spark PUBLIC
                           ${PROJECT_SOURCE_DIR}/src
                           ${PROJECT_SOURCE_DIR}/vendor/spdlog/include
                           ${PROJECT_SOURCE_DIR}/vendor/glm
                           ${PROJECT_SOURCE_DIR}/vendor/imgui
                          )
target_include_directories(spark PRIVATE
                           ${PROJECT_SOURCE_DIR}/vendor/glfw/include
                           Vulkan::Vulkan
                           ${PROJECT_SOURCE_DIR}/vendor/stb
                           ${PROJECT_SOURCE_DIR}/vendor/Cimg
                           ${PROJECT_SOURCE_DIR}/vendor/assimp/include
                          )

target_link_libraries(spark PRIVATE glfw Vulkan::Vulkan assimp)

target_compile_definitions(spark PUBLIC SPARK_DYNAMIC_LINK)
target_compile_definitions(spark PRIVATE SPARK_BUILD_DLL SPARK_PLATFORM_VULKAN VK_USE_PLATFORM_WIN32_KHR NOMINMAX STB_IMAGE_IMPLEMENTATION)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(spark PRIVATE SPARK_DEBUG)
endif()
if(MSVC)
    target_compile_definitions(spark PRIVATE IMGUI_API=__declspec\(dllexport\))
    target_compile_options(spark PRIVATE /w44250)
    target_compile_options(spark PRIVATE /W3 /WX)
else()
    MESSAGE(FATAL_ERROR "Compiler isn't supported")
endif()

# Currently ninja has problems with clean if directories are a product
add_custom_command(OUTPUT stamp
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_EXECUTABLE_PATH}/shaders
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/shaders
                    COMMAND ${CMAKE_COMMAND} -E touch stamp
                    )
file(GLOB shaders ${PROJECT_SOURCE_DIR}/src/platform/vulkan/shaders/*.vert ${PROJECT_SOURCE_DIR}/src/platform/vulkan/shaders/*.frag)
foreach(shader ${shaders})
    get_filename_component(shader_name ${shader} NAME)
    string(REPLACE "." "_"  shader_output_name ${shader_name})
    string(APPEND shader_output_name ".spv")
    message(${shader})
    if("${shader}" MATCHES ".*/shader3dLights.frag")
        string(REPLACE "." "_"  shader_output_name ${shader_name})
        string(APPEND shader_output_name "_color.spv")
        add_custom_command(OUTPUT ${APP_EXECUTABLE_PATH}/shaders/${shader_output_name}
                        COMMAND %VULKAN_SDK%/Bin/glslc.exe ${shader} -DCOLOR -o ${PROJECT_BINARY_DIR}/shaders/${shader_output_name}
                        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/shaders/${shader_output_name} ${APP_EXECUTABLE_PATH}/shaders
                        BYPRODUCTS ${PROJECT_BINARY_DIR}/shaders/${shader_output_name}
                        DEPENDS stamp ${shader}
                        )
        add_custom_target(${shader_name}_color
                        DEPENDS ${APP_EXECUTABLE_PATH}/shaders/${shader_output_name}
                        )
        add_dependencies(spark ${shader_name}_color)
        string(REPLACE "." "_"  shader_output_name ${shader_name})
        string(APPEND shader_output_name "_texture.spv")
        add_custom_command(OUTPUT ${APP_EXECUTABLE_PATH}/shaders/${shader_output_name}
                        COMMAND %VULKAN_SDK%/Bin/glslc.exe ${shader} -DTEXTURE -o ${PROJECT_BINARY_DIR}/shaders/${shader_output_name}
                        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/shaders/${shader_output_name} ${APP_EXECUTABLE_PATH}/shaders
                        BYPRODUCTS ${PROJECT_BINARY_DIR}/shaders/${shader_output_name}
                        DEPENDS stamp ${shader}
                        )
        add_custom_target(${shader_name}_texture
                        DEPENDS ${APP_EXECUTABLE_PATH}/shaders/${shader_output_name}
                        )
        add_dependencies(spark ${shader_name}_texture)
    else()
        add_custom_command(OUTPUT ${APP_EXECUTABLE_PATH}/shaders/${shader_output_name}
                        COMMAND %VULKAN_SDK%/Bin/glslc.exe ${shader} -o ${PROJECT_BINARY_DIR}/shaders/${shader_output_name}
                        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/shaders/${shader_output_name} ${APP_EXECUTABLE_PATH}/shaders
                        BYPRODUCTS ${PROJECT_BINARY_DIR}/shaders/${shader_output_name}
                        DEPENDS stamp ${shader}
                        )
        add_custom_target(${shader_name}
                        DEPENDS ${APP_EXECUTABLE_PATH}/shaders/${shader_output_name}
                        )
        add_dependencies(spark ${shader_name})
    endif()
endforeach()


add_custom_command(TARGET spark POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:spark> ${APP_EXECUTABLE_PATH}
)
