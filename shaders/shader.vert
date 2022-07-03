#version 450

layout(set=0, binding = 0) uniform GlobalUniformBufferObject {
	mat4 view;
	mat4 proj;
	vec3 lightPos1; //the position of the light.
	vec3 lightPos2; //the position of the light.
	vec3 lightColor; //the basic color of the light.
	vec3 ambColor; //the ambient color.
	vec4 coneInOutDecayExp;
} gubo;

layout(set=1, binding = 0) uniform UniformBufferObject {
	mat4 model;
} ubo;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 fragViewDir;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragPos;

void main() {
	gl_Position = gubo.proj * gubo.view * ubo.model * vec4(pos, 1.0);
	fragViewDir  = (gubo.view[3]).xyz - (ubo.model * vec4(pos,  1.0)).xyz;
	fragPos = (ubo.model * vec4(pos, 1.0)).xyz;
	fragNorm     = (ubo.model * vec4(norm, 0.0)).xyz;
	fragTexCoord = texCoord;
}