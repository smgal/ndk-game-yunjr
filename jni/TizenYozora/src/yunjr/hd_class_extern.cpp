
#include "hd_class_extern.h"

#include "hd_class_game_main.h"

#include "hd_class_pc_party.h"
#include "hd_class_pc_enemy.h"
#include "hd_class_pc_player.h"

namespace
{
	sena::vector<hadar::PcPlayer*> s_player;
	sena::vector<hadar::PcEnemy* > s_enemy;
	hadar::PcParty s_party;
	hadar::Map s_map;
}

namespace hadar
{

	namespace data
	{
		const sena::vector<PcPlayer*>& getPlayerList(void)
		{
			return s_player;
		}

		const sena::vector<PcEnemy* >& getEnemyList(void)
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
	//! �ε��� �÷��� ���� ���� �÷� ���� ���� �ش�.
	//! �ε��� �÷��� �̸� ������ 16���� �÷��̴�.
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

	//! ������ �� ���� ��ġ�� �����Ѵ�.
	void warpPrevPos(void)
	{
	}

	//! �ƹ� Ű�� ���� ������ �޽����� ����ϰ� ����Ѵ�.
	void pressAnyKey(void)
	{
	}

	//! �ƹ� Ű�� ���� ������ �޽����� ��¾��� ����Ѵ�.
	void waitForAnyKey(void)
	{
	}

	//! ������ �ð���ŭ ����Ѵ�.
	void wait(unsigned long msec)
	{
	}

	//! ������� ������ ���� ȭ�鿡 �ݿ��Ѵ�.
	void updateScreen(void)
	{
	}

	//! ����� ��ũ��Ʈ ���Ϸκ��� ���� ��ũ��Ʈ�� �����Ѵ�.
	bool loadScript(const char* sz_file_name, int xStart, int yStart)
	{
		return false;
	}

	//! ����� ��ũ��Ʈ ���Ϸκ��� ���� ��ũ��Ʈ�� �����Ѵ�.
	bool playBGM(const char* sz_file_name)
	{
		return false;
	}

	void proccessGameOver(void)
	{
	}

	namespace map
	{
		//! ���� �ʱ�ȭ�ϰ� �� ũ�⸦ �����Ѵ�.
		void init(int width, int height)
		{
		}

		//! ���� �ʿ��� ���� ���� ������ �����Ѵ�.
		void setType(Map::TYPE _type)
		{
		}

		//! ���� �ʿ����� �ڵ�ĸ�� �����Ѵ�.
		void setHandicap(int handicap)
		{
		}

		//! ���� �ʿ��� ���� ���� ������ �����Ѵ�.
		void setEncounter(int offset, int range)
		{
		}

		//! ���� �ʿ��� ���� ������ �����Ѵ�.
		void setStartPos(int x, int y)
		{
		}

		//! �� �����͸� �� ������ �Է��Ѵ�.
		void push(int row, unsigned char* pData, int num_data)
		{
		}
		//! ���� (x,y) ������ Ÿ�� ���� �����Ѵ�.
		void change(int x, int y, int tile)
		{
		}
		void setLight(int x, int y)
		{
		}
		void resetLight(int x, int y)
		{
		}
		//! ���� (x,y) ������ �ùٸ� �̵� ���������� �˷� �ش�.
		bool isValidWarpPos(int x, int y)
		{
			return false;
		}
		//! ���Ϸκ��� ���� �д´�.
		bool loadFromFile(const char* sz_file_name)
		{
			return false;
		}
	}

	namespace console
	{
		//! �ܼ� â��, ����� �÷��� ����� ���� �Ķ������ ���ڿ� ������ ����Ѵ�.
		void writeConsole(unsigned long index, int num_arg, ...)
		{
		}

		//! �ܼ� â��, ����� �÷��� ����� ���ڿ��� ����Ѵ�.
		void writeLine(const char* sz_text, unsigned long color)
		{
		}

		//! �ܼ� â��, ����� �÷��� ����� ���ڿ��� ���� ����Ѵ�.
		void showMessage(unsigned long index, const char* sz_message)
		{
		}

		//! �ܼ� â�� ��µǴ� �ؽ�Ʈ�� alignment �Ӽ��� �����Ѵ�.
		void setTextAlign(TEXTALIGN align)
		{
		}
	}

