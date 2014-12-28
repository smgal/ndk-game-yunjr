
#include "hd_class_game_main.h"

#include "hd_base_state.h"

#include "hd_base_config.h"
//??#include "hd_base_key_buffer.h"
#include "hd_base_sound.h"

#include "hd_class_console.h"
#include "hd_class_extern.h"
#include "hd_class_pc_functor.h"
#include "hd_class_map_event.h"
#include "hd_class_select.h"
//??#include "hd_class_window_map.h"
//??#include "hd_class_window_console.h"
//??#include "hd_class_window_status.h"
//??#include "hd_class_window_battle.h"

#include "hd_res_string.h"
//??#include "hd_res_instance.h"

//??using namespace avej;

////////////////////////////////////////////////////////////////////////////////
// macro

#define CONSOLE_WRITE(format, var) \
	SPRINTF(sz_temp, 256, format, var); \
	console.write(sz_temp);

#define CONSOLE_WRITE2(format, var1, var2) \
	SPRINTF(sz_temp, 256, format, var1, var2); \
	console.write(sz_temp);

#define CONSOLE_WRITE4(format, var1, var2, var3, var4) \
	SPRINTF(sz_temp, 256, format, var1, var2, var3, var4); \
	console.write(sz_temp);

#define NUM_OF_CONSCIOUS_PLAYER(player) sena::for_each(player.begin(), player.end(), FnNumOfConscious<PcPlayer*>()).Result()
#define NUM_OF_CONSCIOUS_ENEMY(enemy) sena::for_each(enemy.begin(), enemy.end(), FnNumOfConscious<PcEnemy*>()).Result()

////////////////////////////////////////////////////////////////////////////////
// private

namespace
{
	template <class type>
	class FnctDelete
	{
	public:
		void operator()(type obj)
		{
			delete obj;
		}
	};
}

////////////////////////////////////////////////////////////////////////////////
// class

namespace hadar
{
	GameMain* s_p_game_main;
}

////////////////////////////////////////////////////////////////////////////////
// class

hadar::GameMain::GameMain()
	: game_state(GAMESTATE_MAP)
{
	// << map convert utility >>
	// MapUtil::DisplayMapAsText_1_2("town1.map");
	// MapUtil::DisplayMapAsText_3("GAEA.M");
	// __asm int 03h;

	player.reserve(MAX_PLAYER);
	enemy.reserve(MAX_ENEMY);

	ASSERT(player.capacity() == MAX_PLAYER);
	ASSERT(enemy.capacity() == MAX_ENEMY);

	// save�� �ؾ� �ϴ� object�� ��� ���
	{
		//?? ��� ������ ������ ��
		save_list.push_back(&party);

		for (int i = 0; i < MAX_PLAYER; i++)
			save_list.push_back(&m_player[i]);

		save_list.push_back(&map);
		save_list.push_back(&game_option);
	}

	// party �ʱ�ȭ
	{
		// �ܺο��� �� ���� �����Ǿ����� ��
		party.x = -1;
		party.y = -1;
		//??
		party.ability.can_use_ESP = false;
		party.ability.can_use_special_magic = false;
	}

	// player �ʱ�ȭ
	{
		m_player[0].setDefault(0);

		m_player[0].setName("����");
		m_player[0].class_ = 8;
		m_player[0].level[0] = 1;
		m_player[0].level[1] = 1;
		m_player[0].level[2] = 1;
		m_player[0].reviseAttribute();

/*@@ ���� �ӽ� party �����*/

		m_player[1].setDefault(0);
		m_player[1].setName("��ŧ����");
		m_player[1].hp = 17;
		m_player[1].sp = 5;
		m_player[1].esp = 5;

		m_player[2].setDefault(0);
		m_player[2].setName("���ڱԽ�");
		m_player[2].gender = PcPlayer::GENDER_FEMALE;
		m_player[2].hp = 7;
		m_player[2].sp = 17;
		m_player[2].esp = 5;

		m_player[3].setDefault(0);
		m_player[3].setName("ī�̳ʽ�");
		m_player[2].hp = 10;
		m_player[2].sp = 11;
		m_player[2].esp = 3;
		m_player[3].class_ = 2;

/* */
		for (int i = 0; i < sizeof(m_player) / sizeof(m_player[0]); i++)
		{
			m_player[i].order = i;
			player.push_back(&m_player[i]);
		}
	}
	
	// map �ʱ�ȭ
	{
		// map���� default handler�� �˷���
		map.act_func[Map::ACT_BLOCK] = &GameMain::m_actBlock;
		map.act_func[Map::ACT_MOVE]  = &GameMain::m_actMove;
		map.act_func[Map::ACT_EVENT] = &GameMain::m_actEvent;
		map.act_func[Map::ACT_ENTER] = &GameMain::m_actEnter;
		map.act_func[Map::ACT_SIGN]  = &GameMain::m_actSign;
		map.act_func[Map::ACT_WATER] = &GameMain::m_actWater;
		map.act_func[Map::ACT_SWAMP] = &GameMain::m_actSwamp;
		map.act_func[Map::ACT_LAVA]  = &GameMain::m_actLava;
		map.act_func[Map::ACT_TALK]  = &GameMain::m_actTalk;
	}
/* //??
	// window �ʱ�ȭ
	{
		// �Ʒ� window���� ���� ������ ������ ����

		// Map�� window
		{
			Window* pWindow = new WindowMap(this);
			const config::Rect& region = config::REGION_MAP_WINDOW;
			pWindow->setRegion(region.x, region.y, region.w, region.h);
			window.push_back(pWindow);
		}

		// Sub-map�� window (temp)
		{
			WindowMap* pWindow = new WindowMap(this);
			const config::Rect& region = config::REGION_MAP_SUB_WINDOW;
			pWindow->setRegion(region.x, region.y, region.w, region.h);
			pWindow->setAnchor("", 50, 50);
			window.push_back(pWindow);
		}

		// Console�� window
		{
			Window* pWindow = new WindowConsole(this);
			const config::Rect& region = config::REGION_CONSOLE_WINDOW;
			pWindow->setRegion(region.x, region.y, region.w, region.h);
			window.push_back(pWindow);
		}

		// Status�� window
		{
			Window* pWindow = new WindowStatus(this);
			const config::Rect& region = config::REGION_STATUS_WINDOW;
			pWindow->setRegion(region.x, region.y, region.w, region.h);
			window.push_back(pWindow);
		}

		// Battle�� window
		{
			Window* pWindow = new WindowBattle(this);
			const config::Rect& region = config::REGION_MAP_WINDOW;
			pWindow->setRegion(region.x, region.y, region.w, region.h);
			pWindow->hide();
			window.push_back(pWindow);
		}
	}
*/
}

