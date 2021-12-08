#include "gametypes.h"

std::string ToStr( const GameState& state ) {
	
	switch (state)
	{
	case LOBBY:
		return "lobby";
	case PLAYING:
		return "in game";
	case COUNTDOWN:
		return "countdown";
	case GAMESTATE_END:
	default:
		SetCritErr("could not perform ostringstream<<GameState as the GameState provided is out of range");
		return "";//to avoid compiler warnings, SetCritErr throws exception
	}
}


Colour RGBToCol( ColComp r, ColComp g, ColComp b ) {
	return (	(alpha 	<< (3*8))	+
				(r 		<< (2*8) ) 	+
				(g	 	<< (1*8) ) 	+
				(b 		<< (0*8) )) ;

}
void ColToRGB( Colour col, ColComp& r, ColComp& g, ColComp& b ) {
	r = (col >> (2*8) ) % 256;
	g= 	(col >> (1*8) ) % 256;
	b = (col >> (0*8) ) % 256;
}

float CalculateAngle( double xDis, double yDis) {	
	float rawAngle =  (float)atan( abs(yDis/xDis) ); //calculate raw angle in first quadrant
	//second quadrant
	if( xDis <= 0 && yDis >= 0 ){
		rawAngle = PI - rawAngle;
	}
	//third quadrant
	else if( xDis<= 0 && yDis <= 0 ) {
		rawAngle += PI;
	}
	//fourth quadrant
	else if( xDis >= 0 && yDis < 0 ) {
		rawAngle = 2*PI - rawAngle;
	}

	return rawAngle;
}

Line::Line( gameint xLeft, gameint xRight, gameint yLeft, gameint yRight)
	:
	x1(xLeft),
	x2(xRight),
	y1(yLeft),
	y2(yRight){
}
Line::Line()
	:x1(0),
	x2(0),
	y1(0),
	y2(0){
}