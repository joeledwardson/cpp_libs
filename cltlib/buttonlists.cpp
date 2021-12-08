#include "buttonlists.h"

BtnList Buttons_MainMenu() {
	BtnList btns;
	btns.push_back( new Button("server list",MM_BTN_SVRLIST));
	btns.push_back( new Button("change name",MM_BTN_CHANGENAME));
	btns.push_back( new Button("exit",MM_BTN_EXIT));
	return btns;
}
BtnList Buttons_Lob_Host() {
	BtnList btns;
	btns.push_back( new Button( "leave lobby",LOB_BTN_LEAVE));
	btns.push_back( new Button( "start game",LOBHST_BTN_START));
	btns.push_back( new Button( "promote player",LOBHST_BTN_PRMT));
	btns.push_back( new Button( "kick player",LOBHST_BTN_KICK));
	return btns;
}
BtnList Buttons_Lob_Client(){
	BtnList btns;
	btns.push_back( new Button( "leave lobby",LOB_BTN_LEAVE));
	return btns;
}
BtnList Buttons_SvrList() {
	BtnList btns;
	btns.push_back( new Button("host game",SVRWND_BTN_HOST));
	btns.push_back( new Button("connect",SVRWND_BTN_CONECT));
	btns.push_back( new Button("cancel",SVRWND_BTN_CANCEL));
	return btns;
}