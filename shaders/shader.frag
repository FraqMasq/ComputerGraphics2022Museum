#version 450

layout(set=0, binding = 0) uniform GlobalUniformBufferObject {
	mat4 view;
	mat4 proj;
	vec3 lightDir; //direction of light
	vec3 lightPos1; //the position of the light.
	vec3 lightPos2; //the position of the light.
	vec3 spotPos1; //the position of the light.
	vec3 spotPos2; //the position of the light.
	vec3 spotPos3; //the position of the light.
	vec3 spotPos4; //the position of the light.
	vec3 lightColor; //the basic color of the light.
	vec3 ambColor; //the ambient color.
	vec4 coneInOutDecayExp; //for point light
} gubo;

layout(set=1, binding = 1) uniform sampler2D texSampler;


layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

vec3 compute_Color(vec3 N, vec3 V, vec3 Cd, vec3 Cs, float gamma) {
	// Lambert + Phong specular + point light + Ambient
	// Parameters are:
	//
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 Cd : main color (diffuse color)
	// vec3 Cs : specular Color
	// float gamma: specular power
	
	
	//Point light dir
	vec3 dir1 = normalize(gubo.lightPos1 - fragPos);
	vec3 dir2 = normalize(gubo.lightPos2 - fragPos);
	
	//Spot light dir
	vec3 sdir1 = normalize(gubo.spotPos1 - fragPos);
	vec3 sdir2 = normalize(gubo.spotPos2 - fragPos);
	vec3 sdir3 = normalize(gubo.spotPos3 - fragPos);
	vec3 sdir4 = normalize(gubo.spotPos4 - fragPos);
	
	//Point light color
	vec3 col1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.lightPos1), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	vec3 col2 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.lightPos2), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	
	//Spot light Color
	float theta = atan(7.3 - gubo.spotPos1.z , 1.3 - gubo.spotPos1.x);
	vec3 d1 =  vec3(cos(theta), sin(theta), 0.0f);
	vec3 fact1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.spotPos1), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	float fact2 = clamp((dot(normalize(gubo.spotPos1 - fragPos), d1) - gubo.coneInOutDecayExp.x) / (gubo.coneInOutDecayExp.y - gubo.coneInOutDecayExp.x),
	                    0,
						1);
	vec3 scol1 = fact1 * fact2;
	
	theta = atan(7.5 - gubo.spotPos2.z , 6.2 - gubo.spotPos2.x);
	vec3 d2 = vec3(cos(theta), sin(theta), 0.0f);
	fact1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.spotPos2), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	fact2 = clamp((dot(normalize(gubo.spotPos2 - fragPos), d2) - gubo.coneInOutDecayExp.x) / (gubo.coneInOutDecayExp.y - gubo.coneInOutDecayExp.x),
	                    0,
						1);
	vec3 scol2 = fact1 * fact2;
	
	theta = atan(3.0 - gubo.spotPos3.z , 0.5 - gubo.spotPos3.x);
	vec3 d3 = vec3(cos(theta), sin(theta), 0.0f);
	fact1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.spotPos3), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	fact2 = clamp((dot(normalize(gubo.spotPos3 - fragPos), d3) - gubo.coneInOutDecayExp.x) / (gubo.coneInOutDecayExp.y - gubo.coneInOutDecayExp.x),
	                    0,
						1);
	vec3 scol3 = fact1 * fact2;
	
	theta = atan(1.3 - gubo.spotPos4.z , 7.6 - gubo.spotPos4.x);
	vec3 d4 = vec3(cos(theta), sin(theta), 0.0f);
	fact1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.spotPos4), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	fact2 = clamp((dot(normalize(gubo.spotPos4 - fragPos), d4) - gubo.coneInOutDecayExp.x) / (gubo.coneInOutDecayExp.y - gubo.coneInOutDecayExp.x),
	                    0,
						1);
	vec3 scol4 = fact1 * fact2;
	
	//Lambert
	float cos_a1 = dot(N, dir1);
	float cos_a2 = dot(N, dir2);
	float cos_a3 = dot(N, sdir1);
	float cos_a4 = dot(N, sdir2); 
	float cos_a5 = dot(N, sdir3);
	float cos_a6 = dot(N, sdir4); 
	vec3 f_diff1 = Cd * clamp(cos_a1, 0, 1);
	vec3 f_diff2 = Cd * clamp(cos_a2, 0, 1);
	vec3 f_diff3 = Cd * clamp(cos_a3, 0, 1);
	vec3 f_diff4 = Cd * clamp(cos_a4, 0, 1);
	vec3 f_diff5 = Cd * clamp(cos_a5, 0, 1);
	vec3 f_diff6 = Cd * clamp(cos_a6, 0, 1);
	
	//Phong
	vec3 r_lx1 = -reflect(dir1, N);
	vec3 r_lx2 = -reflect(dir2, N);
	vec3 r_lx3 = -reflect(sdir1, N);
	vec3 r_lx4 = -reflect(sdir2, N);
	vec3 r_lx5 = -reflect(sdir3, N);
	vec3 r_lx6 = -reflect(sdir4, N);
	
	vec3 f_spec1 = Cs * pow(clamp (dot(V,r_lx1), 0,1), gamma);
	vec3 f_spec2 = Cs * pow(clamp (dot(V,r_lx2), 0,1), gamma);
	vec3 f_spec3 = Cs * pow(clamp (dot(V,r_lx3), 0,1), gamma);
	vec3 f_spec4 = Cs * pow(clamp (dot(V,r_lx4), 0,1), gamma);
	vec3 f_spec5 = Cs * pow(clamp (dot(V,r_lx5), 0,1), gamma);
	vec3 f_spec6 = Cs * pow(clamp (dot(V,r_lx6), 0,1), gamma);
	
	//Ambient
	//vec3 ambient  = (vec3(0.1f,0.1f, 0.1f) * (1.0f + N.y) + vec3(0.0f,0.0f, 0.1f) * (1.0f - N.y)) * Cd;
	vec3 ambient = gubo.ambColor*Cd;
	
	vec3 computed_col = col1*(f_diff1 + f_spec1) +col2 * (f_diff2 + f_spec2) + scol1*(f_diff3 + f_spec3) + scol2*(f_diff4 + f_spec4) + scol3*(f_diff5 + f_spec5) + scol4*(f_diff6 + f_spec6) + ambient ; 
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