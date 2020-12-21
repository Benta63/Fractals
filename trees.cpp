#include <windows.h> //Make sure to include -mwindows 
#include <string>
#include <math.h> 
#include <complex>
#include <iostream>


// NOTE: Where I write "change this" It means that 
using namespace std;

const float PI = 3.141592653f;
const int BMP_SIZE = 600;
const int ITTERATIONS = 512;
const long double FCT = 2.68, hFCT = FCT / 2.0; //hFCT = 1.34

class Bitmap {
public:
	Bitmap() : pen( NULL) {}
	//Destructor
	~Bitmap() {
		DeleteObject( pen );
		DeleteObject( brush );
		DeleteDC( hdc );
		DeleteObject( bmp );

	}

	bool create( int Iwidth, int Iheight) {
		// Creates a custom Bitmap object. This is kind of based on the BITMAP library
		BITMAPINFO bi;
		void *pBits;
		ZeroMemory(&bi, sizeof (bi));
		// Typical BITMAP things
		bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
		bi.bmiHeader.biBitCount = sizeof (DWORD) * 8;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biWidth = Iwidth;
		bi.bmiHeader.biHeight = -Iheight;

		// We are printing the bitmap to the console window
		HDC dc = GetDC (GetConsoleWindow() );
		bmp = CreateDIBSection (dc, &bi, DIB_RGB_COLORS, &pBits, NULL, 0);
		if (!bmp) {
			return false;
		}

		hdc = CreateCompatibleDC( dc ); // Makes memory for the bitmap
		SelectObject(hdc, bmp); // Associates the memory created with the bitmap
		ReleaseDC(GetConsoleWindow(), dc); // Frees the memory

		width = Iwidth;
		height = Iheight;
	
		return true;
	}

	void clear(BYTE clr = 0) {
		memset(pBits, clr, height * width * sizeof(DWORD));
	}
	void setPenColor( DWORD c) {
		clr = c;
		createPen();
	}
	void setPenWidth(int w) {
		wid = w;
		createPen();
	}

	void setBrushColor(DWORD bClr) {
		if (brush) DeleteObject( brush );
		brush = CreateSolidBrush( bClr );
		SelectObject( hdc, brush );
	}

