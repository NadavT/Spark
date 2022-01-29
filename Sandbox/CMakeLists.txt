project(sandbox)

set(SANDBOX_SOURCE
    "main.cpp"
    "layers/sandbox_2d_layer.cpp"
    "layers/sandbox_3d_layer.cpp"
	"layers/editor/editor_3d_layer.cpp"
    "layers/editor/editor_3d_move.cpp"
    "layers/editor/editor_3d_rotate.cpp"
    "layers/editor/editor_3d_scale.cpp"
    )

add_executable(sandbox ${SANDBOX_SOURCE})
set_property(TARGET sandbox PROPERTY CXX_STANDARD 17)
target_link_libraries(sandbox PRIVATE spark)

# Copy resources
add_custom_command(TARGET sandbox POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/resources/model" ${APP_EXECUTABLE_PATH}/model
                  )
add_custom_command(TARGET sandbox POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/resources/textures" ${APP_EXECUTABLE_PATH}/textures
                  )
