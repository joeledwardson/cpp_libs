#ifndef MENUBASE_H
#define MENUBASE_H

#include <stdlib/platform.h>	//win32meanandlean prevents winsock.h being included
#include "bitmap.h"
#include <assert.h>
#include <Windows.h>
#include <string>
#include <vector>



#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

/* default menu dimensions */
#define DEF_MENU_BTN_WIDTH		200
#define DEF_MENU_BTN_HEIGHT		50
#define DEF_MENU_XCOORD			100
#define DEF_MENU_YCOORD			50
#define DEF_MENU_BTN_YDIS		100
/****/

#define MM_EXTRAHWND_X		DEF_MENU_XCOORD + DEF_MENU_BTN_WIDTH + 50

#define LOB_EDIT_MAXINPUT			40

struct WinBackCol {
	WinBackCol( WORD winID, COLORREF colour );
	WORD ID;
	COLORREF c;
};
LRESULT PaintWindows( UINT32 msg, WPARAM wParam, LPARAM lParam );
void AddWinCol( WORD winID, COLORREF colour );
void RmWinCol( WORD winID );

class EditBoxServer {
public:
	EditBoxServer(HWND _hWndEdit,  unsigned long _editID);
	~EditBoxServer();
	HWND hWndEdit;
	unsigned long editID;
	char szEditInput[LOB_EDIT_MAXINPUT+1];
};

class ItemClient;

//sctruct for windows.cpp to interact with when user presses buttons
class ItemServer {
	friend ItemClient;	
public:
	void NoButtonsPressed();
	ItemServer();
	void Update(HWND win, UINT32 msg, WPARAM wParam );	//to be called in msg system in parent window
	bool CatchMsg( MSG& msg );
private:
	std::vector<EditBoxServer*> editBoxes;
	int buttonPressed;
};

class ItemClient {
public:
	//returns 0 if no buttons pressed. otherwise returns identifier
	ItemClient( ItemServer& b );
	int GetButtonPressed();
	std::string GetEditText( unsigned int editIndex);
	void RegisterEditHWND( HWND hwnd,unsigned long _editID );
	void UnRegisterEditHWND(unsigned int editIndex);
private:
	ItemServer& bServ;
};


struct MenuDims {
	MenuDims(	int xCoord =		DEF_MENU_XCOORD	, 
				int yCoord =		DEF_MENU_YCOORD	,
				int yDif = 			DEF_MENU_BTN_YDIS		, 
				int buttonHeight=	DEF_MENU_BTN_HEIGHT		, 
				int buttonWidth	=	DEF_MENU_BTN_WIDTH	);
	int x, y, deltaY, btnHeight, btnWidth;
};

struct Button {
	Button( std::string buttonText,  unsigned long buttonID );
	std::string buttonTxt;
	unsigned long ID;
};

typedef std::vector<Button*> BtnList;

//template class for menu
class BaseMenu {
public:
	BaseMenu( HWND window, BtnList _buttonList, MenuDims dimensions = MenuDims()  );
	~BaseMenu();
protected:
	HWND CreateMenuButton( Button b , int buttonNumber);

	int nButtons;
	HWND hwnd;		//handle for main window. initiliased in consctructor
	HWND * buttonHandles; //pointer to a list of handles for buttons of sub windows for main and sub menu
	BtnList buttonList;
	MenuDims dims;
};

class MenuWBox : public BaseMenu {
public:
	MenuWBox(HWND window, std::vector<Button*> _buttonList,unsigned long extHwndID, MenuDims dimensions = MenuDims() );
	~MenuWBox();
	void ChangeExtText( const std::string& newTxt );
private:
	HWND extBoxHwnd;
};

#endif