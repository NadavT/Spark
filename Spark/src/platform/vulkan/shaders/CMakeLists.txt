project(spark)

# Currently ninja has problems with clean if directories are a product
add_custom_command(OUTPUT ${APP_EXECUTABLE_PATH}/shaders/stamp
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_EXECUTABLE_PATH}/shaders
                    COMMAND ${CMAKE_COMMAND} -E touch ${APP_EXECUTABLE_PATH}/shaders/stamp
                    )
add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/shaders/stamp
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/shaders
                    COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_BINARY_DIR}/shaders/stamp
                    )

# Go over each shader and compile it with glslc
file(GLOB shaders *.vert *.frag)
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
                        DEPENDS ${PROJECT_BINARY_DIR}/shaders/stamp ${APP_EXECUTABLE_PATH}/shaders/stamp ${shader}
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
                        DEPENDS ${PROJECT_BINARY_DIR}/shaders/stamp ${APP_EXECUTABLE_PATH}/shaders/stamp ${shader}
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
                        DEPENDS ${PROJECT_BINARY_DIR}/shaders/stamp ${APP_EXECUTABLE_PATH}/shaders/stamp ${shader}
                        )
        add_custom_target(${shader_name}
                        DEPENDS ${APP_EXECUTABLE_PATH}/shaders/${shader_output_name}
                        )
        add_dependencies(spark ${shader_name})
    endif()
endforeach()
