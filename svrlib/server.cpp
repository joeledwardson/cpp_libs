#include "server.h"

#ifdef GAME_OUT_OSTREAM
std::ostream& operator<<(std::ostream& out,const std::string& str ) {
	return out << str.c_str();
}
#endif

Process::~Process() {
}

UpdatePacketProcess::~UpdatePacketProcess() {
}
bool UpdatePacketProcess::ThisProcess( byte cmType, byte updateType, bool isConnected ) {
	return cmType == BNP::CM_UPDATE_SND && ThisUpTyp( updateType ) && isConnected;
}
GameUpdate::~GameUpdate() {
}
ProcessContainer GetProcessesStd() {
	ProcessContainer prcList;
	prcList.push_back( new  ChatMsg );
	prcList.push_back( new  Quit);
	prcList.push_back( new  ChangeState);
	prcList.push_back( new  PromotePlayer);
	prcList.push_back( new  KickPlayer);
	prcList.push_back( new  LeaveGame);
	prcList.push_back( new  HostGame);
	prcList.push_back( new  JoinGame);
	prcList.push_back( new  JoinMainLobby);
	return prcList;
}

bool Quit::ThisProcess( byte cmType, byte, bool isConnected ) {
	return cmType == BNP::CM_QUIT && isConnected==true;
}
void Quit::ProcessMsg( BNP::NetIPacket& ,BNP::ContPos pos,  SvrInterface& svr ) {
	if( pos.lIndex == LIST_SVRCONTACTS ) {
		GM_OUT<<"client name "<<QTMK<<svr.GetPlrNm(pos.cIndex)<<QTMK<<" quit main lobby"<<NL;
		svr.RmLobbyPlayer( pos.cIndex );
		svr.network.RemoveContact(pos);
	}
	else {
		GameServer * gameSvr= svr.GetServer(pos);
		gameint sIndex = svr.ServerIndex(pos);
		GM_OUT<<"client name "<<QTMK<<gameSvr->GetPlrNm(pos.cIndex)<<QTMK<<" quit game server "<<svr.ServerIndex(pos)<<NL;
		svr.network.RemoveContact( pos );
		svr.RmGamePlayer(pos,"quit");
		if( gameSvr->NPlayers() == 0 ) {
			svr.RmServer( sIndex,"quit");
			GM_OUT<<"final player quit server "<<sIndex<<",deleting..."<<NL;
		}
	}
	
}
bool JoinMainLobby::ThisProcess( byte cmType, byte, bool isConnected ) {
	return cmType==BNP::CM_NORMREQUEST&&isConnected==false;
}
void JoinMainLobby::ProcessMsg( BNP::NetIPacket& input,BNP::ContPos,  SvrInterface& svr ) {
	BNP::IPComm_HostType& network = svr.network;
	try {
		byte denied;
		std::string name;
		input >> name;
		

		if( !EvlClientRq( name,denied,svr) ) {
			GM_OUT << "client "<<QTMK<<name<<QTMK<<"was denied join"<<NL;
			BNP::NetOPacket packet( network.FormReplyFailure( input, denied ) );
			sockaddr s = network.GetSockAddr();
			network.SendPacket( GNP::SOCK_SVR_STDSOCK, packet,&s);
		}
		else {
			GM_OUT << "client "<<QTMK<<name<<QTMK<<" was accepted join request"<<NL;
			BNP::NetOPacket packet( network.FormReplySuccess( input ) );
			
			
			StdOPacket lobReply( FormLobReply(svr));
			packet << lobReply;
			sockaddr s = network.GetSockAddr();
			network.SendPacket( GNP::SOCK_SVR_STDSOCK,packet, &s );
			network.AddContact_Host( LIST_SVRCONTACTS,input,GNP::SOCK_SVR_STDSOCK,network.GetSockAddr());

			svr.AddLobbyPlayer( name );
		}
	}
	catch (PacketInputErr) {
		GM_OUT<<INPRCERR<<"invalid request recieved. probably empty request"<<NL;
	}
	
}
bool JoinMainLobby::EvlClientRq(std::string name, byte& denied,SvrInterface& svr ) {
	if( !name.length() ) {
		denied = GNP::FAIL_NODATA;
		return false;
	}
	if( svr.NLobbyClients() >= MaxClients ) {
		denied = GNP::FAIL_FULL;
		return false;
	}
	
	for( unsigned int i = 0; i <  svr.NLobbyClients(); i++ ) {
		if( name == svr.GetPlrNm(i) ) {
			denied = GNP::FAIL_NAMETAKEN;
			return false;
		}
	}
	return true;
	
}
StdOPacket JoinMainLobby::FormLobReply(SvrInterface& svr) {
	StdOPacket packet;
	gameint nServers = svr.NServers();
	packet << nServers;
	for( gameint i = 0; i < nServers; i++ ) {
		packet << svr.GetServer(i)->GetCore();
	}
	return packet;
}	


