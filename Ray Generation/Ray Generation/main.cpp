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

int main(int argc, char** argv) {
	unsigned int width = 800;
	unsigned int height = 600;

	BMP output;
	output.SetSize(width, height);
	output.SetBitDepth(24);
	
	for(unsigned int x = 0; x < width; x++) {
		for(unsigned int y = 0; y < height; y++) {
			if(x % 2 == 0) {
				if(y % 2 == 0) {
					output(x, y)->Red = 255;
					output(x, y)->Blue = 0;
					output(x, y)->Green = 0;
				}
				else {
					output(x, y)->Red = 0;
					output(x, y)->Blue = 255;
					output(x, y)->Green = 0;
				}
			}
			else {
				if(y % 2 == 0) {
					output(x, y)->Red = 0;
					output(x, y)->Blue = 255;
					output(x, y)->Green = 0;
				}
				else {
					output(x, y)->Red = 255;
					output(x, y)->Blue = 0;
					output(x, y)->Green = 0;
				}
			}
		}
	}

	output.WriteToFile("output.bmp");
	return 0;
}