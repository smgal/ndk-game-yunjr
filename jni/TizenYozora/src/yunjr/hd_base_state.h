
#ifndef __HD_BASE_STATE_H__
#define __HD_BASE_STATE_H__

#include "hd_z_config.h"

namespace hadar
{
	enum GAMESTATE
	{
		GAMESTATE_TITLE = 0,
		GAMESTATE_CREATE_CHARACTER,
		GAMESTATE_GAME_PLAY,
		GAMESTATE_EXIT
	};

	void changeGameState(GAMESTATE state);

	// internal
	bool isGameStateChanging(void);
	GAMESTATE getRequiredGameStateAndReset(void);



	class Exception
	{
	};

	class ExceptionExitGame: public Exception
	{
	};
}

#endif
