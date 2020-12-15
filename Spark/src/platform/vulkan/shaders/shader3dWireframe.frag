#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConsts
{
    vec3 color;
}
pushConsts;

void main()
{
    outColor = vec4(pushConsts.color, 1.0);
}