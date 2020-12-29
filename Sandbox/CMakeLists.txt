project(sandbox)

set(SANDBOX_SOURCE
    "main.cpp"
    "layers/sandbox_2d_layer.cpp"
    "layers/sandbox_3d_layer.cpp"
    )

add_executable(sandbox ${SANDBOX_SOURCE})
set_property(TARGET sandbox PROPERTY CXX_STANDARD 17)
target_link_libraries(sandbox PRIVATE spark)
