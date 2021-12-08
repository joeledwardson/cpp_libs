#include "countdown.h"

uint8_t CountDown_Base::GetSecsEld() {
	return secondsElapsed;
}
uint16_t CountDown_Base::GetMilSecsEld() {
	return milSecsElapsed;
}
CountDown_Base::CountDown_Base(uint8_t startSecs)
	:startingSeconds(startSecs){
	Reset();
}
void CountDown_Base::UpdateCountDown( bool isChangeDelta,bool& secsChanged ) {
	secsChanged = false;
	uint8_t oldSecs = secondsElapsed;

	if( !isChangeDelta ) {
		//change is not delta

		milSecsElapsed = (int) countdownTimer.GetTimeMilli();	//get total milseconds time elapsed
		secondsElapsed = milSecsElapsed / 1000;					//set seconds to total miliseconds / 1000 and rounded down
		milSecsElapsed %= 1000;									//set miliseconds elapsed to miliseconds remainder of total
	}
	else {
		//change is delta

		milSecsElapsed += (int) countdownTimer.GetTimeMilli();	//miliseconds elapsed is appended by time elapsed
		//if miliseconds elapsed has become greater than 1000, seconds is incremeted
		secondsElapsed += (milSecsElapsed / 1000);
		milSecsElapsed %= 1000;	//miliseconds is then cut down to just the miliseconds
		countdownTimer.ResetWatch();
	}
	if( secondsElapsed != oldSecs )
		secsChanged = true;
}
CountDown_Base::~CountDown_Base() {

}
bool CountDown_Base::CountDownFinished() {
	return secondsElapsed >= startingSeconds;
}
void CountDown_Base::Sync( uint8_t secs, uint16_t miliSecs ) {
	secondsElapsed=secs;
	milSecsElapsed=miliSecs;
}
void CountDown_Base::Reset() {
	countdownTimer.ResetWatch();
	milSecsElapsed=0;
	secondsElapsed=0;
}