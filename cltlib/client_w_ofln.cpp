#include "client_w_ofln.h"

ClientWOffline::ClientWOffline(HWND hwnd, ItemServer& iServ, KeyboardServer& _kServ, ULONG svrIP)
	:MasterClient(hwnd,iServ,_kServ,svrIP) {
	
	buttonLists.resize(BTNLST_NLISTS_WOFL);
	buttonLists[BTNLSTIDX_OFFLINE]=Buttons_Offline();
	Buttons_MainMenu_ToOfl(buttonLists[BTNLSTIDX_MM]);
}
ClientWOffline::~ClientWOffline() {
}

MasterInterfaceWOfln::MasterInterfaceWOfln( ClientWOffline& masterReference )
	:MasterInterface(masterReference),
	oflRf(masterReference){
}
bool MasterInterfaceWOfln::IsOnline(){
	return oflRf.isOnline;
}
void MasterInterfaceWOfln::SetOflNPlayers( unsigned int nPlayers ) {
	nOflnPlayers=nPlayers;
}
unsigned int MasterInterfaceWOfln::GetOflnPlayers() {
	return nOflnPlayers;
}

bool Btn_Ofl_Play::ThisProcess( unsigned long buttonID, NumState state ) {
	return (buttonID>=SM_BTN_2PLAYERS&&buttonID<=SM_BTN_4PLAYERS&&state==OFLN_MENU);
}
void Btn_Ofl_Play::Process(unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	MasterInterfaceWOfln& i = (MasterInterfaceWOfln&)intf;
	i.SetOflNPlayers( ID_TO_NPLS(buttonID) );
	newState=COUNTDOWN;
}
bool Btn_Ofl_Cancel::ThisProcess( unsigned long buttonID, NumState state ) {
	return (buttonID==SM_BTN_CANCEL&&state==OFLN_MENU);
}
void Btn_Ofl_Cancel::Process(unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	newState=MAINMENU;
}
BtnProcContainer BtnPrcListWOfln() {
	BtnProcContainer b=StdBtnProcesses();
	b.push_back( new Btn_MM_Ofln );
	b.push_back( new Btn_Ofl_Play );
	b.push_back( new Btn_Ofl_Cancel );
	return b;
}

bool Btn_MM_Ofln::ThisProcess( unsigned long buttonID, NumState state ) {
	return buttonID==MM_BTN_OFFLINE;
}
void Btn_MM_Ofln::Process(unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	newState=OFLN_MENU;
}

void Buttons_MainMenu_ToOfl( BtnList& btns ) {
	btns.insert(btns.begin(), new Button( "play offline",MM_BTN_OFFLINE));
}

BtnList Buttons_Offline() {
	BtnList btns;
	btns.push_back( new Button( "2 players",SM_BTN_2PLAYERS));
	btns.push_back( new Button( "3 players",SM_BTN_3PLAYERS));
	btns.push_back( new Button( "4 players",SM_BTN_4PLAYERS));
	btns.push_back( new Button( "cancel",	SM_BTN_CANCEL));

	return btns;
}

KeyPrcContainer GetPrcKeysWOfl() {
	KeyPrcContainer k(GetPrcKeys());
	k.insert(k.begin(),new KeyPrcOflCountDnEsc);	//MUST preceed KeyPrcOlnEsc as that doesn't check for online/offline etc
	return k;
}
void KeyPrcOflCountDnEsc::ProcessKey( NumState& newState,MasterInterface& intf   ) {
	MasterInterfaceWOfln& i=(MasterInterfaceWOfln&)intf;
	if( !i.IsOnline() ) {
		newState=OFLN_MENU;
	}
	else {
		KeyPrcOlnCountDnEsc c;
		c.ProcessKey(newState,intf);
	}
}
