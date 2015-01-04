
#include "yunjr_class_extern.h"

#include "yunjr_base_key_buffer.h"
#include "yunjr_base_serialized.h"

#include "yunjr_class_console.h"
#include "yunjr_class_map.h"
#include "yunjr_class_pc_party.h"
#include "yunjr_class_pc_player.h"
#include "yunjr_class_pc_enemy.h"
#include "yunjr_class_game_option.h"
#include "yunjr_class_map_event.h"
#include "yunjr_class_control_lv1.h"
#include "yunjr_class_control_lv2.h"
#include "yunjr_class_select.h"

#include "yunjr_res.h"
#include "yunjr_util.h"

#include "../flat_board/target_dep.h"

#include <cassert>
#include <stdarg.h>

#define ASSERT(cond) assert(cond)

// forward
namespace yunjr
{
	void changeWindowForBattle(void);
	void changeWindowForField(void);

	yunjr::BATTLERESULT runBattleMode(bool is_assualt_mode);
	int registerEnemy(int index);
}

// local
namespace
{
	yunjr::Map                            s_map;
	yunjr::PcParty                        s_party;
	sena::vector<yunjr::shared::PcPlayer> s_player;
	sena::vector<yunjr::shared::PcEnemy>  s_enemy;
	sena::vector<yunjr::Serialized*>      s_save_list;
	yunjr::GameOption                     s_game_option;

	template <typename Dst, typename Src>
	Dst convert(Src);
}

namespace
{
	template <>
	yunjr::Map::TYPE convert(yunjr::game::map::TYPE src)
	{
		switch (src)
		{
		case yunjr::game::map::TYPE_TOWN:
			return yunjr::Map::TYPE_TOWN;
		case yunjr::game::map::TYPE_KEEP:
			return yunjr::Map::TYPE_KEEP;
		case yunjr::game::map::TYPE_GROUND:
			return yunjr::Map::TYPE_GROUND;
		case yunjr::game::map::TYPE_DEN:
			return yunjr::Map::TYPE_DEN;
		}

		return yunjr::Map::TYPE_TOWN;
	}
}

namespace yunjr
{
	namespace data
	{
		const sena::vector<Serialized*>& getSaveList(void)
		{
			return s_save_list;
		}

		const sena::vector<shared::PcPlayer>& getPlayerList(void)
		{
			return s_player;
		}

		const sena::vector<shared::PcEnemy>& getEnemyList(void)
		{
			return s_enemy;
		}
		const PcParty& getParty(void)
		{
			return s_party;
		}
		const Map& getMap(void)
		{
			return s_map;
		}
	}

	namespace game
	{
		// unnamed

		//! 인덱스 컬러에 대해 실제 컬러 값을 돌려 준다.
		//! 인덱스 컬러는 미리 설정된 16개의 컬러이다.
		unsigned long getRealColor(int index)
		{
			static const unsigned long s_COLOR_TABLE[16] =
			{
				0xFF000000, 0xFF000080, 0xFF008000, 0xFF008080,
				0xFF800000, 0xFF800080, 0xFF808000, 0xFF808080,
				0xFF404040, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
				0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF
			};

			if ((index < 0) || (index >= 16))
				index = 15;

			return s_COLOR_TABLE[index];
		}

		//! 이전의 맵 상의 위치로 복귀한다.
		void warpPrevPos(void)
		{
			s_party.warp(PcParty::POS_PREV);
		}

		//! 아무 키나 누를 때까지 메시지를 출력하고 대기한다.
		void pressAnyKey(void)
		{
			ControlConsole* p_console = (ControlConsole*)resource::getMainWindow()->findControl("CONSOLE");

			if (p_console)
			{
				// 이전의 기록은 더 빨리 출력되어야 한다
				p_console->invalidate();
				yunjr::game::updateScreen();

				int saved_x1, saved_y1, saved_x2, saved_y2;
				int saved_left, saved_top, saved_right, saved_bottom;

				p_console->getRegion(saved_x1, saved_y1, saved_x2, saved_y2);
				p_console->getMargin(saved_left, saved_top, saved_right, saved_bottom);

				//? 만약 글자가 끝까지 진행했다면 press any key를 표현하기 위해 스크롤이 되어야 한다.
				//? descent -> (DEFAULT_FONT_BTBD/2), 6 is magic key
				p_console->drawText(saved_left, (saved_y2-saved_y1) - (DEFAULT_FONT_BTBD - DEFAULT_FONT_ASCENDER) - 6, L"아무키나 누르십시오 ...", game::getRealColor(14));
			}

			yunjr::game::updateScreen();

			yunjr::KeyBuffer& key_buffer = yunjr::KeyBuffer::getKeyBuffer();

			while (key_buffer.isKeyPressed())
				key_buffer.getKey();

			while (!key_buffer.isKeyPressed())
				target::system::wait(20);

			key_buffer.getKey();

			// 화면 clear
			LoreConsole& console = LoreConsole::getConsole();

			console.clear();
			console.display();

			yunjr::game::updateScreen();
		}