	namespace tile
	{
		void copyToDefaultTile(int ix_tile)
		{
		}
		//! ������ ��������Ʈ�� �ӽ� Ÿ��(55��)�� �����Ѵ�.
		void copyToDefaultSprite(int ix_sprite)
		{
		}
		//! ������ Ÿ���� ������ Ÿ�Ͽ� �����Ѵ�.
		void copyTile(int src_tile, int dst_tile)
		{
		}
	}

	namespace window
	{
		//! ���� ȭ�鿡 ����Ѵ�.
		void displayMap(void)
		{
		}
		//! �ܼ� â�� ȭ�鿡 ����Ѵ�.
		void displayConsole(void)
		{
		}
		//! ���� â�� ȭ�鿡 ����Ѵ�.
		void displayStatus(void)
		{
		}
		//! ���� â�� ȭ�鿡 ����Ѵ�.
		void displayBattle(int param1)
		{
		}
		//! �ܼ� â�� ũ�⸦ ���� �ش�.
		void getRegionForConsole(int* p_out_x, int* p_out_y, int* p_out_width, int* p_out_height)
		{
		}
	}

	namespace status
	{
		//! ���� ���� �������� �ƴ����� �˷� �ش�.
		bool inBattle(void)
		{
			return false;
		}
	}

	namespace object
	{
		//! �Ʊ� ����Ʈ�� vector �������� ���� �ش�.
		sena::vector<PcPlayer*>& getPlayerList(void)
		{
			return s_player;
		}
		//! ���� ����Ʈ�� vector �������� ���� �ش�.
		sena::vector<PcEnemy*>&  getEnemyList(void)
		{
			return s_enemy;
		}
		//! ��Ƽ�� ������ ���� �ش�.
		PcParty& getParty(void)
		{
			return s_party;
		}
		//! ���� ���� ���� ���� ������ ���� �ش�.
		Map& getMap(void)
		{
			return s_map;
		}
	}

	namespace variable
	{
		//! ����� ���� ������ �����Ѵ�.
		void set(int index, int value)
		{
		}
		//! ����� ���� ������ ���� 1���� ��Ų��.
		void add(int index)
		{
		}
		//! ����� ���� ������ ���� ���� �ش�.
		int get(int index)
		{
			return 0;
		}
	}
	
	namespace flag
	{
		//! ����� ���� �÷��׸� �����Ѵ�.
		void set(int index)
		{
		}
		//! ����� ���� �÷��׸� �����Ѵ�.
		void reset(int index)
		{
		}
		//! ����� ���� �÷����� ���� ���θ� ���� �ش�.
		bool isSet(int index)
		{
			return false;
		}
	}

	namespace battle
	{
		void init(void)
		{
		}
		//! ������ �����ͷ� ������ �Ѵ�.
		void start(bool is_assualt_mode)
		{
		}
		//! ������ �����ϴ� ���� �߰��Ѵ�. �Է� ����� ���� index ��ȣ�� �Ѱ� �ִ� ���̴�.
		void registerEnemy(int ix_enemy)
		{
		}
		//! ������ �����ϴ� ���� ȭ�鿡 ǥ���Ѵ�.
		void showEnemy(void)
		{
		}
		//! ������ ����� �����ش�.
		int  getResult(void)
		{
			return 1;
		}
	}

	namespace party
	{
		//! 
		int getPosX(void)
		{
			return 0;
		}
		//! 
		int getPosY(void)
		{
			return 0;
		}
		//! 
		void plusGold(int plus)
		{
		}
		//!
		void move(int x1, int y1)
		{
		}
		//!
		bool checkIf(CHECKIF check)
		{
			return false;
		}
	}
	
	namespace player
	{
		//! ���� �ǽ��� �ִ� �Ʊ��� ���� ���� �ش�.
		int getNumOfConsciousPlayer(void)
		{
			int num_alive = 0;
			return num_alive;
		}

		bool isAvailable(int ix_player)
		{
			return false;
		}
		const char* getName(int ix_player)
		{
			return "";
		}

		const char* getGenderName(int ix_player)
		{
			return "";
		}
		void assignFromEnemyData(int ix_player, int ix_enemy)
		{
		}
		void changeAttribute(int ix_player, const char* sz_attrib, int value)
		{
		}
		void changeAttribute(int ix_player, const char* sz_attrib, const char* data)
		{
		}
		bool getAttribute(int ix_player, const char* sz_attrib, int& value)
		{
			return false;
		}
		bool getAttribute(int ix_player, const char* sz_attrib, smutil::string& data)
		{
			return false;
		}
		void reviseAttribute(int ix_player)
		{
		}
		void applyAttribute(int ix_player)
		{
		}
	}

