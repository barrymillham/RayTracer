#version 130

//there's no rules about the prefixes but they do help
//keep track of things
uniform mat4 u_modelMatrix;
uniform mat4 u_projMatrix;
uniform vec4 u_lightLocation;

in vec4 vs_position;
in vec3 vs_color;
in vec4 vs_normal;

out vec3 fs_color;
out vec4 fs_normal;

void main() {
	// Set the color and normal as just the input from the VBOs,
	// they only get interesting after they've been interpolated by the rasterizer
    //fs_color = vs_color;
	fs_color = (u_projMatrix * u_modelMatrix * vs_normal).xyz; // for debugging!
    
    //built-in things to pass down the pipeline
    gl_Position = u_projMatrix * u_modelMatrix * vs_position;
}