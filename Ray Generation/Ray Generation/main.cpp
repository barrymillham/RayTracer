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
	unsigned int width = 800;
	unsigned int height = 600;

	BMP output;
	output.SetSize(width, height);
	output.SetBitDepth(24);

	/*Pseudocode for what we need to do for this milestone

	//Ray equation: R = E + t(P-E)
	For x = 0 to width - 1					//  For every pixel
		for y = 0 to height - 1				// ^^^
			//Calculate Ray Direction based off of x,y
			
			P = M + (2*x/(width-1)-1)*H + (2*y/(height-1)-1)*V
			D = (P-E)/|P-E|					//D is direction of the ray. E is origin point, P is point somewhere along the ray
			pixel(x,y)->Red = D.x
			pixel(x,y)->Green = D.y
			pixel(x,y)->Blue = D.z
	*/

	/*		~Extra notes~  
	(0,0) is the top left corner of the bmp
	//Look in powerpoints notes: Raytracing part 3 
	V = up * tan(phi)
	H = -u * tan(theta)
	
	
	
	
	*/


	//Yeah this doesn't work. Worth a shot.
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			vec3 E = vec3(x,y,0);
			//vec3 P = vec3(x,y,1);

			//what is M? Also, what is up, phi, u, and theta, to be used in calculating H and V?
			//vec3 P = M + (2*x/(width-1)-1)*H + (2*y/(height-1)-1)*V;

			vec3 D = P - E;
			D = normalize(D);
			output(x,y)->Red = D.x;
			output(x,y)->Green = D.y;
			output(x,y)->Blue = D.z;
		}
	}
	
	
	output.WriteToFile("output.bmp");
	return 0;
}