hadar::GameMain::~GameMain(void)
{
//??	sena::for_each(window.begin(), window.end(), FnctDelete<Window*>());
}

bool hadar::GameMain::loadMapFromFile(const char* sz_file_name)
{
	smutil::FileReadStream stream(sz_file_name);

	if (!stream.isValid())
		return false;

	unsigned char byte;

	map.clearData();

	// ���� ���� map type�� �Ǻ��� �־�� ��
	map.setType(Map::TYPE_TOWN);

	stream.read(&byte, 1);
	map.width  = byte;

	stream.read(&byte, 1);
	map.height = byte;

	for (int y = 0; y < map.height; y++)
	{
		stream.read(&map.data[y][0], map.width);
	}

	return true;
}

bool hadar::GameMain::loadScript(const char* sz_file_name)
{
	game_option.script_file = sz_file_name;

	map_event::load(sz_file_name);

	return true;
}

void hadar::GameMain::changeWindowForBattle(void)
{
/* //??
	window[WINDOWTYPE_MAP]->hide();
	window[WINDOWTYPE_SUBMAP]->hide();

	window[WINDOWTYPE_BATTLE]->show();
*/
}

void hadar::GameMain::changeWindowForField(void)
{
/* //??
	window[WINDOWTYPE_BATTLE]->hide();

	window[WINDOWTYPE_MAP]->show();
	window[WINDOWTYPE_SUBMAP]->show();
*/
}

bool hadar::GameMain::isValidWarpPos(int x, int y)
{
/* //??
	// main map�� ǥ�� ũ�⸦ �������� �ؾ� �� 
	return ((x >= WindowMap::_X_RADIUS) && (x < map.width-WindowMap::_X_RADIUS) && (y >= WindowMap::_Y_RADIUS) && (y < map.height-WindowMap::_Y_RADIUS));
*/
	return false;
}

int hadar::GameMain::selectPlayer(const char* szTitle)
{
	if (szTitle == NULL)
		szTitle = "�Ѹ��� ���ÿ� ---";

	MenuList menu;

	menu.reserve(MAX_PLAYER+1);

	menu.push_back(szTitle);

	int realPlayer[MAX_PLAYER];
	int nReal = 0;
	int count = 0;
	for (sena::vector<PcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj, ++count)
	{
		ASSERT(nReal < MAX_PLAYER);

		if ((*obj)->isValid())
		{
			menu.push_back((*obj)->getName());
			realPlayer[nReal++] = count;
		}
	}

	ASSERT(nReal > 0);

	int selected = MenuSelection(menu)();

	--selected;

	return (selected >= 0) ? realPlayer[selected] : -1;
}

int hadar::GameMain::selectEnemy(void)
{
	return 0;
/* //??
	int num_enabled = enemy.size();
	int selected = 0;
	
	do
	{
		window[WINDOWTYPE_BATTLE]->display(1, selected);

		bool bUpdate = false;

		do
		{
			unsigned short key;
			while ((key = KeyBuffer::getKeyBuffer().getKey()) < 0)
				;

			switch (key)
			{
			case KEY_DIR_UP:
			case KEY_DIR_DOWN:
				{
					int dy = (key == KEY_DIR_UP) ? -1 : +1;
					selected += dy;

					if (selected < 0)
						selected = num_enabled - 1;
					if (selected >= num_enabled)
						selected = 0;

					bUpdate = true;
				}
				break;
// ��ȿ ���ڸ� �������� �ʰ� ��
//			case KEY_BTN_A:
//				selected = -1;
//				// pass through
//
			case KEY_BTN_B:
				window[WINDOWTYPE_BATTLE]->display(1);
				return selected;
			}
		} while (!bUpdate);
		
	} while (1);
*/
}

void hadar::GameMain::detectGameOver(void)
{
	// ��ó��
	sena::for_each(player.begin(), player.end(), FnCheckCondition<PcPlayer*>());

	// ���� ������ �ִ� ���ڸ� Ȯ��
	int num_alive = NUM_OF_CONSCIOUS_PLAYER(player);

	if (num_alive == 0)
		proccessGameOver(EXITCODE_BY_ACCIDENT);
}

bool hadar::GameMain::checkEndOfBattle(ENDBATTLE& exitCode)
{
	int num_alive = NUM_OF_CONSCIOUS_PLAYER(player);

	if (num_alive == 0)
	{
		exitCode = ENDBATTLE_LOSE;
		return true;
	}

	num_alive = NUM_OF_CONSCIOUS_ENEMY(enemy);

	if (num_alive == 0)
	{
		exitCode = ENDBATTLE_WIN;
		return true;
	}

	exitCode = ENDBATTLE_NONE;

	return false;
}

void hadar::GameMain::plusGold(void)
{
	int num_gold = sena::for_each(enemy.begin(), enemy.end(), FnPlusGold<PcEnemy*>()).Result();
	
	party.gold += num_gold;

	game::console::writeConsole(15, 3, "������ ", smutil::IntToStr(num_gold)(), "���� ���� �����.");
}

int hadar::GameMain::registerEnemy(int index)
{
	int ix_enemy = enemy.size();

	m_enemy[ix_enemy] << getEnemyDataFromEnemyTable(index);
	m_enemy[ix_enemy].ed_number = index;
	enemy.push_back(&m_enemy[ix_enemy]);

	return ix_enemy;
}

