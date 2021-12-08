#include "MenuBase.h"

std::vector< WinBackCol > wList;
WinBackCol::WinBackCol( WORD winID, COLORREF colour )
	:ID(winID),
	c(colour)
{}
LRESULT PaintWindows( UINT32 msg, WPARAM wParam, LPARAM lParam ) {
	LRESULT lRet = 0; // return value for our WindowProc.

	if( msg== WM_CTLCOLORSTATIC ) {
		HDC hdc = (HDC)wParam;
		HWND hwnd = (HWND)lParam; 

		// if multiple edits and only one should be colored, use
		// the control id to tell them apart.
		//
		for( UINT32 i = 0; i < wList.size(); i++ ) {
			WinBackCol& w = wList.at(i);

			if (GetDlgCtrlID(hwnd) == w.ID) {
				COLORREF r;
				r = SetBkColor(hdc, w.c); // Set colour
				assert( r != CLR_INVALID );
				r = SetDCBrushColor(hdc, w.c);
				assert( r!= CLR_INVALID );
				lRet = (LRESULT) GetStockObject(DC_BRUSH); // return a DC brush.
				assert(lRet);
				return lRet;
			}
		}
		
	}
	return 0;
}
void AddWinCol( WORD winID, COLORREF colour ) {
	wList.push_back( WinBackCol(winID,colour) );
}
void RmWinCol( WORD winID ) {
	bool found = false;
	UINT32 i;
	for( i  = 0; i < wList.size(); i++ ) {
		if( wList.at(i).ID == winID ) {
			found = true;
			break;
		}
	}
	assert(found);
	wList.erase( wList.begin() + i );
}

EditBoxServer::EditBoxServer(HWND _hWndEdit,  unsigned long _editID)
	:hWndEdit(_hWndEdit),
	editID(_editID) {
	ZeroMemory( szEditInput,LOB_EDIT_MAXINPUT+1);
}
EditBoxServer::~EditBoxServer() {

}

void ItemServer::NoButtonsPressed()
{
	buttonPressed = 0;
}
ItemServer::ItemServer()
{
	NoButtonsPressed();
}
bool ItemServer::CatchMsg( MSG& msg ) {
	if( msg.message == WM_CHAR &&		//key pressed
		msg.wParam == VK_RETURN ) {		//key pressed is return key 
		for( unsigned int i=0;i<editBoxes.size();i++) {
			HWND hWndEdit=editBoxes[i]->hWndEdit;
			if( hWndEdit==msg.hwnd ) {//hwnd is hwnd of editbox
				ZeroMemory(editBoxes[i]->szEditInput,LOB_EDIT_MAXINPUT+1);
				SendMessage( hWndEdit,WM_GETTEXT,LOB_EDIT_MAXINPUT+1,(LPARAM)editBoxes[i]->szEditInput );	//get message	
				SendMessage( hWndEdit,WM_SETTEXT,NULL,(LPARAM)"");//clear edit box display.
				buttonPressed=editBoxes[i]->editID;
				return true;
			}
		}
	}
	return false;		
}
void ItemServer::Update(HWND win,UINT32 msg, WPARAM wParam ) {
	if( msg == WM_COMMAND  ){
		for( unsigned int i=0; i<editBoxes.size();i++) {
			if( LOWORD(wParam) == editBoxes[i]->editID )
				return;
		}
		buttonPressed = LOWORD( wParam );
	}
}

int ItemClient::GetButtonPressed() {
	return bServ.buttonPressed;
}
ItemClient::ItemClient( ItemServer& b )
	:bServ(b) {
}
void ItemClient::RegisterEditHWND( HWND hwnd, unsigned long _editID ) {
	bServ.editBoxes.push_back( new EditBoxServer(hwnd,_editID));
}
void ItemClient::UnRegisterEditHWND(unsigned int editIndex) {
	assert( editIndex < bServ.editBoxes.size() );
	delete bServ.editBoxes[editIndex];
	bServ.editBoxes.erase( bServ.editBoxes.begin() + editIndex );
}
std::string ItemClient::GetEditText( unsigned int editIndex) {
	assert( editIndex < bServ.editBoxes.size() );
	return std::string( bServ.editBoxes[editIndex]->szEditInput );
}

MenuDims::MenuDims(int xCoord , int yCoord,   int yDif ,  int buttonHeight,   int buttonWidth)
	:x(xCoord),
	y(yCoord),
	deltaY(yDif),
	btnHeight(buttonHeight),
	btnWidth(buttonWidth)
{

}
Button::Button(std::string buttonText,  unsigned long buttonID )
:buttonTxt(	buttonText ),
ID( buttonID) {
}

BaseMenu::BaseMenu(  HWND window,BtnList _buttonList,MenuDims dimensions )
	:
	hwnd(window),
	dims(dimensions),
	nButtons( _buttonList.size() ),
	buttonList(_buttonList) {
	
	buttonHandles = new HWND[nButtons];
	
	assert(nButtons >= 1 );
	
	for( int index = 0; index < nButtons; index++ ){
		buttonHandles[index] = CreateMenuButton( *buttonList[index] ,index );
	}
}
BaseMenu::~BaseMenu() {
	for( int index = 0; index < nButtons; index++ ) {
		DestroyWindow( buttonHandles[index] );
	}
	delete[] buttonHandles;
}
HWND BaseMenu::CreateMenuButton( Button b , int buttonNumber ) {
	
	HWND win= CreateWindow( "button",							
							b.buttonTxt.c_str(),
							WS_VISIBLE | WS_CHILD | BS_CENTER,
							dims.x ,dims.y + (dims.deltaY*buttonNumber),
							dims.btnWidth,dims.btnHeight,
							hwnd,
							(HMENU)b.ID,
							NULL,NULL);
	assert(win);
	return win;
};

MenuWBox::MenuWBox(HWND window, std::vector<Button*> _buttonList,unsigned long extHwndID, MenuDims dimensions )
	:BaseMenu( window,_buttonList,dimensions) {

	int y = dimensions.y + (dimensions.btnHeight+DEF_MENU_BTN_YDIS)*(buttonList.size()-1);
	extBoxHwnd =CreateWindow(	"static",
					"",
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_EDITCONTROL,
					MM_EXTRAHWND_X,
					dimensions.y,
					dimensions.btnWidth+100,
					dimensions.btnHeight/2,
					window,
					(HMENU)extHwndID,
					NULL,NULL);
	assert(hwnd );
}
MenuWBox::~MenuWBox() {
	DestroyWindow( extBoxHwnd );
}
void MenuWBox::ChangeExtText( const std::string& newTxt ) {
	SetWindowText( extBoxHwnd,newTxt.c_str() );
}
