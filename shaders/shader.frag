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

vec3 compute_Color(vec3 N, vec3 V, vec3 Cd, vec3 Cs, float gamma) {
	// Lambert + Phong specular + point light + Ambient
	// Parameters are:
	//
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 Cd : main color (diffuse color)
	// vec3 Cs : specular Color
	// float gamma: specular power
	
	const int n_spot = 2;
	
	//Point light dir
	vec3 dir1 = normalize(gubo.lightPos1 - fragPos);
	vec3 dir2 = normalize(gubo.lightPos2 - fragPos);
	
	//Spot Light dir
	vec3 sdir1 = normalize(gubo.spotPosition1 - fragPos);
	vec3 sdir2 = normalize(gubo.spotPosition2 - fragPos);
	vec3 sdir3 = normalize(gubo.spotPosition3 - fragPos);
	vec3 sdir4 = normalize(gubo.spotPosition4 - fragPos);
	

	
	//Point light color
	vec3 col1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.lightPos1), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	vec3 col2 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.lightPos2), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	
	//Spot light color
	vec3 fact1;
	float fact2;
	fact1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.spotPosition1), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	fact2 = clamp((dot(normalize(gubo.spotPosition1 - fragPos), gubo.spotDirection2) - gubo.coneInOutDecayExp.x) / (gubo.coneInOutDecayExp.y - gubo.coneInOutDecayExp.x),
	                    0,
						1);
	vec3 scol1 = fact1 * fact2;
	
	fact1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.spotPosition2), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	fact2 = clamp((dot(normalize(gubo.spotPosition2 - fragPos), gubo.spotDirection2) - gubo.coneInOutDecayExp.x) / (gubo.coneInOutDecayExp.y - gubo.coneInOutDecayExp.x),
	                    0,
						1);
	vec3 scol2 = fact1 * fact2;
	
	fact1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.spotPosition3), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	fact2 = clamp((dot(normalize(gubo.spotPosition3 - fragPos), gubo.spotDirection3) - gubo.coneInOutDecayExp.x) / (gubo.coneInOutDecayExp.y - gubo.coneInOutDecayExp.x),
	                    0,
						1);
	vec3 scol3 = fact1 * fact2;
	
	fact1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.spotPosition4), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	fact2 = clamp((dot(normalize(gubo.spotPosition4 - fragPos), gubo.spotDirection1) - gubo.coneInOutDecayExp.x) / (gubo.coneInOutDecayExp.y - gubo.coneInOutDecayExp.x),
	                    0,
						1);
	vec3 scol4 = fact1 * fact2;
	
	
	//Lambert
	float cos_a1 = dot(N, dir1);
	float cos_a2 = dot(N, dir2);

	float cos_a_s1 = dot(N, sdir1);
	float cos_a_s2 = dot(N, sdir2);
	float cos_a_s3 = dot(N, sdir3);
	float cos_a_s4 = dot(N, sdir4);
	
	
	vec3 f_diff1 = Cd * clamp(cos_a1, 0, 1);
	vec3 f_diff2 = Cd * clamp(cos_a2, 0, 1);
	
	
	vec3 f_diff_s1 = Cd * clamp(cos_a_s1, 0, 1);
	vec3 f_diff_s2 = Cd * clamp(cos_a_s2, 0, 1);
	vec3 f_diff_s3 = Cd * clamp(cos_a_s3, 0, 1);
	vec3 f_diff_s4 = Cd * clamp(cos_a_s4, 0, 1);
	
	
	//Phong
	vec3 r_lx1 = -reflect(dir1, N);
	vec3 r_lx2 = -reflect(dir2, N);

	
	vec3 r_lx_s1 = -reflect(sdir1, N);
	vec3 r_lx_s2 = -reflect(sdir2, N);
	vec3 r_lx_s3 = -reflect(sdir3, N);
	vec3 r_lx_s4 = -reflect(sdir4, N);
	
	
	vec3 f_spec1 = Cs * pow(clamp (dot(V,r_lx1), 0,1), gamma);
	vec3 f_spec2 = Cs * pow(clamp (dot(V,r_lx2), 0,1), gamma);
	
	vec3 f_spec_s1 = Cs * pow(clamp (dot(V,r_lx_s1), 0,1), gamma);
	vec3 f_spec_s2 = Cs * pow(clamp (dot(V,r_lx_s2), 0,1), gamma);
	vec3 f_spec_s3 = Cs * pow(clamp (dot(V,r_lx_s3), 0,1), gamma);
	vec3 f_spec_s4 = Cs * pow(clamp (dot(V,r_lx_s4), 0,1), gamma);
	

	//Ambient
	//vec3 ambient  = (vec3(0.1f,0.1f, 0.1f) * (1.0f + N.y) + vec3(0.0f,0.0f, 0.1f) * (1.0f - N.y)) * Cd;
	vec3 ambient = gubo.ambColor*Cd;
	
	vec3 computed_col = col1*(f_diff1+f_spec1) + col2*(f_diff2+f_spec2) + scol1*(f_diff_s1+f_spec_s1) + scol2*(f_diff_s2+f_spec_s2) + scol3*(f_diff_s3+f_spec_s3) + scol4*(f_diff_s4+f_spec_s4) + ambient ;
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
	outColor = vec4(CompColor, 1.0);//vec4(CompColor, 1.0f);
}