		//! 백버퍼의 내용을 실제 화면에 반영한다.
		void updateScreen(void)
		{
			/* code snippet in case of SDL version
			 *
			 * p_gfx_device->endDraw();
			 * p_gfx_device->flip();
			 * p_gfx_device->beginDraw();
			 *
			 */
			yunjr::ControlWindow* p_main_window = yunjr::resource::getMainWindow();
			const BufferDesc* p_buffer_desc = &gfx::getFrameBuffer()->buffer_desc;

			if (p_main_window && p_buffer_desc)
			{
				const BufferDesc& buffer_desc = *p_buffer_desc;
				int dest_buffer_pitch = (buffer_desc.bytes_per_line << 3) / buffer_desc.bits_per_pixel;

				shared::FlatBoard32 dest_board(new FlatBoard32((FlatBoard32::Pixel*)buffer_desc.p_start_address, buffer_desc.width, buffer_desc.height, dest_buffer_pitch));

				p_main_window->invalidate();
				p_main_window->render(dest_board);

				target::updateScreen();
			}
		}

		namespace map
		{
			//! 맵을 초기화하고 그 크기를 지정한다.
			void init(int width, int height)
			{
				s_map.setType(yunjr::Map::TYPE_TOWN);
				s_map.width  = width;
				s_map.height = height;
				s_map.encounter_offset = 0;
				s_map.encounter_range  = 0;

				sena::memclear(s_map.data, sizeof(s_map.data));
				sena::memclear(s_map.handicap_data, sizeof(s_map.handicap_data));
			}

			//! 현재 맵에서 적의 출현 범위를 지정한다.
			void setType(TYPE _type)
			{
				s_map.setType(convert<yunjr::Map::TYPE, yunjr::game::map::TYPE>(_type));
			}

			//! 현재 맵에서의 핸디캡을 설정한다.
			void setHandicap(int handicap)
			{
				s_map.setHandicap(yunjr::Map::HANDICAP(handicap));
			}

			//! 현재 맵에서 적의 출현 범위를 지정한다.
			void setEncounter(int offset, int range)
			{
				ASSERT(offset > 0 && range >= 0);

				s_map.encounter_offset = offset;
				s_map.encounter_range  = range;
			}

			//! 현재 맵에서 시작 지점을 지정한다.
			void setStartPos(int x, int y)
			{
				s_party.x = x;
				s_party.y = y;
			}

			//! 맵 데이터를 행 단위로 입력한다.
			void push(int row, const unsigned char* p_data, int num_data)
			{
				if (row >= 0 && row < s_map.height)
				{
					if (num_data <= 0)
						return;
					if (num_data > s_map.width)
						num_data = s_map.width;

					sena::memcpy(&s_map.data[row][0], p_data, num_data);
				}
			}
			//! 맵의 (x,y) 지점의 타일 값을 변경한다.
			void change(int x, int y, int tile)
			{
				s_map.changeMap(x, y, tile);
			}
			void setLight(int x, int y)
			{
				s_map.setLight(x, y);
			}
			void resetLight(int x, int y)
			{
				s_map.resetLight(x, y);
			}
			//! 맵의 (x,y) 지점이 올바른 이동 지점인지를 알려 준다.
			bool isValidWarpPos(int x, int y)
			{
				//??
				//return ((x >= WindowMap::_X_RADIUS) && (x < map.width-WindowMap::_X_RADIUS) && (y >= WindowMap::_Y_RADIUS) && (y < map.height-WindowMap::_Y_RADIUS));
				return true;
			}
			//! 파일로부터 맵을 읽는다.
			bool loadFromFile(const char* sz_file_name)
			{
				target::file_io::StreamReadFile file(sz_file_name);

				int buffer_size = file.getSize();

				if (buffer_size <= 0)
					return false;

				s_map.clearData();

				/*
					TYPE_TOWN:   'DEN*.MAP', 'K_DEN1.MAP', 'PYRAMID1.MAP', 'K_DEN2.MAP (last)'
					TYPE_KEEP:   'KEEP*.MAP'
					TYPE_GROUND: 'GROUND*.MAP'
					TYPE_DEN:    'DEN*.MAP', 'K_DEN2.MAP'
				*/ 
				s_map.setType(Map::TYPE_DEN);

				unsigned char byte;

				file.read(&byte, 1);
				s_map.width  = byte;

				file.read(&byte, 1);
				s_map.height = byte;

				for (int y = 0; y < s_map.height; y++)
				{
					file.read(&s_map.data[y][0], s_map.width);
				}

				return true;
			}
		}

