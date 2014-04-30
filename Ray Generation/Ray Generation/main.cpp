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

	//Changed the way the image was being generated so we could more clearly see the bitmap coordinate system:
	//(0,0) is the top left corner of the bmp as this code demonstrates:
	for (unsigned int x = 0; x < width; x++) {
		for (unsigned int y = 0; y < height; y++) {
			output(x, y)->Red = 255;
			output(x, y)->Blue = 255;
			output(x, y)->Green = 0;
		}
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			output(i, j)->Green = 255;
			output(i, j)->Red = 0;
			output(i, j)->Blue = 0;
		}
	}

	output.WriteToFile("output.bmp");
	return 0;
}