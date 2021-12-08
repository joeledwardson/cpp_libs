#ifndef GAMETYPES_H
#define GAMETYPES_H
#include "stdlib/error.h"
#include <math.h>	//atan(), abs()
#include <stdint.h>
#include <stdlib.h>	//required for abs() in linux

#define PI			(float)3.14159265359
#define ANGLETYPE_DEGREES	1
#define ANGLETYPE_RADIANS	0
#define GAMESTATE_START LOBBY
#define MAXNAMELEN	20

enum GameState {
	LOBBY,
	PLAYING ,
	COUNTDOWN,
	GAMESTATE_END,
};


std::string ToStr( const GameState& state ); 



typedef unsigned short gameint;



/*calculate an angle. It is not as simple as just calling tan-1( y / x ) for most angles
because of number signing issues*/
float CalculateAngle( double xDis, double yDis );

typedef uint32_t Colour;
typedef uint8_t ColComp;
static const Colour alpha = 255;

Colour RGBToCol( ColComp r, ColComp g, ColComp b );
void ColToRGB( Colour col, ColComp& r, ColComp& g, ColComp& b );

//Line structure
struct Line {
	//4 points in a line
	gameint x1,x2,y1,y2;
	Line( gameint x1_, gameint x2_, gameint y1_, gameint y2_ );
	Line();
};


#endif