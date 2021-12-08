#include "viscountdown.h"

CountDownVis::CountDownVis( uint8_t startSecs,D3DGraphics& rGfx )
	:CountDown_Base(startSecs),
	gfx(rGfx){

}
void CountDownVis::DrawCountDown(int x, int y) {

	float fade =(float) milSecsElapsed /1000.0f;	//fractional number for degree of fade
	fade=1-fade;
	UINT32 seconds = startingSeconds - secondsElapsed;	//seconds to draw - i.e. if 3 seconds passed and starting seconds is 5 then seconds to draw is 2
	assert( seconds > 0 && seconds <= startingSeconds );
	gfx.DrawChar( '0' + seconds,	//48 is starting value for numbers in ASKII codes
		x,
		y,
		FONT_BIG,	//id for big font
		gfx.CalcFadedColour( LOAD_RED,LOAD_GREEN,LOAD_BLUE,fade),	//function calculates faded colour for given r,g,b and fractional degree of fade value
		COUNTDOWN_ENLARGE);	//number of enlargement of font
}