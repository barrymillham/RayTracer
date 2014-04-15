#version 130

uniform int u_ambientOnly; // boolean switch - turns off advanced lighting if 1

in vec3 fs_color;
in vec3 fs_light;
in vec3 fs_normal;

out vec4 out_Color;

void main() {
    vec3 ambientContrib = fs_color * 0.1;
	vec4 diffuseColor = vec4(fs_color, 1.0);

	// Diffuse lighting
	float diffuseTerm = max(dot(fs_light, fs_normal), 0); // Lambert's equation

	//out_Color = diffuseColor;
	
	// Send either diffuse + ambient, or ambient only as final color based on u_ambientOnly switch
	if(u_ambientOnly == 0)
		out_Color = diffuseTerm * diffuseColor + ambientContrib;
	else
		out_Color = vec4(fs_color, 1.0);
}