		namespace console
		{
			//! 콘솔 창에, 명시한 컬러의 명시한 가변 파라미터의 문자열 집합을 출력한다.
			void writeConsole(unsigned long index, int num_arg, ...)
			{
				yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

				console.setTextColorIndex(index);

				smutil::string s;
				{
					va_list argptr;
					va_start(argptr, num_arg);

					for (int i = 0; i < num_arg; i++)
						s += va_arg(argptr, wchar_t*);

					va_end(argptr);
				}

				console.write(s);
				//@@ 성능 문제
				console.display();
			}

			//! 콘솔 창에, 명시한 컬러의 명시한 문자열을 새로 출력한다.
			void showMessage(unsigned long index, const wchar_t* sz_message)
			{
				LoreConsole& console = LoreConsole::getConsole();

				console.clear();
				console.setTextColorIndex(index);
				console.write(sz_message);
				console.display();
			}

			void clear(void)
			{
				ControlConsole* p_console = (ControlConsole*)resource::getMainWindow()->findControl("CONSOLE");

				if (p_console)
					p_console->clear();
			}

			void writeLine(const char* sz_text, unsigned long color) //??
			{
				ControlConsole* p_console = (ControlConsole*)resource::getMainWindow()->findControl("CONSOLE");

				if (p_console)
				{
					Typeface typeface;
					Text     text(typeface, sz_text);

					text.setColor(color);

					p_console->add(text);
				}
			} 
			void writeLine(const wchar_t* sz_text, unsigned long color) //??
			{
				ControlConsole* p_console = (ControlConsole*)resource::getMainWindow()->findControl("CONSOLE");

				if (p_console)
				{
					Typeface typeface;
					Text     text(typeface, sz_text);

					text.setColor(color);

					p_console->add(text);
				}
			} 
		}

		// namespace tile

		namespace window
		{
			void displayStatus(void)
			{
				INVALIDATE_STATUS;
				game::updateScreen();
			}

			void displayMap(void)
			{
				INVALIDATE_MAP;
				INVALIDATE_SUB_MAP;
				game::updateScreen();
			}

			void displayBattle(int param1)
			{
				ControlBattle* p_battle = (ControlBattle*)resource::getMainWindow()->findControl("BATTLE");
				p_battle->display(param1 != 0);

				game::updateScreen();
			}

			void displayNoti(void)
			{
				INVALIDATE_NOTI;
				game::updateScreen();
			}
		}

		namespace status
		{
			bool inBattle(void) { return false; } //??
		}

		namespace object
		{
			sena::vector<Serialized*>& getSaveList(void)
			{
				return s_save_list;
			}

			//! 아군 리스트를 vector 형식으로 돌려 준다.
			sena::vector<shared::PcPlayer>& getPlayerList(void)
			{
				return s_player;
			}
			//! 적의 리스트를 vector 형식으로 돌려 준다.
			sena::vector<shared::PcEnemy>&  getEnemyList(void)
			{
				return s_enemy;
			}
			//! 파티의 정보를 돌려 준다.
			PcParty& getParty(void)
			{
				return s_party;
			}
			//! 현재 진행 중인 맵의 정보를 돌려 준다.
			Map& getMap(void)
			{
				return s_map;
			}
		}

		// namespace variable

		namespace flag
		{
			//! 명시한 게임 플래그를 설정한다.
			void set(int index)
			{
				if ((index < 0) || (index >= s_game_option.MAX_FLAG))
				{
					ASSERT(false && "flag range overflow");
					return;
				}

				s_game_option.flag[index] = true;
			}
			//! 명시한 게임 플래그를 해제한다.
			void reset(int index)
			{
				if ((index < 0) || (index >= s_game_option.MAX_FLAG))
				{
					ASSERT(false && "flag range overflow");
					return;
				}

				s_game_option.flag[index] = false;
			}
			//! 명시한 게임 플래그의 설정 여부를 돌려 준다.
			bool isSet(int index)
			{
				if ((index < 0) || (index >= s_game_option.MAX_FLAG))
				{
					ASSERT(false && "flag range overflow");
					return false;
				}

				return s_game_option.flag[index];
			}
		}

		namespace battle
		{
			//? 여기에 전투 최종 결과 값을 넣어야 한다.
			static BATTLERESULT s_result = BATTLERESULT_EVADE;

