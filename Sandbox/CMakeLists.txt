project(sandbox)

set(SANDBOX_SOURCE
    "main.cpp"
    "layers/sandbox_2d_layer.cpp"
    "layers/sandbox_3d_layer.cpp"
    )

add_executable(sandbox ${SANDBOX_SOURCE})
target_link_libraries(sandbox PRIVATE spark)