// �������� �ºη� ���̳��� true, ������ ġ�� false
hadar::GameMain::BATTLERESULT hadar::GameMain::runBattleMode(bool is_assualt_mode)
{
	return BATTLERESULT_LOSE;
/* //??
	BATTLERESULT result = BATTLERESULT_WIN;
	game_state = GAMESTATE_BATTLE;

	ENDBATTLE exitCode = ENDBATTLE_NONE;
	int battle[MAX_PLAYER][4];

	do
	{
		if (!is_assualt_mode)
			goto QAZ;

		sena::memclear(battle, sizeof(battle));

		window[WINDOWTYPE_BATTLE]->display(0);

		// ��� ���
		{
			bool bAutoBattle = false;

			for (sena::vector<PcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
			{
				int order = (*obj)->order;

				if ((*obj)->isConscious())
				{
					if (!bAutoBattle)
					{
						MenuList menu;

						smutil::string str1;
						str1 += (*obj)->getName();
						str1 += "�� ���� ��� ===>";

						smutil::string str2;
						str2 += "�� ���� ���� ";
						str2 += resource::getWeaponName((*obj)->weapon).sz_name;
						str2 += resource::getWeaponName((*obj)->weapon).sz_josa_with;
						str2 += "�� ����";

						menu.reserve(8);
						menu.push_back(str1);
						menu.push_back(str2);
						menu.push_back("�� ���� ������ ���� ����");
						menu.push_back("��� ������ ���� ����");
						menu.push_back("������ Ư�� ���� ����");
						menu.push_back("������ ġ��");
						menu.push_back("������ �ʴɷ� ���");

						if (order == 0)
							menu.push_back("���࿡�� ������ ���� �� ���� ����");
						else
							menu.push_back("������ �õ���");

						int selected = MenuSelection(menu)();

						if (selected != 1)
							LoreConsole::getConsole().clear();

						if ((selected == 7) && (order == 0))
						{
							selected = 8;
							bAutoBattle = true;
						}

						battle[order][1] = selected;
					}
					else
					{
						battle[order][1] = 8;
					}

					// �޴� ���ÿ� ���� ���� ��� ����
					switch (battle[order][1])
					{
					case 1:
						{
							int selected = selectEnemy();
							LoreConsole::getConsole().clear();
							if (selected == -1)
								battle[order][1] = 0;
							battle[order][2] = (*obj)->weapon;
							battle[order][3] = selected;
						}
						break;
					case 2:
					case 3:
					case 4:
						{
							int  ixMagicOffset = 0;
							int  num_enabled = 7;
							bool isSingleAttack = true;

							switch (battle[order][1])
							{
							case 2:
								ixMagicOffset = 0;
								{
									int level2 = (*obj)->level[1];
									if      (level2 <= 1)
										num_enabled = 2;
									else if (level2 <= 3)
										num_enabled = 3;
									else if (level2 <= 7)
										num_enabled = 4;
									else if (level2 <= 11)
										num_enabled = 5;
									else if (level2 <= 15)
										num_enabled = 6;
								}
								break;
							case 3:
								ixMagicOffset = 6;
								{
									int level2 = (*obj)->level[1];
									if      (level2 <= 1)
										num_enabled = 1;
									else if (level2 <= 2)
										num_enabled = 2;
									else if (level2 <= 5)
										num_enabled = 3;
									else if (level2 <= 9)
										num_enabled = 4;
									else if (level2 <= 13)
										num_enabled = 5;
									else if (level2 <= 17)
										num_enabled = 6;
								}
								isSingleAttack = false;
								break;
							case 4:
								ixMagicOffset = 12;
								{
									int level2 = (*obj)->level[1];
									if      (level2 <= 4)
										num_enabled = 1;
									else if (level2 <= 9)
										num_enabled = 2;
									else if (level2 <= 11)
										num_enabled = 3;
									else if (level2 <= 13)
										num_enabled = 4;
									else if (level2 <= 15)
										num_enabled = 5;
									else if (level2 <= 17)
										num_enabled = 6;
								}
								break;
							default:
								ASSERT(false);
							}

							MenuList menu;

							menu.reserve(8);
							menu.push_back("");
							menu.push_back("����");
							for (int i = 0; i < 6; i++)
								menu.push_back(resource::getMagicName(i+1+ixMagicOffset).sz_name);

							int selected = MenuSelection(menu, num_enabled)();
							int ix_enemy  = -1;

							if ((selected == 0) || (selected == 1))
								battle[order][1] = 0;
							else if (isSingleAttack)
								ix_enemy = selectEnemy();

							LoreConsole::getConsole().clear();

							battle[order][2] = selected - 1;
							battle[order][3] = ix_enemy;
						}
						break;
					case 5:
						player[order]->castCureSpell();
						break;
					case 6:
						{
							MenuList menu;

							menu.reserve(8);
							menu.push_back("");
							for (int i = 0; i < 5; i++)
								menu.push_back(resource::getMagicName(i+1+40).sz_name);

							int selected = MenuSelection(menu)();
							int ix_enemy  = -1;

							if (selected == 0)
								battle[order][1] = 0;
							else
								ix_enemy = selectEnemy();

							LoreConsole::getConsole().clear();

							battle[order][2] = selected;
							battle[order][3] = ix_enemy;

							if ((ix_enemy < 0) || (enemy[ix_enemy]->unconscious > 0) || (enemy[ix_enemy]->dead > 0))
								battle[order][1] = 0;
						}
						break;
					case 8:
						switch ((*obj)->class_)
						{
						case  0:
						case  1:
						case  4:
						case  5:
						case  6:
						case  7:
						case  8:
						case 10:
							battle[order][1] = 1;
							battle[order][2] = (*obj)->weapon;
							battle[order][3] = 0;
							break;
						case  2:
						case  9:
							battle[order][1] = 2;

							switch ((*obj)->level[1])
							{
							case 0:
							case 1:
							case 2:
							case 3:
								battle[order][2] = 1;
								break;
							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
							case 11:
								battle[order][2] = 2;
								break;
							default:
								battle[order][2] = 3;
								break;
							}
							battle[order][3] = 0;
							break;
						case  3:
							battle[order][1] = 6;
							battle[order][2] = 5;
							{
								int target = 0;
								for (int ix_enemy = 1; ix_enemy < int(enemy.size()); ++ix_enemy)
								{ // @@ dead �³�?
									if ((enemy[ix_enemy]->unconscious == 0) && (enemy[ix_enemy]->dead))
										target = ix_enemy;
								}
								battle[order][3] = target;
							}

							break;
						default:
							ASSERT(false);
						}
						break;
					}
				}
			}
		}

		LoreConsole::getConsole().clear();

		// ��� �ؼ�
		{
			for (sena::vector<PcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
			{
				if ((*obj)->isConscious())
				{
					int order = (*obj)->order;

					static smutil::SmSet s_printMessage("0,4,6..8");

					if (s_printMessage.isSet(battle[order][1]))
					{
						smutil::string s = getBattleMessage(**obj, battle[order][1], battle[order][2], *enemy[battle[order][3]]);
						LoreConsole::getConsole().setTextColorIndex(15);
						LoreConsole::getConsole().write(s);
					}

					switch (battle[order][1])
					{
						case 1:
							player[order]->attackWithWeapon(battle[order][2], battle[order][3]);
							break;
						case 2:
							player[order]->castSpellToOne(battle[order][2], battle[order][3]);
							break;
						case 3:
							player[order]->castSpellToAll(battle[order][2]);
							break;
						case 4:
							player[order]->castSpellWithSpecialAbility(battle[order][2], battle[order][3]);
							break;
						case 6:
							player[order]->useESPForBattle(battle[order][2], battle[order][3]);
							break;
						case 7:
							{
								if ((*obj)->tryToRunAway())
								{
									game::pressAnyKey();

									LoreConsole::getConsole().clear();

									exitCode = ENDBATTLE_RUN_AWAY;

									goto END_OF_BATTLE;
								}
							}
					}
					
					LoreConsole::getConsole().write("");
				}
			}
			
			window[WINDOWTYPE_BATTLE]->display(1);

			LoreConsole::getConsole().write("");
			LoreConsole::getConsole().display();
			game::updateScreen();
			game::pressAnyKey();
		}
QAZ:
		LoreConsole::getConsole().clear();

		{
			for (sena::vector<PcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
			{
				if ((*obj)->poison > 0)
				{
					if ((*obj)->unconscious > 0)
					{
						(*obj)->dead = 1;
					}
					else
					{
						(*obj)->hp -= (*obj)->poison;
						if ((*obj)->hp <= 0)
							(*obj)->unconscious = 1;
					}
				}
				if (((*obj)->unconscious == 0) && ((*obj)->dead == 0))
				{
					(*obj)->attack();
				}
			}
		}

		window[WINDOWTYPE_STATUS]->setUpdateFlag();
		window[WINDOWTYPE_STATUS]->display();

		game::updateScreen();

		if (checkEndOfBattle(exitCode))
			break;

		is_assualt_mode = true;

		// ���ۿ����� �� ���� �Ŀ� Press Any Key ���� ����� ������
		game::pressAnyKey();

	} while (1);

END_OF_BATTLE:
	switch (exitCode)
	{
	case ENDBATTLE_LOSE:
		proccessGameOver(EXITCODE_BY_ENEMY);
		return BATTLERESULT_LOSE;
	case ENDBATTLE_WIN:
		LoreConsole::getConsole().clear();
		plusGold();
		break;
	case ENDBATTLE_RUN_AWAY:
		result = BATTLERESULT_EVADE;
		break;
	default:
		ASSERT(false);
	}

	window[WINDOWTYPE_MAP]->setUpdateFlag();
	window[WINDOWTYPE_MAP]->display();

	window[WINDOWTYPE_SUBMAP]->setUpdateFlag();
	window[WINDOWTYPE_SUBMAP]->display();

	game_state = GAMESTATE_MAP;

	return result;
*/
}