			//! 전투 상황에 돌입했음을 알려 준다.
			void init(void)
			{
				game::object::getEnemyList().clear();
				//game::object::getEnemyList().push_back(shared::PcEnemy(new PcEnemy));
			}
			//! 현재의 데이터로 전투를 한다.
			void start(bool is_assualt_mode)
			{
				s_result = yunjr::runBattleMode(is_assualt_mode);
				yunjr::changeWindowForField();
			}
			//! 전투에 참가하는 적을 추가한다. 입력 방식은 적의 index 번호를 넘겨 주는 것이다.
			void registerEnemy(int ix_enemy)
			{
				yunjr::registerEnemy(ix_enemy);
			}
			//! 전투에 참가하는 적을 화면에 표시한다.
			void showEnemy(void)
			{
				yunjr::changeWindowForBattle();
				game::updateScreen();
			}
			//! 전투의 결과를 돌려준다.
			int  getResult(void)
			{
				switch (s_result)
				{
				case BATTLERESULT_EVADE:
					return 0;
				case BATTLERESULT_WIN:
					return 1;
				case BATTLERESULT_LOSE:
					return 2;
				default:
					ASSERT(0);
					return 1;
				}
			}
		}

		// namespace party

		namespace player
		{
			int getNumOfConsciousPlayer(void) { return 1; } //??
		}

		namespace enemy
		{
			int getNumOfConsciousEnemy(void) { return 1; } //??
		}

		namespace select
		{
			//? MenuSelection의 default parameter에 대한 변경을 하는 것이 필요할지도 모름
			static MenuList s_menu;
			static int s_result = 0;

			//! 선택 아이템을 초기화 한다.
			void init(void)
			{
				s_menu.clear();
				s_result = 0;
			}
			//! 선택 아이템을 추가 한다.
			void add(const wchar_t* sz_string)
			{
				s_menu.push_back(sz_string);
			}
			//! 현재 선택된 아이템으로 선택을 한다.
			void run(void)
			{
				s_result = MenuSelection(s_menu)();
			}
			//! 선택된 결과를 가져 온다.
			int  getResult(void)
			{
				return s_result;
			}
		}

	}

}

#if 0

#include "hd_class_extern.h"

#include "util/sm_util.h"

#include "hd_base_config.h"
#include "hd_base_gfx.h"
#include "hd_base_key_buffer.h"

#include "hd_class_game_main.h"
#include "hd_class_pc_party.h"
#include "hd_class_pc_enemy.h"
#include "hd_class_pc_player.h"
#include "hd_class_select.h"
#include "hd_class_window.h"

#include "hd_res_instance.h"

#include <stdarg.h>

using namespace avej;

namespace hadar
{
	namespace data
	{
		const sena::vector<PcPlayer*>& getPlayerList(void)
		{
			assert(s_p_game_main);
			return s_p_game_main->player;
		}

		const sena::vector<PcEnemy* >& getEnemyList(void)
		{
			assert(s_p_game_main);
			return s_p_game_main->enemy;
		}
		const PcParty& getParty(void)
		{
			assert(s_p_game_main);
			return s_p_game_main->party;
		}
		const Map& getMap(void)
		{
			assert(s_p_game_main);
			return s_map;
		}
	}

namespace game
{
	//! 인덱스 컬러에 대해 실제 컬러 값을 돌려 준다.
	//! 인덱스 컬러는 미리 설정된 16개의 컬러이다.
	unsigned long getRealColor(int index)
	{
		static const unsigned long s_COLOR_TABLE[16] =
		{
			0xFF000000, 0xFF000080, 0xFF008000, 0xFF008080,
			0xFF800000, 0xFF800080, 0xFF808000, 0xFF808080,
			0xFF404040, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
			0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF
		};

		if ((index < 0) || (index >= 16))
			index = 15;

		return s_COLOR_TABLE[index];
	}

	//! 이전의 맵 상의 위치로 복귀한다.
	void warpPrevPos(void)
	{
		s_p_game_main->party.warp(PcParty::POS_PREV);
	}

	//! 아무 키나 누를 때까지 메시지를 출력하고 대기한다.
	void pressAnyKey(void)
	{
		//? 만약 글자가 끝까지 진행했다면 press any key를 표현하기 위해 스크롤이 되어야 한다.
		int xRegion, yRegion, wRegion, hRegion;
		s_p_game_main->window[GameMain::WINDOWTYPE_CONSOLE]->getRegion(&xRegion, &yRegion, &wRegion, &hRegion);

		gfx::drawText(xRegion, yRegion+hRegion-(config::DEFAULT_FONT_HEIGHT-1), "아무키나 누르십시오 ...", game::getRealColor(14));
		game::updateScreen();

		hadar::KeyBuffer& keyBuffer = KeyBuffer::getKeyBuffer();

		while (keyBuffer.isKeyPressed())
			keyBuffer.getKey();

		while (!keyBuffer.isKeyPressed())
			smutil::delay(20);

		keyBuffer.getKey();

		// 화면 clear
		LoreConsole& console = LoreConsole::getConsole();

		console.clear();
		console.display();
		game::updateScreen();
	}

