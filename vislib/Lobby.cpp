#include "Lobby.h"

MenuDims lobbyDims( LOBBY_XCOORD_RIGHT ,LOBBY_YCOORD_TOP,60 );

unsigned int LobbyInt::EditIndex() {
	return 0;
}
LobbyInt::LobbyInt(ItemServer& itemServ , HWND _window, const std::vector<Button*>& buttonList )
	:menu(NULL),
	items(itemServ),
	window(_window) {
	

	windowHandles[idx_playerList] = CreateWindow("listbox",
												"player list",
												WS_VISIBLE | WS_CHILD | WS_BORDER,
												LOBBY_XCOORD_LEFT,
												LOBBY_YCOORD_TOP,
												LOBBY_STD_WIDTH,
												LOBBY_PL_HEIGHT,
												window,
												(HMENU)LOB_ITM_LB_NAMES,
												NULL,NULL);	

	windowHandles[idx_Title] = CreateWindow("static",
												NULL,
												WS_CHILD | WS_VISIBLE | WS_BORDER | SS_EDITCONTROL,
												LOBBY_XCOORD_LEFT,
												LOB_ITM_TITLE_YCOORD,
												LOB_ITM_TITLE_WIDTH,
												LOBBY_STD_HEIGHT,
												window,
												(HMENU)LOB_ITM_TITLE,
												NULL,NULL);

	windowHandles[idx_lobbyData] = CreateWindow("static",
												NULL,
												WS_CHILD |WS_VISIBLE | WS_BORDER | SS_EDITCONTROL,
												LOBBY_XCOORD_RIGHT,
												LOBBY_LD_YCOORD,
												LOBBY_STD_WIDTH,
												LOBBY_STD_HEIGHT,
												window,
												(HMENU)LOB_ITM_DATA,
												NULL,NULL);

	windowHandles[idx_chatDisplay] =CreateWindow("edit",
												NULL,
												WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
												LOBBY_XCOORD_CHAT,
												LOBBY_YCOORD_TOP,
												LOBBY_CHAT_WIDTH,
												LOBBY_CHATOT_HEIGHT,
												window,
												(HMENU)LOB_ITM_CHAT_DISP,
												NULL,NULL);

	
	windowHandles[idx_chatEdit] = 	CreateWindow("edit",
												NULL,
												WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE,
												LOBBY_XCOORD_CHAT,
												LOBBY_CHATIN_YCOORD,
												LOBBY_CHAT_WIDTH,
												LOBBY_STD_HEIGHT,
												window,
												(HMENU)LOB_ITM_EDIT,
												NULL,NULL);
	items.RegisterEditHWND(windowHandles[idx_chatEdit],LOB_ITM_EDIT);
	Change_Menu( buttonList );


	for( unsigned int i=0;i<LOBBY_NITEMS;i++) {
		assert( windowHandles[i] );
	}

	SendMessage( windowHandles[idx_chatEdit],EM_LIMITTEXT,(WPARAM)LOB_EDIT_MAXINPUT,NULL);
	ExtendChat("      Chat     ");
	ExtendChat("********************");
	ExtendChat("");
}
void LobbyInt::Change_Menu( const BtnList& newButtonList ) {
	if( menu )
		delete menu;
	menu = new BaseMenu(window,newButtonList,lobbyDims);
}
std::string LobbyInt::GetEditText() {
	return items.GetEditText(EditIndex());
}
LobbyInt::~LobbyInt() {
	items.UnRegisterEditHWND( EditIndex()  );
	for( int index = 0; index < LOBBY_NITEMS ; index++ ) {
		DestroyWindow( windowHandles[index]);
	}
	if( menu ) {
		delete menu;
	}

}
void LobbyInt::ExtendChat( const std::string& message ) {
	editString+=message+"\r\n";
	int r=0;
	int lenDif=editString.length()-maxCharStore;
	if( lenDif > 0 )
		editString.erase( editString.begin(),editString.begin()+lenDif);
	HWND chatWnd=windowHandles[idx_chatDisplay];
	SendMessage(windowHandles[idx_chatDisplay],WM_SETTEXT,NULL,(LPARAM)editString.c_str());
	int newNLines=SendMessage(chatWnd,EM_GETLINECOUNT,0,0);
	SendMessage(chatWnd,EM_LINESCROLL,0,std::numeric_limits<LPARAM>::max());
}
void LobbyInt::SetTitle( const std::string& title) {
	SetWindowText( windowHandles[idx_Title],title.c_str());
}
void LobbyInt::AddItem( const std::string& itemName  ) {
	SendMessage( windowHandles[idx_playerList],LB_ADDSTRING,NULL,(LPARAM)itemName.c_str());
	UpdateWindow(windowHandles[idx_playerList]);
}
void LobbyInt::RemoveItem( UINT index ) {
	int nItems = SendMessage( windowHandles[idx_playerList],LB_GETCOUNT,NULL,NULL);
	assert( nItems != LB_ERR );
	assert( (int)index < nItems );
	int r = SendMessage( windowHandles[idx_playerList],LB_DELETESTRING,(WPARAM)index,NULL);
	assert(r!=LB_ERR);
	UpdateWindow(windowHandles[idx_playerList]);
}
int LobbyInt::GetSelItem() {
	return SendMessage( windowHandles[idx_playerList],LB_GETCURSEL,WPARAM(0),LPARAM(0));
}
int LobbyInt::GetNItems() {
	int nItems = SendMessage( windowHandles[idx_playerList],LB_GETCOUNT,NULL,NULL);
	assert( nItems != LB_ERR );
	return nItems;
}
void LobbyInt::GetItemName( UINT index,std::string& itemName ) {
	LRESULT r;
	int nItems = SendMessage( windowHandles[idx_playerList],LB_GETCOUNT,NULL,NULL);
	assert( nItems != LB_ERR );
	assert( (int)index < nItems );
	char tmp[LOB_EDIT_MAXINPUT+1] = {0};
	r =  SendMessage( windowHandles[idx_playerList],LB_GETTEXT,index,(LPARAM)tmp);
	assert( r != LB_ERR );
	itemName=tmp;	
}
void LobbyInt::SetDataBox( const std::string& boxText ) {
	SetWindowText( windowHandles[idx_lobbyData],boxText.c_str());
}
void LobbyInt::SetSelItem( unsigned int index ) {
	SendMessage(windowHandles[idx_playerList],LB_SETCURSEL,index,0);
	//returns -1 even on success....
}

