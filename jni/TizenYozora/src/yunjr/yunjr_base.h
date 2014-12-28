
#ifndef __YUNJR_BASE_H__
#define __YUNJR_BASE_H__

#include "yunjr_z_config.h"

#include "yunjr_base_res.h"

#include "util/sm_util_sena.h"

#include <cassert>
#include <vector>
#include <utility>

namespace yunjr
{
	enum ENDBATTLE
	{
		ENDBATTLE_NONE,
		ENDBATTLE_LOSE,
		ENDBATTLE_WIN,
		ENDBATTLE_RUN_AWAY,
	};

	enum GAMESTATE
	{
		GAMESTATE_MAP,
		GAMESTATE_BATTLE,
	};

	enum EXITCODE
	{
		EXITCODE_BY_USER,
		EXITCODE_BY_ACCIDENT,
		EXITCODE_BY_ENEMY,
		EXITCODE_BY_FORCE,
	};

	enum BATTLERESULT
	{
		BATTLERESULT_EVADE,
		BATTLERESULT_WIN,
		BATTLERESULT_LOSE,
	};
}

namespace yunjr
{
	const int NUM_TILE_ID_GROUND_W = (1344 / TILE_W);
	const int NUM_TILE_ID_GROUND_H = (96 / TILE_H);
	const int NUM_TILE_ID_GROUND = NUM_TILE_ID_GROUND_W * NUM_TILE_ID_GROUND_H;
	const int NUM_TILE_ID_PLAYER = (672 / TILE_W) * (24 / TILE_H);

	enum TileId
	{
		TILE_ID_MIN    = 0,
		TILE_ID_GROUND = TILE_ID_MIN,
		TILE_ID_PLAYER = TILE_ID_GROUND + NUM_TILE_ID_GROUND,
		TILE_ID_MAX    = TILE_ID_PLAYER + NUM_TILE_ID_PLAYER
	};
}

namespace yunjr
{
	using flat_board::Color32;

	typedef TileBase<ResId> Tile;

	template <typename T>
	void deleteVector(std::vector<T*>& container)
	{
		typename std::vector<T*>::iterator item = container.begin();

		for ( ; item != container.end(); ++item)
			delete *item;

		container.clear();
	}

	template <typename T1, typename T2>
	void deleteVectorForPair(std::vector<std::pair<T1, T2*> >& container)
	{
		typename std::vector<std::pair<T1, T2*> >::iterator item = container.begin();

		for ( ; item != container.end(); ++item)
			delete item->second;

		container.clear();
	}

	template <typename T, typename Param1>
	struct Operator
	{
		Param1 param;

		Operator(Param1 in_param)
			: param(in_param) {	}

		void operator()(T p_obj);
	};
}

#endif // #ifndef __YUNJR_BASE_H__
