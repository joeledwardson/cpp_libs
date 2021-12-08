#ifndef SERVER_H
#define SERVER_H

//#define OUT_OSTREAM

#define INPRCERR "input process error: "
#ifndef GM_OUT
#define GAME_OUT_OSTREAM
#include <iostream>
#define GM_OUT std::cout
#define QTMK "\""
#define NL std::endl;
std::ostream& operator<<(std::ostream& out,const std::string& str );
#else
#define QTMK """"
#define NL	"\n"
#endif


#include <string>
#include <sstream>      // std::istringstream
#include "stdlib/error.h"
#include "gamesvr.h"

#define LIST_SVRCONTACTS	0
static const unsigned int MaxClients = 100;
static const unsigned int MaxServers = 20;

class SvrInterface;


class Process;
typedef std::vector< Process* > ProcessContainer;
class Process : public virtual BNP::RecvPacketProcess {
public:
	//returns true if value has changed
	virtual void ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr ) = 0;
	virtual ~Process();
};
class UpdatePacketProcess : public virtual Process {
public:
	bool ThisProcess( byte cmType, byte updateType, bool isConnected );
	virtual ~UpdatePacketProcess();
protected:
	virtual bool ThisUpTyp( byte updateType )=0;
};
//chat message - for main lobby or game lobby
class ChatMsg : public virtual UpdatePacketProcess {
public:
	bool ThisUpTyp( byte upType );
	void ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos,  SvrInterface& svr );
};
//quit message (CM_QUIT)
class Quit : public Process  {
public:
	bool ThisProcess( byte cmType, byte updateType, bool isConnected );
	void ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos,  SvrInterface& svr );
};
class GameUpdate : public virtual UpdatePacketProcess {
public:
	~GameUpdate();
	void ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr );
	virtual void ProcessUpdate(BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr,GameServer * game )=0;
};
class ChangeState : public GameUpdate  {
public:
	bool ThisUpTyp( byte upType );
	void ProcessUpdate(BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr,GameServer * game );
};
class PromotePlayer : public GameUpdate  {
public:
	bool ThisUpTyp( byte upType );
	void ProcessUpdate(BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr,GameServer * game );
};
class KickPlayer : public GameUpdate  {
public:
	bool ThisUpTyp( byte upType );
	void ProcessUpdate(BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr,GameServer * game );
};
class LeaveGame : public GameUpdate  {
public:
	bool ThisUpTyp( byte upType );
	void ProcessUpdate(BNP::NetIPacket& input,BNP::ContPos pos, SvrInterface& svr,GameServer * game );
};
//main lobby updates
class HostGame : public UpdatePacketProcess  {
public:
	bool ThisUpTyp( byte upType );
	void ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos,  SvrInterface& svr );
private:
	bool EvaluateRequest( std::string lobbyName,uint8_t& denied, SvrInterface& svr );
};
class JoinGame : public UpdatePacketProcess  {
public:
	bool ThisUpTyp( byte upType );
	void ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos,  SvrInterface& svr );
private:
	bool EvalRequest( std::string name, uint8_t& failReason, gameint svrIndex,SvrInterface& svr );
};
class JoinMainLobby : public Process  {
public:
	bool ThisProcess( byte cmType, byte updateType, bool isConnected );
	void ProcessMsg( BNP::NetIPacket& input,BNP::ContPos pos,  SvrInterface& svr );
	static StdOPacket FormLobReply(SvrInterface& svr);	
private:
	bool EvlClientRq(std::string name, byte& denied,SvrInterface& svr );	//evaluate request
};

ProcessContainer GetProcessesStd();


class SvrInterface {
public:
	SvrInterface( BNP::IPComm_HostType& net,std::vector< std::string >& _clientNames, std::vector< GameServer* >& _serverList);	//mus
	virtual ~SvrInterface() {}

	GameServer * GetServer( BNP::ContPos pos );
	GameServer * GetServer( gameint sIndex );
	IPComm_SvrSubList ServerNetwork(BNP::ContPos pos );
	gameint ServerIndex( BNP::ContPos pos );
	void InitSvrUpdate( gameint sIndex, const GNP::Game& svr );
	
	
	
	void MvGamePlayerToLobby(  BNP::ContPos pos,std::string reason );
	void RmGamePlayer( BNP::ContPos pos,std::string reason );
	void RmGamePlayer(gameint sIndex, gameint pIndex,std::string reason );
	void RmServer( uint16_t hIndex, std::string reason );
	gameint NServers();


	void AddServer( GameServer *  svr );
	virtual GameServer * CreateServer( gameint netListIndx,std::string hostName, std::string lobbyName,BNP::NetIPacket& input )=0;
	
	std::string GetPlrNm(gameint pIndex);
	unsigned int GetNPlayers();
	void RmLobbyPlayer( gameint pIndex );
	//this MUST be initialised fist as its passed to mainLobby in constructor
	
	void AddLobbyPlayer( std::string player);
	gameint NLobbyClients();
	
	BNP::IPComm_HostType& network;
protected:
	void InitDelSvr(gameint sIndex);
	void InitNewSvr( const GNP::Game& svr );

	virtual void DelSvr( gameint sIndex )=0;
	std::vector<std::string >& clientNames;
	std::vector< GameServer* >& serverList;	//must be pointers - not copy constructable
	
};

#endif
