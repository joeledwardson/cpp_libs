#include "timer.h"

uint64_t TimeStamp() {
	uint64_t ll_now;
#if PLATFORM==PLATFORM_WINDOWS
	FILETIME ft_now; 
	GetSystemTimeAsFileTime(&ft_now);
	ll_now = (uint64_t)ft_now.dwLowDateTime + ((uint64_t)(ft_now.dwHighDateTime) << 32LL);
	ll_now -= 116444736000000000LL;
	ll_now /= 10000;
#elif PLATFORM==PLATFORM_LINUX
	timeval end;
	gettimeofday( (timeval*)&end,NULL );
	ll_now=(uint64_t)(end.tv_usec)/1000LL;
	ll_now += ((uint64_t)(end.tv_sec) * 1000LL);
#endif
	return ll_now;
}

Timer::Timer()
{
#if PLATFORM == PLATFORM_WINDOWS
	int64_t frequency;
	QueryPerformanceFrequency( (LARGE_INTEGER*)&frequency );
	invFreqMilli = 1.0f / (float)((double)frequency / 1000.0);
#endif
	ResetWatch();
}

void Timer::ResetWatch()
{
#if PLATFORM == PLATFORM_WINDOWS
	QueryPerformanceCounter( (LARGE_INTEGER*)&startCount );
#else
	gettimeofday( &start,NULL);
#endif
}

float Timer::GetTimeMilli() const
{

#if PLATFORM == PLATFORM_WINDOWS
		QueryPerformanceCounter( (LARGE_INTEGER*)&currentCount );
		return (float)(currentCount - startCount) * invFreqMilli;
#else
	gettimeofday( (timeval*)&end,NULL );
	return  int64_t( (end.tv_sec - start.tv_sec )*1000) +
			int64_t( (end.tv_usec- start.tv_usec)/1000);
#endif
}
