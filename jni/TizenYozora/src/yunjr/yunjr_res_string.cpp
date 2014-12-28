
#include "yunjr_res_string.h"
#include "util/sm_util_set.h"

using yunjr::resource::HanString;

////////////////////////////////////////////////////////////////////////////////
// 게임의 기본 속성 문자열 (player, weapon 등)

#define RETURN_HAN_STRING(josa_set, default_has_jongsung) \
	HanString han_string; \
	\
	if ((index >= 0) && (index < sizeof(NAME)/sizeof(NAME[0]))) \
		han_string.sz_name = NAME[index]; \
	else \
		han_string.sz_name = NAME_UNKNOWN; \
	\
	static smutil::SmSet s_josa(josa_set); \
	\
	if (s_josa.isSet(index) || ((han_string.sz_name == NAME_UNKNOWN) && default_has_jongsung)) \
	{ \
		han_string.sz_josa_sub1 = L"은"; \
		han_string.sz_josa_sub2 = L"이"; \
		han_string.sz_josa_obj  = L"을"; \
		han_string.sz_josa_with = L"으"; \
	} \
	else \
	{ \
		han_string.sz_josa_sub1 = L"는"; \
		han_string.sz_josa_sub2 = L"가"; \
		han_string.sz_josa_obj  = L"를"; \
		han_string.sz_josa_with = L""; \
	} \
	\
	return han_string;


HanString yunjr::resource::getWeaponName(int index)
{
	const wchar_t* NAME_UNKNOWN = L"불확실한 무기";
	const wchar_t* NAME[] =
	{
		L"맨손",
		L"단도",
		L"곤봉",
		L"미늘창",
		L"장검",
		L"철퇴",
		L"기병창",
		L"도끼창",
		L"삼지창",
		L"화염검",
	};

	RETURN_HAN_STRING("0,2..4,6..9", false)
}

HanString yunjr::resource::getShieldName(int index)
{
	const wchar_t* NAME_UNKNOWN = L"불확실한 방패";
	const wchar_t* NAME[] =
	{
		L"없음",
		L"가죽 방패",
		L"청동 방패",
		L"강철 방패",
		L"은제 방패",
		L"금제 방패",
	};

	RETURN_HAN_STRING("0", false)
}

HanString yunjr::resource::getArmorName(int index)
{
	const wchar_t* NAME_UNKNOWN = L"불확실한 갑옷";
	const wchar_t* NAME[] =
	{
		L"없음",
		L"가죽 갑옷",
		L"청동 갑옷",
		L"강철 갑옷",
		L"은제 갑옷",
		L"금제 갑옷",
	};

	RETURN_HAN_STRING("0..5", true)
}

HanString yunjr::resource::get3rdPersonName(int index)
{
	const wchar_t* NAME_UNKNOWN = L"그";
	const wchar_t* NAME[] =
	{
		L"그",
		L"그녀",
	};

	RETURN_HAN_STRING("", false)
}

HanString yunjr::resource::getGenderName(int index)
{
	const wchar_t* NAME_UNKNOWN = L"불확실함";
	const wchar_t* NAME[] =
	{
		L"남성",
		L"여성",
	};

	RETURN_HAN_STRING("0, 1", true)
}

HanString yunjr::resource::getConditionName(CONDITION index)
{
	const wchar_t* NAME_UNKNOWN = L"이상상태";
	const wchar_t* NAME[] =
	{
		L"good",
		L"poisoned",
		L"unconscious",
		L"dead",
	};

	RETURN_HAN_STRING("", false);
}

HanString yunjr::resource::getClassName(int index)
{
	const wchar_t* NAME_UNKNOWN = L"불확실함";
	const wchar_t* NAME[] =
	{
		L"불확실함",
		L"기사",
		L"마법사",
		L"에스퍼",
		L"전사",
		L"전투승",
		L"닌자",
		L"사냥꾼",
		L"떠돌이",
		L"혼령",
		L"반신반인",
	};

	RETURN_HAN_STRING("0,5,7,9,10", true)
}

