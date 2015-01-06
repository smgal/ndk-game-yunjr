
#include "yunjr_script_def.h"

////////////////////////////////////////////////////////////////////////////////

DECLARE_MAP(sample)

////////////////////////////////////////////////////////////////////////////////

DEF_s(sample, Identify)

	return (strcmp(sz_map_name, "sample") == 0);

ENDDEF

////////////////////////////////////////////////////////////////////////////////

DEF_v(sample, Load)

	_Map_Init(30, 30)
	// _Map_SetEncounter(1, 10)
	_Map_SetStartPos(15, 15)

	_Map_SetTileAsDefault(TILE_SET_TOWN)

	_Map_SetRow(L"　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　")
	_Map_SetRow(L"　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　")
	_Map_SetRow(L"　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　")
	_Map_SetRow(L"　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　")
	_Map_SetRow(L"　　　　■■■■■■■■■■■■■■■■■■■■■■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　　　■円■　　　　　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　　　　　　　　　　　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　　　　　　　　　　　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　早　　　　　　　♨♨　　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　　　　■　　♨　　　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　　※　■　　　♨　　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　毒毒毒毒　　男■　　♨　　　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　毒毒　　　　　　　　男　■　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　毒毒　　　．　　　　　■　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　　　．　　　　■■■■　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　早川　　　　　＋＋＋＋　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　　川　　男※　＋＋＋＋　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　■　　　川川　　　　　　　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　■　　　川川川　■　　早　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　■　　　川川　■　　　　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　■■　　川　　　　　　　　　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　　．　　┌┬┬┐　××÷××■　　　　")
	_Map_SetRow(L"　　　　■　　　　．　　　　　├┼┼┤　××　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　　　　　├┼┼┤　×÷　　　■　　　　")
	_Map_SetRow(L"　　　　■　　　　　　　　　　└┴┴┘　××　　　■　　　　")
	_Map_SetRow(L"　　　　■■■■■■■■■■■■■■■■■■■■■■　　　　")
	_Map_SetRow(L"　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　")
	_Map_SetRow(L"　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　")
	_Map_SetRow(L"　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　")
	_Map_SetRow(L"　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　")

ENDDEF

////////////////////////////////////////////////////////////////////////////////

DEF_v(sample, Unload)

ENDDEF

////////////////////////////////////////////////////////////////////////////////

DEF_ii(sample, Talk)

	IF ON(15,16) THEN
		_Talk(L"내가 한 날 길을 걷고 있는데, 길에서 산삼 같은 것이 있지 뭔가. 그래서 혹시 산삼인가 해서 줄기를 잡고 뽑아 올렸더니 갑자기 뿌리 부분에서 시끄럽게 말을 해 대는 것이 아닌가.")
		_Talk(L"")
		_Talk(L"  \"내가 당근이야? 내가 무야? 내가 당근이냐고!\"")
		_Talk(L"")
		_Talk(L"그래서 재빨리 다시 땅에 넣어 주고는 모른 척하고 가던 길을 계속 갔다네.")
		HALT
	END

	IF ON(18,12) THEN
		_Talk(L"Talk script 2-1")
		_Talk("Talk script 2-2")
		_Talk("Talk script 2-3")
		_Talk("Talk script 2-4")
		HALT
	END

	IF ON(13,11) THEN
		_Talk(L"러시아 학자들이 과학위성을 이용해 생명이 우주에서 기원했다는 가설인 '배종(胚種)발달설'을 증명했다고 현지 인테르팍스 통신이 29일(현지시간) 보도했다.")
		_Talk(L"")
		_Talk(L"아니 그럼 그 '생명'의 기원은 뭐란 말이냐")
		HALT
	END

ENDDEF

////////////////////////////////////////////////////////////////////////////////

DEF_ii(sample, Read)

	IF ON(11,15) THEN
		_Talk(L"안세준 바보다!!!")
	ELSEIF ON(9,8) THEN
		_Talk("Sign script 2")
	ELSEIF ON(19,18) THEN
		_Talk("Sign script 3")
	END

ENDDEF

////////////////////////////////////////////////////////////////////////////////

DEF_ii(sample, Occur)

	IF ON(12,10) THEN
		_Talk("Event script 1")
		_WarpPrevPos()

	ELSEIF ON(16,16) THEN
		IF _Flag_IsSet(_FLAG_BATTLE) THEN

			_Battle_Init()
/*
			_Battle_RegisterEnemy(69)
			_Battle_RegisterEnemy(71)
			_Battle_RegisterEnemy(75)
*/
			_Battle_RegisterEnemy(1)
			_Battle_RegisterEnemy(3)
			_Battle_RegisterEnemy(2)
			_Battle_RegisterEnemy(4)
			_Battle_ShowEnemy()

			_Talk(L"여기는 Necromancer를 만날 수 있는 이벤트다")
			_PressAnyKey()

			_Battle_Start(true)

			DECLARE(temp)
			ASSIGN(temp, _Battle_Result())

			IF EQUAL(temp, 1) THEN
				_Talk(L"정상적인 전투 종료")
			ELSE
				_Talk(L"도망")
			END

		ELSE
			_Map_ChangeTile(17, 16, 7)
			_Map_ChangeTile(17, 17, 7)
			_Map_ChangeTile(17, 18, 7)
			_Flag_Set(_FLAG_BATTLE)
		END
	END

ENDDEF

////////////////////////////////////////////////////////////////////////////////

DEF_ii(sample, Enter)

	IF ON(14,5) THEN
		_Select_Init()
		_Select_Add(L"새로운 스크립트를 로드하시겠습니까?")
		_Select_Add(L"예")
		_Select_Add(L"아니오")
		_Select_Run()

		DECLARE(temp)
		ASSIGN(temp, _Select_Result())
/*
		IF EQUAL(temp, 1) THEN
			_LoadScript("town2.cm2")
		END*/
	END

ENDDEF
