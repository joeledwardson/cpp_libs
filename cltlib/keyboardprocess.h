#pragma once

#include "vislib/keyboard.h"
#include "cltstates.h"
#include "cltinterface.h"

class KeyProcess {
public:
	virtual bool ThisKeyPressed( KeyboardClient& kbd, NumState state )=0;
	virtual void ProcessKey( NumState& newState,MasterInterface& intf )=0;
	virtual ~KeyProcess();
};
class KeyPrcHost : public virtual KeyProcess {
public:
	virtual ~KeyPrcHost();
	static void ChangeState( NumState newState, MasterInterface& intf  );
};
class KeyPrcOlnCountDnEsc : public KeyPrcHost {
public:
	bool ThisKeyPressed( KeyboardClient& kbd, NumState state );
	void ProcessKey( NumState& newState,MasterInterface& intf  );
};
typedef std::vector<KeyProcess*>KeyPrcContainer;

KeyPrcContainer GetPrcKeys();
