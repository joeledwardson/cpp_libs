#ifndef MASTERMENU_H
#define MASTERMENU_H

#include <stdlib/platform.h>	//win32meanandlean prevents winsock.h being included
#include <Windows.h>
#include <mmsystem.h>
#include "cltinterface.h"
#include "vislib/menubase.h"
#include "gmelib/gametypes.h"
#include "vislib/lobby.h"
#include "gmelib/countdown.h"
#include "gmelib/gamenet.h"
#include "vislib/inputbox.h"
#include "gamebase.h"
#include "gmelib/player.h"
#include "vislib/D3DGraphics.h"
#include "gmelib/sound.h"
#include "vislib/keyboard.h"
#include "cltlib/viscountdown.h"
#include "stdlib/Format.h"

#include "cltstates.h"
#include "buttonlists.h"

#define BTNLST_NLISTS		4
#define BTNLSTIDX_MM		0
#define BTNLSTIDX_SVRLST	2
#define BTNLSTIDX_LOBHST	3
#define BTNLSTIDX_LOBCLT	1

#define LOADICO_MENU_X		MM_EXTRAHWND_X + 50
#define LOADICO_MENU_Y		200	



bool StateIsOnline( NumState state );
bool StateIsD3d( NumState state );


class MasterInterface;

class MasterClient {
	friend MasterInterface;
public:
	MasterClient(HWND hwnd, ItemServer& iServ, KeyboardServer& _kServ, ULONG svrIP);
	virtual ~MasterClient();
protected:
	void InitSvrList(BNP::NetIPacket& p);
	void InitLobby(BNP::NetIPacket& p);
	std::vector< BtnList >buttonLists;

	//server list
	Container_Game gameList;
	//

	//operation timeout - to avoid stacking update operations
	bool doingOp;
	Timer opTimeout;
	//

	//general variables
	std::string name;
	bool isHost;
	NumState state;
	BNP::IPComm_StdClient network;

	//visual variables
	D3DGraphics gfx;
	HWND window;
	ItemServer& _iServ;
	ItemClient itemIntf;
	KeyboardServer& kServ;
	KeyboardClient kbd;

	//visual displays
	BaseMenu * pMenu;
	LobbyInt * pLobby;
	CountDownVis countdown;
	//
	
};









#endif