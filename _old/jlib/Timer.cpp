
/******************************************************************************************
 *	Chili DirectX Framework Version 12.04.24											  *
 *	Timer.cpp																			  *
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
#include "Timer.h"

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
