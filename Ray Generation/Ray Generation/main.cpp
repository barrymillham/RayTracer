/**
 * An example program that creates a 24-bit bitmap file that is 800 x 600 and makes a blue/red checkerboard pattern.
 * Uses EasyBMP
 *
 * Cory Boatright
 * University of Pennsylvania, Fall 2011
 **/

#include "EasyBMP.h"
#include "../glm/gtc/matrix_transform.hpp"

using namespace std;
using namespace glm;

int main(int argc, char** argv) {
	unsigned int width = 800; //V
	unsigned int height = 600; //H

	//build a fake camera
	vec3 eye = vec3(0,0,0);
	vec3 M = vec3(0,0,1);
	vec3 C = M - eye;
	vec3 UP = vec3(0,1,0);
	vec3 V = UP;
	V *= glm::tan(glm::radians(45.0f));
	mat4 rotationMatrix = glm::rotate(mat4(1.0f), 90.0f, C);
	vec4 H4 = vec4(V.x, V.y, V.z, 0.0f) * rotationMatrix;
	H4 *= 1.333f;
	vec3 H = vec3(H4.x, H4.y, H4.z);

	BMP output;
	output.SetSize(width, height);
	output.SetBitDepth(24);

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			vec3 rayPositionH = H;
			vec3 rayPositionV = V;
			rayPositionH *= (2 * (double)x) / (width - 1) - 1;
			rayPositionV *= (2 * (double)y) / (height - 1) - 1;
			vec3 P = M + rayPositionH + rayPositionV;
			
			//D = (P-E)/|P-E|
			vec3 D = normalize(P - eye);
			output(x,y)->Red = glm::abs(D.x*255);
			output(x,y)->Green = glm::abs(D.y*255);
			output(x,y)->Blue = glm::abs(D.z*255);
		}
	}

	output.WriteToFile("output.bmp");
	return 0;
}