bool ChatMsg::ThisUpTyp( byte upType ) {
	return upType==GNP::CHAT_XTND;
}
void ChatMsg::ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos,  SvrInterface& svr ) {
	if( pos.lIndex != LIST_SVRCONTACTS ) {
		if( svr.GetServer( pos )->GetCore().state != LOBBY ) {
			GM_OUT<<"error, chat message, state is not lobby"<<NL;
			return;
		}
	}
	try {
		std::string message,name;
		input >> message;
		if( pos.lIndex == LIST_SVRCONTACTS ) {
			name = svr.GetPlrNm( pos.cIndex );
		}
		else {
			name = svr.GetServer(pos)->GetPlrNm(pos.cIndex);
		}
		SvrChatPacket chatPacket( message,name,true);
		StdOPacket packet;
		packet << chatPacket;
		svr.network.AddUpdate( GNP::CHAT_XTND,packet );
		svr.network.InitCurUpdate( pos.lIndex );
	}
	catch( PacketInputErr ) {
		GM_OUT<<INPRCERR<<"processing chat message"<<NL;
	}

}

void GameUpdate::ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr ) {
	if( pos.lIndex == LIST_SVRCONTACTS ) {
		GM_OUT<<"error: recevied change state message from server list contact "<<pos.cIndex<<NL;
		return;
	}
	else {
		GameServer * game = svr.GetServer(pos);
		ProcessUpdate( input,pos,svr,game);
	}
}

