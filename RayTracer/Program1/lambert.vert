#version 130

uniform mat4 u_modelMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_cameraMatrix;
uniform vec4 u_lightPos;
uniform vec3 u_color;

in vec4 vs_position;
//in vec3 vs_color;
in vec3 vs_normal;

out vec3 fs_color;
out vec3 fs_light;
out vec3 fs_normal;

void main() {
	fs_color = u_color;
	//fs_color = (u_projMatrix * u_cameraMatrix * u_modelMatrix * vs_normal).xyz; // DEBUG - for checking normals
	fs_normal = normalize((u_cameraMatrix * u_modelMatrix * vec4(vs_normal,0)).xyz);
	
	// Calculate a normal vector pointing from the vertex to the light source
	fs_light = normalize((u_cameraMatrix * u_lightPos - u_cameraMatrix * u_modelMatrix * vs_position).xyz);
    
    // built-in things to pass down the pipeline
    gl_Position = u_projMatrix * u_cameraMatrix * u_modelMatrix * vs_position;
}