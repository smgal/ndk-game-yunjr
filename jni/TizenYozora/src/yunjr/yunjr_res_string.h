
#ifndef __YUNJR_RES_STRING_H__
#define __YUNJR_RES_STRING_H__

#include "yunjr_base.h"

namespace yunjr
{
	namespace resource
	{
		///////////////////////////////////////////////////////////////////////////////
		// 게임의 기본 속성 문자열 (player, weapon 등)

		enum CONDITION
		{
			CONDITION_GOOD        = 0,
			CONDITION_POISONED    = 1,
			CONDITION_UNCONSCIOUS = 2,
			CONDITION_DEAD        = 3,
		};

		enum AUX
		{
			AUX_RESERVED = 0,
			AUX_MAX
		};

		struct HanString
		{
			const wchar_t* sz_name;
			const wchar_t* sz_josa_sub1;
			const wchar_t* sz_josa_sub2;
			const wchar_t* sz_josa_obj;
			const wchar_t* sz_josa_with;

			HanString(void)
				: sz_name(L"")
				, sz_josa_sub1(L"")
				, sz_josa_sub2(L"")
				, sz_josa_obj(L"")
				, sz_josa_with(L"")
			{
			}
		};

		HanString getWeaponName(int index);
		HanString getShieldName(int index);
		HanString getArmorName(int index);

		HanString get3rdPersonName(int index);
		HanString getGenderName(int index);
		HanString getClassName(int index);
		HanString getConditionName(CONDITION index);

		HanString getMagicName(int index);

		HanString getAuxName(AUX index);

		///////////////////////////////////////////////////////////////////////////////
		// 게임의 메시지 문자열

		enum MESSAGE
		{
			MESSAGE_NOT_ENOUGH_SP,
			MESSAGE_ANONYMOUS_NAME,
			MESSAGE_CANNOT_USE_ATTACK_MAGIC_NOT_IN_BATTLE_MODE,
			MESSAGE_TO_WHOM,
			MESSAGE_TO_EVERYBODY,
			MESSAGE_SELECTION,
			MESSAGE_CANNOT_USE_POWERFUL_CURE_SPELL,
			MESSAGE_SELECT_DIRECTION,
			MESSAGE_TELEPORT_TO_NORTH,
			MESSAGE_TELEPORT_TO_SOUTH,
			MESSAGE_TELEPORT_TO_EAST,
			MESSAGE_TELEPORT_TO_WEST,
			MESSAGE_MAX
		};

		const wchar_t* getMessageString(MESSAGE message);

		///////////////////////////////////////////////////////////////////////////////
		// Resource 관리 utility

		wchar_t getLastMultiByte(const wchar_t* sz_text);
		bool hasJongsung(wchar_t code);

	} // namespace resource

} // namespace yunjr
/*
namespace yunjr
{
	namespace res
	{
		const wchar_t* TEST_STRING1 = L"가시는 걸음 걸음 고이 접어 나빌레라";
		const wchar_t* TEST_STRING2 = L"I am the captain of my soul.";
		const wchar_t* TEST_STRING3 = L"僕と契約して魔法少女になってよ！";

		const wchar_t* TEST_STRING4 = L"야메로 이런 싸움은 모 야메룽다";
		const wchar_t* TEST_STRING5 = L"호라 모 젠젠 멀쩡하자나.";
		const wchar_t* TEST_STRING6 = L"닝겐노 유리와 튼튼데스네";
	}
}
*/
#endif
