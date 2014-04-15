#version 130 

//these are the interpolated values out of the rasterizer, so you can't know
//their specific values without knowing the vertices that contributed to them
in vec3 fs_color;
in vec4 fs_normal;

out vec4 out_Color;

void main() {
    
    out_Color = vec4(fs_color, 1.0);
}