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
} gubo;

layout(set=1, binding = 1) uniform sampler2D texSampler;


layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

vec3 compute_Color(vec3 L, vec3 N, vec3 V, vec3 Cd, float sigma) {

	//Oren Nayar diffuse
	float A = 1 - 0.5*(pow(sigma,2)/(pow(sigma,2) + 0.33));
	float B = 0.45*(pow(sigma,2)/(pow(sigma,2) + 0.09));
	float theta_i= acos(dot(L,N));
	float theta_r= acos(dot(V,N));
	float alpha = max(theta_i, theta_r);
	float beta = min(theta_i, theta_r);
	
	
	vec3 vi = normalize(L - (dot(L,N)*N));
	vec3 vr = normalize(V - (dot(V,N)*N));
	float G = max(0, dot(vi, vr));
	vec3 Lo = Cd * clamp(dot(L,N),0,1);
	
	vec3 f_diff = Lo*(A + B*G*sin(alpha)*sin(beta));
	
	return f_diff;
}

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(fragViewDir);
	float sigma = 1.5f;
	
	float AmbFact = 0.025;
	
	vec3 DifCol = texture(texSampler, fragTexCoord).rgb;
	//vec3 SpecCol = vec3(1.0f, 1.0f, 1.0f);

	vec3 Ambient = AmbFact * DifCol;
	vec3 Dir =  vec3(cos(radians(150.0f)) * cos(radians(-60.0f)), sin(radians(150.0f)), 	cos(radians(150.0f)) * sin(radians(-60.0f)));
	vec3 Diffuse = compute_Color(Dir, Norm, EyeDir, DifCol, sigma) * gubo.lightColor ;
	outColor = vec4(Diffuse + Ambient, 1.0f);	
}