	namespace enemy
	{
		//! ���� �ǽ��� �ִ� ���� ���� ���� �ش�.
		int getNumOfConsciousEnemy(void)
		{
			int num_alive = 0;
			return num_alive;
		}

		void changeAttribute(int ix_enemy, const char* sz_attrib, int value)
		{
		}
		void changeAttribute(int ix_enemy, const char* sz_attrib, const char* data)
		{
		}
	}

	namespace select
	{
		//! ���� �������� �ʱ�ȭ �Ѵ�.
		void init(void)
		{
		}
		//! ���� �������� �߰� �Ѵ�.
		void add(const char* sz_string)
		{
		}
		//! ���� ���õ� ���������� ������ �Ѵ�.
		void run(void)
		{
		}
		//! ���õ� ����� ���� �´�.
		int  getResult(void)
		{
			return 0;
		}
	}

} // namespace game

} // namespace hadar




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
			return s_p_game_main->map;
		}
	}

namespace game
{
	//! �ε��� �÷��� ���� ���� �÷� ���� ���� �ش�.
	//! �ε��� �÷��� �̸� ������ 16���� �÷��̴�.
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

	//! ������ �� ���� ��ġ�� �����Ѵ�.
	void warpPrevPos(void)
	{
		s_p_game_main->party.warp(PcParty::POS_PREV);
	}

	//! �ƹ� Ű�� ���� ������ �޽����� ����ϰ� ����Ѵ�.
	void pressAnyKey(void)
	{
		//? ���� ���ڰ� ������ �����ߴٸ� press any key�� ǥ���ϱ� ���� ��ũ���� �Ǿ�� �Ѵ�.
		int xRegion, yRegion, wRegion, hRegion;
		s_p_game_main->window[GameMain::WINDOWTYPE_CONSOLE]->getRegion(&xRegion, &yRegion, &wRegion, &hRegion);

		gfx::drawText(xRegion, yRegion+hRegion-(config::DEFAULT_FONT_HEIGHT-1), "�ƹ�Ű�� �����ʽÿ� ...", game::getRealColor(14));
		game::updateScreen();

		hadar::KeyBuffer& keyBuffer = KeyBuffer::getKeyBuffer();

		while (keyBuffer.isKeyPressed())
			keyBuffer.getKey();

		while (!keyBuffer.isKeyPressed())
			smutil::delay(20);

		keyBuffer.getKey();

		// ȭ�� clear
		LoreConsole& console = LoreConsole::getConsole();

		console.clear();
		console.display();
		game::updateScreen();
	}

	//! �ƹ� Ű�� ���� ������ �޽����� ��¾��� ����Ѵ�.
	void waitForAnyKey(void)
	{
		hadar::KeyBuffer& keyBuffer = KeyBuffer::getKeyBuffer();

		while (keyBuffer.isKeyPressed())
			keyBuffer.getKey();

		while (!keyBuffer.isKeyPressed())
			smutil::delay(20);

		keyBuffer.getKey();
	}

	//! ������ �ð���ŭ ����Ѵ�.
	void wait(unsigned long msec)
	{
		smutil::delay(msec);
	}

	//! ������� ������ ���� ȭ�鿡 �ݿ��Ѵ�.
	void updateScreen(void)
	{
		p_gfx_device->endDraw();
		p_gfx_device->flip();
		p_gfx_device->beginDraw();
	}

	//! ����� ��ũ��Ʈ ���Ϸκ��� ���� ��ũ��Ʈ�� �����Ѵ�.
	bool loadScript(const char* sz_file_name, int xStart, int yStart)
	{
		if (!s_p_game_main->loadScript(sz_file_name))
			return false;

		if (xStart > 0 || yStart > 0)
		{
			s_p_game_main->party.x = xStart;
			s_p_game_main->party.y = yStart;
		}

		// ���۽� �ٶ󺸴� ������ ����
		if (s_p_game_main->party.y > s_p_game_main->map.height / 2)
			s_p_game_main->party.face(0, -1);
		else
			s_p_game_main->party.face(0, 1);

		game::window::displayMap();

		return true;
	}

