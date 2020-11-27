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
    int numberOfPointLights;
}
pushConsts;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

void main()
{
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;

    // Converting positions to view space.
    fragPos = vec3(mvp.view * mvp.model * vec4(inPosition, 1.0));
    fragNormal = (mat3(transpose(inverse(mvp.view * mvp.model))) * inNormal);
}
