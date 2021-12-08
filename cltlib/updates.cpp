#include "updates.h"

UpdateProcess::~UpdateProcess() {
}
bool UpPrc_AddPlyr::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::ADD_PLAYER&&state==LOBBY;
}
void UpPrc_AddPlyr::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	std::string plrName;
	inPacket>>plrName;
	intf.GetLobby()->AddItem(plrName);
}
bool UpPrc_ChangeState::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::CHANGE_STATE;
}
void UpPrc_ChangeState::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	uint8_t nState;
	inPacket>>nState;
	newState=nState;
	intf.SetRecvPacket( inPacket );
}
bool UpPrc_LobRmPlr::ThisUpType( byte updateType, NumState state ) {
	return (state==LOBBY)&&(updateType==GNP::RMV_PLYR_DEF);
}
void UpPrc_LobRmPlr::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	intf.OpStopped();
	gameint playerIndex;
	inPacket>>playerIndex;
	LobbyInt *lob=intf.GetLobby();
	int nItems=lob->GetNItems();
	if( playerIndex >= nItems)
		SetCritErr("kick player packet contained index out of range!");
	intf.GetLobby()->RemoveItem(playerIndex);
}
bool UpPrc_CountDnSync::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::COUNTDOWN_SYNC&&state==COUNTDOWN;
}
void UpPrc_CountDnSync::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	uint8_t nSecsElpd;
	inPacket>>nSecsElpd;
	intf.SyncCountDn(nSecsElpd);
}
bool UpPrc_Promote::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::PROMOTE;
}
void UpPrc_Promote::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	intf.OpStopped();
	bool prmt;
	inPacket>>prmt;
	if ( prmt==intf.AmHost())
		SetCritErr("attempted promotion/demotion failed");
	if( !prmt ) {
		intf.ToCLient();
		if( newState==LOBBY )
			intf.GetLobby()->ExtendChat("demoted to client");
	}
	else {
		intf.ToHost();
		if( newState==LOBBY )
			intf.GetLobby()->ExtendChat("promoted to host");
	}
}
bool UpPrc_ChatMsg::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::CHAT_XTND&&(state==LOBBY||state==SERVERLIST);
}
void UpPrc_ChatMsg::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	GNP::ChatPacket chat;
	inPacket>>chat;
	if( chat.soundIndex )
		intf.PlayLobbySound( chat.soundIndex );
	intf.GetLobby()->ExtendChat( chat.chatMsg );
}
bool UpPrc_SvrAdd::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::ADD_SVR&&state==SERVERLIST;
}
void UpPrc_SvrAdd::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	GNP::Game h;
	Container_Game& hList=intf.GetSvrList();
	bool wasEmpty=hList.empty();
	inPacket>>h;
	hList.push_back(h);
	intf.GetLobby()->AddItem(h.lobName);
	if( wasEmpty ) {
		intf.SetDataBox( h );
		intf.GetLobby()->SetSelItem( 0 );
	}
}
bool UpPrc_SvrRm::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::RM_SVR&&state==SERVERLIST;
}
void UpPrc_SvrRm::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	gameint serverIndex;
	inPacket>>serverIndex;
	Container_Game& hList=intf.GetSvrList();
	if( serverIndex>=hList.size() )
		SetCritErr("delete server packet containing out of range index");
	hList.erase(hList.begin()+serverIndex);
	intf.GetLobby()->RemoveItem(serverIndex);
	if( hList.empty() ) {
		intf.GetLobby()->SetDataBox("no servers found");
	}
}
bool UpPrc_SvrUpdate::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::SVR_UPDATE&&state==SERVERLIST;
}
void UpPrc_SvrUpdate::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	gameint serverIndex;
	GNP::Game h;
	inPacket>>serverIndex>>h;
	Container_Game& hList=intf.GetSvrList();
	if( serverIndex>=hList.size() )
		SetCritErr("updating server packet containing out of range index");
	int selHost=intf.GetLobby()->GetSelItem();
	if( selHost==serverIndex ) {
		intf.SetDataBox(h);
	}
}
bool UpPrc_HostGm::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::HOST_GAME&&state==SERVERLIST;
}
void UpPrc_HostGm::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	if( UpPrc_JoinGm::PrcRq(intf,inPacket,newState,"host"))
		intf.ToHost();
}
bool UpPrc_JoinGm::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::JOIN_GAME&&state==SERVERLIST;
}
void UpPrc_JoinGm::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	if( PrcRq( intf,inPacket,newState,"join"))
		intf.ToCLient();
}
bool UpPrc_JoinGm::PrcRq( MasterInterface& intf, BNP::NetIPacket& p,NumState& newState,std::string typ ) {
	intf.OpStopped();
	uint8_t success;
	p>>success;
	if( !success ) {
		uint8_t denyReason;
		p>>denyReason;
		std::string strDeny( GNP::ToStr( (GNP::REPLY_TYPES)denyReason ));
		intf.GetLobby()->ExtendChat("failed to "+typ+" game: "+strDeny); 
		intf.ToCLient();
	}
	else {
		intf.SetRecvPacket( p );
		newState=LOBBY;
	}
	return success!=0;
}
bool UpPrc_LeaveGame::ThisUpType( byte updateType, NumState state ) {
	return updateType==GNP::LEAVE_GAME&&StateIsOnline(state);
}
void UpPrc_LeaveGame::Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState ) {
	intf.SetRecvPacket(inPacket);
	newState=SERVERLIST;
}


UpdateContainer GetUpdatesStd() {
	UpdateContainer updates;
	updates.push_back( new UpPrc_AddPlyr );
	updates.push_back( new UpPrc_LobRmPlr );
	updates.push_back( new UpPrc_CountDnSync );
	updates.push_back( new UpPrc_Promote );
	updates.push_back( new UpPrc_ChatMsg );
	updates.push_back( new UpPrc_SvrAdd );
	updates.push_back( new UpPrc_SvrRm );
	updates.push_back( new UpPrc_SvrUpdate );
	updates.push_back( new UpPrc_HostGm );
	updates.push_back( new UpPrc_JoinGm );
	updates.push_back( new UpPrc_LeaveGame );
	updates.push_back( new UpPrc_ChangeState );
	return updates;
}
