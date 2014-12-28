
#include "yunjr_script_def.h"

// TOWN
	// 01 object - pillar - on stone floor
	// 02 object - tree - on grass
	// 03 object - bed left - on stone floor
	// 04 object - bed right - on stone floor
	// 05 wall - horiz
	// 06 wall - verti
	// 07 wall - by brick
	// 08 storefront sign - FOOD
	// 09 storefront sign - CURE
	// 10 storefront sign - ARMS
	// 11 storefront sign - EXP
	// 12 object - sack - on stone floor
	// 13 object - pot - on stone floor
	// 14 object - steel case - on wood floor
	// 15 object - flowerpot - on wood floor
	// 16 wall - gradation
	// 17 object - well - on earthen floor
	// 18 object - stone - on earthen floor
	// 19 wall - wood(?)
	// 20 object - sword - on brick floor
	// 21 object - armor - on brick floor
	// 22 entrance - dungeon
	// 23 sign - on earthen floor
	// 24 floor - water
	// 25 floor - swamp
	// 26 floor - lava
	// 27 floor - stone floor with grass
	// 28 floor - grass with flowers 1
	// 29 floor - grass with flowers 2
	// 30 floor - wood 1-1
	// 31 floor - wood 1-2
	// 32 floor - wood 2
	// 33 floor - brick floor top-right
	// 34 floor - brick floor top-left
	// 35 floor - brick floor
	// 36 floor - brick floor top
	// 37 floor - brick floor right
	// 38 floor - brick floor left
	// 39 floor - brick floor left-bottom
	// 40 floor - brick floor right-bottom
	// 41 floor - brick floor bottom
	// 42 floor - red carpet
	// 43 floor - red carpet with deco
	// 44 floor - stone
	// 45 floor - earthen floor with grass
	// 46 floor - grass with flowers 3 (solid grass)
	// 47 floor - earthen floor
	// 48 character - skeleton - on brick floor
	// 49 character - man - on earthen floor
	// 50 character - woman - on earthen floor
	// 51 character - merchant - on wood floor
	// 52 character - mage - on stone floor
	// 53 character - knight - on stone floor
	// 54 character - LordAhn - on red carpet
	// 55 temporary

namespace
{
	void SetTownEmoji(MapTemplate& map_template)
	{
		_Map_SetTile(L"※", 0)

		// 47 floor - earthen floor
		_Map_SetTile(L"　", 47)
		// 45 floor - earthen floor with grass
		_Map_SetTile(L"．", 45)
		// 28 floor - grass with flowers 1
		_Map_SetTile(L"：", 28)
		// 29 floor - grass with flowers 2
		_Map_SetTile(L"；", 29)
		// 46 floor - grass with flowers 3 (solid grass)
		_Map_SetTile(L"∴", 46)

		// 44 floor - stone
		_Map_SetTile(L"×", 44)
		// 27 floor - stone floor with grass
		_Map_SetTile(L"÷", 27)
			
		// 30 floor - wood 1-1
		_Map_SetTile(L"++", 30)
		// 31 floor - wood 1-2
		_Map_SetTile(L"-+", 31)
		// 32 floor - wood 2
		_Map_SetTile(L"＋", 32)

		// 35 floor - brick floor
		_Map_SetTile(L"┼", 35)
		// 33 floor - brick floor top-right
		_Map_SetTile(L"┐", 33)
		// 34 floor - brick floor top-left
		_Map_SetTile(L"┌", 34)
		// 36 floor - brick floor top
		_Map_SetTile(L"┬", 36)
		// 37 floor - brick floor right
		_Map_SetTile(L"┤", 37)
		// 38 floor - brick floor left
		_Map_SetTile(L"├", 38)
		// 39 floor - brick floor left-bottom
		_Map_SetTile(L"└", 39)
		// 40 floor - brick floor right-bottom
		_Map_SetTile(L"┘", 40)
		// 41 floor - brick floor bottom
		_Map_SetTile(L"┴", 41)

		// 42 floor - red carpet
		_Map_SetTile(L"↕", 42)
		// 43 floor - red carpet with deco
		_Map_SetTile(L"∬", 43)

		// 24 floor - water
		_Map_SetTile(L"川", 24)
		// 25 floor - swamp
		_Map_SetTile(L"毒", 25)
		// 26 floor - lava
		_Map_SetTile(L"♨", 26)

		// 01 object - pillar - on stone floor
		_Map_SetTile(L"¶", 1)
		// 02 object - tree - on grass
		_Map_SetTile(L"♧", 2)
		// 03 object - bed left - on stone floor
		_Map_SetTile(L"【", 3)
		// 04 object - bed right - on stone floor
		_Map_SetTile(L"】", 4)
		// 12 object - sack - on stone floor
		_Map_SetTile(L"◇", 12)
		// 13 object - pot - on stone floor
		_Map_SetTile(L"△", 13)
		// 14 object - steel case - on wood floor
		_Map_SetTile(L"▲", 14)
		// 15 object - flowerpot - on wood floor
		_Map_SetTile(L"古", 15)
		// 17 object - well - on earthen floor
		_Map_SetTile(L"ㅁ", 17)
		// 18 object - stone - on earthen floor
		_Map_SetTile(L"▣", 18)
		// 20 object - sword - on brick floor
		_Map_SetTile(L"†", 20)
		// 21 object - armor - on brick floor
		_Map_SetTile(L"⊙", 21)

		// 05 wall - horiz
		_Map_SetTile(L"▤", 5)
		// 06 wall - verti
		_Map_SetTile(L"▥", 6)
		// 07 wall - by brick
		_Map_SetTile(L"■", 7)
		// 16 wall - gradation
		_Map_SetTile(L"▦", 16)
		// 19 wall - wood(?)
		_Map_SetTile(L"□", 19)

		// 08 storefront sign - FOOD
		_Map_SetTile(L"ⓕ", 8)
		// 09 storefront sign - CURE
		_Map_SetTile(L"ⓒ", 9)
		// 10 storefront sign - ARMS
		_Map_SetTile(L"ⓐ", 10)
		// 11 storefront sign - EXP
		_Map_SetTile(L"ⓔ", 11)

		// 22 entrance - dungeon
		_Map_SetTile(L"円", 22)

		// 23 sign - on earthen floor
		_Map_SetTile(L"早", 23)

		// 48 character - skeleton - on brick floor
		_Map_SetTile(L"骨", 48)
		// 49 character - man - on earthen floor
		_Map_SetTile(L"男", 49)
		// 50 character - woman - on earthen floor
		_Map_SetTile(L"女", 50)
		// 51 character - merchant - on wood floor
		_Map_SetTile(L"商", 51)
		// 52 character - mage - on stone floor
		_Map_SetTile(L"法", 52)
		// 53 character - knight - on stone floor
		_Map_SetTile(L"武", 53)
		// 54 character - LordAhn - on red carpet
		_Map_SetTile(L"王", 54)

		// 55 temporary
		_Map_SetTile(L"？", 55)
	}
}

//?? which namespace?
void setTileAsDefault(TILE_SET tile_set, MapTemplate& map_template)
{
	switch (tile_set)
	{
	case TILE_SET_TOWN:
		SetTownEmoji(map_template);
		break;
	default:
		break;
	}
}
