#ifndef GAMESVR_H
#define GAMESVR_H

#include "gmelib/gamenet.h"
#include <sstream>
#include "gmelib/sound.h"
#include "gmelib/player.h"



struct SvrChatPacket : public GNP::ChatPacket {
	SvrChatPacket( std::string chatMessage, std::string playerName = "", bool activeSound = false );
};	

class GameServer {

public:
	GameServer( BNP::IPComm_HostType& net, uint32_t gameNetIndex,std::string lobNm );
	
	virtual void ChangeState( GameState newState  )=0;
	virtual void ChangeState( BNP::NetIPacket& packet,GameState newState )=0;
	virtual void Go( bool& updateRequired ) = 0;
	

	void SvrRm( uint32_t sIndex );	//a server is removed of networking index sIndex - needed to update gameNet
	//format success - IMPORTANT* -JUST THE HOST DATA AND NAMES!
	StdOPacket FormReplySuccess( );	
	void AddPlayer( std::string name );
	void RmPlayer( uint16_t pIndex, std::string reason );
	std::string GetPlrNm( uint16_t pIndex );
	void LobChatMsg( SvrChatPacket& packet );
	bool PrcRq( std::string name, uint8_t& failReason );	
	gameint GetHost();
	void SetHost( gameint index );
	void Promote( gameint playerIndex );
	void Demote( gameint playerIndex );
	GNP::Game GetCore();
	gameint NPlayers();
	GameState GetState();
protected:
	void _promote(gameint playerIndex, bool isPromote );
	virtual void Game_InitRmPlayer( gameint playerIndex )=0;
	bool IsNameTaken( std::string name );
	//variables
	IPComm_SvrSubList gameNet;
	std::vector< Player_Base > playerList;
	GNP::Game core;
	gameint hostIndex;
	
};



#endif