	//! ����� ��ũ��Ʈ ���Ϸκ��� ���� ��ũ��Ʈ�� �����Ѵ�.
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
		//! ���� �ʱ�ȭ�ϰ� �� ũ�⸦ �����Ѵ�.
		void init(int width, int height)
		{
			s_p_game_main->map.setType(Map::TYPE_TOWN);
			s_p_game_main->map.width  = width;
			s_p_game_main->map.height = height;
			s_p_game_main->map.encounter_offset = 0;
			s_p_game_main->map.encounter_range  = 0;

			sena::memclear(s_p_game_main->map.data, sizeof(s_p_game_main->map.data));
			sena::memclear(s_p_game_main->map.handicap_data, sizeof(s_p_game_main->map.handicap_data));
		}

		//! ���� �ʿ��� ���� ���� ������ �����Ѵ�.
		void setType(Map::TYPE _type)
		{
			s_p_game_main->map.setType(_type);
		}

		//! ���� �ʿ����� �ڵ�ĸ�� �����Ѵ�.
		void setHandicap(int handicap)
		{
			s_p_game_main->map.setHandicap(Map::HANDICAP(handicap));
		}

		//! ���� �ʿ��� ���� ���� ������ �����Ѵ�.
		void setEncounter(int offset, int range)
		{
			ASSERT(offset > 0 && range >= 0);
			s_p_game_main->map.encounter_offset = offset;
			s_p_game_main->map.encounter_range  = range;
		}

		//! ���� �ʿ��� ���� ������ �����Ѵ�.
		void setStartPos(int x, int y)
		{
			s_p_game_main->party.x = x;
			s_p_game_main->party.y = y;
		}

