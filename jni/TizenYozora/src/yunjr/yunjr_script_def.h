
#ifndef __YUNJR_SCRIPT_DEF_H__
#define __YUNJR_SCRIPT_DEF_H__

#include "yunjr_class_extern.h"
#include "yunjr_script.h"

#include <map>
#include <vector>
#include <cassert>

// 제어 코드
#define PREPARE_TO_Identify          bool result = false;
#define PREPARE_TO_Load              MapTemplate map_template; bool result = true;
#define PREPARE_TO_Unload            bool result = true;
#define PREPARE_TO_Talk              bool result = false;
#define PREPARE_TO_Read              bool result = false;
#define PREPARE_TO_Occur             bool result = false;
#define PREPARE_TO_Enter             bool result = false;
#define HALT                         return result;

// 문법 변경
#define NOT                          !
#define IF                           if (
#define THEN                         ) { result = true;
#define ELSEIF                       } else if (
#define ELSE                         } else { result = true;
#define END                          result = false; }

// 유틸리티 함수
#define DECLARE_MAP(map_name) \
	namespace yunjr \
	{ \
		namespace script \
		{ \
			namespace map_name \
			{ \
				bool onIdentify(const char* sz_map_name); \
				bool onLoad(void); \
				bool onUnload(void); \
				bool onTalk(int x, int y); \
				bool onRead(int x, int y); \
				bool onOccur(int x, int y); \
				bool onEnter(int x, int y); \
				Callback callback(onIdentify, onLoad, onUnload, onTalk, onRead, onOccur, onEnter); \
			} \
		} \
	}
#define DEF_v(map_name, event_name)  bool yunjr::script::map_name::on##event_name(void) { PREPARE_TO_##event_name
#define DEF_s(map_name, event_name)  bool yunjr::script::map_name::on##event_name(const char* sz_map_name) { PREPARE_TO_##event_name
#define DEF_ii(map_name, event_name) bool yunjr::script::map_name::on##event_name(int x, int y) { PREPARE_TO_##event_name
#define ENDDEF                       HALT }
#define DECLARE(var)                 int var;
#define ASSIGN(var, val)             var = val;
#define EQUAL(v1, v2)                (v1 == v2)
#define ON(_x, _y)                   (x == (_x) && y == (_y))

// 외부 함수 연결
#define _WarpPrevPos()              game::warpPrevPos();
#define _PressAnyKey()              game::pressAnyKey();

#define _Talk(message)              game::console::writeLine(message, 0xFFAAAAAA);
#define _LoadScript(s)              game::loadScript(s);
#define _LoadScriptXY(s, x, y)      game::loadScript(s, x, y);

#define _Flag_IsSet(index)          game::flag::isSet(index)
#define _Flag_Set(index)            game::flag::set(index);

#define _Select_Init()              game::select::init();
#define _Select_Add(s)              game::select::add(s);
#define _Select_Run()               game::select::run();
#define _Select_Result()            game::select::getResult()

#define _Battle_Init()              game::battle::init();
#define _Battle_RegisterEnemy(num)  game::battle::registerEnemy(num);
#define _Battle_ShowEnemy()         game::battle::showEnemy();
#define _Battle_Start(b)            game::battle::start(b);
#define _Battle_Result()            game::battle::getResult()

#define _Map_Init(p1, p2)           map_template.init(p1, p2);
#define _Map_SetType(p1, p2)        game::map::setType(p1, p2);
#define _Map_SetEncounter(p1, p2)   game::map::setEncounter(p1, p2);
#define _Map_SetStartPos(p1, p2)    game::map::setStartPos(p1, p2);
#define _Map_SetTile(p1, p2)        map_template.setTile(p1, p2);
#define _Map_SetRow(s)              map_template.setRow(s);
#define _Map_ChangeTile(p1, p2, p3) game::map::change(p1, p2, p3);
#define _Map_LoadFromFile(s)        game::map::loadFromFile(s);
#define _Map_SetTileAsDefault(ts)   setTileAsDefault(ts, map_template);

// 상수
enum TILE_SET
{
	TILE_SET_TOWN,
	TILE_SET_KEEP,
	TILE_SET_GROUND,
	TILE_SET_DEN
};

#define _FLAG_BATTLE 10

// 미지정(아직 쓰이지 않음)
#define keyword(szWord)  } else if strcmp(szWord, szKeyWord) == 0 then

#define GetScriptFlag        _GetScriptFlag
#define SetScriptFlag(flag)  _SetScriptFlag(flag);
#define RemoveKeyWord(str)   _RemoveKeyWord(str);
#define RegisterKeyWord(str) _RegisterKeyWord(str);

struct MapTemplate
{
	int width;
	int height;
	int row;

	typedef std::map<wchar_t, unsigned char> Convert;
	Convert convert;

	MapTemplate()
		: width(0)
		, height(0)
		, row(0)
	{
	}

	void init(int width, int height)
	{
		this->width  = width;
		this->height = height;
		this->row    = 0;
		this->convert.clear();

		yunjr::game::map::init(this->width, this->height);
	}

	void setTile(const wchar_t emoji[], int assigned_index)
	{
		this->setTile(emoji[0], assigned_index);
	}

	void setTile(wchar_t emoji, int assigned_index)
	{
		this->convert[emoji] = assigned_index;
	}

	void setRow(const wchar_t* p_row_data)
	{
		const wchar_t* p = p_row_data;

		while (*p)
			++p;

		int len = p - p_row_data;
		wchar_t* p_char = (wchar_t*)p_row_data;

		std::vector<unsigned char> row;
		row.reserve(len);

		int loop = len;
		while (--loop >= 0)
		{
			MapTemplate::Convert::iterator i = this->convert.find(*p_char++);

			if (i != this->convert.end())
			{
				row.push_back(i->second);
				continue;
			}
			assert(false);
			row.push_back(0);
		}

		yunjr::game::map::push(this->row++, &(*row.begin()), len);
	}
};

////////////

void setTileAsDefault(TILE_SET tile_set, MapTemplate& map_template);

#endif // #ifndef __YUNJR_SCRIPT_DEF_H__
