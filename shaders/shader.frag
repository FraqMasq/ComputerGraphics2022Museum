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

vec3 compute_diff_lambert(vec3, vec3, vec3 , vec3 );
vec3 compute_spec_phong(vec3, vec3, vec3, vec3, vec3, float);
vec3 compute_color_spot(float, float, vec3, vec3, vec3, vec3, float, float);

vec3 compute_Light(vec3 N, vec3 V, vec3 Cd, vec3 Cs, float gamma) {
	// Lambert + Phong specular + point light + Ambient
	// Parameters are:
	//
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 Cd : main color (diffuse color)
	// vec3 Cs : specular Color
	// float gamma: specular power
	
	//Lambert diffuse
	
	vec3 f_diff1 = compute_diff_lambert(N, fragPos, gubo.lightPos1 ,Cd);
	vec3 f_diff2 = compute_diff_lambert(N, fragPos, gubo.lightPos2 ,Cd);
	vec3 f_diff_s1 = compute_diff_lambert(N, fragPos, gubo.spotPosition1 ,Cd);
	vec3 f_diff_s2 = compute_diff_lambert(N, fragPos, gubo.spotPosition2 ,Cd);
	vec3 f_diff_s3 = compute_diff_lambert(N, fragPos, gubo.spotPosition3 ,Cd);
	vec3 f_diff_s4 = compute_diff_lambert(N, fragPos, gubo.spotPosition4 ,Cd);
	
	//Phong specular
	
	vec3 f_spec1 = compute_spec_phong(N, fragPos, gubo.lightPos1, Cs, V, gamma);
	vec3 f_spec2 = compute_spec_phong(N, fragPos, gubo.lightPos2, Cs, V, gamma);
	vec3 f_spec_s1 = compute_spec_phong(N, fragPos, gubo.spotPosition1, Cs, V, gamma);
	vec3 f_spec_s2 = compute_spec_phong(N, fragPos, gubo.spotPosition2, Cs, V, gamma);
	vec3 f_spec_s3 = compute_spec_phong(N, fragPos, gubo.spotPosition3, Cs, V, gamma);
	vec3 f_spec_s4 = compute_spec_phong(N, fragPos, gubo.spotPosition4, Cs, V, gamma);
	
	//Point light color
	vec3 col1 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.lightPos1), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	vec3 col2 = pow(gubo.coneInOutDecayExp.z/length(fragPos - gubo.lightPos2), gubo.coneInOutDecayExp.w) * gubo.lightColor;
	
	//Spot light color
	
	vec3 scol1 = compute_color_spot(gubo.coneInOutDecayExp.w, gubo.coneInOutDecayExp.z, fragPos, gubo.spotPosition1, gubo.lightColor, gubo.spotDirection2,
	                                gubo.coneInOutDecayExp.x, gubo.coneInOutDecayExp.y);
	vec3 scol2 = compute_color_spot(gubo.coneInOutDecayExp.w, gubo.coneInOutDecayExp.z, fragPos, gubo.spotPosition2, gubo.lightColor, gubo.spotDirection2,
	                                gubo.coneInOutDecayExp.x, gubo.coneInOutDecayExp.y);
	vec3 scol3 = compute_color_spot(gubo.coneInOutDecayExp.w, gubo.coneInOutDecayExp.z, fragPos, gubo.spotPosition3, gubo.lightColor, gubo.spotDirection3,
	                                gubo.coneInOutDecayExp.x, gubo.coneInOutDecayExp.y);
	vec3 scol4 = compute_color_spot(gubo.coneInOutDecayExp.w, gubo.coneInOutDecayExp.z, fragPos, gubo.spotPosition4, gubo.lightColor, gubo.spotDirection1,
	                                gubo.coneInOutDecayExp.x, gubo.coneInOutDecayExp.y);
	
	//Ambient
	//vec3 ambient  = (vec3(0.1f,0.1f, 0.1f) * (1.0f + N.y) + vec3(0.0f,0.0f, 0.1f) * (1.0f - N.y)) * Cd;
	vec3 ambient = gubo.ambColor*Cd;
	
	vec3 computed_col = col1*(f_diff1+f_spec1) + col2*(f_diff2+f_spec2) + scol1*(f_diff_s1+f_spec_s1) + scol2*(f_diff_s2+f_spec_s2) + scol3*(f_diff_s3+f_spec_s3) + scol4*(f_diff_s4+f_spec_s4) + ambient ;
	return computed_col;
}

//computes diffuse component of light with Lambert

vec3 compute_diff_lambert(vec3 N, vec3 pos, vec3 lightPos, vec3 Cd){
	vec3 direction = normalize(lightPos - pos);
	
	float cos_a = dot(N, direction);
	
	vec3 f_diffuse = Cd * clamp(cos_a, 0, 1);
	
	return f_diffuse;
}

//compute specular component of light with phong
vec3 compute_spec_phong(vec3 N, vec3 pos, vec3 lightPos, vec3 Cs, vec3 V, float gamma){
	vec3 direction = normalize(lightPos - pos);
	
	vec3 r_lx = -reflect(direction, N);
	
	vec3 f_specular =  Cs * pow(clamp (dot(V,r_lx), 0,1), gamma);
	
	return f_specular;
}

//compute the color 
vec3 compute_color_spot(float beta, float g, vec3 pos, vec3 lightPos, vec3 lightColor, vec3 lightDir, float cos, float sin){
//isSpot deletes the second factor if the light is not a spot light
	vec3 fact1 = pow(beta/length(pos - lightPos), g) * lightColor;
	float fact2 = clamp((dot(normalize(lightPos - pos), lightDir) - cos) / (sin - cos),
						0,
						1);
	vec3 col = fact1 * fact2;
	
	return col;
}




void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(fragViewDir);
	float gamma = 200.0f;
	//float AmbFact = 0.025;
	
	vec3 DifCol = texture(texSampler, fragTexCoord).rgb;
	//vec3 SpecCol = texture(spec_texSampler, fragTexCoord).rgb;
	vec3 SpecCol = vec3(1.0f, 1.0f, 1.0f);


	vec3 CompColor = compute_Light(Norm, EyeDir, DifCol, SpecCol,  gamma) ;
	outColor = vec4(CompColor, 1.0);//vec4(CompColor, 1.0f);
}