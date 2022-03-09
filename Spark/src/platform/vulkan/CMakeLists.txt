project(spark)

# Find and link Vulkan to spark
find_package(Vulkan REQUIRED)
target_compile_definitions(spark PRIVATE SPARK_PLATFORM_VULKAN)
target_compile_definitions(spark PRIVATE LOAD_BLANK_TEXTURE)
target_link_libraries(spark PRIVATE Vulkan::Vulkan)

# Spark Vulkan platform sources
target_sources(spark
    PRIVATE "renderer.cpp"
    PRIVATE "vulkan_context.cpp"
    PRIVATE "framebuffer/framebuffer.cpp"
    PRIVATE "framebuffer/framebuffer2d.cpp"
    PRIVATE "framebuffer/framebuffer3d.cpp"
    PRIVATE "overlay/overlay.cpp"
    PRIVATE "pipeline/pipeline.cpp"
    PRIVATE "pipeline/pipeline2d.cpp"
    PRIVATE "pipeline/pipeline3d.cpp"
    PRIVATE "pipeline/pipeline3d_lights.cpp"
    PRIVATE "pipeline/pipeline3d_outline.cpp"
    PRIVATE "pipeline/pipeline3d_wireframe.cpp"
    PRIVATE "pipeline/pipeline3d_model.cpp"
    PRIVATE "pipeline/pipeline_triangle.cpp"
    PRIVATE "vertex/vertex2d.cpp"
    PRIVATE "vertex/vertex3d.cpp"
    PRIVATE "test/triangle_layer.cpp"
    PRIVATE "test/layer2d.cpp"
    PRIVATE "resource/mesh.cpp"
    PRIVATE "resource/texture.cpp"
    PRIVATE "resource/texture_sampler.cpp"
    PRIVATE "resource/texture_image.cpp"
    PRIVATE "layers/layer_renderer_2d.cpp"
    PRIVATE "layers/layer_renderer_3d.cpp"
    PRIVATE "layers/layer_renderer_3d_lights.cpp"
    PRIVATE "layers/layer_renderer_3d_model.cpp"
    PRIVATE "drawables/quad.cpp"
    PRIVATE "drawables/colored_drawable.cpp"
    PRIVATE "drawables/textured_drawable.cpp"
    PRIVATE "drawables/textured_box.cpp"
    PRIVATE "drawables/textured_cylinder.cpp"
    PRIVATE "drawables/textured_sphere.cpp"
    PRIVATE "drawables/colored_box.cpp"
    PRIVATE "drawables/colored_cylinder.cpp"
    PRIVATE "drawables/colored_sphere.cpp"
    PRIVATE "drawables/box.cpp"
    PRIVATE "drawables/cylinder.cpp"
    PRIVATE "drawables/sphere.cpp"
    PRIVATE "drawables/point_light.cpp"
    PRIVATE "drawables/model.cpp"
    PRIVATE "drawables/pipe.cpp"
    PRIVATE "drawables/colored_pipe.cpp"
    PRIVATE "drawables/textured_pipe.cpp"
    PRIVATE "drawables/vulkan_drawable.cpp"
    )
   
# Compile shaders as well
add_subdirectory(shaders)