HanString yunjr::resource::getMagicName(int index)
{
	const wchar_t* NAME_UNKNOWN = L"";
	const wchar_t* NAME[46] =
	{
		L"", 
		L"마법 화살", 
		L"마법 화구", 
		L"마법 단창", 
		L"독 바늘", 
		L"맥동 광선", 
		L"직격 뇌전", 
		L"공기 폭풍", 
		L"열선 파동", 
		L"초음파", 
		// 10
		L"초냉기", 
		L"인공 지진", 
		L"차원 이탈", 
		L"독", 
		L"기술 무력화", 
		L"방어 무력화", 
		L"능력 저하", 
		L"마법 불능", 
		L"탈 초인화", 
		L"한명 치료", 
		L"한명 독 제거", 
		L"한명 치료와 독제거", 
		L"한명 의식 돌림", 
		L"한명 부활", 
		L"한명 치료와 독제거와 의식돌림", 
		L"한명 복합 치료", 
		L"모두 치료", 
		L"모두 독 제거", 
		L"모두 치료와 독제거", 
		L"모두 의식 돌림", 
		L"모두 치료와 독제거와 의식돌림", 
		L"모두 부활", 
		L"모두 복합 치료", 
		L"마법의 햇불", 
		L"공중 부상", 
		L"물위를 걸음", 
		L"늪위를 걸음", 
		L"기화 이동", 
		L"지형 변화", 
		L"공간 이동", 
		L"식량 제조", 
		L"투시", 
		L"예언", 
		L"독심", 
		L"천리안", 
		L"염력@@"
	};

	RETURN_HAN_STRING("1,3..8,11..13,17,22..24,29..31,33..37,39,42..45", false)
}

HanString yunjr::resource::getAuxName(AUX index)
{
	const wchar_t* NAME[] =
	{
		L"Reserved",
	};

	HanString han_string;
	
	if ((index >= 0) && (index < AUX_MAX))
	{
		han_string.sz_name = NAME[index];
	}

	return han_string;
}

////////////////////////////////////////////////////////////////////////////////
// 게임의 메시지 문자열

const wchar_t* yunjr::resource::getMessageString(MESSAGE message)
{
	static const wchar_t* MESSAGE_STRING_TABLE[MESSAGE_MAX] =
	{
		L"마법 지수가 부족했다",
		L"Noname",
		L"전투 모드가 아닐때는 공격 마법을 사용할 수 없습니다.",
		L"누구에게",
		L"모든 사람들에게",
		L"선택",
		L"강한 치료 마법은 아직 불가능 합니다.",
		L"@F<<<  방향을 선택하시오  >>>@@",
		L"북쪽으로 공간이동",
		L"남쪽으로 공간이동",
		L"동쪽으로 공간이동",
		L"서쪽으로 공간이동",
	};

	return MESSAGE_STRING_TABLE[message];
}

////////////////////////////////////////////////////////////////////////////////
// Resource 관리 utility

wchar_t yunjr::resource::getLastMultiByte(const wchar_t* sz_text)
{
	wchar_t prev = 0;
	
	while (*sz_text)
		prev = *sz_text++;
	
	return prev;
}

bool yunjr::resource::hasJongsung(wchar_t code)
{
	if (code >= 0xAC00 && code <= 0xD7A3)
	{
		const int MAX_SM1 = 19;
		const int MAX_SM2 = 21;
		const int MAX_SM3 = 28;

		code -= 0xAC00;

		unsigned long SM1 = code / (MAX_SM2 * MAX_SM3);
		unsigned long SM2 = (code - SM1 * (MAX_SM2 * MAX_SM3)) / MAX_SM3;
		unsigned long SM3 = code - SM1 * (MAX_SM2 * MAX_SM3) - SM2 * MAX_SM3;

		return (SM3 > 0);
	}

	//?? 영어에 대해서도 적용해야 함
	// default
	return true;
}