	//! 아무 키나 누를 때까지 메시지를 출력없이 대기한다.
	void waitForAnyKey(void)
	{
		hadar::KeyBuffer& keyBuffer = KeyBuffer::getKeyBuffer();

		while (keyBuffer.isKeyPressed())
			keyBuffer.getKey();

		while (!keyBuffer.isKeyPressed())
			smutil::delay(20);

		keyBuffer.getKey();
	}

	//! 지정한 시간만큼 대기한다.
	void wait(unsigned long msec)
	{
		smutil::delay(msec);
	}

	//! 백버퍼의 내용을 실제 화면에 반영한다.
	void updateScreen(void)
	{
		p_gfx_device->endDraw();
		p_gfx_device->flip();
		p_gfx_device->beginDraw();
	}

	//! 명시한 스크립트 파일로부터 새로 스크립트를 시작한다.
	bool loadScript(const char* sz_file_name, int xStart, int yStart)
	{
		if (!s_p_game_main->loadScript(sz_file_name))
			return false;

		if (xStart > 0 || yStart > 0)
		{
			s_p_game_main->party.x = xStart;
			s_p_game_main->party.y = yStart;
		}

		// 시작시 바라보는 방향을 정함
		if (s_p_game_main->party.y > s_map.height / 2)
			s_p_game_main->party.face(0, -1);
		else
			s_p_game_main->party.face(0, 1);

		game::window::displayMap();

		return true;
	}

	//! 명시한 스크립트 파일로부터 새로 스크립트를 시작한다.
	bool playBGM(const char* sz_file_name)
	{
		sound::playBGM(sz_file_name);
		return true;
	}

	void proccessGameOver(void)
	{
		s_p_game_main->proccessGameOver(GameMain::EXITCODE_BY_FORCE);
	}

	namespace map
	{
		//! 맵을 초기화하고 그 크기를 지정한다.
		void init(int width, int height)
		{
			s_map.setType(Map::TYPE_TOWN);
			s_map.width  = width;
			s_map.height = height;
			s_map.encounter_offset = 0;
			s_map.encounter_range  = 0;

			sena::memclear(s_map.data, sizeof(s_map.data));
			sena::memclear(s_map.handicap_data, sizeof(s_map.handicap_data));
		}

		//! 현재 맵에서 적의 출현 범위를 지정한다.
		void setType(Map::TYPE _type)
		{
			s_map.setType(_type);
		}

		//! 현재 맵에서의 핸디캡을 설정한다.
		void setHandicap(int handicap)
		{
			s_map.setHandicap(Map::HANDICAP(handicap));
		}

		//! 현재 맵에서 적의 출현 범위를 지정한다.
		void setEncounter(int offset, int range)
		{
			ASSERT(offset > 0 && range >= 0);
			s_map.encounter_offset = offset;
			s_map.encounter_range  = range;
		}

		//! 현재 맵에서 시작 지점을 지정한다.
		void setStartPos(int x, int y)
		{
			s_p_game_main->party.x = x;
			s_p_game_main->party.y = y;
		}

		//! 맵 데이터를 행 단위로 입력한다.
		void push(int row, unsigned char* pData, int num_data)
		{
			if (row >= 0 && row < s_map.height)
			{
				if (num_data <= 0)
					return;
				if (num_data > s_map.width)
					num_data = s_map.width;

				sena::memcpy(&s_map.data[row][0], pData, num_data);
			}
		}
		//! 맵의 (x,y) 지점의 타일 값을 변경한다.
		void change(int x, int y, int tile)
		{
			s_map.changeMap(x, y, tile);
		}
		void setLight(int x, int y)
		{
			s_map.setLight(x, y);
		}
		void resetLight(int x, int y)
		{
			s_map.resetLight(x, y);
		}
		//! 맵의 (x,y) 지점이 올바른 이동 지점인지를 알려 준다.
		bool isValidWarpPos(int x, int y)
		{
			return s_p_game_main->isValidWarpPos(x, y);
		}
		//! 파일로부터 맵을 읽는다.
		bool loadFromFile(const char* sz_file_name)
		{
			if (!s_p_game_main->loadMapFromFile(sz_file_name))
				return false;

			return true;
		}
	}

	namespace console
	{
		//! 콘솔 창에, 명시한 컬러의 명시한 가변 파라미터의 문자열 집합을 출력한다.
		void writeConsole(unsigned long index, int num_arg, ...)
		{
			LoreConsole& console = LoreConsole::getConsole();

			console.setTextColorIndex(index);
			smutil::string s;
			{
				va_list argptr;
				va_start(argptr, num_arg);

				for (int i = 0; i < num_arg; i++)
					s += va_arg(argptr, char*);

				va_end(argptr);
			}

			console.write(s);
			//@@ 성능 문제
			console.display();
		}

