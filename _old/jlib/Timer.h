
/******************************************************************************************
 *	Chili DirectX Framework Version 12.04.24											  *
 *	Timer.h																				  *
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
#ifndef TIMER_H
#define TIMER_H

#include "Platform.h"
#include <stdint.h>

#if PLATFORM == PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/time.h>
#endif

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
