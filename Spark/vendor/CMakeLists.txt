project(spark)

# Add GLFW to project
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_include_directories(spark PRIVATE
                           ${PROJECT_SOURCE_DIR}/glfw/include
                          )

# Add assimp to project
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp)
target_compile_options(assimp PRIVATE /w44267 /w44244)
target_include_directories(spark PRIVATE
                           ${PROJECT_SOURCE_DIR}/assimp/include
                          )

# Add imgui to project
target_sources(spark
    PRIVATE imgui/imgui.cpp
    PRIVATE imgui/imgui_demo.cpp
    PRIVATE imgui/imgui_draw.cpp
    PRIVATE imgui/imgui_widgets.cpp
    PRIVATE imgui/examples/imgui_impl_vulkan.cpp
    PRIVATE imgui/examples/imgui_impl_glfw.cpp
    )
target_include_directories(spark PUBLIC
                           ${PROJECT_SOURCE_DIR}/imgui
                          )

# Add spdlog and glm to project
target_include_directories(spark PUBLIC
                           ${PROJECT_SOURCE_DIR}/spdlog/include
                           ${PROJECT_SOURCE_DIR}/glm
                          )

# Add stb to project
target_include_directories(spark PRIVATE
                           ${PROJECT_SOURCE_DIR}/stb
                          )
target_compile_definitions(spark PRIVATE STB_IMAGE_IMPLEMENTATION)

# Link libraries with project
target_link_libraries(spark PRIVATE glfw assimp)
