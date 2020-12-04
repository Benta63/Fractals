#include <iostream>
#include <fstream> 
#include <complex> //Fractals have complex numbers I think

using namespace std;

int value (int x_coord, int y_coord, float width, float height) {
	complex<float> point ((float)x_coord/width-1.5, (float)y_coord/height-0.5);
	complex<float> z(0,0);

		unsigned int iter = 0;
		while (abs(z) < 2 && iter <= 50) {
			z = z * z + point;
			iter++;
		}
		if (iter < 50) return 996 *iter/38;
		else return 0;
}


int main() {

	// Defining the image dimensions
	float width = 600;
	float height = 600;

    ofstream image ("mandelbrot.ppm");
    



	if (image.is_open()) {
		image << "P3\n" << width << " " << height << " 255" <<endl;
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				int val = value(i, j, width, height);
				image << val << ' ' << 0 << ' ' << 0 << endl;
			}
		}
		image.close();
	}
	else cout << "Could not open file";

	return 0;
}