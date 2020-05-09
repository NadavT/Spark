#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_ELEMENTS_AMOUNT 100

struct MVP {
	mat3 transformation;
};

layout(set = 0, binding = 0) uniform MVPs {
	MVP values[MAX_ELEMENTS_AMOUNT];
} mvps;

layout(push_constant) uniform PushConsts {
	int mvpOffset;
} pushConsts;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
	MVP mvp = mvps.values[pushConsts.mvpOffset];
    gl_Position = vec4(mvp.transformation * vec3(inPosition, 1.0), 1.0);
    fragColor = inColor;
	fragTexCoord = inTexCoord;
}
