/****************************************************************************************** 
 *	Chili DirectX Framework Version 12.04.24											  *	
 *	D3DGraphics.cpp																		  *
 *	Copyright 2012 PlanetChili.net														  *
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
#include "D3DGraphics.h"



Font::Font(int charactersPerRow,  const char * filename,int characterHeight, int characterWidth)
	:charWidth( characterWidth ),
	charHeight( characterHeight ),
	nCharsPerRow( charactersPerRow )
{
	LoadBmp( filename, &surface );

}
Font::~Font()
{
	delete[] surface;
}


D3DGraphics::D3DGraphics( HWND hWnd )
	:pSysBuffer(NULL),
	loadIco( LOADICO_OUTRAD,LOADICO_INRAD,LOAD_RED,LOAD_GREEN,LOAD_BLUE),
	loadTheta( 2 * PI ),
	loadIcoActive(false)
{
	ZeroMemory(fonts,sizeof(fonts));

	HRESULT result;

	backRect.pBits = NULL;
	
	pDirect3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( pDirect3D == NULL ) {
		std::string err = "error: could not create d3d9, "+GetSysError();
		SetCritErr(err);
	}
	assert( pDirect3D != NULL );

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp,sizeof( D3DPRESENT_PARAMETERS ) );

	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    result = pDirect3D->CreateDevice( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,&d3dpp,&pDevice );
	if( FAILED( result ) ){
		std::string err = "error: could not create d3d device";
		switch (result)
		{
		case D3DERR_DEVICELOST:
			err+="D3DERR_DEVICELOST";
			break;
		case D3DERR_INVALIDCALL:
			err+="D3DERR_INVALIDCALL";
			break;
		case D3DERR_NOTAVAILABLE:
			err+="D3DERR_NOTAVAILABLE";
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			err+="D3DERR_OUTOFVIDEOMEMORY";
			break;
		default:
			break;
		}
		SetCritErr(err);
	}
	assert( !FAILED(result));

	result = pDevice->GetBackBuffer( 0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer );
	if( FAILED( result ) ){
		std::string err="cout not get back buffer, "+GetSysError();
		SetCritErr(err);
	}
	assert( !FAILED(result));

	pSysBuffer = new D3DCOLOR[ SCREEN_WIDTH*SCREEN_HEIGHT];
	

	fonts[FONT_SMALL] = new Font(	32,"Consolas13x24.bmp",24,13);
	fonts[FONT_BIG]	=	new Font(	32,"fixedsys16x28.bmp",28,16);
		
	
}
D3DGraphics::~D3DGraphics(){
	if( loadIcoActive )
		loadIcoThread.Parent_EndFunc();

	if( pSysBuffer )
		delete[] pSysBuffer;
	if( pDevice ){
		pDevice->Release();
		pDevice = NULL;
	}
	if( pDirect3D )	{
		pDirect3D->Release();
		pDirect3D = NULL;
	}
	if( pBackBuffer ){
		pBackBuffer->Release();
		pBackBuffer = NULL;
	}
	for( int index = 0; index < FONT_NFONTS; index++ )
		delete fonts[index];
}
void D3DGraphics::ClearGraphicsFrame(){
	memset( pSysBuffer, 0xFF , sizeof( D3DCOLOR) * SCREEN_WIDTH * SCREEN_HEIGHT );
}
void D3DGraphics::EndGraphicsFrame(RECT * dest)
{
	HRESULT result;

	result = pBackBuffer->LockRect( &backRect,NULL,NULL );
	if( FAILED( result ) ) {
		std::string err="failed to lock rect, "+GetSysError();
		SetCritErr(err);
	}

	
	for( int y = 0; y < SCREEN_HEIGHT; y++ ) {
		memcpy( &((BYTE*)backRect.pBits)[backRect.Pitch*y],
				&pSysBuffer[SCREEN_WIDTH*y] ,
				sizeof(D3DCOLOR)*SCREEN_WIDTH);
	}
	

	result = pBackBuffer->UnlockRect();
	if( FAILED( result ) ) {
		std::string err="could not unlock d3d rect, "+GetSysError();
		SetCritErr(err);
	}

	result = pDevice->Present( dest,dest,NULL,NULL );
	if( FAILED( result ) ) {
		std::string err ="could not present d3d device"+GetSysError();
		SetCritErr(err);
	}
}
void D3DGraphics::WipeScreen( D3DCOLOR col ) {
	ClearGraphicsFrame();
	for( unsigned int i=0;i<SCREEN_WIDTH;i++ ) {
		for( unsigned int y=0;y<SCREEN_HEIGHT;y++ ) {
			D3DGraphicsClient(*this).PutPixel(i,y,col);
		}
	}
	EndGraphicsFrame();
}

D3DGraphicsClient::D3DGraphicsClient( D3DGraphics& ref )
	:rGraphics( ref)
{

}
D3DCOLOR D3DGraphicsClient::GetPixel( int x, int y)
{
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < 800 );
	assert( y < 600 );
	return rGraphics.pSysBuffer[ x + (SCREEN_WIDTH * y) ];
}
void D3DGraphicsClient::PutPixelAlpha( int x, int y, D3DCOLOR c )
{
	
	const D3DCOLOR src=	c;
	const D3DCOLOR dst = GetPixel(x,y);
			
	//extract channels
	const byte srcAlpha =	byte((src & 0xFF000000) >> 24	);
	const byte srcRed =		byte((src & 0x00FF0000) >> 16	);
	const byte srcGreen =	byte((src & 0x0000FF00) >> 8	);
	const byte srcBlue =	byte((src & 0x000000FF)			);
		  					
	const byte dstRed =		byte((dst & 0x00FF0000) >> 16	);
	const byte dstGreen =	byte((dst & 0x0000FF00) >> 8	);
	const byte dstBlue =	byte((dst & 0x000000FF)			 );

	//blend channels
	const byte rltRed =		(srcRed*srcAlpha + dstRed*(255-srcAlpha))/255;
	const byte rltGreen =	(srcGreen*srcAlpha + dstGreen*(255-srcAlpha))/255;
	const byte rltBlue =	(srcBlue*srcAlpha + dstBlue*(255-srcAlpha))/255;
			
	//pack channels back into pixel
	PutPixel(x,y,D3DCOLOR_XRGB(rltRed,rltGreen,rltBlue));

}
void D3DGraphicsClient::DrawVLine( int x1, int x2, int y1, int y2,D3DCOLOR color )
{
	int dx = x2 - x1;
	int dy = y2 - y1;

	assert( dx == 0 || dy == 0 );
	assert( x2 >= x1 );
	assert( y2 >= y1 );


	if( dy == 0 && dx == 0 )
	{
		PutPixel( x1,y1,color );
	}
	else if( dx == 0 )
	{
		for( UINT16 index = y1; index <= y2; index++ )
		{
			PutPixel( x1,index,color);
		}
	}
	else if( dy == 0 )
	{
		for( UINT16 index = x1; index <= x2; index++ )
		{
			PutPixel( index,y1,color);
		}
	}
}
void D3DGraphicsClient::PutPixel( int x,int y,int r,int g,int b ) {	
	PutPixel( x,y,D3DCOLOR_XRGB(r,g,b));
}
void D3DGraphicsClient::PutPixel( int x,int y,D3DCOLOR c )
{	
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < SCREEN_WIDTH );
	assert( y < SCREEN_HEIGHT );
	rGraphics.pSysBuffer[ x + SCREEN_WIDTH * y ] = c;
}
void D3DGraphicsClient::DrawVLine( Line line, D3DCOLOR color )
{
	DrawVLine( line.x1,line.x2, line.y1, line.y2 , color );
	

}
void D3DGraphicsClient::DrawVLine_VP( int x, int y, Line line, D3DCOLOR color )
{
	int dx = line.x2 - line.x1;
	int dy = line.y2 - line.y1;

	assert( dx == 0 || dy == 0 );
	assert( line.x2 >= line.x1 );
	assert( line.y2 >= line.y1 );

	INT32 x1,x2,y1,y2;
	x1 = line.x1;
	x2 = line.x2;
	y1 = line.y1;
	y2 = line.y2;
	
	x1 -= x;
	x2 -= x;
	y1 -= y;
	y2 -= y;

	bool x1Small = x1 < 0;
	bool x2Small = x2 < 0;
	bool y1Small = y1 < 0;
	bool y2Small = y2 < 0;
	bool x1Big	 = x1 >= SCREEN_WIDTH;
	bool x2Big	 = x2 >= SCREEN_WIDTH;
	bool y1Big	 = y1 >= SCREEN_HEIGHT;
	bool y2Big	 = y2 >= SCREEN_HEIGHT;

	if( x2Small || x1Big || y2Small || y1Big )
		return;

	if( x1Small )
		x1 = 0;
	if( x2Big )
		x2 = SCREEN_WIDTH - 1;
	if( y1Small )
		y1 = 0;
	if( y2Big )
		y2 = SCREEN_HEIGHT - 1;

	line = Line( x1,x2,y1,y2 );

	DrawVLine( line,color );

}
void D3DGraphicsClient::DrawRectangle( int x, int y, int width, int height, D3DCOLOR colour)
{
	for( int x_ = 0; x_ <= width; x_++ )
	{
		for( int y_ = 0; y_ <= height; y_++ )
		{
			PutPixel(x+x_,y+y_,colour);
		}
	}
}
void D3DGraphicsClient::DrawRectangle( int x, int y, int width, int height, int r, int g, int b)
{
	for( int x_ = 0; x_ <= width; x_++ )
	{
		for( int y_ = 0; y_ <= height; y_++ )
		{
			PutPixel(x+x_,y+y_,r,g,b);
		}
	}
}
void D3DGraphicsClient::DrawBitmap(int x, int y,int width, int height, D3DCOLOR background, D3DCOLOR* pic)
{
	for( int y_ = 0; y_ <= height-3; y_++ )
	{
		for( int x_ = 0; x_ <= width; x_++ )
		{
			if( pic[x_ + y_*width ] != background)
				PutPixel(x+x_,+y+y_,pic[ x_ + y_*width ]);
		}
	}
}
void D3DGraphicsClient::DrawChar( char c,int xOff,int yOff,int fontType,D3DCOLOR color,int enlargment )
{
	assert( fontType >= 0 && fontType < FONT_NFONTS );
	if( c < ' ' || c > '~' )
		return;

	const int sheetIndex = c - ' ';
	const int sheetCol = sheetIndex % rGraphics.fonts[fontType]->nCharsPerRow;
	const int sheetRow = sheetIndex / rGraphics.fonts[fontType]->nCharsPerRow;
	const int xStart = sheetCol * rGraphics.fonts[fontType]->charWidth;
	const int yStart = sheetRow * rGraphics.fonts[fontType]->charHeight;
	const int xEnd = xStart + rGraphics.fonts[fontType]->charWidth;
	const int yEnd = yStart + rGraphics.fonts[fontType]->charHeight;
	const int surfWidth = rGraphics.fonts[fontType]->charWidth * rGraphics.fonts[fontType]->nCharsPerRow;

	for( int y = yStart; y < yEnd; y++ ){
		for( int x = xStart; x < xEnd; x++ ){
			if( rGraphics.fonts[fontType]->surface[ x + y * surfWidth ] == BLACK ){
				for( int indexY = 0; indexY < enlargment; indexY++ ){
					for( int indexX = 0; indexX < enlargment; indexX++ ){
						PutPixel(indexX + xOff + (x-xStart)*enlargment,indexY + yOff + (y-yStart)*enlargment,color );
					}
				}
			
			}
		}
	}
}
void D3DGraphicsClient::DrawString(const std::string& str,int xoff,int yoff,int fontType,D3DCOLOR color,int enlargement ) {
	int xDis = 0;
	int yDis = 0;
	for( int index = 0; str[ index ] != '\0'; index++ ){
		DrawChar( str[ index ],xoff + xDis,yoff+yDis,fontType,color,enlargement );
		if( str[index] == '\n' ){
			xDis = 0;
			yDis += rGraphics.fonts[fontType]->charHeight;
		}
		else{
			xDis += rGraphics.fonts[fontType]->charWidth;
		}
		
	}
}
D3DCOLOR D3DGraphicsClient::FadePixel(D3DCOLOR c, float fade ) {
	ColComp r,g,b;
	ColToRGB( c,r,g,b);
	int newR = r + int((255 - r)*fade);
	int newG = g + int((255 - g)*fade);
	int newB = b + int((255 - b)*fade);
	return D3DCOLOR_XRGB(newR,newG,newB);
}
D3DCOLOR D3DGraphicsClient::GenRandCol( byte maxCol )
{
	byte r = rand() % maxCol;
	byte g = rand() % maxCol;
	byte b = rand() % maxCol;
	return D3DCOLOR_XRGB( r, g , b );
}
D3DCOLOR D3DGraphicsClient::CalcFadedColour( byte r, byte g, byte b, float fade ) {
	assert( fade >= 0 && fade <= 1 );
	byte newR = byte( 255-((255-r)*fade));
	byte newG = byte( 255-((255-g)*fade));
	byte newB = byte( 255-((255-b)*fade));
	return D3DCOLOR_XRGB(newR,newG,newB);
}
void D3DGraphicsClient::DrawCircle( Circle& circle, int x, int y ) {
	for(unsigned int index = 0; index < circle.pointList.size(); index++ )
		PutPixel( x + circle.pointList[index].x , y - circle.pointList[index].y, circle.c);
}
void D3DGraphicsClient::DrawLoadIco( int x, int y) {
	LoadIcoUpdate();
	Circle& l  =rGraphics.loadIco;

	const float fadeTheta = PI; // fade duration is semi-circle
	for( unsigned int index = 0; index < l.pointList.size(); index++ ){

		float difTheta =  l.pointList[index].theta - rGraphics.loadTheta; //diftheta is angle between point and angle to present
		while( difTheta < 0 )
			difTheta += 2*PI;
		if( difTheta>PI)
			continue;
		float a=1-difTheta/PI;		
		/*if diftheta is in correct region (within semi-circle of display angle) draw point
		with colour faded proportionate to diftheta*/
		if( difTheta >= 0 && difTheta <= fadeTheta )
			PutPixel(		x + l.pointList[index].x , 
							y + l.pointList[index].y, 
							CalcFadedColour(LOAD_RED,LOAD_GREEN,LOAD_BLUE,a));
	}

}
void D3DGraphicsClient::Begin_DrawLoadIco( int x, int y ) {
	assert( !rGraphics.loadIcoActive);
	rGraphics.loadIcoActive=true;
	//set dest positions
	rGraphics.icoDest.left		= x;
	rGraphics.icoDest.right		= x + LOADICO_OUTRAD*2 + 1;
	rGraphics.icoDest.top		= y;
	rGraphics.icoDest.bottom	= y + LOADICO_OUTRAD*2 + 1;
	
	//begin thread
	rGraphics.loadIcoThread.Parent_BeginFunc( (void*(*)(void*)) &DrawLoadIco_Wrap,&rGraphics);
}
void D3DGraphicsClient::End_DrawLoadIco(){
	assert( rGraphics.loadIcoActive);
	rGraphics.loadIcoActive=false;
	//end function
	rGraphics.loadIcoThread.Parent_EndFunc();
	//clear framefrom loading icon
	rGraphics.ClearGraphicsFrame();
	rGraphics.EndGraphicsFrame( &rGraphics.icoDest );
}
void D3DGraphicsClient::DrawLoadIco_Wrap( void * loadicoargs ) {
	//format arguments into loadicoargs format
	D3DGraphics * l = (D3DGraphics*)loadicoargs;
	Thread_Params& thread( l->loadIcoThread);
	RECT dest( l->icoDest );
	//while the parent does not call exit, draw loading icon.
	D3DGraphicsClient gfx(*l);
	while( !thread.Child_CheckForEnd()  ) {
		l->ClearGraphicsFrame();
		gfx.DrawLoadIco( dest.left,dest.top);
		l->EndGraphicsFrame(&dest);
	}
	l->loadIcoThread.Child_EndFunc();
}
void D3DGraphicsClient::LoadIcoUpdate() {
	//update current theta
	rGraphics.loadTheta -= 0.05f;
	if( rGraphics.loadTheta <= 0 )
		rGraphics.loadTheta = 2*PI;
}

Circle::Circle( unsigned int outerRadius,unsigned int innerRadius, ColComp red, ColComp green, ColComp blue  ) {
	*this = Circle(outerRadius,innerRadius,D3DCOLOR_XRGB(red,green,blue));
}
Circle::Circle(  unsigned int outerRadius,unsigned int innerRadius, D3DCOLOR col  )
	:
	outR(outerRadius),
	inR(innerRadius),
	c(col) {

	//initialise point list. nvalid points is set to the number of valid points. yes there will be wasted space in pointlist	
	pointList.resize( (outR*2)*(outR*2) );
	unsigned int index = 0;
	for( unsigned int y =  0; y <= outR*2; y++ ){
		for( unsigned int x =  0; x <= outR*2; x++ ){
			unsigned int r = unsigned int( sqrt(pow((double)x-outR,2) + pow((double)y-outR,2)) + 0.5);
			if( r <= outR && r >= inR ){
				PCPoint& p( pointList[index] );
				p.x = x;
				p.y = y;
				p.theta = CalculateAngle(double(x)-double(outR),double(y)-double(outR));
				index++;
			}
		}
	}
	pointList.resize(index);
}
Circle::~Circle(){

}


