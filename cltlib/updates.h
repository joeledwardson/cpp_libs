#ifndef UPDATES_H
#define UPDATES_H

#include "client.h"
#include <sstream>

class UpdateProcess{
public:
	virtual bool ThisUpType( byte updateType, NumState state )=0;
	virtual void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState )=0;
	virtual ~UpdateProcess();
};
class UpPrc_ChangeState : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};
class UpPrc_AddPlyr : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};
class UpPrc_LobRmPlr : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};
class UpPrc_CountDnSync : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};
class UpPrc_Promote : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};
class UpPrc_ChatMsg : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};
class UpPrc_SvrAdd : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};
class UpPrc_SvrRm : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};
class UpPrc_SvrUpdate : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};
class UpPrc_HostGm : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};
class UpPrc_JoinGm : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
	static bool PrcRq( MasterInterface& intf, BNP::NetIPacket& p,NumState& newState,std::string typ );
};
class UpPrc_LeaveGame : public UpdateProcess {
public:
	bool ThisUpType( byte updateType, NumState state );
	void Process(MasterInterface& intf,BNP::NetIPacket& inPacket, NumState& newState );
};

typedef std::vector<UpdateProcess*> UpdateContainer;
UpdateContainer GetUpdatesStd();

#endif