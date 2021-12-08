#ifndef ERROR_H
#define ERROR_H
#include <exception>
#include "platform.h"
#if PLATFORM==PLATFORM_WINDOWS
#include <windows.h>	//FormatMessage(),Getlasterror()
#elif PLATFORM==PLATFORM_LINUX
#include <errno.h>	//errno
#include <string.h>	//strerror()
#endif


//NON CRITICAL ERROR - will not throw upon calling
void AddErr( JSTRING err );
JSTRING GetLastErr(  );
void RmLastErr();
bool IsErr();

//CRITICAL ERROR - ONLY 1 - will throw upon calling
void SetCritErr( std::string err );

//get system error code
std::string GetSysError();

#endif
