#ifndef LOBBY_H
#define LOBBY_H

#include "MenuBase.h"
#include <wchar.h>
#include <string>
#include "stdlib/error.h"

#ifdef max
#undef max	//conflits with std::numeric_limits<>::max()
#endif

#define LOBBY_XCOORD_LEFT	200
#define LOBBY_XCOORD_RIGHT	800
#define LOBBY_YCOORD_TOP	200
#define LOBBY_YCOORD_BOTTOM	

#define LOBBY_XCOORD_CHAT	450
#define LOBBY_CHAT_WIDTH	300
#define LOBBY_CHATOT_HEIGHT	260
#define LOBBY_CHATIN_YCOORD	500

#define LOBBY_STD_WIDTH		200
#define LOBBY_STD_HEIGHT	50

#define LOB_ITM_TITLE_YCOORD	100
#define LOB_ITM_TITLE_WIDTH	800


#define LOBBY_PL_HEIGHT		360

#define LOBBY_LD_YCOORD		500

//window IDs
static unsigned long LOB_ITM_LB_NAMES		= 117;
static unsigned long LOB_ITM_TITLE			= 118;
static unsigned long LOB_ITM_DATA			= 119;
static unsigned long LOB_ITM_CHAT_DISP		= 120;
static unsigned long LOB_ITM_EDIT			= 121;

/*create a button with with set x coord width and height (see definitions) and assigns
the handle the place in the buttonHandles array given by the identifier param */
//HWND CreateButton( HWND parent,wchar_t * displayString, int xCoord,int yCoord,  int identifier, int bWidth, int bHeight );

class LobbyInt {
public:
	void ExtendChat( const std::string& message );

	void SetDataBox( const std::string& boxText );

	LobbyInt(ItemServer& itemServ , HWND _window, const BtnList& buttonList );
	
	void SetTitle(const std::string& title);

	void AddItem( const std::string& itemName  );

	void RemoveItem( UINT index );

	void GetItemName( UINT index,std::string& itemName );

	int GetSelItem();

	int GetNItems();

	std::string GetEditText();

	~LobbyInt();

	void Change_Menu( const BtnList& newButtonList ); 

	void SetSelItem( unsigned int index );
protected:
	typedef const int LOBBY_ID;
	static const int LOBBY_NITEMS =5;
	static unsigned int EditIndex();
	static const int maxCharStore=1000;
	HWND windowHandles[LOBBY_NITEMS];
	HWND window;
	BaseMenu * menu;
	ItemClient items;
	std::string editString;

	static LOBBY_ID idx_playerList = 1;
	static LOBBY_ID idx_lobbyData = 2;
	static LOBBY_ID idx_Title = 0;
	static LOBBY_ID idx_chatDisplay= 3;
	static LOBBY_ID idx_chatEdit = 4;
};

#endif