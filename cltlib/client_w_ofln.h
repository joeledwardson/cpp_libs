#pragma once

#include "buttonlists.h"
#include "buttonprocesses.h"
#include "keyboardprocess.h"

enum OflnState {
	OFLN_MENU=CLTSTATES_END,
	OFLN_STATES_END
};

static unsigned long MM_BTN_OFFLINE		= 112;

static unsigned long SM_BTN_2PLAYERS		= 113;
static unsigned long SM_BTN_3PLAYERS		= 114;
static unsigned long SM_BTN_4PLAYERS		= 115;
static unsigned long SM_BTN_CANCEL		= 116;
#define ID_TO_NPLS(BTNID)	BTNID-SM_BTN_2PLAYERS+2

#define BTNLST_NLISTS_WOFL	5
#define BTNLSTIDX_OFFLINE	4

class MasterInterfaceWOfln;

class ClientWOffline : public MasterClient {
	friend MasterInterfaceWOfln;
public:
	ClientWOffline(HWND hwnd, ItemServer& iServ, KeyboardServer& _kServ, ULONG svrIP);
	virtual ~ClientWOffline();
protected:
	bool isOnline;
};

class MasterInterfaceWOfln : public MasterInterface {
public:
	bool IsOnline();
	MasterInterfaceWOfln( ClientWOffline& masterReference );
	void SetOflNPlayers( unsigned int nPlayers );
	unsigned int GetOflnPlayers();
private:
	ClientWOffline& oflRf;
	unsigned int nOflnPlayers;
};


class Btn_Ofl_Play : public ButtonProcess {
	bool ThisProcess( unsigned long buttonID, NumState state );
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
};
class Btn_Ofl_Cancel : public ButtonProcess {
	bool ThisProcess( unsigned long buttonID, NumState state );
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
};
BtnProcContainer BtnPrcListWOfln();
BtnList Buttons_Offline();

class KeyPrcOflCountDnEsc : public KeyPrcOlnCountDnEsc {
public:
	void ProcessKey( NumState& newState,MasterInterface& intf  );
};
KeyPrcContainer GetPrcKeysWOfl();

class Btn_MM_Ofln : public ButtonProcess {
	bool ThisProcess( unsigned long buttonID, NumState state );
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
};
void Buttons_MainMenu_ToOfl( BtnList& btns );
