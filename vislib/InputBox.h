#pragma once

#include <stdlib/platform.h>	//win32meanandlean prevents including of winsock.h
#include <Windows.h>
#include "resource.h"
#include "assert.h"
#include <string>

#define INPUTBOX_MAXLEN	100

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

//create input box. returns true if user enters value, false if cancels
bool CreateInputBox(
	HWND parentWin,
	const std::string& boxTitle,
	const std::string& boxCaption,
	std::string& editTxt,
	int maxLen = INPUTBOX_MAXLEN,
	std::string startingEditText = "enter text here" );
