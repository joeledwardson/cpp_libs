#ifndef COUNTDOWN_H
#define COUNTDOWN_H
#include "netlib/timer.h"



//countdown abstract class
class CountDown_Base {
public:
	CountDown_Base(uint8_t startSecs);

	virtual ~CountDown_Base();

	uint8_t GetSecsEld();
	uint16_t GetMilSecsEld();
	/*update countdown - increases seconds and milseconds elapsed according 
	to how much time has passed since last update.
	if isChangeDelta is false - seconds and milseconds elapsed are overwritten with time elapsed from
	countdowntimer.gettimemilli().
	if isChangeDelta is true - seconds and milseconds elapsed are incremented with the time returned
	from countdowntimer.gettimemilli()*/	
	void UpdateCountDown( bool isChangeDelta,bool& secsChanged );	

	bool CountDownFinished(); //returns true if countdown has finished. i.e. has reached zero

	void Sync( uint8_t secs, uint16_t miliSecs );

	void Reset();
protected:
	
	uint8_t secondsElapsed;		//no. of seconds elapsed since start
	uint16_t milSecsElapsed;		//milsecs elapsed since last second. cannot be above 1000
	const uint8_t startingSeconds;		//amount of seconds countdown starts at
	Timer countdownTimer;

};

#endif