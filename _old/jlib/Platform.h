#pragma once

#pragma warning( disable: 4996 )	//annoying unsafe warnings

#define _CRT_SECURE_NO_WARNINGS

#define PLATFORM_WINDOWS	1
#define PLATFORM_LINUX		2
#define PLATFORM_UNKOWN		3

#define COMPILER_VS			69
#define COMPILER_GCC		1994
#define COPMILER_UNKOWN		0110

#ifdef _WIN32
#define PLATFORM PLATFORM_WINDOWS
#elif defined __linux__
#define PLATFORM PLATFORM_LINUX
#else
#define PLATFORM PLATFORM_UNKOWN
#endif

#ifdef _MSC_VER
#define COMPILER COMPILER_VS
#elif defined __GNUC__
#define COMPILER COMPILER_GCC
#else
#define COMPILER COMPILER_UNKOWN
#endif

#if COMPILER != COMPILER_VS
//some helpful windows definitions
#ifndef NULL
#define NULL 0
#endif

#define ZeroMemory(p, sz) memset((p), 0, (sz))

typedef unsigned char byte;
typedef byte BYTE;
typedef unsigned int UINT;
typedef UINT UINT32;
typedef unsigned short UINT16;
typedef unsigned int ULONG;


#endif