void hadar::GameMain::encounterEnemy(void)
{
/* //??
	if (map.getEncounteredEnemy() == 0)
		return;

	enemy.clear();

	int nEnemy = (smutil::random(party.max_enemy) + 1);

	int enemyAgility = 0;
	for (int i = 0; i < nEnemy; i++)
	{
		int ix_enemy = map.getEncounteredEnemy();
		ASSERT(ix_enemy > 0);

		int index = registerEnemy(ix_enemy);
		enemyAgility += enemy[index]->agility;
	}

	ASSERT(nEnemy > 0);
	enemyAgility /= nEnemy;

	LoreConsole& console = LoreConsole::getConsole();

	console.clear();

	console.setTextColorIndex(12);
	console.write("���� �����ߴ� !!!");
	console.write("");

	console.setTextColorIndex(11);
	console.write("���� ��� ��ø��");
	{
		char sz_temp[256];
		int avgAgility = sena::for_each(enemy.begin(), enemy.end(), FnAverageAgility<PcEnemy*>()).Result();
		CONSOLE_WRITE(" : %d", avgAgility);
	}

	console.display();

	changeWindowForBattle();

	game::updateScreen();

	// ���� ���ڸ� ����ϱ� ���ؼ� �ӽ÷� console â�� ����� ���δ�.
	config::Rect savedRect;
	int reducedSize = 4*config::DEFAULT_FONT_HEIGHT;

	window[WINDOWTYPE_CONSOLE]->getRegion(&savedRect.x, &savedRect.y, &savedRect.w, &savedRect.h);
	window[WINDOWTYPE_CONSOLE]->setRegion(savedRect.x, savedRect.y+reducedSize, savedRect.w, savedRect.h-reducedSize);

	MenuList menu;

	menu.reserve(3);
	menu.push_back("");
	menu.push_back("���� �����Ѵ�");
	menu.push_back("��������");

	bool willingToAvoidBattle = (MenuSelection(menu)() != 1);

	// console â�� ����� ���ͽ�Ų��.
	window[WINDOWTYPE_CONSOLE]->setRegion(savedRect.x, savedRect.y, savedRect.w, savedRect.h);

	// '��������'�� �������� �� �� ������ bg color�� ä��� ����
	console.clear();
	console.display();

	do
	{
		bool is_assualt_mode;

		if (willingToAvoidBattle)
		{
			int avgLuck    = sena::for_each(player.begin(), player.end(), FnAverageLuck<PcPlayer*>()).Result();
			int avgAgility = sena::for_each(enemy.begin(), enemy.end(), FnAverageAgility<PcEnemy*>()).Result();

			if (avgLuck > avgAgility)
				break; // ������ ȸ��

			is_assualt_mode = false;
		}
		else
		{
			int avgAgility1 = sena::for_each(player.begin(), player.end(), FnAverageAgility<PcPlayer*>()).Result();
			int avgAgility2 = sena::for_each(enemy.begin(), enemy.end(), FnAverageAgility<PcEnemy*>()).Result();

			is_assualt_mode = (avgAgility1 > avgAgility2);
		}

		runBattleMode(is_assualt_mode);
	} while(0);

	changeWindowForField();
*/
}

void hadar::GameMain::showPartyStatus()
{
	char sz_temp[256];

	LoreConsole& console = LoreConsole::getConsole();

	console.clear();
	console.setTextColorIndex(7);

	CONSOLE_WRITE("X �� = %d", party.x);
	CONSOLE_WRITE("Y �� = %d", party.y);
	CONSOLE_WRITE("���� �ķ� = %d", party.food);
	CONSOLE_WRITE("���� Ȳ�� = %d", party.gold);

	console.write("");

	CONSOLE_WRITE("������ ȶ�� : %d", party.ability.magic_torch);
	CONSOLE_WRITE("���� �λ�   : %d", party.ability.levitation);
	CONSOLE_WRITE("������ ���� : %d", party.ability.walk_on_water);
	CONSOLE_WRITE("������ ���� : %d", party.ability.walk_on_swamp);

	console.display();
}

