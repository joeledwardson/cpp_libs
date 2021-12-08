#include "wincurs.h" 

const char *PDWindow::szPreInput = ">";
const unsigned int	PDWindow::preInputLen = strlen(PDWindow::szPreInput);	



void PDWindow::ReDrawOutput( uint32_t topLine ) {
	
	unsigned int lineEnd,lineStart;	//index in lineOut of bottom and top line to print

	//make sure line is in range
	assert( topLine <= outList.size() );
	
	wclear(wOutput);	//clear output window
	
	lineStart = topLine;	//set top line
		
	
	lineEnd = lineStart + outHeight;	//set bottom line to top plus full length of window
	if( lineEnd > outList.size() )		//if bottom line is passed output line list range
		lineEnd = outList.size();
	
	
	//print lines
	for( unsigned int i = lineStart; i < lineEnd; i++ ) {
		wprintw( wOutput,outList.at(i).c_str() );
	}
	//refresh screen to take effect
	wrefresh(wOutput);
	
}
viewpoint PDWindow::GetBottomLine()
{
	if( outList.size() > outHeight )
		return outList.size() - outHeight;
	else
		return 0;

}
void PDWindow::Update()
{
	ProcessInput();
}
PDWindow& PDWindow::operator>>(std::string& inBuf) {
	/* if there is any input in the input list, copy to buffer and return true*/
	if( inList.size() ) {
		inBuf = inList.back();
		inList.erase( inList.end()-1 );
	}
	return *this;
}
bool PDWindow::IsInput() {
	return inList.size() > 0;
}
void PDWindow::NewLine(bool addNLChar) {
	bool atBottom = (curView == GetBottomLine() );	//is the current view at the bottom of the output?	
	if( addNLChar )	
		outList.back().append("\n");	
	outList.push_back( std::string() );
	
	if( atBottom ) {
		curView = GetBottomLine();
	}

		//if outputlist has exceeded maximum lines allowed to be stored in memory, remove them
	while( outList.size() > MAXLINEMEM )
	{
		if( curView > 0 )
			curView--;

		outList.erase( outList.begin() );
	}
	
}
PDWindow& PDWindow::operator<<( std::string output ) {
	
	
	while(1) {
		std::string::size_type pos = output.find( '\n' );
		if( pos != std::string::npos ) {
			std::string tmp = output;
			output = output.substr( pos +1 );
			tmp.resize( pos );
			*this << tmp;  
			PDWindow::NewLine();
		}
		else
			break;
	}

	outList.back().append( output );


	while( outList.back().size() + output.size() >= width ) {	
		
		size_t deltaLen = width - outList.back().size();
		

		std::string tmp = output;			
		output =  output.substr( deltaLen );
		tmp.resize(deltaLen);
		outList.back().append( tmp );
	

		NewLine( false);

	}
	ReDrawOutput( curView );
	//if( (curView == 0 && outList.size() == MAXLINEMEM) || (curView == GetBottomLine()) ) {
	//redraw output with new output characters/lines
	
	//}
	return *this;
	
}
void PDWindow::MoveInputCursor(int newPos)
{

	assert( newPos >= 0 && newPos < (int)maxInputLen);	//check in range
	wclear( wCursor );								//remove old cursor
	mvwprintw( wCursor,0,newPos+preInputLen,"^");	//set new cursory to start position of string + relative x coord (newPos)
}
void PDWindow::ProcessInput()
{
	int c;		//character inputted
	bool redraw;//redraw at end of input?
	unsigned int inLen;	//length of current string in input 

	//(set to -1 if no input)
	c = wgetch( wInput );
	
	redraw = false;	//initially don't want to redraw window

	while(c > 0 )	{
		
		inLen = strlen(curInput.c_str());

		//if user presses left and not at left of screen then move cursor left
		if( c == KEY_LEFT && cursorX >= 1  )	{
			cursorX--;	
		}
		//if user presses right and cursory is not at end of string - move right
		else if( c == KEY_RIGHT && cursorX < inLen )	{
			cursorX++;

		}
		//if user presses down - and not at bottom - move view down and redraw output
		else if( c == KEY_DOWN && curView < GetBottomLine())	{
			curView++;
			ReDrawOutput(curView);
		}
		//if user presses up - and not at top - move view up and redraw output
		else if( c == KEY_UP && curView> 0) {
			curView--;
			ReDrawOutput(curView);
		}
		//if backspace is pressed ( and cursor is not at start of string )
		//or if delete is pressed 
		
		else if((c == '\b' || c == KEY_BACKSPACE || (c == 127 && cursorX >= 1)) ||
				c == KEY_DC ) {
			if(  c== '\b' || c == KEY_BACKSPACE || c == 127)
				cursorX--;	//move left if backspace pressed

			curInput.erase(cursorX,1);
			
			//need to redraw input window in this case
			redraw = true;

			
		}
		//if enter pressed - and the string has length i.e. the user has typed something 
		else if( c == KEY_ENTER || (c=='\n' && inLen) ) {
			//add new line to current input			
			//curInput.append("\n"); 
			/*** '%' characters treated as special characters - for an actual '%' c++ requires two
			'%' characters next to each other - stream through adding % character whever one is found*/

			int pos = curInput.find('%');
			while(pos>= 0) {
				curInput.insert(pos,"%");
				++++pos;
				pos = curInput.find('%',++pos);
			}

			inList.push_back( curInput );	//add formatted input string to input message buffer
			cursorX = 0;					//set input cursor back to 0
			curInput.clear();				//clear input buffer
			redraw = true;					//again - redrawing required
			
		}
		//if CHARACTER pressed (0<=c<=255) and not at end of string  
		else if( c >= 0 && c <= 255 && inLen < (width - preInputLen - 1) ) {
			
			//currently only allowing alphanumeric charaters, punctuation or spaces
			if( isalnum(c) || ispunct(c) || c== ' ') {
							

				//if cursor is inside string then redrawing is required
				if( cursorX < inLen ) {
					redraw = true;
				}
				else {
					//otherwise simply print character with no redrawing
					mvwaddch( wInput,0,cursorX+preInputLen,c);
				}

				//insert character into input buffer
				curInput.insert( cursorX,1,c);
				
				//cursory is moved right
				cursorX++;
			}
		}

		//redraw input window if required
		if( redraw) {
			wclear(wInput);
			wprintw(wInput, "%s%s",szPreInput, curInput.c_str() );
		}
		
		wmove(wInput,0,cursorX);	//move physical curse into right position
		MoveInputCursor(cursorX);	//move visible cursor into position
		
		//get character ready for new loop
		c = wgetch( wInput );
	}

	wrefresh(wInput);			//refersh input and cursor windows to see changes
	wrefresh(wCursor);
	
}
PDWindow::PDWindow()
{
	
	

	WINDOW * w=initscr();	//initilaise screen, creating master screen
	clear();	//clear it
	

	getmaxyx(w, height, width);			//get maximum height and width of window
	maxInputLen = width - preInputLen;	//input length doesn't include preliminary input string
	outHeight = height - 2;				//2 lines for input and input cursor
	curView = 0;						//current ouput line is 0
	cbreak();							//we want to handle line buffering

	curInput.resize( maxInputLen );	//make sure input string has enough space to begin with
	curInput.clear();				//make sure its empty

	//create the 3 subwindows
	//					height,		width,	starty,		startx	
	wOutput =	newwin( outHeight,	width,	0,			0);
	wInput =	newwin( 1,			width,	outHeight,	0);	
	wCursor =	newwin( 1,			width,	height-1,0);	
	
	//make sure windows exist
	assert(wOutput);
	assert(wInput);
	assert(wCursor);

	noecho();	//dont echo on input
	
	

	wtimeout(wInput,0);	//set timeout for input to 0 so we don't have to wait for it until user enters something
	keypad(wInput,true);//initilaise keypad so arrow keys can be used
	curs_set(0);		//we don't want a cursor, we are using our own ;)
	
	wprintw(wInput,szPreInput);	//print pre input string
	cursorX = 0;				//cursor's initial x position will be zero
	MoveInputCursor(cursorX);	//set cursor position
	wrefresh(wInput);			//refresh input and output windows to see visible changes
	wrefresh(wCursor);

	outList.push_back( std::string() );//make sure there is a string in the vector to write to!
}
PDWindow::~PDWindow()
{
	//delete master window
	endwin();
}
