#include "buttonprocesses.h"

ButtonProcess::~ButtonProcess() {
}
bool MMBtnProcess::ThisProcess( unsigned long buttonID, NumState state ) {
	return state=MAINMENU&&ThisButton(buttonID);
}
bool Btn_MM_Exit::ThisButton( unsigned long buttonID ) {
	return buttonID==MM_BTN_EXIT;
}
void Btn_MM_Exit::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	newState=STATE_EXIT;
}
bool Btn_MM_ChangeNM::ThisButton( unsigned long buttonID ) {
	return buttonID==MM_BTN_CHANGENAME;
}
void Btn_MM_ChangeNM::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	std::string newName;
	if( CreateInputBox( intf.GetWind(),"input","enter new name",newName,MAXNAMELEN)) {
		intf.ChangeName(newName);
	}
}
bool Btn_MM_SvrList::ThisButton( unsigned long buttonID ) {
	return buttonID==MM_BTN_SVRLIST;
}
void Btn_MM_SvrList::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	StdOPacket p;
	p<<intf.name();
	byte deny;
	BNP::NetIPacket inPacket;
	D3DGraphicsClient gfx( intf.Graphics() );
	gfx.Begin_DrawLoadIco(400,400);
	if( intf.net.Connect( deny,p,inPacket)) {
		intf.SetRecvPacket(inPacket);
		newState=SERVERLIST;
	}
	else {
		std::string msg="failed to join server list: ",reason;
	
			GNP::REPLY_TYPES denyReason=(GNP::REPLY_TYPES)deny;
		switch (denyReason)
		{
		case GNP::FAIL_INGAME:
			reason="server is in game!";
			break;
		case GNP::FAIL_FULL:
			reason="server is at maximum capacity";
			break;
		case GNP::FAIL_NAMETAKEN:
			reason="player with that name already exists on server";
			break;
		case GNP::FAIL_NOTFOUND:
			reason="server not found...?";
			break;
		case GNP::FAIL_NODATA:
			reason="no data...?";
			break;
		default:
			if( !inPacket.len() )
				reason="no reply received...";
			else
				reason="unkown reason";
			break;
		}
		
		msg+=reason;
		MessageBox( intf.GetWind(),msg.c_str(),"error",MB_ICONINFORMATION);
	}
	gfx.End_DrawLoadIco();
	
}
bool Btn_Chat::ThisProcess( unsigned long buttonID, NumState state ) {
	return buttonID==LOB_ITM_EDIT&&(state==LOBBY||state==SERVERLIST);
}
void Btn_Chat::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	std::string chatMsg = intf.GetLobby()->GetEditText();
	if( chatMsg.length() ) {
		StdOPacket p;
		p<<chatMsg;
		intf.net.AddUpdate(GNP::CHAT_XTND,p);
		intf.net.InitCurUpdate();
	}
}
bool SvrListBtnProcess::ThisProcess( unsigned long buttonID, NumState state ) {
	return state=SERVERLIST&&ThisButton(buttonID);
}
bool Btn_SvrLst_Host::ThisButton( unsigned long buttonID ) {
	return buttonID==SVRWND_BTN_HOST;
}
void Btn_SvrLst_Host::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	
	LobbyInt * lob = intf.GetLobby();
	if( intf.OpInProg() ) {
		lob->ExtendChat("operation already in progress...");
		return;
	}
	intf.net.AddUpdate( GNP::HOST_GAME,StdOPacket());
	intf.net.InitCurUpdate();
	intf.InitOp();
}
bool Btn_SvrLst_Cancel::ThisButton( unsigned long buttonID ) {
	return buttonID==SVRWND_BTN_CANCEL;
}
void Btn_SvrLst_Cancel::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	BNP::PacketCore core;
	BNP::ZeroCore(core);
	core[BNP::byte_CM]=BNP::CM_QUIT;
	BNP::NetOPacket packet(core);
	intf.net.SendPacket(packet);
	newState=MAINMENU;
}
bool Btn_SvrLst_Join::ThisButton( unsigned long buttonID ) {
	return buttonID==SVRWND_BTN_CONECT;
}
void Btn_SvrLst_Join::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	LobbyInt * lob = intf.GetLobby();
	if( intf.OpInProg() ) {
		lob->ExtendChat("operation already in progress...");
		return;
	}
	GNP::Game h;
	int svrIndex=lob->GetSelItem();
	if( svrIndex < 0 ) {
		lob->ExtendChat("you need to select a server...");
	}
	else if( intf.GetServerData(svrIndex,h)) {
		if( h.noOfPlayers < h.maxPlayers ) {
			uint16_t i = (uint16_t)svrIndex;
			StdOPacket p;
			p<<i;
			intf.net.AddUpdate( GNP::JOIN_GAME,p);
			intf.net.InitCurUpdate();
			intf.InitOp();
		}
		else {
			lob->ExtendChat("server is full :(");
		}
	}
	else {
		SetCritErr("unable to get server data");
	}
}
bool Btn_SvrLst_SelSvr::ThisButton( unsigned long buttonID ) {
	return buttonID==LOB_ITM_LB_NAMES;
}
void Btn_SvrLst_SelSvr::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	if( newState!=SERVERLIST)
		return;
	LobbyInt * lob = intf.GetLobby();
	int svrIndex=lob->GetSelItem();
	GNP::Game g;
	if( svrIndex < 0 ) {
		return;
	}
	else if( intf.GetServerData(svrIndex,g)) {
		intf.SetDataBox(g);
	}
	else {
		SetCritErr("unable to get server data");
	}
}
bool Btn_Lob_Leave::ThisProcess( unsigned long buttonID, NumState state ) {
	return buttonID==LOB_BTN_LEAVE&&state==LOBBY;
}
void Btn_Lob_Leave::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	if( intf.OpInProg() ) {
		intf.GetLobby()->ExtendChat("error: operation in progress");
		return;
	}
	StdOPacket p;
	intf.net.AddUpdate(GNP::LEAVE_GAME,p);
	intf.net.InitCurUpdate();
	intf.InitOp();
	
}
bool Btn_Lob_Kick::ThisProcess( unsigned long buttonID, NumState state ) {
	return buttonID==LOBHST_BTN_KICK&&state==LOBBY;
}
void Btn_Lob_Kick::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	LobbyInt*lob=intf.GetLobby();
	if( !intf.AmHost() ) {
		SetCritErr("kick button parsed to client");
	}
	if( intf.OpInProg() ) {
		lob->ExtendChat("error: operation in progress");
		return;
	}
	int pIndex=lob->GetSelItem();
	if( pIndex<0 ) {
		lob->ExtendChat("you need to select a player to kick");
		return;
	}
	gameint i=pIndex;
	StdOPacket p;
	p<<i;
	intf.net.AddUpdate(GNP::RMV_PLYR_DEF,p);
	intf.net.InitCurUpdate();
	intf.InitOp();
	
}
bool Btn_Lob_Start::ThisProcess( unsigned long buttonID, NumState state ) {
	return buttonID==LOBHST_BTN_START&&state==LOBBY;
}
void Btn_Lob_Start::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	LobbyInt*lob=intf.GetLobby();
	if( !intf.AmHost() ) {
		SetCritErr("kick button parsed to client");
	}
	if( intf.OpInProg() ) {
		lob->ExtendChat("error: operation in progress");
		return;
	}
	StdOPacket p;
	NumState state=COUNTDOWN;
	p<<state;
	intf.net.AddUpdate(GNP::CHANGE_STATE,p);
	intf.net.InitCurUpdate();
	intf.InitOp();
}
bool Btn_Lob_Promote::ThisProcess( unsigned long buttonID, NumState state ) {
	return buttonID==LOBHST_BTN_PRMT&&state==LOBBY;
}
void Btn_Lob_Promote::Process( unsigned long buttonID, MasterInterface& intf, NumState& newState ) {
	LobbyInt*lob=intf.GetLobby();
	if( !intf.AmHost() ) {
		SetCritErr("kick button parsed to client");
	}
	if( intf.OpInProg() ) {
		lob->ExtendChat("error: operation in progress");
		return;
	}
	int pIndex=lob->GetSelItem();
	if( pIndex<0 ) {
		lob->ExtendChat("you need to select a player to kick");
		return;
	}
	gameint player=pIndex;
	StdOPacket p;
	p<<player;
	intf.net.AddUpdate(GNP::PROMOTE,p);
	intf.net.InitCurUpdate();
	intf.InitOp();
}

BtnProcContainer StdBtnProcesses() {
	BtnProcContainer btns;
	btns.push_back( new Btn_MM_Exit );
	btns.push_back( new Btn_MM_ChangeNM );
	btns.push_back( new Btn_MM_SvrList );
	btns.push_back( new Btn_Chat );
	btns.push_back( new Btn_SvrLst_Host );
	btns.push_back( new Btn_SvrLst_Cancel );
	btns.push_back( new Btn_SvrLst_Join );
	btns.push_back( new Btn_Lob_Leave );
	btns.push_back( new Btn_Lob_Start );
	btns.push_back( new Btn_Lob_Promote );
	btns.push_back( new Btn_Lob_Kick );
	btns.push_back( new Btn_SvrLst_SelSvr );
	return btns;
}


