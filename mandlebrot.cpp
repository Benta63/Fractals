#include <iostream>
#include <fstream> 
#include <complex> //Fractals have complex numbers I think
#include <stdlib.h>
#include <time.h>

using namespace std;

// Making random colors
void rgb_color_code(int rgb[]);

void rgb_color_code(int rgb[]) {
	for (int primary = 0; primary < 3; primary++) {
		rgb[primary] = rand()%256;
	}
}


int value (int x_coord, int y_coord, float width, float height) {
	complex<float> point ((float)x_coord/width-1.5, (float)y_coord/height-0.5);
	complex<float> z(0,0);

		unsigned int iter = 0;
		while (abs(z) < 2 && iter <= 150) {
			z = z * z + point;
			iter++;
		}
		if (iter < 150) return 255 *iter/200;
		else return 0;
}


void make_mandelbrot() {
	float width = 600;
	float height = 600;


	float green_diff = 100;
	float red_diff = 1;
	float blue_diff = 10;

	unsigned color_depth = 255;
    ofstream image ("mandelbrot.ppm");

	if (image.is_open()) {
		image << "P3\n" << width << " " << height << " 255" <<endl;
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				int val = value(i, j, width, height);
				int color[3] = {0,0,0};
				int primary = rand()%3;
				color[0] = val / red_diff;
				color[1] = val / green_diff;
				color[2] = val / blue_diff;

				image << color[0] << ' ' << color[1] << ' ' << color[2] << endl;
			}
		}
		image.close();
	}
	else cout << "Could not open file";
}

int main() {

	// Defining the image dimensions
	make_mandelbrot();

	return 0;
}