#include "Thread.h"

Thread_Params::Thread_Params()
{
	complete = false; //complete var. set to true by child when completed.
	
	exitFunc = false; /*exit var. set to true by parent when they wish for child to end function.
						they then wait for child to complete function and set complete to true. */
	
	inProgress = false; // set to true when function is in progress. used as only one thread should be created per thread_params struct.
}
void Thread_Params::Parent_BeginFunc(void Func( void * vars ),void * vars)
{
	assert( !inProgress ); //make sure thread is not already in progress.
	inProgress = true;
	int r;
#if PLATFORM==PLATFORM_WINDOWS
	_beginthread( Func,0,vars);
#elif PLATFORM==PLATFORM_LINUX
	pthread_t thread;
	r=pthread_create(&thread,NULL,Func,vars);	
#endif
	if( r ) {
		std::string err = "error creating thread!: " + GetSysError();
		SetCritErr(err);
	}
}
void Thread_Params::Parent_EndFunc()
{
	assert( inProgress ); //make sure thread is in progress.
	assert( !exitFunc );	//make sure programmer has not already called for process to be terminated.

	exitFunc = true;		//set exitfunc to true so child knows to exit
	while( !complete ) {}	//wait for child to exit
	//reset
	complete = false;	
	exitFunc = false;
	inProgress = false;
}
bool Thread_Params::Child_CheckForEnd()
{
	return exitFunc; //checks if parent wishes for child to terminate.
}
void Thread_Params::Child_EndFunc()
{
	complete = true; //set complete to true. parent knows child has finished.
}
