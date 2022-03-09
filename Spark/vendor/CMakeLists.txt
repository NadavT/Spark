project(spark)

# Add GLFW to project
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_include_directories(spark SYSTEM PRIVATE
                           ${PROJECT_SOURCE_DIR}/glfw/include
                          )

# Add assimp to project
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp)
target_compile_options(assimp PRIVATE /w44267 /w44244)
target_include_directories(spark SYSTEM PRIVATE
                           ${PROJECT_SOURCE_DIR}/assimp/include
                          )

# Add imgui to project
add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/examples/imgui_impl_vulkan.cpp
    imgui/examples/imgui_impl_glfw.cpp
    )
target_include_directories(imgui SYSTEM PUBLIC
                           ${PROJECT_SOURCE_DIR}/imgui
                          )

if (MSVC)
    target_compile_definitions(imgui PRIVATE IMGUI_API=__declspec\(dllexport\))
endif()

# Add spdlog and glm to project
target_include_directories(spark SYSTEM PUBLIC
                           ${PROJECT_SOURCE_DIR}/spdlog/include
                           ${PROJECT_SOURCE_DIR}/glm
                          )

# Add stb to project
target_include_directories(spark SYSTEM PRIVATE
                           ${PROJECT_SOURCE_DIR}/stb
                          )
target_compile_definitions(spark PRIVATE STB_IMAGE_IMPLEMENTATION)

# Find and link Vulkan to imgui
find_package(Vulkan REQUIRED)

# Link libraries with project
target_link_libraries(imgui PRIVATE glfw Vulkan::Vulkan)
target_link_libraries(spark PRIVATE glfw assimp imgui)
target_link_libraries(spark PUBLIC imgui)
