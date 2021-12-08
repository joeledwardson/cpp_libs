#pragma once

#include "gmelib/gamenet.h"
#include "vislib/d3dgraphics.h"
#include "vislib/lobby.h"
#include "cltstates.h"
#include "gmelib/sound.h"
#include <sstream>

class MasterClient;

class MasterInterface {
public:
	void SetDataBox( const GNP::Game& g );
	MasterInterface( MasterClient& masterReference );
	bool AmHost();
	void ToHost();
	void ToCLient();
	bool GetServerData( unsigned int sIndex,GNP::Game& svr );
	void SetRecvPacket( const BNP::NetIPacket& packet );
	BNP::NetIPacket GetRecvPacket();
	D3DGraphicsClient Graphics();
	void ChangeName( const std::string& newName );
	HWND GetWind();
	LobbyInt * GetLobby();
	bool OpInProg();
	void InitOp();
	void OpStopped();
	std::string name();
	BNP::IPComm_StdClient& net;
	NumState GetState();
	void SyncCountDn( uint8_t secs );
	void PlayLobbySound( soundint soundIndex );
	Container_Game& GetSvrList();
protected:
	//TRANSITION variables
	BNP::NetIPacket recvPacket;
	//////////
	MasterClient& rf;
};
