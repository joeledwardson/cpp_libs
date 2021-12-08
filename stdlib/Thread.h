#ifndef THREAD_H
#define THREAD_H


#include <assert.h>	 //assertions
#include "error.h"
#include "platform.h"
#if PLATFORM==PLATFORM_WINDOWS
#include <process.h> //threading
#elif PLATFORM==PLATFORM_LINUX
#include <pthread.h>
#endif



//structure for creating a thread. IMPORTANT: I probably massive overuse threads in this program, which is bad! I would advise you don't use them.
//"parent" refers to the caller of the thread function. "Child" refers to the calls made from inside a thread function
struct Thread_Params
{
	Thread_Params();
	void Parent_BeginFunc(void*(*Func)( void * vars ),void * vars);
	void Parent_EndFunc();
	bool Child_CheckForEnd();
	void Child_EndFunc();
private:
	bool exitFunc,complete,inProgress;
};

#endif