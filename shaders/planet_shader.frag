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
	vec3 emittingColor = vec3(0.1, 0.0, 0.0) * ubo.isEmitting.x ;
	float sigma = 1.5f;
	
	//float AmbFact = 0.025;
	
	vec3 DifCol = texture(texSampler, fragTexCoord).rgb;
	//vec3 SpecCol = vec3(1.0f, 1.0f, 1.0f);

	vec3 Ambient = gubo.ambColor * DifCol + 0.5*DifCol*ubo.isEmitting.x;
	vec3 Dir =  normalize(gubo.lightPos - fragPos);//vec3(cos(radians(150.0f)) * cos(radians(-60.0f)), sin(radians(150.0f)), 	cos(radians(150.0f)) * sin(radians(-60.0f)));

	vec3 pointLightCol = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.lightPos), gubo.coneInOutDecayExp.w) * gubo.lightColor;

	vec3 Diffuse = compute_Color(Dir, Norm, EyeDir, DifCol, sigma) * pointLightCol ;
	outColor = vec4(Diffuse + Ambient + emittingColor, 1.0f);
}