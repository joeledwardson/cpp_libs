#include "keyboardprocess.h"

KeyProcess::~KeyProcess() {
}
KeyPrcHost::~KeyPrcHost() {
}
void KeyPrcHost::ChangeState( NumState newState, MasterInterface& intf  ) {
	if( intf.AmHost() ) {
		StdOPacket p;
		p<<newState;
		intf.net.AddUpdate( GNP::CHANGE_STATE,p);
		intf.net.InitCurUpdate();
	}	
}
bool KeyPrcOlnCountDnEsc::ThisKeyPressed( KeyboardClient& kbd, NumState state ) {
	return kbd.keyPressedSF(VK_ESCAPE) && state==COUNTDOWN;	
}
void KeyPrcOlnCountDnEsc::ProcessKey( NumState& newState,MasterInterface& intf   ) {
	ChangeState( LOBBY,intf);
}

KeyPrcContainer GetPrcKeys() {
	KeyPrcContainer k;
	k.push_back( new KeyPrcOlnCountDnEsc );
	return k;
};