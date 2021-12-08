#ifndef TIMER_H
#define TIMER_H

#include "stdlib/platform.h"
#include <stdint.h>

#if PLATFORM == PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/time.h>
#endif

uint64_t TimeStamp();

class Timer
{
public:
	
	Timer();
	float GetTimeMilli() const;
	void ResetWatch();
private:
#if PLATFORM == PLATFORM_WINDOWS
	float invFreqMilli;
	
	int64_t currentCount;
	int64_t startCount;
#else
	timeval start;
	timeval end;
#endif
};

#endif
