#ifndef GAMENET_H
#define GAMENET_H

#include "netlib/networking.h"	//BNP, BaseNetworking{}...
#include "gametypes.h"

#define SYNCTIMER_TO	500	//syncs every x miliseconds

//game networking protocol (for tron)
namespace GNP {
	//socket identifiers to pass to send and receive functions
	std::vector<SockData> SvrSockList();
	static const char * SVR_HOSTNAME = "joelyboy94.zapto.org";

	/*identifiers for client, i.e. anyone playing the game*/
	NID SOCK_CLT_NSOCKETS				= 1;
	NID SOCK_CLT_STDSOCK				= 0;	//connection to server


	NID SOCK_SVR_NSOCKETS			= 1;
	NID SOCK_SVR_STDSOCK			= 0;

	NID MAX_CLIENT_RQST				= 100; //max client requests at a time
	NID MAX_HOSTS					= 100; //max hosts at a time

	/******************  port IDs  ******************/
	NID_BIG PORT_SVR				= 33334; //sever comm ip port
	/************************************************/

	
	enum CORE_UPDATES {
		//changing state of game - host->server, server->ALL
		CHANGE_STATE,
		//in game updates
		ADD_PLAYER,
		RMV_PLYR_DEF,	//player leaves/disconnects etc - this message is between server clients and master server
		EDIT_LOB_NAME,
		EDIT_MAX_PLYS,
		COUNTDOWN_SYNC ,
		GAME_SYNC,
		LEAVE_GAME, //used as client to leave game, for host means close game
		PROMOTE, //promote player to host		
		//extend chat - used in game and server list
		CHAT_XTND,
		//server list updates
		ADD_SVR,
		RM_SVR,
		SVR_UPDATE,
		JOIN_GAME,
		HOST_GAME,
		CORE_UPDATES_END
	};

	enum REPLY_TYPES {
		FAIL_INGAME,
		FAIL_FULL,
		FAIL_NAMETAKEN,
		FAIL_NOTFOUND,
		FAIL_NODATA,
		REPLY_TYPES_END
	};

	std::string ToStr( const GNP::REPLY_TYPES& denyReason );

	struct ChatPacket {
		ChatPacket( );
		uint8_t soundIndex;
		std::string chatMsg;
	};	
	
	
	static const uint16_t	DEF_MAXPLAYERS = 4;
	static const uint16_t	DEF_NPLAYERS = 0;
	static const uint8_t	DEF_STATE = LOBBY;

	struct Game {
		Game();
		uint16_t maxPlayers;
		uint16_t noOfPlayers;
		std::string lobName;
		uint8_t state;
	};
	/************************************************/

	
}

typedef std::vector<GNP::Game> Container_Game;

class IPComm_SvrSubList
{
public:
	IPComm_SvrSubList( BNP::IPComm_HostType& net, uint32_t netListIndex )
		:sNet(net),
		netIndex( netListIndex )
	{}
	void AddUpdate( int updateType, StdOPacket& packet , bool forceSend = false) {	
		sNet.AddUpdate( updateType, packet , forceSend);
	}
	void InitCurUpdate() {
		sNet.InitCurUpdate( netIndex );
	}
	void InitCurUpdate( uint32_t pIndex ) {
		sNet.InitCurUpdate( BNP::ContPos(netIndex, pIndex ));
	}
	void SvrRm( uint32_t sNetIndex ) {
		if( sNetIndex < netIndex )
			netIndex--;
	}

private:

	BNP::IPComm_HostType& sNet;
	uint32_t netIndex;
};




#endif
