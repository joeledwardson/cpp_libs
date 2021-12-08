#ifndef VISCOUNTDOWN_H
#define VISCOUNTDOWN_H

#include "gmelib/countdown.h"
#include "vislib/d3dgraphics.h"

#define COUNTDOWN_ENLARGE 7
#define COUNTDOWN_X 200
#define COUNTDOWN_Y	200

class CountDownVis : public CountDown_Base {
public:
	CountDownVis( uint8_t startSecs, D3DGraphics& rGfx );
	void DrawCountDown( int x, int y );
protected:
	D3DGraphicsClient gfx;
};

#endif