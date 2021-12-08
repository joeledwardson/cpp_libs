#include "gamesvr.h"

template<>
StdOPacket& operator<<(StdOPacket& p, const SvrChatPacket& chat ) {
	p << chat.soundIndex << chat.chatMsg;
	return p;
}

SvrChatPacket::SvrChatPacket( std::string chatMessage, std::string playerName, bool activeSound   ) {
	int tmp;
	std::istringstream in(chatMessage.c_str());

	//if chat message is a number between the first sound index (1) and the last sound index, then return the value
	//other wise return 0, because there is no sound to play

	in >> tmp;
	if(tmp >= SOUND_FIRSTSOUND && tmp <= SOUND_LASTSOUND && in.eof() && activeSound ) {
		soundIndex = (soundint)tmp;
	}
	else
		soundIndex = 0;

	if( playerName.size() )
		chatMsg = playerName + ": " + chatMessage;
	else
		chatMsg = chatMessage;
}

bool GameServer::PrcRq( std::string name, uint8_t& failReason ) {
	if( core.noOfPlayers >= core.maxPlayers ) {
		failReason = GNP::FAIL_FULL ;
		return false;
	}

	if( core.state != LOBBY ) {
		failReason = GNP::FAIL_INGAME;
		return false;
	}
			
	if( IsNameTaken( name )) {
		failReason = GNP::FAIL_NAMETAKEN;
		return false;
	}
	return true;
}	
gameint GameServer::GetHost() {
	return hostIndex;
}
void GameServer::SetHost( gameint index ) {
	assert( index < playerList.size() );
	hostIndex = index;
}
void GameServer::Promote( gameint playerIndex ) {
	_promote(playerIndex,true);
	SetHost(playerIndex);
}
void GameServer::Demote( gameint playerIndex ) {
	_promote(playerIndex,false);
}
void GameServer::_promote( gameint playerIndex, bool isPromote ) {
	assert( playerIndex < playerList.size() );
	StdOPacket p;
	p<<isPromote;
	gameNet.AddUpdate( GNP::PROMOTE, p );
	gameNet.InitCurUpdate( playerIndex );
}
GNP::Game GameServer::GetCore() {
	return core;
}
GameServer::GameServer( BNP::IPComm_HostType& net, uint32_t gameNetIndex,std::string lobNm )
	:gameNet(net,gameNetIndex) {
	hostIndex = 0;
	core.lobName = lobNm;
}
std::string GameServer::GetPlrNm( uint16_t pIndex ) {
	assert( pIndex < playerList.size() );
	return playerList[pIndex].name;
}
void GameServer::LobChatMsg( SvrChatPacket& packet ) {
	assert( core.state == LOBBY );
	StdOPacket p;
	p << packet;
	gameNet.AddUpdate( GNP::CHAT_XTND, p );
	gameNet.InitCurUpdate();
}
void GameServer::SvrRm( uint32_t sNetIndex ) {	
	gameNet.SvrRm( sNetIndex );
}
StdOPacket GameServer::FormReplySuccess(  ) {
	StdOPacket p;
	p << core;

	for(unsigned int i = 0; i < playerList.size(); i++ ) {
		p << playerList[i].name ;
	}
	return p;
}
bool GameServer::IsNameTaken( std::string name ) {
	for( unsigned int i = 0; i < playerList.size(); i++ )	{
		if( name ==  playerList[i].name  ) {
			return true;
		}
	}
	return false;
}
void GameServer::AddPlayer( std::string name ) {
	assert( core.state == LOBBY );
	assert( core.noOfPlayers == playerList.size() );	
	StdOPacket p;
	p<<name;
	gameNet.AddUpdate(GNP::ADD_PLAYER,p);
	gameNet.InitCurUpdate();
	core.noOfPlayers++;
	playerList.push_back( Player_Base(name) );
	
}
void GameServer::RmPlayer( uint16_t pIndex, std::string reason ) {
	assert( pIndex < playerList.size() );
	assert( core.noOfPlayers == playerList.size() );
	assert( pIndex < playerList.size() );
	std::string name = playerList[pIndex].name;
		
	if( core.state == LOBBY ) {
		StdOPacket p;
		p << pIndex;
		gameNet.AddUpdate( GNP::RMV_PLYR_DEF,p );
		gameNet.InitCurUpdate();
	}
	else if( (core.state == PLAYING || core.state==COUNTDOWN) && playerList.size()<2) {
		ChangeState( LOBBY );
		SvrChatPacket svrMsg( name + " quit: " + reason+ ". not enough players to continue");
		LobChatMsg( svrMsg );
	}
	else if( core.state == PLAYING )
		Game_InitRmPlayer( pIndex );
	
	core.noOfPlayers--;
	playerList.erase( playerList.begin() + pIndex );

}
gameint GameServer::NPlayers() {
	return playerList.size();
}
GameState GameServer::GetState() {
	return (GameState)core.state;
}