	void saveBitmap( string path ) {
		// Saving all the important info of the BITMAP to a file. 
		BITMAPFILEHEADER fileheader;
		BITMAPINFO infoheader;
		BITMAP bitmap;
		DWORD* dwBits;
		DWORD wb;
		HANDLE file;

		GetObject(bmp, sizeof(bitmap), &bitmap);

		dwBits = new DWORD[bitmap.bmWidth * bitmap.bmHeight];
		ZeroMemory (dwBits, bitmap.bmWidth * bitmap.bmHeight * sizeof(DWORD) );
		ZeroMemory(&infoheader, sizeof(BITMAPINFO) );
		ZeroMemory(&fileheader, sizeof(BITMAPFILEHEADER) );

		infoheader.bmiHeader.biBitCount = sizeof(DWORD) * 8;
		infoheader.bmiHeader.biCompression = BI_RGB;
		infoheader.bmiHeader.biPlanes = 1;
		infoheader.bmiHeader.biSize = sizeof(infoheader.bmiHeader);
		infoheader.bmiHeader.biHeight = bitmap.bmHeight;
		infoheader.bmiHeader.biWidth = bitmap.bmWidth;
		infoheader.bmiHeader.biSizeImage = bitmap.bmWidth * bitmap.bmHeight * sizeof(DWORD);

		fileheader.bfType = 0x4D42; //hex for WORD bfType Decimal is 19778
		fileheader.bfOffBits = sizeof(infoheader.bmiHeader) + sizeof(BITMAPFILEHEADER);
		fileheader.bfSize = fileheader.bfOffBits + infoheader.bmiHeader.biSizeImage;

		GetDIBits( hdc, bmp, 0, height, (LPVOID)dwBits, &infoheader, DIB_RGB_COLORS);

		file = CreateFile(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile( file, &fileheader, sizeof(BITMAPFILEHEADER), &wb, NULL);
		WriteFile( file, &infoheader.bmiHeader, sizeof( infoheader.bmiHeader), &wb, NULL);
		WriteFile( file, dwBits, bitmap.bmWidth * bitmap.bmHeight * 4, &wb, NULL);
		CloseHandle (file); //more efficient way of doing things

		delete [] dwBits;

	}

	HDC getDC() const{ return hdc; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	DWORD * bits() const {return (DWORD*)pBits; }

private:
	HBITMAP bmp;
	HDC hdc;
	HPEN pen;
	HBRUSH brush;
	int width, height, wid;
	void *pBits;
	DWORD clr;
	void createPen() {
		if ( pen ) DeleteObject( pen );
		pen = CreatePen (PS_SOLID, wid, clr);
		SelectObject(hdc, pen);
	} 

};

class julia {
public:
	void draw(complex<long double> p) {
		bmp.create( BMP_SIZE, BMP_SIZE );

		DWORD* bits = bmp.bits();
		int res, pos;
		complex<long double> com, factor(FCT / BMP_SIZE, FCT/BMP_SIZE);

		for (int y = 0; y < BMP_SIZE; y++) {
			pos = y * BMP_SIZE;
			com.imag( (factor.imag() * y) - hFCT );

			for (int x = 0; x < BMP_SIZE; x++ ) {
				com.real(factor.real() * x -FCT );
				res = inSet ( com, p );
				if (res) {
					int num_res = res % 255; //255 is the max of a color
					if (res < (ITTERATIONS >> 1)) {
						res = RGB( num_res << 2, num_res << 3, num_res << 4);
					}
					else {
						res = RGB(num_res << 4, num_res << 1, num_res << 5);
					}
				} //I may have gotten the bit shifts wrong.
				bits[pos++] = res;
			}
		}
		bmp.saveBitmap("./julia.bmp");
	}
private:
	int inSet (complex<long double> z, complex<long double> c) {
		long double dist;
		for (int itr = 0; itr < ITTERATIONS; itr++) {
			z = z*z;
			z = z + c;
			dist = (z.imag() * z.imag() ) + (z.real() + z.real() );
			if (dist > 3) {
				return(itr);
			}
		}
		return 0;
	}
	Bitmap bmp;
};



class myVector {
public:
	// What do each of the lines look like?
	myVector() { x=y=0; }
	myVector( int w, int h) {x=w; y=h; }
	void set( int w, int h ) {x=w; y=h; }
	void rotate( float angle_r )
	{
		float _x = static_cast<float>( x ),
			_y = static_cast<float>( y ),
			s = sinf( angle_r ),
			c = cosf( angle_r ),
			w = _x * c - _y * s,
			h = _x * s + _y * c;
		x = static_cast<int>( w );
		y = static_cast<int>( h );
	}
	int x, y;
};

class fTree {
public:
	// Degree of the branches. Change this.

	fTree() { _angle = DegtoRadian( 22.0f); }
	float DegtoRadian(float degree) {return degree * (PI / 180.0f); }

	void create(Bitmap* bmp) {
		_bmp = bmp;
		//Begining line length. Change this.
		float line_len = 110.0f;

		myVector sp( _bmp->getWidth() / 2, _bmp->getHeight() - 1);
		MoveToEx( _bmp->getDC(), sp.x, sp.y, NULL );
		sp.y -= static_cast<int>(line_len);
		LineTo( _bmp->getDC(), sp.x, sp.y);

		drawRL(&sp, line_len, 0, true);
		drawRL(&sp, line_len, 0, false);
	}
private:
	void drawRL(myVector * sp, float line_len, float t, bool rg) {
		// drawing the line. Change these.
		line_len *= 0.75f;
		if (line_len < 2.0f) return;

		MoveToEx(_bmp->getDC(), sp->x, sp->y, NULL);
		myVector r(0, static_cast<int>(line_len));

		if(rg) t -= _angle;
		else t += _angle;

		r.rotate( t );
		r.x += sp->x; r.y = sp->y - r.y;
		LineTo(_bmp->getDC(), r.x, r.y);

		drawRL(&r, line_len, t, true);
		drawRL(&r, line_len, t, false);

	}
	Bitmap* _bmp;
	float _angle;
};

int main(int argc, char* argv[]) {

	//How to draw Julia Set
	// complex <long double> c;
	// long double factor = FCT /BMP_SIZE;
	// c.imag( ( factor * 184) + -1.4);
	// c.real( (factor * 307) + -2.0);
	// julia j;
	// j.draw(c);

	// How to draw a tree
	// ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
	Bitmap bmp;
	bmp.create(BMP_SIZE, BMP_SIZE); // Size of the tree
	bmp.setPenColor(RGB(51, 124, 36)); //RGB color of the tree
	fTree tree;
	tree.create(&bmp);
	//Printing the tree to the console window

	bmp.saveBitmap("fractree.bmp"); //Saving to a file. bmp should be detailed enough

	return 0;
}