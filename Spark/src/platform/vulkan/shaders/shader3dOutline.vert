
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform MVPUniform
{
    mat4 model;
    mat4 view;
    mat4 proj;
}
mvp;

layout(push_constant) uniform PushConsts
{
    vec3 color;
    float lineWidth;
}
pushConsts;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

void main()
{
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inPosition + inNormal * pushConsts.lineWidth, 1.0);
}