void hadar::GameMain::showCharacterStatus(void)
{
	int selected = selectPlayer("�ɷ��� ������� �ι��� �����Ͻÿ�");
	if (selected >= 0)
	{
		ASSERT(selected < MAX_PLAYER);
		showCharacterStatus(*player[selected]);
	}
}

void hadar::GameMain::showCharacterStatus(const PcPlayer& selected)
{
	ASSERT(selected.isValid());

	if (!selected.isValid())
		return;

	char sz_temp[256];

	LoreConsole& console = LoreConsole::getConsole();

	console.clear();

	console.setTextColorIndex(11);
	CONSOLE_WRITE("# �̸� : %s", selected.getName())
	CONSOLE_WRITE("# ���� : %s", selected.getGenderName())
	CONSOLE_WRITE("# ��� : %s", selected.getClassName())

	console.write("");

	console.setTextColorIndex(3);
	CONSOLE_WRITE("ü��   : %d", selected.strength)
	CONSOLE_WRITE("���ŷ� : %d", selected.mentality)
	CONSOLE_WRITE("���߷� : %d", selected.concentration)
	CONSOLE_WRITE("�γ��� : %d", selected.endurance)
	CONSOLE_WRITE("���׷� : %d", selected.resistance)
	CONSOLE_WRITE("��ø�� : %d", selected.agility)
	CONSOLE_WRITE("���   : %d", selected.luck)

	console.display();
	game::updateScreen();

	game::pressAnyKey();

	console.clear();

	// �� �κ��� �� �� �� �ݺ�
	console.setTextColorIndex(11);
	CONSOLE_WRITE("# �̸� : %s", selected.getName())
	CONSOLE_WRITE("# ���� : %s", selected.getGenderName())
	CONSOLE_WRITE("# ��� : %s", selected.getClassName())

	console.write("");

	console.setTextColorIndex(3);
	CONSOLE_WRITE2("������ ��Ȯ��   : %2d    ���� ����   : %2d", selected.accuracy[0], selected.level[0])
	CONSOLE_WRITE2("���ŷ��� ��Ȯ�� : %2d    ���� ����   : %2d", selected.accuracy[1], selected.level[1])
	CONSOLE_WRITE2("�ʰ����� ��Ȯ�� : %2d    �ʰ��� ���� : %2d", selected.accuracy[2], selected.level[2])
	CONSOLE_WRITE("## ����ġ   : %d", selected.experience)

	console.write("");

	console.setTextColorIndex(2);
	CONSOLE_WRITE("��� ���� - %s", selected.getWeaponName())
	CONSOLE_WRITE2("���� - %s           ���� - %s", selected.getShieldName(), selected.getArmorName())

	console.display();
}