bool ChangeState::ThisUpTyp( byte upType ) {
	return upType==GNP::CHANGE_STATE;
}
void ChangeState::ProcessUpdate(BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr,GameServer * game ) {
	if( pos.cIndex != game->GetHost() ) {
		GM_OUT<<"error: non-host client send change-state packet"<<NL;
		return;
	}
	try {
		uint8_t newState,oldState;
		input >> newState;
		if( newState==game->GetCore().state ) {
			GM_OUT<<"error: new-state = current-state in change-state packet"<<NL;
			return;
		}
		if( newState >= GAMESTATE_END ) {
			GM_OUT<<"error: new-state is out of range in chage-state packet"<<NL;
			return;
		}
		//success
		oldState=game->GetState();
		gameint svrIndex = svr.ServerIndex(pos);
		if( newState==LOBBY ) {
			GM_OUT<<"game server index "<<svrIndex<<" exited game to lobby"<<NL;
		}
		else if( newState==COUNTDOWN&&oldState==LOBBY )
			GM_OUT<<"game server index "<<svrIndex<<" started game"<<NL;
		game->ChangeState( input,GameState(newState) );
		svr.InitSvrUpdate( svr.ServerIndex(pos),game->GetCore() );
	}
	catch (PacketInputErr ) {
		GM_OUT<<INPRCERR<<"game server changing state"<<NL;
	}
}
bool PromotePlayer::ThisUpTyp( byte upType ) {
	return upType==GNP::PROMOTE;
}
void PromotePlayer::ProcessUpdate(BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr,GameServer * game ) {
	if( pos.cIndex != game->GetHost() ) {
		GM_OUT<<"error: non-host player tried to kick player"<<NL;
		return;
	}
	try {
		gameint newHost;
		input>>newHost;
		if( newHost==game->GetHost() ) {
			GM_OUT<<"error: in game index "<<svr.ServerIndex(pos)<<", host tried to promote himself"<<NL;
		}
		else {
			GM_OUT<<"game index "<<svr.ServerIndex(pos)<<"; promoted player "<<game->GetPlrNm(newHost)<<NL;
			game->Demote( game->GetHost() );
			game->Promote(newHost);
		}
	}
	catch( PacketInputErr) {
		GM_OUT<<INPRCERR<<"host promotion packet"<<NL;
	}
}
bool KickPlayer::ThisUpTyp( byte upType ) {
	return upType==GNP::RMV_PLYR_DEF;
}
void KickPlayer::ProcessUpdate(BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr,GameServer * game ) {
	if( pos.cIndex != game->GetHost() ) {
		GM_OUT<<"error: non-host player tried to kick player"<<NL;
		return;
	}
	try {
		
		gameint playerIndex;
		input>>playerIndex;
		svr.MvGamePlayerToLobby(BNP::ContPos(pos.lIndex,playerIndex),"kicked");
		svr.InitSvrUpdate( svr.ServerIndex(pos),game->GetCore());
		
	}
	catch( PacketInputErr ) {
		GM_OUT<<INPRCERR<<"kick player packet"<<NL;
	}
}
bool LeaveGame::ThisUpTyp( byte upType ) {
	return upType==GNP::LEAVE_GAME;
}
void LeaveGame::ProcessUpdate(BNP::NetIPacket&,BNP::ContPos pos, SvrInterface& svr,GameServer *  ) {
	svr.MvGamePlayerToLobby( pos,"left");
}
bool HostGame::ThisUpTyp( byte upType ) {
	return upType==GNP::HOST_GAME;
}
bool HostGame::EvaluateRequest( std::string lobbyName,uint8_t& denied, SvrInterface& svr ) {
	if( svr.NServers() >= MaxServers ) {
		denied= GNP::FAIL_FULL;
		return false;
	}
	for( uint32_t i = 0; i < svr.NServers(); i++ ) {		
		if( lobbyName == svr.GetServer(i)->GetCore().lobName ) {
			denied = GNP::FAIL_NAMETAKEN;
			return false;
		}
	}
	return true;
}
void HostGame::ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos,  SvrInterface& svr ) {
	if( pos.lIndex != LIST_SVRCONTACTS ) {
		GM_OUT<<"error: client already in game server requested to host game"<<NL;
		return;
	}
	uint8_t denyReason,success;
	StdOPacket reply;
	std::string name=svr.GetPlrNm( pos.cIndex ),lobbyName=name+"'s lobby";
	success=EvaluateRequest(lobbyName,denyReason,svr);	
	reply << success;
	if( !success ) {	//fail
		GM_OUT << "lobby of name "<<QTMK<<lobbyName<<QTMK<<" denied"<<NL;
		reply << denyReason;
		svr.network.AddUpdate( GNP::HOST_GAME, reply );
		svr.network.InitCurUpdate( pos );
	}
	else {
		GM_OUT << "lobby of name "<<QTMK<<lobbyName<<QTMK<<" accepted"<<NL;
		unsigned int listIndex = svr.network.GetNLists();
		svr.network.AddList();
		
		GameServer * newGameSvr = svr.CreateServer(listIndex, name,lobbyName,input);
		svr.AddServer( newGameSvr );
		
		GNP::Game core = svr.GetServer( svr.NServers() - 1 )->GetCore();
		reply << core << name;
		svr.network.AddUpdate( GNP::HOST_GAME, reply );
		svr.network.InitCurUpdate( pos );
		
		svr.network.MoveContact( pos,svr.network.GetNLists()-1);

		svr.RmLobbyPlayer( pos.cIndex );
		
	}
}
bool JoinGame::ThisUpTyp( byte upType ) {
	return upType==GNP::JOIN_GAME;
}
void JoinGame::ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos,  SvrInterface& svr ) {
	if( pos.lIndex != LIST_SVRCONTACTS ) {
		GM_OUT<<"error main lobby client "<<pos.cIndex<<" tried to join game while in a game"<<NL;
		return;
	}
	std::string name = svr.GetPlrNm( pos.cIndex );
	gameint svrIndex;
	uint8_t failReason;
	StdOPacket reply;
	try {
		input >> svrIndex;
		uint8_t success = EvalRequest( name,failReason,svrIndex,svr);
		reply << success;
		if( success ) {
			GameServer * game = svr.GetServer( svrIndex);
			reply << game->FormReplySuccess();
			svr.network.AddUpdate( GNP::JOIN_GAME,reply);
			svr.network.InitCurUpdate(pos);
			svr.RmLobbyPlayer( pos.cIndex );

			svr.network.MoveContact( pos,svrIndex+1);
			game->AddPlayer( name );
			
			svr.InitSvrUpdate( svrIndex,game->GetCore());
			
			GM_OUT<<"main lobby client "<<pos.cIndex<<" successfuly joined game "<<svrIndex<<NL;
		}
		else {
			GM_OUT<<"main lobby client "<<pos.cIndex<<" failed to join game "<<svrIndex<<NL;
			reply << failReason;
			svr.network.AddUpdate( GNP::JOIN_GAME,reply);
			svr.network.InitCurUpdate(pos);
		}
	}
	catch( PacketInputErr ) {
		GM_OUT<<INPRCERR<<"joining game server request"<<NL;
	}

}
bool JoinGame::EvalRequest( std::string name,uint8_t& failReason, gameint svrIndex,SvrInterface& svr ) {
	if( svrIndex >= svr.NServers() ) {
		failReason = GNP::FAIL_NOTFOUND;
		return false;
	}
	if( !svr.GetServer(svrIndex)->PrcRq( name,failReason ) )
		return false;
	
	return true;
}


