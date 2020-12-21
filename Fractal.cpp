#include <windows.h>
#include <string>
#include <complex>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
 

const int BMP_SIZE = 600, ITERATIONS = 1024;
const long double FCT = 2.82, hFCT = FCT / 2.0;

const float PI = 3.141592653f;


class Bitmap {
public:
    Bitmap() : pen( NULL ), brush( NULL ), clr( 0 ), wid( 1 ) {}
    //Destructor
    ~Bitmap() {
        DeleteObject( pen );
        DeleteObject( brush );
        DeleteDC( hdc );
        DeleteObject( bmp );
    }
    bool create( int w, int h ) {
        //Drawing stuff
        BITMAPINFO bi;
        ZeroMemory( &bi, sizeof( bi ) );
        bi.bmiHeader.biSize = sizeof( bi.bmiHeader );
        bi.bmiHeader.biBitCount = sizeof( DWORD ) * 8;
        bi.bmiHeader.biCompression = BI_RGB;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;
        HDC dc = GetDC( GetConsoleWindow() );
        bmp = CreateDIBSection( dc, &bi, DIB_RGB_COLORS, &pBits, NULL, 0 );
        if( !bmp ) return false;
        hdc = CreateCompatibleDC( dc );
        SelectObject( hdc, bmp );
        ReleaseDC( GetConsoleWindow(), dc );
        width = w; height = h;
        return true;
    }
    //Typical Bitmap stuff in case we extend the class to do other stuff
    void clear( BYTE clr = 0 ) {
        memset( pBits, clr, width * height * sizeof( DWORD ) );
    }
   
    void setPenColor( DWORD c ) {
        clr = c; createPen();
    }
    void setPenWidth( int w ) {
        wid = w; createPen();
    }
     void setBrushColor( DWORD bClr ) {
        if( brush ) DeleteObject( brush );
        brush = CreateSolidBrush( bClr );
        SelectObject( hdc, brush );
    }