		//! �� �����͸� �� ������ �Է��Ѵ�.
		void push(int row, unsigned char* pData, int num_data)
		{
			if (row >= 0 && row < s_p_game_main->map.height)
			{
				if (num_data <= 0)
					return;
				if (num_data > s_p_game_main->map.width)
					num_data = s_p_game_main->map.width;

				sena::memcpy(&s_p_game_main->map.data[row][0], pData, num_data);
			}
		}
		//! ���� (x,y) ������ Ÿ�� ���� �����Ѵ�.
		void change(int x, int y, int tile)
		{
			s_p_game_main->map.changeMap(x, y, tile);
		}
		void setLight(int x, int y)
		{
			s_p_game_main->map.setLight(x, y);
		}
		void resetLight(int x, int y)
		{
			s_p_game_main->map.resetLight(x, y);
		}
		//! ���� (x,y) ������ �ùٸ� �̵� ���������� �˷� �ش�.
		bool isValidWarpPos(int x, int y)
		{
			return s_p_game_main->isValidWarpPos(x, y);
		}
		//! ���Ϸκ��� ���� �д´�.
		bool loadFromFile(const char* sz_file_name)
		{
			if (!s_p_game_main->loadMapFromFile(sz_file_name))
				return false;

			return true;
		}
	}

	namespace console
	{
		//! �ܼ� â��, ����� �÷��� ����� ���� �Ķ������ ���ڿ� ������ ����Ѵ�.
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
			//@@ ���� ����
			console.display();
		}

		//! �ܼ� â��, ����� �÷��� ����� ���ڿ��� ����Ѵ�.
		void writeLine(const char* sz_text, unsigned long color)
		{
			LoreConsole& console = LoreConsole::getConsole();

			console.setTextColor(color);
			console.write(sz_text);
			console.display();
			//@@ �ӵ� ���ϰ� �������� ������?
			game::updateScreen();
		}

		//! �ܼ� â��, ����� �÷��� ����� ���ڿ��� ���� ����Ѵ�.
		void showMessage(unsigned long index, const char* sz_message)
		{
			LoreConsole& console = LoreConsole::getConsole();

			console.clear();
			console.setTextColorIndex(index);
			console.write(sz_message);
			console.display();
		}

		//! �ܼ� â�� ��µǴ� �ؽ�Ʈ�� alignment �Ӽ��� �����Ѵ�.
		void setTextAlign(TEXTALIGN align)
		{
			LoreConsole::getConsole().setTextAlign(LoreConsole::TEXTALIGN(align));
		}
	}

	namespace tile
	{
		//? �ӽ�
		const int TILE_X_SIZE = config::DEFAULT_TILE_DISPLAY_WIDTH;
		const int TILE_Y_SIZE = config::DEFAULT_TILE_DISPLAY_HEIGHT;
		//! ������ Ÿ���� �ӽ� Ÿ��(55��)�� �����Ѵ�.
		void copyToDefaultTile(int ix_tile)
		{
			int y_dst = s_p_game_main->map.type * TILE_Y_SIZE;
			p_tile_image->bitBlt(55*TILE_X_SIZE, y_dst, p_tile_image, ix_tile*TILE_X_SIZE, y_dst, TILE_X_SIZE, TILE_Y_SIZE);
		}
		//! ������ ��������Ʈ�� �ӽ� Ÿ��(55��)�� �����Ѵ�.
		void copyToDefaultSprite(int ix_sprite)
		{
			int y_dst = s_p_game_main->map.type * TILE_Y_SIZE;
			p_tile_image->bitBlt(55*TILE_X_SIZE, y_dst, p_sprite_image, ix_sprite*TILE_X_SIZE, 0, TILE_X_SIZE, TILE_Y_SIZE);
		}
		//! ������ Ÿ���� ������ Ÿ�Ͽ� �����Ѵ�.
		void copyTile(int src_tile, int dst_tile)
		{
			int y_dst = s_p_game_main->map.type * TILE_Y_SIZE;
			p_tile_image->bitBlt(dst_tile*TILE_X_SIZE, y_dst, p_tile_image, src_tile*TILE_X_SIZE, y_dst, TILE_X_SIZE, TILE_Y_SIZE);
		}
	}

	namespace window
	{
		//! ���� ȭ�鿡 ����Ѵ�.
		void displayMap(void)
		{
			s_p_game_main->window[GameMain::WINDOWTYPE_MAP]->setUpdateFlag();
			s_p_game_main->window[GameMain::WINDOWTYPE_MAP]->display((s_p_game_main->map.type == Map::TYPE_DEN) ? 1 : 0, s_p_game_main->party.ability.magic_torch);

			s_p_game_main->window[GameMain::WINDOWTYPE_SUBMAP]->setUpdateFlag();
			s_p_game_main->window[GameMain::WINDOWTYPE_SUBMAP]->display((s_p_game_main->map.type == Map::TYPE_DEN) ? 1 : 0, s_p_game_main->party.ability.magic_torch);
		}
		//! �ܼ� â�� ȭ�鿡 ����Ѵ�.
		void displayConsole(void)
		{
			s_p_game_main->window[GameMain::WINDOWTYPE_CONSOLE]->setUpdateFlag();
			s_p_game_main->window[GameMain::WINDOWTYPE_CONSOLE]->display();
		}
		//! ���� â�� ȭ�鿡 ����Ѵ�.
		void displayStatus(void)
		{
			s_p_game_main->window[GameMain::WINDOWTYPE_STATUS]->setUpdateFlag();
			s_p_game_main->window[GameMain::WINDOWTYPE_STATUS]->display();
		}
		//! ���� â�� ȭ�鿡 ����Ѵ�.
		void displayBattle(int param1)
		{
			s_p_game_main->window[GameMain::WINDOWTYPE_BATTLE]->setUpdateFlag();
			s_p_game_main->window[GameMain::WINDOWTYPE_BATTLE]->display(param1);
		}
		//! �ܼ� â�� ũ�⸦ ���� �ش�.
		void getRegionForConsole(int* p_out_x, int* p_out_y, int* p_out_width, int* p_out_height)
		{
			LoreConsole::getConsole().getRegion(p_out_x, p_out_y, p_out_width, p_out_height);
			// �Ʒ�ó�� �ϸ� �ȵ�. console window ���� client������ ���� �ϱ� ������.
			// s_p_game_main->window[WINDOWTYPE_CONSOLE]->getRegion(pX, pY, pW, pH);
		}
	}

	namespace status
	{
		//! ���� ���� �������� �ƴ����� �˷� �ش�.
		bool inBattle(void)
		{
			return (s_p_game_main->game_state == GameMain::GAMESTATE_BATTLE);
		}
	}

	namespace object
	{
		//! �Ʊ� ����Ʈ�� vector �������� ���� �ش�.
		sena::vector<PcPlayer*>& getPlayerList(void)
		{
			return s_p_game_main->player;
		}
		//! ���� ����Ʈ�� vector �������� ���� �ش�.
		sena::vector<PcEnemy*>&  getEnemyList(void)
		{
			return s_p_game_main->enemy;
		}
		//! ��Ƽ�� ������ ���� �ش�.
		PcParty& getParty(void)
		{
			return s_p_game_main->party;
		}
		//! ���� ���� ���� ���� ������ ���� �ش�.
		Map& getMap(void)
		{
			return s_p_game_main->map;
		}
	}

	namespace variable
	{
		//! ����� ���� ������ �����Ѵ�.
		void set(int index, int value)
		{
			if ((index < 0) || (index >= s_p_game_main->game_option.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return;
			}

			s_p_game_main->game_option.variable[index] = value;
		}
		//! ����� ���� ������ ���� 1���� ��Ų��.
		void add(int index)
		{
			if ((index < 0) || (index >= s_p_game_main->game_option.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return;
			}

			++s_p_game_main->game_option.variable[index];
		}
		//! ����� ���� ������ ���� ���� �ش�.
		int get(int index)
		{
			if ((index < 0) || (index >= s_p_game_main->game_option.MAX_VARIABLE))
			{
				ASSERT(false && "variable range overflow");
				return false;
			}

			return s_p_game_main->game_option.variable[index];
		}
	}
	
	namespace flag
	{
		//! ����� ���� �÷��׸� �����Ѵ�.
		void set(int index)
		{
			if ((index < 0) || (index >= s_p_game_main->game_option.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return;
			}

			s_p_game_main->game_option.flag[index] = true;
		}
		//! ����� ���� �÷��׸� �����Ѵ�.
		void reset(int index)
		{
			if ((index < 0) || (index >= s_p_game_main->game_option.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return;
			}

			s_p_game_main->game_option.flag[index] = false;
		}
		//! ����� ���� �÷����� ���� ���θ� ���� �ش�.
		bool isSet(int index)
		{
			if ((index < 0) || (index >= s_p_game_main->game_option.MAX_FLAG))
			{
				ASSERT(false && "flag range overflow");
				return false;
			}

			return s_p_game_main->game_option.flag[index];
		}
	}

	namespace battle
	{
		//? ���⿡ ���� ���� ��� ���� �־�� �Ѵ�.
		static GameMain::BATTLERESULT s_result = GameMain::BATTLERESULT_EVADE;

		//! ���� ��Ȳ�� ���������� �˷� �ش�.
		void init(void)
		{
			s_p_game_main->enemy.clear();
		}
		//! ������ �����ͷ� ������ �Ѵ�.
		void start(bool is_assualt_mode)
		{
			s_result = s_p_game_main->runBattleMode(is_assualt_mode);
			s_p_game_main->changeWindowForField();
		}
		//! ������ �����ϴ� ���� �߰��Ѵ�. �Է� ����� ���� index ��ȣ�� �Ѱ� �ִ� ���̴�.
		void registerEnemy(int ix_enemy)
		{
			s_p_game_main->registerEnemy(ix_enemy);
		}
		//! ������ �����ϴ� ���� ȭ�鿡 ǥ���Ѵ�.
		void showEnemy(void)
		{
			s_p_game_main->changeWindowForBattle();
			game::updateScreen();
		}
		//! ������ ����� �����ش�.
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
		//! ���� �ǽ��� �ִ� �Ʊ��� ���� ���� �ش�.
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
		//! ���� �ǽ��� �ִ� ���� ���� ���� �ش�.
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
		//? MenuSelection�� default parameter�� ���� ������ �ϴ� ���� �ʿ������� ��
		static MenuList s_menu;
		static int s_result = 0;

		//! ���� �������� �ʱ�ȭ �Ѵ�.
		void init(void)
		{
			s_menu.clear();
			s_result = 0;
		}
		//! ���� �������� �߰� �Ѵ�.
		void add(const char* sz_string)
		{
			s_menu.push_back(sz_string);
		}
		//! ���� ���õ� ���������� ������ �Ѵ�.
		void run(void)
		{
			s_result = MenuSelection(s_menu)();
		}
		//! ���õ� ����� ���� �´�.
		int  getResult(void)
		{
			return s_result;
		}
	}

} // namespace game

} // namespace hadar

#endif