		//! 콘솔 창에, 명시한 컬러의 명시한 문자열을 출력한다.
		void writeLine(const char* sz_text, unsigned long color)
		{
			LoreConsole& console = LoreConsole::getConsole();

			console.setTextColor(color);
			console.write(sz_text);
			console.display();
			//@@ 속도 저하가 생기지는 않을까?
			game::updateScreen();
		}

		//! 콘솔 창에, 명시한 컬러의 명시한 문자열을 새로 출력한다.
		void showMessage(unsigned long index, const char* sz_message)
		{
			LoreConsole& console = LoreConsole::getConsole();

			console.clear();
			console.setTextColorIndex(index);
			console.write(sz_message);
			console.display();
		}

		//! 콘솔 창에 출력되는 텍스트의 alignment 속성을 설정한다.
		void setTextAlign(TEXTALIGN align)
		{
			LoreConsole::getConsole().setTextAlign(LoreConsole::TEXTALIGN(align));
		}
	}

	namespace tile
	{
		//? 임시
		const int TILE_X_SIZE = config::DEFAULT_TILE_DISPLAY_WIDTH;
		const int TILE_Y_SIZE = config::DEFAULT_TILE_DISPLAY_HEIGHT;
		//! 지정한 타일을 임시 타일(55번)에 복사한다.
		void copyToDefaultTile(int ix_tile)
		{
			int y_dst = s_map.type * TILE_Y_SIZE;
			p_tile_image->bitBlt(55*TILE_X_SIZE, y_dst, p_tile_image, ix_tile*TILE_X_SIZE, y_dst, TILE_X_SIZE, TILE_Y_SIZE);
		}
		//! 지정한 스프라이트를 임시 타일(55번)에 복사한다.
		void copyToDefaultSprite(int ix_sprite)
		{
			int y_dst = s_map.type * TILE_Y_SIZE;
			p_tile_image->bitBlt(55*TILE_X_SIZE, y_dst, p_sprite_image, ix_sprite*TILE_X_SIZE, 0, TILE_X_SIZE, TILE_Y_SIZE);
		}
		//! 지정한 타일을 지정한 타일에 복사한다.
		void copyTile(int src_tile, int dst_tile)
		{
			int y_dst = s_map.type * TILE_Y_SIZE;
			p_tile_image->bitBlt(dst_tile*TILE_X_SIZE, y_dst, p_tile_image, src_tile*TILE_X_SIZE, y_dst, TILE_X_SIZE, TILE_Y_SIZE);
		}
	}

	namespace window
	{
		//! 맵을 화면에 출력한다.
		void displayMap(void)
		{
			s_p_game_main->window[GameMain::WINDOWTYPE_MAP]->setUpdateFlag();
			s_p_game_main->window[GameMain::WINDOWTYPE_MAP]->display((s_map.type == Map::TYPE_DEN) ? 1 : 0, s_p_game_main->party.ability.magic_torch);

			s_p_game_main->window[GameMain::WINDOWTYPE_SUBMAP]->setUpdateFlag();
			s_p_game_main->window[GameMain::WINDOWTYPE_SUBMAP]->display((s_map.type == Map::TYPE_DEN) ? 1 : 0, s_p_game_main->party.ability.magic_torch);
		}
		//! 콘솔 창을 화면에 출력한다.
		void displayConsole(void)
		{
			s_p_game_main->window[GameMain::WINDOWTYPE_CONSOLE]->setUpdateFlag();
			s_p_game_main->window[GameMain::WINDOWTYPE_CONSOLE]->display();
		}
		//! 상태 창을 화면에 출력한다.
		void displayStatus(void)
		{
			s_p_game_main->window[GameMain::WINDOWTYPE_STATUS]->setUpdateFlag();
			s_p_game_main->window[GameMain::WINDOWTYPE_STATUS]->display();
		}
		//! 전투 창을 화면에 출력한다.
		void displayBattle(int param1)
		{
			s_p_game_main->window[GameMain::WINDOWTYPE_BATTLE]->setUpdateFlag();
			s_p_game_main->window[GameMain::WINDOWTYPE_BATTLE]->display(param1);
		}
		//! 콘솔 창의 크기를 돌려 준다.
		void getRegionForConsole(int* p_out_x, int* p_out_y, int* p_out_width, int* p_out_height)
		{
			LoreConsole::getConsole().getRegion(p_out_x, p_out_y, p_out_width, p_out_height);
			// 아래처럼 하면 안됨. console window 내에 client영역을 얻어야 하기 때문임.
			// s_p_game_main->window[WINDOWTYPE_CONSOLE]->getRegion(pX, pY, pW, pH);
		}
	}