    void saveBitmap( std::string path ) {

        BITMAPFILEHEADER fileheader;
        BITMAPINFO infoheader;
        BITMAP bitmap;
        DWORD wb;
        DWORD* dwBits;
        HANDLE file;


        GetObject( bmp, sizeof( bitmap ), &bitmap );
        dwBits = new DWORD[bitmap.bmWidth * bitmap.bmHeight];
        ZeroMemory( dwBits, bitmap.bmWidth * bitmap.bmHeight * sizeof( DWORD ) );
        ZeroMemory( &infoheader, sizeof( BITMAPINFO ) );
        ZeroMemory( &fileheader, sizeof( BITMAPFILEHEADER ) );
        infoheader.bmiHeader.biBitCount = sizeof( DWORD ) * 8;
        infoheader.bmiHeader.biCompression = BI_RGB;
        infoheader.bmiHeader.biPlanes = 1;
        infoheader.bmiHeader.biSize = sizeof( infoheader.bmiHeader );
        infoheader.bmiHeader.biHeight = bitmap.bmHeight;
        infoheader.bmiHeader.biWidth = bitmap.bmWidth;
        infoheader.bmiHeader.biSizeImage = bitmap.bmWidth * bitmap.bmHeight * sizeof( DWORD );
        fileheader.bfType    = 0x4D42;
        fileheader.bfOffBits = sizeof( infoheader.bmiHeader ) + sizeof( BITMAPFILEHEADER );
        fileheader.bfSize    = fileheader.bfOffBits + infoheader.bmiHeader.biSizeImage;
        GetDIBits( hdc, bmp, 0, height, ( LPVOID )dwBits, &infoheader, DIB_RGB_COLORS );
        file = CreateFile( path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
            FILE_ATTRIBUTE_NORMAL, NULL );
        WriteFile( file, &fileheader, sizeof( BITMAPFILEHEADER ), &wb, NULL );
        WriteFile( file, &infoheader.bmiHeader, sizeof( infoheader.bmiHeader ), &wb, NULL );
        WriteFile( file, dwBits, bitmap.bmWidth * bitmap.bmHeight * 4, &wb, NULL );
        CloseHandle( file );
        delete [] dwBits;
    }
    HDC getDC() const { return hdc; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    DWORD* bits() const { return ( DWORD* )pBits; }
private:
    void createPen() {
        if( pen ) {
            DeleteObject( pen );
        }
        pen = CreatePen( PS_SOLID, wid, clr );
        SelectObject( hdc, pen );
    }
    HBITMAP bmp; 
    HDC hdc;
    HPEN pen; 
    HBRUSH brush;
    void *pBits; 
    int width, height, wid;
    DWORD clr;
};

class julia {
public:
    void draw( std::complex<long double> k ) {
        bmp.create( BMP_SIZE, BMP_SIZE );
        DWORD* bits = bmp.bits();
        int res, pos;
        std::complex<long double> com, factor( FCT / BMP_SIZE, FCT / BMP_SIZE ) ;
 
        for( int y = 0; y < BMP_SIZE; y++ ) {
            pos = y * BMP_SIZE;
 
            com.imag( ( factor.imag() * y ) - hFCT );
 
            for( int x = 0; x < BMP_SIZE; x++ ) {
                com.real( factor.real() * x - hFCT );
                res = inSet( com, k );
                if( res ) {
                    int n_res = res % 255;
                    if( res < ( ITERATIONS >> 1 ) ) {
                        res = RGB( n_res << 2, n_res << 3, n_res << 3 );
                    }
                    else {
                        res = RGB( n_res << 4, n_res << 1, n_res << 5 );
                    }
                }
                bits[pos++] = res; //This is how we draw
            }
        }
        bmp.saveBitmap( "./julia.bmp" );
    }
private:
    int inSet( std::complex<long double> z, std::complex<long double> com ) {
        long double dist;//, three = 3.0;
        for( int ec = 0; ec < ITERATIONS; ec++ ) {
            z = z * z; z = z + com;
            dist = ( z.imag() * z.imag() ) + ( z.real() * z.real() );
            if( dist > 3 ) return( ec );
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

int value_mandel (int x_coord, int y_coord, float width, float height, int color) {
    std::complex<float> point ((float)x_coord/width-1.5, (float)y_coord/height-0.5);
    std::complex<float> z(0,0);

        unsigned int iter = 0;
        while (abs(z) < 2 && iter <= ITERATIONS) {
            z = z * z + point;
            iter++;
        }
        if (iter < ITERATIONS) return 255 *iter/color;
        else return 0;
}

void drawJulia(long double factor_mult1, long double factor_mult2, float offset1, float offset2) {
    std::complex<long double> com;
    long double factor = FCT / BMP_SIZE;
    com.imag( ( factor * factor_mult1) + offset1);
    com.real( ( factor * factor_mult2) + offset2);
    julia j;
    j.draw( com );

}

void drawTree(int red, int green, int blue) {
    Bitmap bmp;
    bmp.create(BMP_SIZE, BMP_SIZE);
    bmp.setPenColor(RGB(red, green, blue));
    fTree tree;
    tree.create(&bmp);
    bmp.saveBitmap("fractree.bmp");
}

void drawMandelbrot(int red, int green, int blue, int color_offset) {
    std::ofstream image ("mandelbrot.ppm");

    if (image.is_open()) {
        image << "P3\n" <<BMP_SIZE << " " << BMP_SIZE << " 255" <<std::endl;
        for (int x = 0; x < BMP_SIZE; x++ ){
            for (int  y = 0; y < BMP_SIZE; y++) {
                int val = value_mandel(x, y, BMP_SIZE, BMP_SIZE, color_offset);
                int color[3] = {0,0,0};
                color[0] = val / red;
                color[1] = val / green;
                color[2] = val / blue;

                image << color[0] << ' ' <<color[1] << ' ' << color[2] <<std::endl;
            }
        } 
        image.close();
    }
    else std::cout << "Could not open file." <<std::endl;
}

int main( int argc, char* argv[] ) {
    
    long double factor1 = 184;
    long double factor2 = 307;
    float offset1 = -1.4;
    float offset2 = -2.0;
    drawJulia(factor1, factor2, offset1, offset2);
    

    int red = 51; int green = 124; int blue = 36;
    drawTree(red, green, blue);

    int color_offset;
    drawMandelbrot(red, green, blue, color_offset);
    return 0;
   
}