#version 450

layout(set=1, binding = 1) uniform sampler2D texSampler;
/*
layout(set= 1, binding = 2) uniform GlobalUniformBufferObject {
	// Direct lights parameters (for diffuse and specular)
	
	vec3 lightDir; //the direction of the light. 
	vec3 lightPos; //the position of the light.
	vec3 lightColor; //the basic color of the light.
	vec4 lightParams; //the other parameters of the spot light
	
	// Other parameters (not relevant for the exercise)
	vec3 eyePos;
	//vec4 selector;
} gubo;
*/

layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

vec3 compute_Color(vec3 N, vec3 V, vec3 Cd, vec3 Cs, float gamma) {
	// Lambert + Phong specular + point light
	// Parameters are:
	//
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 Cd : main color (diffuse color)
	// float sigma : roughness of the material
	// float gamma: specular power
	
	//@todo add this in global uniform buffer object
	vec3 lightPos = vec3 (3.0f, 3.0f, 6.58f);
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec4 coneInOutDecayExp = vec4(0.9f, 0.92f, 2.0f, 0.0f);
	
	
	//Point light dir
	vec3 dir = normalize(lightPos - fragPos);
	
	//Point light color
	vec3 col = pow(coneInOutDecayExp.z/length(fragPos - lightPos), coneInOutDecayExp.w) * lightColor;
	
	//Lambert
	float cos_a = dot(N, dir); 
	vec3 f_diff = Cd * clamp(cos_a, 0, 1);
	
	//Phong
	vec3 r_lx = -reflect(dir, N);
	
	vec3 f_spec = Cs * pow(clamp (dot(V,r_lx), 0,1), gamma);
	
	//Ambient
	vec3 ambient  = (vec3(0.1f,0.1f, 0.1f) * (1.0f + N.y) + vec3(0.0f,0.0f, 0.1f) * (1.0f - N.y)) * Cd;
	
	//@todo check if this is the correct way to add ambient light
	vec3 computed_col = col*(f_diff + f_spec) + ambient ; 
	return computed_col;
}

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(fragViewDir);
	float gamma = 200.0f;
	
	//float AmbFact = 0.025;
	
	vec3 DifCol = texture(texSampler, fragTexCoord).rgb;
	//vec3 SpecCol = texture(spec_texSampler, fragTexCoord).rgb;
	vec3 SpecCol = vec3(1.0f, 1.0f, 1.0f);


	vec3 CompColor = compute_Color(Norm, EyeDir, DifCol, SpecCol,  gamma) ;
	
	outColor = vec4(CompColor, 1.0f);	
}