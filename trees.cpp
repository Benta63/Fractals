#include <windows.h>
#include <string>
#include <math.h>

using namespace std;

const float PI = 3.141592653f;

class Bitmap {
public:
	Bitmap() : pen( NULL) {}
	//Destructor
	~Bitmap() {
		DeleteObject( pen );
		DeleteDC( hdc );
		DeleteObject( bmp );

	}

	bool create( int Iwidth, int Iheight) {
		BITMAPINFO bi;
		void *pBits;
		ZeroMemory(&bi, sizeof (bi));
		bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
		bi.bmiHeader.biBitCount = sizeof (DWORD) * 8;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biWidth = Iwidth;
		bi.bmiHeader.biHeight = -Iheight;

		HDC dc = GetDC (GetConsoleWindow() );
		bmp = CreateDIBSection (dc, &bi, DIB_RGB_COLORS, &pBits, NULL, 0);
		if (!bmp) {
			return false;
		}

		hdc = CreateCompatibleDC( dc );
		SelectObject(hdc, bmp);
		ReleaseDC(GetConsoleWindow(), dc);

		width = Iwidth;
		height = Iheight;
	
		return true;
	}

	void setPenColor( DWORD clr) {
		if(pen) DeleteObject(pen);
		pen = CreatePen(PS_SOLID, 1, clr);
		SelectObject(hdc, pen);
	}

	void saveBitmap( string path ) {
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

	HDC getDC() { return hdc; }
	int getWidth() { return width; }
	int getHeight() { return height; }

private:
	HBITMAP bmp;
	HDC hdc;
	HPEN pen;
	int width, height;
};

class myVector {
public:
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
	fTree() { _angle = DegtoRadian( 22.0f); }
	float DegtoRadian(float degree) {return degree * (PI / 180.0f); }

	void create(Bitmap* bmp) {
		_bmp = bmp;
		float line_len = 140.0f;

		myVector sp( _bmp->getWidth() / 2, _bmp->getHeight() - 1);
		MoveToEx( _bmp->getDC(), sp.x, sp.y, NULL );
		sp.y -= static_cast<int>(line_len);
		LineTo( _bmp->getDC(), sp.x, sp.y);

		drawRL(&sp, line_len, 0, true);
		drawRL(&sp, line_len, 0, false);
	}
private:
	void drawRL(myVector * sp, float line_len, float t, bool rg) {
		line_len *= 0.71f;
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
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
	Bitmap bmp;
	bmp.create(600, 600);
	bmp.setPenColor(RGB(51, 124, 36));
	fTree tree;
	tree.create(&bmp);
	BitBlt(GetDC(GetConsoleWindow() ), 0, 20, 600, 600, bmp.getDC(), 0, 0, SRCCOPY);

	bmp.saveBitmap("fractree.bmp");

	system("pause");
	return 0;
}