void hadar::GameMain::showQuickView(void)
{
	char sz_temp[256];

	LoreConsole& console = LoreConsole::getConsole();

	console.clear();

	console.setTextColorIndex(15);
	console.write("                �̸�    �ߵ�  �ǽĺҸ�    ����");
	console.write("");

	console.setTextColorIndex(7);
	for (sena::vector<PcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
	{
		if ((*obj)->isValid())
		{
			CONSOLE_WRITE4("%20s   %5d %9d %7d", (*obj)->getName(), (*obj)->poison, (*obj)->unconscious, (*obj)->dead)
		}
	}

	console.display();
}

void hadar::GameMain::castSpell(void)
{
	int selected = selectPlayer();
	if (selected < 0)
		return;

	if (!player[selected]->isConscious())
	{
		game::console::writeConsole(7, 2, player[selected]->get3rdPersonName(), "�� ������ ����� �� �ִ� ���°� �ƴմϴ�.");
		return;
	}

	MenuList menu;

	menu.reserve(4);
	menu.push_back("����� ������ ���� ===>");
	menu.push_back("���� ����");
	menu.push_back("ġ�� ����");
	menu.push_back("��ȭ ����");

	switch (MenuSelection(menu)())
	{
	case 1:
		player[selected]->castAttackSpell();
		break;
	case 2:
		player[selected]->castCureSpell();
		break;
	case 3:
		player[selected]->castPhenominaSpell();
		break;
	default:
		break;
	}
}

void hadar::GameMain::useExtrasense(void)
{
	LoreConsole::getConsole().clear();

	int selected = selectPlayer();
	if (selected < 0)
		return;

	if (!player[selected]->isConscious())
	{
		game::console::writeConsole(7, 2, player[selected]->get3rdPersonName(), "�� �ʰ����� ����Ҽ��ִ� ���°� �ƴմϴ�.");
		return;
	}

	player[selected]->useESP();
}

void hadar::GameMain::restHere(void)
{
/* //??
	LoreConsole& console = LoreConsole::getConsole();

	console.clear();

	sena::for_each(player.begin(), player.end(), FnRestHere<PcPlayer*>());

	if (party.ability.magic_torch > 0)
		--party.ability.magic_torch;

	party.ability.levitation  = 0;
	party.ability.walk_on_water = 0;
	party.ability.walk_on_swamp = 0;
	party.ability.mind_control = 0;

	console.display();

	window[WINDOWTYPE_STATUS]->setUpdateFlag();
	window[WINDOWTYPE_STATUS]->display();

	game::updateScreen();
	//@@ �ʿ��Ѱ�?
	game::pressAnyKey();
*/
}

namespace
{
	///////////////////////////////////////////////////////////////////////////////
	// GetSaveFileName class;
	const char* SAVE_FILE_NAME = "gamedat";
	const char* SAVE_FILE_EXT = ".sav";

	class GetSaveFileName
	{
	public:
		GetSaveFileName(int index)
		{
			m_name += "./";
			m_name += SAVE_FILE_NAME;
			m_name += smutil::IntToStr(index)();
			m_name += SAVE_FILE_EXT;
		}
		~GetSaveFileName(void)
		{
			m_name += "./";
		}
		operator const char*(void)
		{
			return m_name;
		}

	private:
		smutil::string m_name;

	};
}

bool hadar::GameMain::loadGame(int index)
{
/* //??
	SerializedStream stream(GetSaveFileName(index), SerializedStream::STREAMTYPE_READ);

	for (sena::vector<Serialized*>::iterator obj = save_list.begin(); obj != save_list.end(); ++obj)
	{
		stream >> *(*obj);
	}

	map_event::registerEventFileName(game_option.script_file);

	window[WINDOWTYPE_MAP]->setUpdateFlag();
	window[WINDOWTYPE_SUBMAP]->setUpdateFlag();
	window[WINDOWTYPE_STATUS]->setUpdateFlag();

	return true;
*/
	return false;
}

bool hadar::GameMain::saveGame(int index)
{
	SerializedStream stream(GetSaveFileName(index), SerializedStream::STREAMTYPE_WRITE);

	for (sena::vector<Serialized*>::iterator obj = save_list.begin(); obj != save_list.end(); ++obj)
	{
		stream << *(*obj);
	}

	return true;
}

bool hadar::GameMain::selectLoadMenu(void)
{
/* //??
	MenuList menu;

	menu.push_back("�ҷ� ���� ���� ������ �����Ͻʽÿ�.");
	menu.push_back("�����ϴ�");
	menu.push_back("�� ���� ����Ÿ");
	menu.push_back("���� ����Ÿ 1 (��)");
	menu.push_back("���� ����Ÿ 2 (��)");
	menu.push_back("���� ����Ÿ 3 (��)");

	int selected = MenuSelection(menu)();

	if (selected <= 1)
		return false;

	{
		LoreConsole& console = LoreConsole::getConsole();

		console.clear();
		console.setTextColorIndex(11);
		console.write("�����ߴ� ������ �ٽ� �ҷ��ɴϴ�");
		console.display();

		// 0~3 ���� ��
		if (!loadGame(selected-2))
		{
			// ������ ���� ���� ����
			//?? ���õ� �޽����� ������ �ٽ� �޴��� �����ϰ� �ؾ� ��
			//?? �Ǵ� �ڵ����� �����Ͱ� �ִ����� Ȯ���Ͽ� �޴��� ���̶���Ʈ�� ���Ѿ� ��
			return false;
		}

		window[WINDOWTYPE_MAP]->setUpdateFlag();
		window[WINDOWTYPE_SUBMAP]->setUpdateFlag();
		window[WINDOWTYPE_STATUS]->setUpdateFlag();
		console.clear();
		console.display();
	}

	return true;
*/
	return false;
}

bool hadar::GameMain::selectSaveMenu(void)
{
	MenuList menu;

	menu.push_back("������ ���� ��Ҹ� �����Ͻʽÿ�.");
	menu.push_back("�����ϴ�");
	menu.push_back("�� ���� ����Ÿ");
	menu.push_back("���� ����Ÿ 1 (��)");
	menu.push_back("���� ����Ÿ 2 (��)");
	menu.push_back("���� ����Ÿ 3 (��)");

	int selected = MenuSelection(menu)();

	if (selected <= 1)
		return false;

	{
		LoreConsole& console = LoreConsole::getConsole();

		console.clear();
		console.setTextColorIndex(12);
		console.write("������ ������ �����մϴ�");
		console.display();

		// 0~3 ���� ��
		if (!saveGame(selected-2))
		{
			//?? ���� ������ �˷� ��� �ϳ�?
			return false;
		}

		console.setTextColorIndex(7);
		console.write("");
		console.write("�����߽��ϴ�");
		console.display();

		game::pressAnyKey();
	}

	return true;
}

void hadar::GameMain::selectGameOption(void)
{
/* //??
	MenuList menu;

	menu.reserve(7);
	menu.push_back("���� ���� ��Ȳ");
	menu.push_back("���̵� ����");
	menu.push_back("���� ������ ���� ����");
	menu.push_back("���࿡�� ���� ��Ŵ");
	menu.push_back("������ ������ �簳");
	menu.push_back("������ ������ ����");
	menu.push_back("������ ��ħ");

	int selected = MenuSelection(menu)();

	switch (selected)
	{
	case 0:
		break;
	case 1:
		// ������ �����ϴ� ���� �ִ�ġ ����
		{
			menu.clear();
			menu.push_back("�ѹ��� �����ϴ� ������ �ִ�ġ�� �����Ͻʽÿ�");
			menu.push_back("3���� ����");
			menu.push_back("4���� ����");
			menu.push_back("5���� ����");
			menu.push_back("6���� ����");
			menu.push_back("7���� ����");

			int selected = MenuSelection(menu, -1, party.max_enemy - 2)();

			if (selected == 0)
				break;

			party.max_enemy = selected + 2;
		}

		// ������ �����ϴ� ���� �ִ�ġ ����
		{
			menu.clear();
			menu.push_back("������� ���� ������ ����ϱ� ?");
			menu.push_back("�Ϻη� ������ ���ϰ� �ʹ�");
			menu.push_back("�ʹ� ���� ������ ������ �ʴ´�");
			menu.push_back("����ģ ������ ������ �ϰڴ�");
			menu.push_back("���̴� ������� ��� �����ϰڴ�");
			menu.push_back("�׵��� �ǿ� ���ַ� �ִ�");

			int selected = MenuSelection(menu, -1, 6 - party.encounter)();

			if (selected == 0)
				break;

			party.encounter = 6 - selected;
		}
		break;
	case 2:
		{
			int  ix_player;
			int  indexTable[6] = {0, };
			int* pIndex = &indexTable[1];

			LoreConsole& console = LoreConsole::getConsole();

			console.clear();

			console.setTextColorIndex(12);
			console.write("������ �Ͽ��� ���� ������ ���� �Ͻʽÿ�.");
			console.write("");
			console.display();

			menu.clear();
			menu.push_back("@B������ �ٲ� �Ͽ� 1@@");

			for (ix_player = 1; ix_player < 5; ix_player++)
			{
				if (player[ix_player]->isValid())
				{
					menu.push_back(player[ix_player]->getName());
					*pIndex++ = ix_player;
				}
			}

			// ���ΰ��� ������ ������ 1�� ���϶�� �� ����� ������ �ʿ䰡 ����.
			if (menu.size() < 3)
			{
				LoreConsole& console = LoreConsole::getConsole();

				console.clear();
				console.setTextColorIndex(7);
				console.write("���� ������ ���� �� �ʿ䰡 �����ϴ�.");
				console.display();

				game::pressAnyKey();

				break;
			}

			// ���� ���ڸ� ����ϱ� ���ؼ� �ӽ÷� console â�� ����� ���δ�.
			config::Rect savedRect;
			int reducedSize = 3*config::DEFAULT_FONT_HEIGHT;

			window[WINDOWTYPE_CONSOLE]->getRegion(&savedRect.x, &savedRect.y, &savedRect.w, &savedRect.h);
			window[WINDOWTYPE_CONSOLE]->setRegion(savedRect.x, savedRect.y+reducedSize, savedRect.w, savedRect.h-reducedSize);

			do
			{
				int selected1 = MenuSelection(menu)();
				if (selected1 == 0)
					break;

				menu[0] = "@B������ �ٲ� �Ͽ� 2@@";

				int selected2 = MenuSelection(menu)();
				if (selected2 == 0)
					break;

				if (selected1 != selected2)
				{
					int ixPlayer1 = indexTable[selected1];
					int ixPlayer2 = indexTable[selected2];
					assert(ixPlayer1 > 0 && ixPlayer1 < 6);
					assert(ixPlayer2 > 0 && ixPlayer2 < 6);

					if (ixPlayer1 > ixPlayer2)
						sena::swap(ixPlayer1, ixPlayer2);

					assert(ixPlayer1 < ixPlayer2);

					// ���� ��� ������ �ٲٱ�
					sena::swap(player[ixPlayer1], player[ixPlayer2]);
					// order�� ��� ��ȣ�̹Ƿ� ������ ���ƾ� �Ѵ�
					sena::swap(player[ixPlayer1]->order, player[ixPlayer2]->order);
				}

				window[WINDOWTYPE_STATUS]->setUpdateFlag();
				window[WINDOWTYPE_STATUS]->display();
			} while(0);

			// console â�� ����� ���ͽ�Ų��.
			window[WINDOWTYPE_CONSOLE]->setRegion(savedRect.x, savedRect.y, savedRect.w, savedRect.h);

			console.clear();
		}
			
		break;
	case 3:
		{
			int  ix_player;
			int  indexTable[6] = {0, };
			int* pIndex = &indexTable[1];

			menu.clear();
			menu.push_back("@C���࿡�� ���� ��Ű�� ���� ����� ���ʽÿ�.@@");

			for (ix_player = 1; ix_player < 6; ix_player++)
			{
				if (player[ix_player]->isValid())
				{
					menu.push_back(player[ix_player]->getName());
					*pIndex++ = ix_player;
				}
			}

			// ���ΰ����̶�� �� ����� ������ �ʿ䰡 ����.
			if (menu.size() < 2)
			{
				LoreConsole& console = LoreConsole::getConsole();

				console.clear();
				console.setTextColorIndex(7);
				console.write("���ܽ�ų ������ �����ϴ�.");
				console.display();

				game::pressAnyKey();

				break;
			}

			int selected1 = MenuSelection(menu)();
			if (selected1 == 0)
				break;

			ix_player = indexTable[selected1];
			assert(ix_player > 0 && ix_player < 6);

			player[ix_player]->setName("");

			window[WINDOWTYPE_STATUS]->setUpdateFlag();
			window[WINDOWTYPE_STATUS]->display();
		}
		break;
	case 4:
		selectLoadMenu();
		break;
	case 5:
		selectSaveMenu();
		break;
	case 6:
		proccessGameOver(EXITCODE_BY_USER);
		break;
	}
*/
}

void hadar::GameMain::selectMainMenu(void)
{
	MenuList menu;

	menu.reserve(8);
	menu.push_back("����� ����� ���ÿ� ===>");
	menu.push_back("������ ��Ȳ�� ����");
	menu.push_back("������ ��Ȳ�� ����");
	menu.push_back("������ �ǰ� ���¸� ����");
	menu.push_back("������ ����Ѵ�");
	menu.push_back("�ʴɷ��� ����Ѵ�");
	menu.push_back("���⼭ ����");
	menu.push_back("���� ���� ��Ȳ");

	int selected = MenuSelection(menu)();

	switch (selected)
	{
	case 0:
		break;
	case 1:
		showPartyStatus();
		break;
	case 2:
		showCharacterStatus();
		break;
	case 3:
		showQuickView();
		break;
	case 4:
		castSpell();
		break;
	case 5:
		useExtrasense();
		break;
	case 6:
		restHere();
		break;
	case 7:
		selectGameOption();
		break;
	default:
		ASSERT(0);
	}
}

void hadar::GameMain::proccessGameOver(EXITCODE code)
{
	switch (code)
	{
	case EXITCODE_BY_USER:
		{
			MenuList menu;

			menu.push_back("������ �����ڽ��ϱ� ?"); // ������ 10�� ��
			menu.push_back("       << �ƴϿ� >>");
			menu.push_back("       <<   ��   >>");

			if (MenuSelection(menu)() != 2)
				return;
		}
		break;
	case EXITCODE_BY_ACCIDENT:
/* //??
		{
			LoreConsole& console = LoreConsole::getConsole();

			console.clear();
			console.setTextColorIndex(13);
			console.write("������ �����߿� ��� ����� �Ҿ���.");
			console.display();

			window[WINDOWTYPE_MAP]->display();
			window[WINDOWTYPE_SUBMAP]->display();

			window[WINDOWTYPE_STATUS]->setUpdateFlag();
			window[WINDOWTYPE_STATUS]->display();

			game::updateScreen();

			game::pressAnyKey();

			if (selectLoadMenu())
				return;
		}
*/
		break;
	case EXITCODE_BY_ENEMY:
		{
			LoreConsole& console = LoreConsole::getConsole();

			console.clear();
			console.setTextColorIndex(13);
			console.write("������ ��� �������� ���ߴ� !!");
			console.display();

			game::pressAnyKey();

			console.clear();

			MenuList menu;
			menu.reserve(8);
			menu.push_back("    @A��� �Ͻðڽ��ϱ� ?@@");
			menu.push_back("   ������ ������ �簳�Ѵ�");
			menu.push_back("       ������ ������");

			int selected = MenuSelection(menu)();

			if (selected == 1)
			{
				if (selectLoadMenu())
					return;
			}
		}
		break;
	case EXITCODE_BY_FORCE:
		// Ư���ϰ� ���ؾ��� ������ ����
		break;
	default:
		ASSERT(false);
	}

	//??throw ExceptionExitGame();
}

bool hadar::GameMain::process(void)
{
/* //??
	p_gfx_device->beginDraw();

	if (hadar::KeyBuffer::getKeyBuffer().isKeyPressed())
	{
		hadar::KeyBuffer::Key key = KeyBuffer::getKeyBuffer().getKey();
		bool bTimePassed = true;

		assert(key >= 0);

		// command ó��
		switch (key)
		{
		case KEY_Z:
			//hadar::sound::playFx(hadar::sound::SOUND_WALK);
			this->game_option.is_dubug_mode = !this->game_option.is_dubug_mode;
			break;
		case KEY_P:
			showPartyStatus();
			break;
		case KEY_V:
			showCharacterStatus();
			break;
		case KEY_Q:
			showQuickView();
			break;
		case KEY_C:
			castSpell();
			break;
		case KEY_E:
			useExtrasense();
			break;
		case KEY_R:
			restHere();
			break;
		case KEY_G:
			selectGameOption();
			break;
		case KEY_SYS_SELECT:
			// space ó��
			selectMainMenu();
			break;
		default:
			bTimePassed = false;
		}
		
		{
			// ȭ��ǥ Űó��
			int x1 = 0, y1 = 0;
			bool keyPressed = true;

			switch (key)
			{
				case KEY_DIR_UP:    y1 = -1; break;
				case KEY_DIR_DOWN:  y1 =  1; break;
				case KEY_DIR_LEFT:  x1 = -1; break;
				case KEY_DIR_RIGHT: x1 =  1; break;
				default:
					keyPressed = false;
			}

			if (keyPressed || bTimePassed)
			{
				unsigned char tile = map(party.x + x1, party.y + y1);

				(this->*map.act_list[tile])(x1, y1, true);

				hadar::sound::playFx(hadar::sound::SOUND_WALK);
			}

		}
	}

	sena::for_each(window.begin(), window.end(), FnDisplay<Window*>());

	p_gfx_device->endDraw();

	p_gfx_device->flip();

	return true;
*/
	return false;
}

void hadar::GameMain::m_actBlock(int x1, int y1, bool bUseless)
{
	party.face(x1, y1);

	return;
}

void hadar::GameMain::m_actMove(int x1, int y1, bool bEncounter)
{
/* //??
	party.move(x1, y1);

	window[WINDOWTYPE_MAP]->setUpdateFlag();

	int num_updated;

	num_updated = sena::for_each(player.begin(), player.end(), FnTimeGoes<PcPlayer*>()).Result();

	// player�� ���÷��� �� ��ġ�� ��ȭ�� ����ٸ�
	if (num_updated > 0)
	{
		window[WINDOWTYPE_STATUS]->setUpdateFlag();
	}

	detectGameOver();
	
	// ���ɼ��� ��� ���̶�� ��� �ð��� ���� ��Ŵ
	if (party.ability.mind_control > 0)
		--party.ability.mind_control;

	if (bEncounter)
	{
		if (smutil::random(party.encounter*20) == 0)
			encounterEnemy();
	}
*/
}

void hadar::GameMain::m_actEvent(int x1, int y1, bool bUseless)
{
/* //??
	party.move(x1, y1);

	window[WINDOWTYPE_MAP]->setUpdateFlag();

	LoreConsole::getConsole().clear();

	map_event::occur(map_event::TYPE_EVENT, 0, party.x, party.y);

	return;
*/
}

void hadar::GameMain::m_actEnter(int x1, int y1, bool bUseless)
{
	LoreConsole::getConsole().clear();

	map_event::occur(map_event::TYPE_ENTER, 0, party.x + x1, party.y + y1);
}

void hadar::GameMain::m_actSign(int x1, int y1, bool bUseless)
{
	party.face(x1, y1);

	LoreConsole& console = LoreConsole::getConsole();

	console.clear();
	console.setTextColorIndex(7);
	console.write("ǥ���ǿ��� ������ ���� ���� �־���.");
	console.write("");

	map_event::occur(map_event::TYPE_SIGN, 0, party.x + x1, party.y + y1);
}

void hadar::GameMain::m_actWater(int x1, int y1, bool bUseless)
{
/* //??
	if (party.ability.walk_on_water > 0)
	{
		--party.ability.walk_on_water;
		party.move(x1, y1);
		window[WINDOWTYPE_MAP]->setUpdateFlag();

		if (smutil::random(party.encounter*30) == 0)
			encounterEnemy();
	}
	else
	{
		party.face(x1, y1);
	}
*/
}

void hadar::GameMain::m_actSwamp(int x1, int y1, bool bUseless)
{
/* //??
	m_actMove(x1, y1, false);

	// walkOnSwamp�� ���� ó��
	if (party.ability.walk_on_swamp > 0)
	{
		--party.ability.walk_on_swamp;
	}
	else
	{
		LoreConsole& console = LoreConsole::getConsole();
		console.clear();
		console.setTextColorIndex(13);
		console.write("������ ���� �ִ� �˿� ���� !!!");
		console.write("");

		int num_updated;

		num_updated = sena::for_each(player.begin(), player.end(), FnEnterSwamp<PcPlayer*>()).Result();

		console.display();

		// player�� ���÷��� �� ��ġ�� ��ȭ�� ����ٸ�
		if (num_updated > 0)
		{
			window[WINDOWTYPE_STATUS]->setUpdateFlag();
		}
	}

	detectGameOver();
*/
}

void hadar::GameMain::m_actLava(int x1, int y1, bool bUseless)
{
/* //??
	m_actMove(x1, y1, false);

	{
		LoreConsole& console = LoreConsole::getConsole();
		console.clear();
		console.setTextColorIndex(12);
		console.write("������ �������� ���� !!!");
		console.write("");

		int num_updated;

		num_updated = sena::for_each(player.begin(), player.end(), FnEnterLava<PcPlayer*>()).Result();

		console.display();

		window[WINDOWTYPE_STATUS]->setUpdateFlag();
	}

	detectGameOver();

	return;
*/
}

void hadar::GameMain::m_actTalk(int x1, int y1, bool bUseless)
{
/* //??
	party.face(x1, y1);

	window[WINDOWTYPE_MAP]->setUpdateFlag();
	window[WINDOWTYPE_MAP]->display();
	window[WINDOWTYPE_SUBMAP]->display();

	LoreConsole::getConsole().clear();

	map_event::occur(map_event::TYPE_TALK, 0, party.x + x1, party.y + y1);

	return;
*/
}
