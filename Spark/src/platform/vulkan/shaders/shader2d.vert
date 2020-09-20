#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Transformation
{
    mat4 transformMatrix;
}
transformation;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main()
{
    gl_Position = transformation.transformMatrix * vec4(inPosition, 1.0, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}