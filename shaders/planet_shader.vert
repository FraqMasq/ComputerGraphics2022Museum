#version 450

layout(set=0, binding = 0) uniform GlobalUniformBufferObject {
    vec3 lightPos;
    vec3 lightColor;
    vec3 ambColor;
    vec4 coneInOutDecayExp;
    mat4 view;
    mat4 proj;
} gubo;

layout(set=1, binding = 0) uniform UniformBufferObject {
	mat4 model;
    vec2 isEmitting;
} ubo;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 fragViewDir;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragPos;
layout(location = 4) out vec2 isEmittingOut;

void main() {
	gl_Position = gubo.proj * gubo.view * ubo.model * vec4(pos, 1.0);
	fragViewDir  = (gubo.view[3]).xyz - (ubo.model * vec4(pos,  1.0)).xyz;
	fragPos = (ubo.model * vec4(pos, 1.0)).xyz;
	fragNorm     = (ubo.model * vec4(norm, 0.0)).xyz;
	fragTexCoord = texCoord;
    isEmittingOut = ubo.isEmitting;
}