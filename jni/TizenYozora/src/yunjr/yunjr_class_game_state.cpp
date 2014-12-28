
#include "yunjr_class_game_state.h"

namespace
{
	sena::auto_ptr<yunjr::GameState> s_game_state;
}

const yunjr::GameState& yunjr::GameState::getInstance(void)
{
	return yunjr::GameState::getMutableInstance();
}

yunjr::GameState& yunjr::GameState::getMutableInstance(void)
{
	if (s_game_state.get() == 0)
		s_game_state.bind(new yunjr::GameState());

	return *(s_game_state.get());
}
