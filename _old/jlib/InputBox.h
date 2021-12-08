#pragma once
#if COMPILER == COMPILER_VS && PLATFORM == PLATFORM_WINDOWS

#include <Windows.h>
#include "Resource.h"
#include "assert.h"
#include "Platform.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef UNICODE
#define jchar wchar_t
#else
#define jchar char
#endif

//create input box. returns true if user enters value, false if cancels
bool CreateInputBox(
	HWND parentWin,
	char * returnString,
	const jchar * title,
	const jchar * request,
	const jchar * startingEditText = 
#ifdef UNICODE 
	L"enter text here" ,
#else
	"enter text here",
#endif
	int maxLen = 20 );

#endif