SvrInterface::SvrInterface(BNP::IPComm_HostType& net, std::vector< std::string >& _clientNames, std::vector< GameServer* >& _serverList )
	:network(net),
	clientNames( _clientNames ),
	serverList( _serverList ) {

}
void SvrInterface::MvGamePlayerToLobby( BNP::ContPos pos,std::string reason ) {
	GameServer * svr( GetServer(pos) );
	GM_OUT<<"player name "<<QTMK<<svr->GetPlrNm(pos.cIndex)<<QTMK<<"left game "<<ServerIndex(pos)<<" and joined main lobby because "<<reason<<NL;
	std::string  name = svr->GetPlrNm(pos.cIndex);
	network.MoveContact(pos,LIST_SVRCONTACTS);
	RmGamePlayer(pos,reason);
	if( !svr->NPlayers() )
		RmServer( ServerIndex(pos),reason );
	AddLobbyPlayer( name );
	StdOPacket p;
	p << JoinMainLobby::FormLobReply(*this);
	pos.lIndex = LIST_SVRCONTACTS;
	pos.cIndex = clientNames.size()-1;
	network.AddUpdate( GNP::LEAVE_GAME,p);
	network.InitCurUpdate( pos );
	
}
void SvrInterface::RmGamePlayer( BNP::ContPos pos,std::string reason ) {
	RmGamePlayer( ServerIndex(pos),pos.cIndex,reason);
}
void SvrInterface::RmGamePlayer(gameint sIndex, gameint pIndex,std::string reason ) {
	GameServer * game = serverList[sIndex];
	GM_OUT<<"client "<<pIndex<<" of game server "<<sIndex<<" left because: "<<reason<<NL;
	game->RmPlayer( pIndex, reason );
	GNP::Game core = game->GetCore();
	InitSvrUpdate( sIndex,core );
	if( core.noOfPlayers>0 ) {
		if(pIndex == game->GetHost() ) {
			GM_OUT<<"game server "<<sIndex<<" host left, promoting default player 0"<<NL;
			game->Promote( 0 );
		}
	}
}
void SvrInterface::InitSvrUpdate( gameint sIndex, const GNP::Game& svr ) {
	StdOPacket p;
	p << sIndex << svr;
	network.AddUpdate( GNP::SVR_UPDATE,p);
	network.InitCurUpdate( LIST_SVRCONTACTS );
}
void SvrInterface::InitNewSvr( const GNP::Game& svr ) {
	StdOPacket p;
	p << svr;
	network.AddUpdate( GNP::ADD_SVR,p );
	network.InitCurUpdate( LIST_SVRCONTACTS );
}
void SvrInterface::InitDelSvr(gameint sIndex) {
	StdOPacket p;
	assert( sIndex < serverList.size());
	p << sIndex;
	network.AddUpdate( GNP::RM_SVR,p);
	network.InitCurUpdate( LIST_SVRCONTACTS );
}
void SvrInterface::RmServer( uint16_t sIndex,std::string ) {
	assert( sIndex < serverList.size() );
	assert( serverList[ sIndex ]->GetCore().noOfPlayers == 0 );	//currently only empty servers quit - player promoted if host quits	
		
	InitDelSvr( sIndex );
	for( unsigned int i = 0; i < serverList.size(); i++ ) {
		serverList[i]->SvrRm( sIndex );
	}	
	network.RmList( sIndex + 1 );	
	DelSvr(sIndex);
	serverList.erase( serverList.begin() + sIndex );
}
GameServer * SvrInterface::GetServer( BNP::ContPos pos ) {
	assert( ServerIndex(pos) < serverList.size());
	return serverList[ServerIndex(pos)];
}
GameServer * SvrInterface::GetServer( gameint sIndex ) {
	assert( sIndex < NServers() );
	return serverList[sIndex];
}
IPComm_SvrSubList SvrInterface::ServerNetwork(BNP::ContPos pos ) {
	assert( ServerIndex(pos) < serverList.size());
	return IPComm_SvrSubList( network,pos.lIndex );
}
gameint SvrInterface::ServerIndex( BNP::ContPos pos ) {
	assert( pos.lIndex-1 < serverList.size());
	return pos.lIndex-1;
}
gameint SvrInterface::NServers() {
	return serverList.size();
}
void SvrInterface::AddServer( GameServer *  svr ) {
	assert( NServers() < MaxServers );
	serverList.push_back(svr);
	InitNewSvr(svr->GetCore());
}
std::string SvrInterface::GetPlrNm(gameint pIndex) {
	assert( pIndex < clientNames.size() );
	return clientNames.at(pIndex);
}
unsigned int SvrInterface::GetNPlayers() {
	return clientNames.size();
}
void SvrInterface::RmLobbyPlayer( gameint pIndex ) {
	assert( pIndex < clientNames.size() );
	clientNames.erase( clientNames.begin() + pIndex );
}
void SvrInterface::AddLobbyPlayer( std::string player) {
	assert( clientNames.size() < MaxClients );
	clientNames.push_back( player );
}
gameint SvrInterface::NLobbyClients() {
	return clientNames.size();
}
	

