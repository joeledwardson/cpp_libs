/****************************************************************************************** 
 *	Chili DirectX Framework Version 12.04.24											  *	
 *	D3DGraphics.h																		  *
 *	Copyright 2012 PlanetChili <http://www.planetchili.net>								  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdlib/platform.h>	//win32_mean_and_lean to prevent windows.h including winsock.h
#include "gmelib/gametypes.h"	//struct line
#include "stdlib/error.h"
#include "stdlib/thread.h"
// **winsys.h**
#include <assert.h>	 //assertions
#include <Windows.h> //win32 calls
#include <process.h> //threading
//*************

#include <vector>
#include <math.h>
#include "bitmap.h"
#include <d3d9.h>

#include "stdlib/thread.h"
#include <stdint.h>

//screen dimensions
#define SCREEN_WIDTH	1200
#define SCREEN_HEIGHT	800

//font IDs, to pass to calls to draw fonts
#define FONT_BIG 0
#define FONT_SMALL 1
#define FONT_NFONTS 2

//angle types to pass to calculate angle



//useful pre-defined colours
#define RED		D3DCOLOR_XRGB(255,0,0)
#define BLUE	D3DCOLOR_XRGB(0,0,255)
#define GREEN	D3DCOLOR_XRGB(0,255,0)
#define YELLOW	D3DCOLOR_XRGB(180,180,0)
#define BLACK	D3DCOLOR_XRGB(0,0,0)

//loading icon colours
#define LOAD_RED 0
#define LOAD_GREEN 127
#define LOAD_BLUE 85
//loading icon dimensions
#define LOADICO_OUTRAD	100
#define LOADICO_INRAD	50

class D3DGraphicsClient;

class Circle {
	friend D3DGraphicsClient;
public:
	Circle( unsigned int outerRadius,unsigned int innerRadius, ColComp red, ColComp green, ColComp blue );
	Circle( unsigned int outerRadius,unsigned int innerRadius, D3DCOLOR col );

	virtual ~Circle();
protected:
	//polar and cartesian point
	struct PCPoint {
		short x,y;
		float theta;
	} ;
	unsigned int outR,inR; //radii
	D3DCOLOR c;
	std::vector< PCPoint >pointList;

	
};

struct Font {
	const int charWidth;
	const int charHeight;
	const int nCharsPerRow;
	
	D3DCOLOR* surface;
	Font(int charactersPerRow,  const char * filename,int characterHeight, int characterWidth);
	//equals operator. important as fonts are copied using = when initialised
	~Font();
};

class D3DGraphics {
	friend D3DGraphicsClient;
	
public:
	
	D3DGraphics( HWND hWnd );
	~D3DGraphics();
		
	
	void ClearGraphicsFrame();
	void EndGraphicsFrame(RECT * dest=NULL);
	void WipeScreen( D3DCOLOR col );

private:
	D3DCOLOR* pSysBuffer;
	Font	*	fonts[FONT_NFONTS];
	IDirect3D9*			pDirect3D;
	IDirect3DDevice9*	pDevice;
	IDirect3DSurface9*	pBackBuffer;
	D3DLOCKED_RECT		backRect;
	Circle loadIco;
	float loadTheta;
	bool loadIcoActive;
	Thread_Params loadIcoThread;
	RECT icoDest;
};


class D3DGraphicsClient {

	
public:
	
	D3DGraphicsClient( D3DGraphics& ref );
	void PutPixel( int x,int y,int r,int g,int b );			//put pixel using rgb colour
	void PutPixel( int x,int y,D3DCOLOR c );				//put pixel using D3DCOLOR colour
	void PutPixelAlpha( int x, int y, D3DCOLOR c );			//put pixel (calculating alpha with respect to existing pixel)
	D3DCOLOR GetPixel( int x, int y );						//get current pixel
	void DrawVLine( Line line,D3DCOLOR color );				//draw vertical line (can also be horizontal)
	void DrawVLine( int x1, int x2, int y1, int y2,D3DCOLOR color );	//draw vertical line (can also be horizontal)
	void DrawVLine_VP( int x, int y, Line line, D3DCOLOR color );		//draw line variable position, (may be off the screen)
	void DrawRectangle( int x, int y, int width, int height, D3DCOLOR colour);
	void DrawRectangle( int x, int y, int width, int height, int r, int g, int b);
	void DrawBitmap(int x, int y,int width, int height, D3DCOLOR background, D3DCOLOR* pic);
	void DrawChar( char c,int x,int y,int fontType,D3DCOLOR color, int enlargment );				//draw character
	void DrawString( const std::string& str,int x,int y,int fontType,D3DCOLOR color, int enlargment );	//draw string
	D3DCOLOR FadePixel(D3DCOLOR c, float fade );
	static D3DCOLOR GenRandCol( byte maxCol );				//generate random colour, pass number up to 255 for max value
	static D3DCOLOR CalcFadedColour( byte r, byte g, byte b, float fade );	//fade colour 1 > fade var > 0 
	void DrawCircle( Circle& c, int x, int y );
	void DrawLoadIco( int x, int y);




	void Begin_DrawLoadIco( int x, int y );					//start a thread drawing the load icon
	void End_DrawLoadIco();									//end drawing loading icon thread

private:
	void LoadIcoUpdate();
	static void DrawLoadIco_Wrap( void *  loadicoargs);		//static function which draws load icon. paramter should be of format loadicoargs

	D3DGraphics& rGraphics;	
};

#endif

