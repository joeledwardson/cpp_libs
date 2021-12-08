#ifndef CLTSTATES_H
#define CLTSTATES_H

#include "gmelib/gametypes.h"

enum ProgramState {
	MAINMENU=GAMESTATE_END,
	SERVERLIST,
	PGRMST_END,
	STATE_EXIT,
	CLTSTATES_END
};

typedef uint8_t NumState;

#endif