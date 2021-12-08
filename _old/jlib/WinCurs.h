#ifndef WINCURS 

#include "Platform.h"

#define _CRT_SECURE_NO_WARNINGS	//strcpy() warnings
#define MAXLINEMEM 100	//maximum lines allowed to be stored at a time for output


#if COMPILER == COMPILER_VS
#include <Windows.h>
#pragma comment( lib, "C:\\Program Files (x86)\\pdcurses\\win32a\\pdcurses.lib")	//linking curses library
#include "C:\\Program Files (x86)\\pdcurses\\curses.h"							//curses header for all pdcurses functions, directory set by vs
#elif COMPILER == COMPILER_GCC
#if PLATFORM == PLATFORM_WINDOWS
#include "C:\\Program Files (x86)\\pdcurses\\curses.h"	
#elif PLATFORM == PLATFORM_LINUX
#include <curses.h>
#include <string>	//std::string
#include <string.h>	//strcpy() strlen() etc
#endif
#endif

#include <ctype.h>		//isalphanum(), ispunct()
#include <vector>		//vectors of lines used to store input/output
#include <assert.h>		//use of assert() functions
#include <stdint.h>		//uint32_t and other integer definitions

using namespace std;	//saves the std:: everywhere


class PDWindow
{
public:
	PDWindow();
	~PDWindow();
	//prints to the output window. pre-process buffer with sprintf before calling function
	void PrintOutput( const char * buf);
	//check for input. returns true and stores input in buf if there is.
	//N.B. true return DOES NOT NECESSARY MEAN there is no more data to read
	bool ReadInput( string& inBuf );			
	//call every frame. checks for input
	void Update();
	//public function so user knows buffer size to allocate. INCLUDES SPACE FOR NULL IN RETURN SIZE
	int GetMaxInutSize();
private:
	vector<std::string> outList,inList;	//outList - list of strings containing output lines - max size MAXLINEMEM - after that lines are deleted
	
	//wOutput - output window, wInput - single line input window, wCursor - single line window for drawing manual text cursor
	WINDOW *wOutput,*wInput, *wCursor;	
	//string containing the current input being processed as the user types
	std::string curInput;

	//redraw the output window. topLine = index of line to be at of window
	void ReDrawOutput(uint32_t topLine);

	void ProcessInput();		//checks for input and processes it accordingly
	
	const static char * szPreInput;	//string to preceed input
	const static int preInputLen;	//length of szPreInput
	

	uint32_t height, width;	//total height and width of complete window
	int maxInputLen;	//maximum length of possible string to be inputted
	int cursorX;		//current position of hand made cursor, RELATIVE TO START COORDS OF STRING (preInputLen)
	uint32_t outHeight;		//height of output window

	//gets bottom line
	uint32_t GetBottomLine();		
	//index of line in outList which is at the top of the output window
	uint32_t curTopLine;			
	
	//updates cursor position in wCursor (pass coordinate relative to start of input string). wRrefresh for this to take effect
	//i.e. ignore the pre input string
	void MoveInputCursor(int newPos);	
};

#endif