	namespace status
	{
		//! 현재 전투 상태인지 아닌지를 알려 준다.
		bool inBattle(void)
		{
			return (s_p_game_main->game_state == GameMain::GAMESTATE_BATTLE);
		}
	}

	namespace object
	{
		//! 아군 리스트를 vector 형식으로 돌려 준다.
		sena::vector<PcPlayer*>& getPlayerList(void)
		{
			return s_p_game_main->player;
		}
		//! 적의 리스트를 vector 형식으로 돌려 준다.
		sena::vector<PcEnemy*>&  getEnemyList(void)
		{
			return s_p_game_main->enemy;
		}
		//! 파티의 정보를 돌려 준다.
		PcParty& getParty(void)
		{
			return s_p_game_main->party;
		}
		//! 현재 진행 중인 맵의 정보를 돌려 준다.
		Map& getMap(void)
		{
			return s_map;
		}
	}

	namespace variable
	{
		//! 명시한 게임 변수를 설정한다.
		void set(int index, int value)
		{
			if ((index < 0) || (index >= s_game_option.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return;
			}

			s_game_option.variable[index] = value;
		}
		//! 명시한 게임 변수의 값을 1증가 시킨다.
		void add(int index)
		{
			if ((index < 0) || (index >= s_game_option.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return;
			}

			++s_game_option.variable[index];
		}
		//! 명시한 게임 변수의 값을 돌려 준다.
		int get(int index)
		{
			if ((index < 0) || (index >= s_game_option.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return false;
			}

			return s_game_option.variable[index];
		}
	}
	
	namespace flag
	{
		//! 명시한 게임 플래그를 설정한다.
		void set(int index)
		{
			if ((index < 0) || (index >= s_game_option.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return;
			}

			s_game_option.flag[index] = true;
		}
		//! 명시한 게임 플래그를 해제한다.
		void reset(int index)
		{
			if ((index < 0) || (index >= s_game_option.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return;
			}

			s_game_option.flag[index] = false;
		}
		//! 명시한 게임 플래그의 설정 여부를 돌려 준다.
		bool isSet(int index)
		{
			if ((index < 0) || (index >= s_game_option.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return false;
			}

			return s_game_option.flag[index];
		}
	}

	namespace battle
	{
		//? 여기에 전투 최종 결과 값을 넣어야 한다.
		static GameMain::BATTLERESULT s_result = GameMain::BATTLERESULT_EVADE;

		//! 전투 상황에 돌입했음을 알려 준다.
		void init(void)
		{
			s_p_game_main->enemy.clear();
		}
		//! 현재의 데이터로 전투를 한다.
		void start(bool is_assualt_mode)
		{
			s_result = s_p_game_main->runBattleMode(is_assualt_mode);
			s_p_game_main->changeWindowForField();
		}
		//! 전투에 참가하는 적을 추가한다. 입력 방식은 적의 index 번호를 넘겨 주는 것이다.
		void registerEnemy(int ix_enemy)
		{
			s_p_game_main->registerEnemy(ix_enemy);
		}
		//! 전투에 참가하는 적을 화면에 표시한다.
		void showEnemy(void)
		{
			s_p_game_main->changeWindowForBattle();
			game::updateScreen();
		}
		//! 전투의 결과를 돌려준다.
		int  getResult(void)
		{
			switch (s_result)
			{
			case GameMain::BATTLERESULT_EVADE:
				return 0;
			case GameMain::BATTLERESULT_WIN:
				return 1;
			case GameMain::BATTLERESULT_LOSE:
				return 2;
			default:
				ASSERT(0);
				return 1;
			}
		}
	}

	namespace party
	{
		//! 
		int getPosX(void)
		{
			return s_p_game_main->party.x;
		}
		//! 
		int getPosY(void)
		{
			return s_p_game_main->party.y;
		}
		//! 
		void plusGold(int plus)
		{
			s_p_game_main->party.gold += plus;
		}
		//!
		void move(int x1, int y1)
		{
			s_p_game_main->party.move(x1, y1);
		}
		//!
		bool checkIf(CHECKIF check)
		{
			switch (check)
			{
			case CHECKIF_MAGICTORCH:
				return (s_p_game_main->party.ability.magic_torch > 0);
			case CHECKIF_LEVITATION:
				return (s_p_game_main->party.ability.levitation > 0);
			case CHECKIF_WALKONWATER:
				return (s_p_game_main->party.ability.walk_on_water > 0);
			case CHECKIF_WALKONSWAMP:
				return (s_p_game_main->party.ability.walk_on_swamp > 0);
			case CHECKIF_MINDCONTROL:
				return (s_p_game_main->party.ability.mind_control > 0);
			default:
				ASSERT(0);
				return false;
			}
		}
	}
	
	namespace player
	{
		//! 현재 의식이 있는 아군의 수를 돌려 준다.
		int getNumOfConsciousPlayer(void)
		{
			int num_alive = 0;
			for (sena::vector<PcPlayer*>::iterator obj = s_p_game_main->player.begin(); obj != s_p_game_main->player.end(); ++obj)
				num_alive += ((*obj)->isConscious()) ? 1 :0;

			return num_alive;
		}

		bool isAvailable(int ix_player)
		{
			if ((ix_player < 0) || (ix_player >= int(s_p_game_main->player.size())))
				return false;

			return s_p_game_main->player[ix_player]->isValid();
		}
		const char* getName(int ix_player)
		{
			if ((ix_player < 0) || (ix_player >= int(s_p_game_main->player.size())))
				return static_cast<const char*>(0);

			return s_p_game_main->player[ix_player]->getName();
		}

		const char* getGenderName(int ix_player)
		{
			if ((ix_player < 0) || (ix_player >= int(s_p_game_main->player.size())))
				return static_cast<const char*>(0);

			return s_p_game_main->player[ix_player]->getGenderName();
		}
		void assignFromEnemyData(int ix_player, int ix_enemy)
		{
			if ((ix_player < 0) || (ix_player >= int(s_p_game_main->player.size())))
				return;

			*s_p_game_main->player[ix_player] << getEnemyDataFromEnemyTable(ix_enemy);
		}
		void changeAttribute(int ix_player, const char* sz_attrib, int value)
		{
			if ((ix_player < 0) || (ix_player >= int(s_p_game_main->player.size())))
				return;

			s_p_game_main->player[ix_player]->changeAttribute(sz_attrib, value);
		}
		void changeAttribute(int ix_player, const char* sz_attrib, const char* data)
		{
			if ((ix_player < 0) || (ix_player >= int(s_p_game_main->player.size())))
				return;

			s_p_game_main->player[ix_player]->changeAttribute(sz_attrib, data);
		}
		bool getAttribute(int ix_player, const char* sz_attrib, int& value)
		{
			if ((ix_player < 0) || (ix_player >= int(s_p_game_main->player.size())))
				return false;

			return s_p_game_main->player[ix_player]->getAttribute(sz_attrib, value);
		}
		bool getAttribute(int ix_player, const char* sz_attrib, smutil::string& data)
		{
			if ((ix_player < 0) || (ix_player >= int(s_p_game_main->player.size())))
				return false;

			return s_p_game_main->player[ix_player]->getAttribute(sz_attrib, data);
		}
		void reviseAttribute(int ix_player)
		{
			if ((ix_player < 0) || (ix_player >= int(s_p_game_main->player.size())))
				return;

			s_p_game_main->player[ix_player]->reviseAttribute();
		}
		void applyAttribute(int ix_player)
		{
			if ((ix_player < 0) || (ix_player >= int(s_p_game_main->player.size())))
				return;

			s_p_game_main->player[ix_player]->applyAttribute();
		}
	}

	namespace enemy
	{
		//! 현재 의식이 있는 적의 수를 돌려 준다.
		int getNumOfConsciousEnemy(void)
		{
			int num_alive = 0;
			for (sena::vector<PcEnemy*>::iterator obj = s_p_game_main->enemy.begin(); obj != s_p_game_main->enemy.end(); ++obj)
				num_alive += ((*obj)->isConscious()) ? 1 :0;

			return num_alive;
		}

		void changeAttribute(int ix_enemy, const char* sz_attrib, int value)
		{
			if ((ix_enemy < 0) || (ix_enemy >= int(s_p_game_main->enemy.size())))
				return;

			s_p_game_main->enemy[ix_enemy]->changeAttribute(sz_attrib, value);
		}
		void changeAttribute(int ix_enemy, const char* sz_attrib, const char* data)
		{
			if ((ix_enemy < 0) || (ix_enemy >= int(s_p_game_main->enemy.size())))
				return;

			s_p_game_main->enemy[ix_enemy]->changeAttribute(sz_attrib, data);
		}
	}

	namespace select
	{
		//? MenuSelection의 default parameter에 대한 변경을 하는 것이 필요할지도 모름
		static MenuList s_menu;
		static int s_result = 0;

		//! 선택 아이템을 초기화 한다.
		void init(void)
		{
			s_menu.clear();
			s_result = 0;
		}
		//! 선택 아이템을 추가 한다.
		void add(const char* sz_string)
		{
			s_menu.push_back(sz_string);
		}
		//! 현재 선택된 아이템으로 선택을 한다.
		void run(void)
		{
			s_result = MenuSelection(s_menu)();
		}
		//! 선택된 결과를 가져 온다.
		int  getResult(void)
		{
			return s_result;
		}
	}

} // namespace game

} // namespace hadar

#endif
