
#include "hd_script_def.h"

////////////////////////////////////////////////////////////////////////////////

DECLARE_MAP(sample)

////////////////////////////////////////////////////////////////////////////////

DEF_s(sample, Identify)

	return (strcmp(sz_map_name, "sample") == 0);

ENDDEF

////////////////////////////////////////////////////////////////////////////////

DEF_v(sample, Load)

	_Map_Init(30, 30)
	//_Map_SetEncounter(1, 10)
	_Map_SetStartPos(15, 15)

	_Map_SetTileAsDefault(TILE_SET_TOWN)

	_Map_SetRow("������������������������������������������������������������")
	_Map_SetRow("������������������������������������������������������������")
	_Map_SetRow("������������������������������������������������������������")
	_Map_SetRow("������������������������������������������������������������")
	_Map_SetRow("������������������������������ᡡ������")
	_Map_SetRow("���������ᡡ�������������������ᡡ�����������������ᡡ������")
	_Map_SetRow("���������ᡡ���������������������������������������ᡡ������")
	_Map_SetRow("���������ᡡ���������������������������������������ᡡ������")
	_Map_SetRow("���������ᡡ�������ġ��������������͢͡������������ᡡ������")
	_Map_SetRow("���������ᡡ�����������������ᡡ���͡��������������ᡡ������")
	_Map_SetRow("���������ᡡ�������������ء��ᡡ�����͡������������ᡡ������")
	_Map_SetRow("���������ᡡ��ԸԸԸԸ�������ᡡ���͡��������������ᡡ������")
	_Map_SetRow("���������ᡡ����ԸԸ���������������������ᡡ�������ᡡ������")
	_Map_SetRow("���������ᡡ������ԸԸ�������������������ᡡ�������ᡡ������")
	_Map_SetRow("���������ᡡ����������������������������ᡡ�����ᡡ������")
	_Map_SetRow("���������ᡡ�����������ġ�����������++++++++�������ᡡ������")
	_Map_SetRow("���������ᡡ���������������������ء����������������ᡡ������")
	_Map_SetRow("���������ᡡ�����ᡡ�������������������������������ᡡ������")
	_Map_SetRow("���������ᡡ�����ᡡ�������������ᡡ���ġ����������ᡡ������")
	_Map_SetRow("���������ᡡ�������ᡡ�����������ᡡ���������������ᡡ������")
	_Map_SetRow("���������ᡡ��������ᡡ���������������������������ᡡ������")
	_Map_SetRow("���������ᡡ���������������������������������������ᡡ������")
	_Map_SetRow("���������ᡡ���������������������������������������ᡡ������")
	_Map_SetRow("���������ᡡ���������������������������������������ᡡ������")
	_Map_SetRow("���������ᡡ���������������������������������������ᡡ������")
	_Map_SetRow("������������������������������ᡡ������")
	_Map_SetRow("������������������������������������������������������������")
	_Map_SetRow("������������������������������������������������������������")
	_Map_SetRow("������������������������������������������������������������")
	_Map_SetRow("������������������������������������������������������������")

ENDDEF

////////////////////////////////////////////////////////////////////////////////

DEF_v(sample, Unload)

ENDDEF

////////////////////////////////////////////////////////////////////////////////

DEF_ii(sample, Talk)

	IF ON(15,16) THEN
		_Talk("�����޺��� �������� ��¥�� �Ǹ����� ������ �����̱� ����. �޴��� ������ ���ޱ����� ���ݺ��� �� ��ȭ�� �����̾ �������� ������ ���� ������ ���ݺ��� 10���� �̻� ������ ������ ����ǰ� �ִ�. �̷��� �Ǹ� �ҹ�  �չ� ������ ������ ��ȣ���� �Ӵ��� ������� �޴����� �����ϴ� ������ ������ �������� ���ñ��� �޴� <���̳ʽ� ��>���� ���� ���ɼ��� ����.")
		HALT
	END

	IF ON(18,12) THEN
		_Talk("Talk script 2-1")
		_Talk("Talk script 2-2")
		_Talk("Talk script 2-3")
		_Talk("Talk script 2-4")
		HALT
	END

	IF ON(13,11) THEN
		_Talk("Talk script 3-1")
		_Talk("Talk script 3-2")
		_Talk("Talk script 3-3")
		HALT
	END

ENDDEF

////////////////////////////////////////////////////////////////////////////////

DEF_ii(sample, Read)

	IF ON(11,15) THEN
		_Talk("Sign script 1")
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
			_Battle_RegisterEnemy(69)
			_Battle_RegisterEnemy(71)
			_Battle_RegisterEnemy(75)
			_Battle_RegisterEnemy(3)
			_Battle_RegisterEnemy(1)
			_Battle_ShowEnemy()

			_Talk("����� Necromancer�� ���� �� �ִ� �̺�Ʈ��")
			_PressAnyKey()

			_Battle_Start(true)

			DECLARE(temp)
			ASSIGN(temp, _Battle_Result())

			IF EQUAL(temp, 1) THEN
				_Talk("�������� ���� ����")
			ELSE
				_Talk("����")
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
		_Select_Add("���ο� ��ũ��Ʈ�� �ε��Ͻðڽ��ϱ�?")
		_Select_Add("��")
		_Select_Add("�ƴϿ�")
		_Select_Run()

		DECLARE(temp)
		ASSIGN(temp, _Select_Result())

		IF EQUAL(temp, 1) THEN
			_LoadScript("town2.cm2")
		END
	END

ENDDEF