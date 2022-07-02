#version 450

layout(set=0, binding = 0) uniform GlobalUniformBufferObject {
	mat4 view;
	mat4 proj;
	vec3 lightPos1; //the position of the light.
	vec3 lightPos2; //the position of the light.
	vec3 spotDir[4]; //direction of light
	vec3 spotPositions[4];
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
	
	const int n_spot = 4;
	
	//Point light dir
	vec3 dir1 = normalize(gubo.lightPos1 - fragPos);
	vec3 dir2 = normalize(gubo.lightPos2 - fragPos);
	
	//Spot light dir
	vec3 sdir[n_spot];
	for(int i=0; i<n_spot; i++){
		sdir[i] = normalize(gubo.spotPositions[i] - fragPos);
	}
	
	//Point light color
	vec3 col1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.lightPos1), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	vec3 col2 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.lightPos2), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	
	//Spot light Color
	vec3 scol[n_spot];
	vec3 fact1;
	float fact2;
	for(int i=0; i<n_spot; i++){
		fact1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.spotPositions[i]), gubo.coneInOutDecayExp.w) * gubo.lightColor;
		fact2 = clamp((dot(normalize(gubo.spotPositions[i] - fragPos), gubo.spotDir[i]) - gubo.coneInOutDecayExp.x) / (gubo.coneInOutDecayExp.y - gubo.coneInOutDecayExp.x),
	                    0,
						1);
		scol[i] = fact1 * fact2;
	}
	
	//Lambert
	float cos_a1 = dot(N, dir1);
	float cos_a2 = dot(N, dir2);
	
	float cos_a_spot[n_spot];
	for(int i=0; i<n_spot; i++){
		cos_a_spot[i] = dot(N, sdir[i]);
	}
	
	vec3 f_diff1 = Cd * clamp(cos_a1, 0, 1);
	vec3 f_diff2 = Cd * clamp(cos_a2, 0, 1);
	
	vec3 f_diff_spot[n_spot];
	for(int i=0; i<n_spot; i++){ 
		f_diff_spot[i] = Cd * clamp(cos_a_spot[i], 0, 1);
	}
	
	//Phong
	vec3 r_lx1 = -reflect(dir1, N);
	vec3 r_lx2 = -reflect(dir2, N);
	
	vec3 r_lx_spot[n_spot];
	for(int i=0; i<n_spot; i++){
		r_lx_spot[i] = -reflect(sdir[i], N);
	}
	
	vec3 f_spec1 = Cs * pow(clamp (dot(V,r_lx1), 0,1), gamma);
	vec3 f_spec2 = Cs * pow(clamp (dot(V,r_lx2), 0,1), gamma);
	
	vec3 f_spec_spot[n_spot];
	for(int i=0; i<n_spot; i++){
		f_spec_spot[i] = Cs * pow(clamp (dot(V,r_lx_spot[i]), 0,1), gamma);
	}

	vec3 spot_contrib = vec3(0.0f, 0.0f, 0.0f);
	for(int i=0; i<n_spot; i++){
		spot_contrib = spot_contrib + scol[i]*(f_diff_spot[i] + f_spec_spot[i]);
	}
	
	//Ambient
	//vec3 ambient  = (vec3(0.1f,0.1f, 0.1f) * (1.0f + N.y) + vec3(0.0f,0.0f, 0.1f) * (1.0f - N.y)) * Cd;
	vec3 ambient = gubo.ambColor*Cd;
	
	vec3 computed_col = col1*(f_diff1 + f_spec1) +col2 * (f_diff2 + f_spec2) + spot_contrib + ambient ; 
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