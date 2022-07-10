#version 450

layout(set=0, binding = 0) uniform GlobalUniformBufferObject {
    vec3 lightPos1;
    vec3 lightPos2;
    vec3 lightColor;
    vec3 ambColor;
    vec4 coneInOutDecayExp;
	vec3 spotPosition1;
    vec3 spotPosition2;
    vec3 spotPosition3;
    vec3 spotPosition4;
    vec3 spotDirection1;
    vec3 spotDirection2;
    vec3 spotDirection3;
	mat4 view;
	mat4 proj;
    mat4 lightVP;
} gubo;

layout(set=1, binding = 0) uniform UniformBufferObject {
	mat4 model;
    bool projectShadow;
} ubo;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 fragViewDir;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragPos;
layout (location = 4) out vec4 outShadowCoord;

const mat4 biasMat = mat4(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
0.5, 0.5, 0.0, 1.0 );

void main() {
	gl_Position = gubo.proj * gubo.view * ubo.model * vec4(pos, 1.0);
	fragViewDir  = (gubo.view[3]).xyz - (ubo.model * vec4(pos,  1.0)).xyz;
	fragPos = (ubo.model * vec4(pos, 1.0)).xyz;
	fragNorm     = (ubo.model * vec4(norm, 0.0)).xyz;
	fragTexCoord = texCoord;
    outShadowCoord = ( biasMat * gubo.lightVP * ubo.model ) * vec4(pos, 1.0);
    if(!ubo.projectShadow){
        outShadowCoord = vec4(0);
    }
}