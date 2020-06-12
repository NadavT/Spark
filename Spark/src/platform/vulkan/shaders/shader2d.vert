#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform Transformation {
    mat4 transformMatrix;
} transformation;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    vec4 final_position = transformation.transformMatrix * vec4(inPosition, 1.0, 1.0);
    gl_Position = final_position;
    fragColor = inColor;
}