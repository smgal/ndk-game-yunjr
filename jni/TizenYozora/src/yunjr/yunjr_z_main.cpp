
/* Aspect ratio
 800 x 1280 Galaxy Note 1  (1.600)
 800 x 1232 Galaxy Note 10.1 (1.540)
 720 x 1280 Galaxy Note 2, Galaxy S3 (1.777)
1600 x 2560 Galaxy S Tab 10.5 (1.600)
*/

#include "yunjr_base.h"
#include "yunjr_base_gfx.h"
#include "yunjr_base_font.h"
#include "yunjr_base_text.h"
#include "yunjr_base_key_buffer.h"
#include "yunjr_base_serialized.h"

#include "yunjr_class.h"
#include "yunjr_class_console.h"
#include "yunjr_class_chara.h"
#include "yunjr_class_control_lv1.h"
#include "yunjr_class_control_lv2.h"
#include "yunjr_class_game_state.h"
#include "yunjr_class_map_event.h"
#include "yunjr_class_map.h"
#include "yunjr_class_select.h"
#include "yunjr_class_pc_party.h"
#include "yunjr_class_pc_player.h"
#include "yunjr_class_pc_functor.h"

#include "yunjr_res.h"
#include "yunjr_res_string.h"

#include "yunjr_util.h"

#include <new>
#include <vector>
#include <string.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// local

//??
const int MAX_PLAYER = 6;

////////////////////////////////////////////////////////////////////////////////
// type definition

////////////////////////////////////////////////////////////////////////////////
// local

namespace yunjr
{
	// for updating
	template <>
	void yunjr::Operator<ControlId, unsigned long>::operator()(ControlId obj)
	{
		(obj.second) ? obj.second->update(param) : (void)0;
	}

	// for rendering
	template <>
	void yunjr::Operator<ControlId, shared::FlatBoard32>::operator()(ControlId obj)
	{
		*param << param->getDefaultFillRect();
		*param << param->getDefaultBitBlt();

		obj.second->render(param);
	}
}

////////////////////////////////////////////////////////////////////////////////
// main

#define CONSOLE_WRITE_SS(s1, s2) { smutil::string s(s1); s += s2; console.write(s); }
#define CONSOLE_WRITE_SSSS(s1, s2, s3, s4) { smutil::string s(s1); s += s2; s += s3; s += s4; console.write(s); }

#define CONSOLE_WRITE(format, var) \
	smutil::composeString(sz_temp, format, var); \
	console.write(sz_temp);

#define CONSOLE_WRITE2(format, var1, var2) \
	smutil::composeString(sz_temp, format, var1, var2); \
	console.write(sz_temp);

#define CONSOLE_WRITE4(format, var1, var2, var3, var4) \
	smutil::composeString(sz_temp, format, var1, var2, var3, var4); \
	console.write(sz_temp);

#define NUM_OF_CONSCIOUS_PLAYER(player) sena::for_each(player.begin(), player.end(), FnNumOfConscious<yunjr::shared::PcPlayer>()).Result()
#define NUM_OF_CONSCIOUS_ENEMY(enemy) sena::for_each(enemy.begin(), enemy.end(), FnNumOfConscious<yunjr::shared::PcEnemy>()).Result()

namespace
{
	///////////////////////////////////////////////////////////////////////////////
	// GetSaveFileName class;

	char _SAVE_DATA_PATH[1024] = { 0, };
	const char* SAVE_FILE_NAME = "gamedat";
	const char* SAVE_FILE_EXT = ".sav";

	class GetSaveFileName
	{
	public:
		GetSaveFileName(int index)
		{
			m_name += _SAVE_DATA_PATH;
			m_name += "/";
			m_name += SAVE_FILE_NAME;
			m_name += smutil::IntToStr<char>(index)();
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
		smutil::string8 m_name;

	};
}

namespace yunjr
{
	// forward
	void proccessGameOver(yunjr::EXITCODE code);

	// local
	yunjr::ControlWindow* s_p_main_window = 0;

	void changeWindowForBattle(void)
	{
		((yunjr::ControlConsole*)yunjr::resource::getMainWindow()->findControl("MAP"))->hide();
		((yunjr::ControlConsole*)yunjr::resource::getMainWindow()->findControl("BATTLE"))->show();
	}

	void changeWindowForField(void)
	{
		((yunjr::ControlConsole*)yunjr::resource::getMainWindow()->findControl("BATTLE"))->hide();
		((yunjr::ControlConsole*)yunjr::resource::getMainWindow()->findControl("MAP"))->show();
	}

	bool isValidWarpPos(int x, int y)
	{
	/* //??
		// main map의 표현 크기를 기준으로 해야 함 
		return ((x >= WindowMap::_X_RADIUS) && (x < map.width-WindowMap::_X_RADIUS) && (y >= WindowMap::_Y_RADIUS) && (y < map.height-WindowMap::_Y_RADIUS));
	*/
		return false;
	}

	int selectPlayer(const wchar_t* sz_title = 0)
	{
		if (sz_title == NULL)
			sz_title = L"한명을 고르시오 ---";

		yunjr::MenuList menu;

		menu.reserve(MAX_PLAYER+1);

		menu.push_back(sz_title);

		int real_player[MAX_PLAYER];
		int n_real = 0;
		int count = 0;

		sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

		for (sena::vector<yunjr::shared::PcPlayer>::iterator obj = player_list.begin(); obj != player_list.end(); ++obj, ++count)
		{
			assert(n_real < MAX_PLAYER);

			if ((*obj)->isValid())
			{
				menu.push_back((*obj)->getName());
				real_player[n_real++] = count;
			}
		}

		assert(n_real > 0);

		int selected = yunjr::MenuSelection(menu)();

		--selected;

		return (selected >= 0) ? real_player[selected] : -1;
	}

	int selectEnemy(void)
	{
		yunjr::ControlBattle* p_battle = (yunjr::ControlBattle*)yunjr::resource::getMainWindow()->findControl("BATTLE");
		sena::vector<yunjr::shared::PcEnemy>& enemy_list = yunjr::game::object::getEnemyList();

		int num_enabled = enemy_list.size();
		int selected = 0;
		
		do
		{
			p_battle->display(true, selected);

			bool bUpdate = false;

			do
			{
				unsigned short key;
				while ((key = yunjr::KeyBuffer::getKeyBuffer().getKey()) < 0)
					;

				switch (key)
				{
				case yunjr::KEY_DIR_UP:
				case yunjr::KEY_DIR_DOWN:
					{
						int dy = (key == yunjr::KEY_DIR_UP) ? -1 : +1;
						selected += dy;

						if (selected < 0)
							selected = num_enabled - 1;
						if (selected >= num_enabled)
							selected = 0;

						bUpdate = true;
					}
					break;
	// 무효 숫자를 돌려주지 않게 함
	//			case KEY_BTN_B:
	//				selected = -1;
	//				// pass through
	//
				case yunjr::KEY_BTN_A:
					p_battle->display(true);
					return selected;
				}
			} while (!bUpdate);
			
		} while (1);
	}

	void detectGameOver(void)
	{
		sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

		// 전처리
		sena::for_each(player_list.begin(), player_list.end(), FnCheckCondition<yunjr::shared::PcPlayer>());

		// 실제 생존해 있는 숫자를 확인
		int num_alive = NUM_OF_CONSCIOUS_PLAYER(player_list);

		if (num_alive == 0)
			proccessGameOver(yunjr::EXITCODE_BY_ACCIDENT);
	}

	bool checkEndOfBattle(yunjr::ENDBATTLE& exitCode)
	{
		sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

		int num_alive = NUM_OF_CONSCIOUS_PLAYER(player_list);

		if (num_alive == 0)
		{
			exitCode = yunjr::ENDBATTLE_LOSE;
			return true;
		}

		sena::vector<yunjr::shared::PcEnemy>& enemy_list = yunjr::game::object::getEnemyList();

		num_alive = NUM_OF_CONSCIOUS_ENEMY(enemy_list);

		if (num_alive == 0)
		{
			exitCode = yunjr::ENDBATTLE_WIN;
			return true;
		}

		exitCode = yunjr::ENDBATTLE_NONE;

		return false;
	}

	void plusGold(void)
	{
		yunjr::PcParty& party = yunjr::game::object::getParty();
		sena::vector<yunjr::shared::PcEnemy>& enemy_list = yunjr::game::object::getEnemyList();

		int num_gold = sena::for_each(enemy_list.begin(), enemy_list.end(), FnPlusGold<yunjr::shared::PcEnemy>()).Result();
		
		party.gold += num_gold;

		yunjr::game::console::writeConsole(15, 3, L"일행은 ", smutil::IntToStr<wchar_t>(num_gold)(), L"개의 금을 얻었다.");
	}

	int registerEnemy(int index)
	{
		sena::vector<yunjr::shared::PcEnemy>& enemy_list = yunjr::game::object::getEnemyList();

		yunjr::shared::PcEnemy enemy(new yunjr::PcEnemy);

		*enemy << yunjr::getEnemyDataFromEnemyTable(index);
		enemy->ed_number = index;

		enemy_list.push_back(enemy);

		return enemy_list.size() - 1;
	}

	// 정상적인 승부로 끝이나면 true, 도망을 치면 false
	yunjr::BATTLERESULT runBattleMode(bool is_assualt_mode)
	{
		yunjr::ControlBattle* p_battle = (yunjr::ControlBattle*)yunjr::resource::getMainWindow()->findControl("BATTLE");
		sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();
		sena::vector<yunjr::shared::PcEnemy>& enemy_list = yunjr::game::object::getEnemyList();

		yunjr::BATTLERESULT result = yunjr::BATTLERESULT_WIN;

		yunjr::GameState::getMutableInstance().setGameState(yunjr::GAMESTATE_BATTLE);

		yunjr::ENDBATTLE exitCode = yunjr::ENDBATTLE_NONE;
		int battle[MAX_PLAYER][4];

		do
		{
			if (!is_assualt_mode)
				goto QAZ;

			sena::memclear(battle, sizeof(battle));

			p_battle->display(false);

			// 명령 등록
			{
				bool bAutoBattle = false;

				sena::vector<yunjr::shared::PcPlayer>::iterator obj = player_list.begin();

				for ( ; obj != player_list.end(); ++obj)
				{
					int order = (*obj)->order;

					if ((*obj)->isConscious())
					{
						if (!bAutoBattle)
						{
							yunjr::MenuList menu;

							smutil::string str1;
							str1 += (*obj)->getName();
							str1 += L"의 전투 모드 ===>";

							smutil::string str2;
							str2 += L"한 명의 적을 ";
							str2 += yunjr::resource::getWeaponName((*obj)->weapon).sz_name;
							str2 += yunjr::resource::getWeaponName((*obj)->weapon).sz_josa_with;
							str2 += L"로 공격";

							menu.reserve(8);
							menu.push_back(str1);
							menu.push_back(str2);
							menu.push_back(L"한 명의 적에게 마법 공격");
							menu.push_back(L"모든 적에게 마법 공격");
							menu.push_back(L"적에게 특수 마법 공격");
							menu.push_back(L"일행을 치료");
							menu.push_back(L"적에게 초능력 사용");

							if (order == 0)
								menu.push_back(L"일행에게 무조건 공격 할 것을 지시");
							else
								menu.push_back(L"도망을 시도함");

							int selected = yunjr::MenuSelection(menu)();

							if (selected != 1)
								yunjr::LoreConsole::getConsole().clear();

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

						// 메뉴 선택에 따른 동작 방식 결정
						switch (battle[order][1])
						{
						case 1:
							{
								int selected = selectEnemy();
								yunjr::LoreConsole::getConsole().clear();
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
									assert(false);
								}

								yunjr::MenuList menu;

								menu.reserve(8);
								menu.push_back(L"");
								menu.push_back(L"없음");
								for (int i = 0; i < 6; i++)
									menu.push_back(yunjr::resource::getMagicName(i+1+ixMagicOffset).sz_name);

								int selected = yunjr::MenuSelection(menu, num_enabled)();
								int ix_enemy  = -1;

								if ((selected == 0) || (selected == 1))
									battle[order][1] = 0;
								else if (isSingleAttack)
									ix_enemy = selectEnemy();

								yunjr::LoreConsole::getConsole().clear();

								battle[order][2] = selected - 1;
								battle[order][3] = ix_enemy;
							}
							break;
						case 5:
							player_list[order]->castCureSpell();
							break;
						case 6:
							{
								yunjr::MenuList menu;

								menu.reserve(8);
								menu.push_back(L"");
								for (int i = 0; i < 5; i++)
									menu.push_back(yunjr::resource::getMagicName(i+1+40).sz_name);

								int selected = yunjr::MenuSelection(menu)();
								int ix_enemy  = -1;

								if (selected == 0)
									battle[order][1] = 0;
								else
									ix_enemy = selectEnemy();

								yunjr::LoreConsole::getConsole().clear();

								battle[order][2] = selected;
								battle[order][3] = ix_enemy;

								if ((ix_enemy < 0) || (enemy_list[ix_enemy]->unconscious > 0) || (enemy_list[ix_enemy]->dead > 0))
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
									for (int ix_enemy = 1; ix_enemy < int(enemy_list.size()); ++ix_enemy)
									{ // @@ dead 맞나?
										if ((enemy_list[ix_enemy]->unconscious == 0) && (enemy_list[ix_enemy]->dead))
											target = ix_enemy;
									}
									battle[order][3] = target;
								}

								break;
							default:
								assert(false);
							}
							break;
						}
					}
				}
			}

			yunjr::LoreConsole::getConsole().clear();

			// 명령 해석
			{
				sena::vector<yunjr::shared::PcPlayer>::iterator obj = player_list.begin();

				for ( ; obj != player_list.end(); ++obj)
				{
					if ((*obj)->isConscious())
					{
						int order = (*obj)->order;

						static smutil::SmSet s_printMessage("0,4,6..8");

						if (s_printMessage.isSet(battle[order][1]))
						{
							smutil::string s = getBattleMessage(**obj, battle[order][1], battle[order][2], *enemy_list[battle[order][3]]);
							yunjr::LoreConsole::getConsole().setTextColorIndex(15);
							yunjr::LoreConsole::getConsole().write(s);
						}

						switch (battle[order][1])
						{
							case 1:
								player_list[order]->attackWithWeapon(battle[order][2], battle[order][3]);
								break;
							case 2:
								player_list[order]->castSpellToOne(battle[order][2], battle[order][3]);
								break;
							case 3:
								player_list[order]->castSpellToAll(battle[order][2]);
								break;
							case 4:
								player_list[order]->castSpellWithSpecialAbility(battle[order][2], battle[order][3]);
								break;
							case 6:
								player_list[order]->useESPForBattle(battle[order][2], battle[order][3]);
								break;
							case 7:
								{
									if ((*obj)->tryToRunAway())
									{
										yunjr::game::pressAnyKey();

										yunjr::LoreConsole::getConsole().clear();

										exitCode = yunjr::ENDBATTLE_RUN_AWAY;

										goto END_OF_BATTLE;
									}
								}
						}
						
						yunjr::LoreConsole::getConsole().write(L"");
					}
				}
				
				p_battle->display(true);

				yunjr::LoreConsole::getConsole().write(L"");
				yunjr::LoreConsole::getConsole().display();
				yunjr::game::updateScreen();
				yunjr::game::pressAnyKey();
			}
QAZ:
			yunjr::LoreConsole::getConsole().clear();

			{
				sena::vector<yunjr::shared::PcEnemy>::iterator obj = enemy_list.begin();

				for ( ; obj != enemy_list.end(); ++obj)
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

			INVALIDATE_STATUS;

			yunjr::game::updateScreen();

			if (checkEndOfBattle(exitCode))
				break;

			is_assualt_mode = true;

			// 원작에서는 적 공격 후에 Press Any Key 문자 출력이 없었음
			yunjr::game::pressAnyKey();

		} while (1);

END_OF_BATTLE:
		switch (exitCode)
		{
		case yunjr::ENDBATTLE_LOSE:
			proccessGameOver(yunjr::EXITCODE_BY_ENEMY);
			return yunjr::BATTLERESULT_LOSE;
		case yunjr::ENDBATTLE_WIN:
			yunjr::LoreConsole::getConsole().clear();
			plusGold();
			break;
		case yunjr::ENDBATTLE_RUN_AWAY:
			result = yunjr::BATTLERESULT_EVADE;
			break;
		default:
			assert(false);
		}

		INVALIDATE_MAP;
		INVALIDATE_SUB_MAP;

		yunjr::GameState::getMutableInstance().setGameState(yunjr::GAMESTATE_MAP);

		return result;
	}

	void encounterEnemy(void)
	{
		yunjr::Map& map = yunjr::game::object::getMap();

		if (map.getEncounteredEnemy() == 0)
			return;

		yunjr::PcParty& party = yunjr::game::object::getParty();
		sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();
		sena::vector<yunjr::shared::PcEnemy>& enemy_list = yunjr::game::object::getEnemyList();

		enemy_list.clear();

		int num_enemy = (smutil::random(party.max_enemy) + 1);

		int enemyAgility = 0;
		for (int i = 0; i < num_enemy; i++)
		{
			int ix_enemy = map.getEncounteredEnemy();
			assert(ix_enemy > 0);

			int index = registerEnemy(ix_enemy);
			enemyAgility += enemy_list[index]->agility;
		}

		assert(num_enemy > 0);
		enemyAgility /= num_enemy;

		yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

		console.clear();

		console.setTextColorIndex(12);
		console.write(L"적이 출현했다 !!!");
		console.write(L"");

		console.setTextColorIndex(11);
		console.write(L"적의 평균 민첩성");
		{
			wchar_t sz_temp[256];
			int avgAgility = sena::for_each(enemy_list.begin(), enemy_list.end(), FnAverageAgility<yunjr::shared::PcEnemy>()).Result();
			CONSOLE_WRITE(L" : @", avgAgility);
		}

		console.display();

		changeWindowForBattle();

		yunjr::game::updateScreen();

		yunjr::ControlConsole* p_console = (yunjr::ControlConsole*)yunjr::resource::getMainWindow()->findControl("CONSOLE");

		int saved_x1, saved_y1, saved_x2, saved_y2;
		int saved_left, saved_top, saved_right, saved_bottom;

		// 위의 글자를 출력하기 위해서 임시로 console 창의 사이즈를 줄인다.
		{
			p_console->getRegion(saved_x1, saved_y1, saved_x2, saved_y2);
			p_console->getMargin(saved_left, saved_top, saved_right, saved_bottom);

			int reduced_size = 4 * yunjr::DEFAULT_FONT_BTBD + saved_top;

			p_console->setRegion(saved_x1, saved_y1 + reduced_size, saved_x2, saved_y2);
			p_console->setMargin(saved_left, 0, saved_right, saved_bottom);
		}

		yunjr::MenuList menu;

		menu.reserve(3);
		menu.push_back(L"");
		menu.push_back(L"적과 교전한다");
		menu.push_back(L"도망간다");

		bool willing_to_avoid_battle = (yunjr::MenuSelection(menu)() != 1);

		// console 창의 사이즈를 복귀시킨다.
		{
			p_console->setMargin(saved_left, saved_top, saved_right, saved_bottom);
			p_console->setRegion(saved_x1, saved_y1, saved_x2, saved_y2);
		}

		// '도망간다'를 선택했을 때 전 영역을 bg color로 채우기 위함
		console.clear();
		console.display();

		do
		{
			bool is_assualt_mode;

			if (willing_to_avoid_battle)
			{
				int avgLuck    = sena::for_each(player_list.begin(), player_list.end(), FnAverageLuck<yunjr::shared::PcPlayer>()).Result();
				int avgAgility = sena::for_each(enemy_list.begin(), enemy_list.end(), FnAverageAgility<yunjr::shared::PcEnemy>()).Result();

				if (avgLuck > avgAgility)
					break; // 전투를 회피

				is_assualt_mode = false;
			}
			else
			{
				int avgAgility1 = sena::for_each(player_list.begin(), player_list.end(), FnAverageAgility<yunjr::shared::PcPlayer>()).Result();
				int avgAgility2 = sena::for_each(enemy_list.begin(), enemy_list.end(), FnAverageAgility<yunjr::shared::PcEnemy>()).Result();

				is_assualt_mode = (avgAgility1 > avgAgility2);
			}

			runBattleMode(is_assualt_mode);
		} while(0);

		changeWindowForField();
	}

	void showPartyStatus()
	{
		wchar_t sz_temp[256];

		yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();
		yunjr::PcParty& party = yunjr::game::object::getParty();

		console.clear();
		console.setTextColorIndex(7);

		CONSOLE_WRITE(L"X 축 = @", party.x);
		CONSOLE_WRITE(L"Y 축 = @", party.y);
		CONSOLE_WRITE(L"남은 식량 = @", party.food);
		CONSOLE_WRITE(L"남은 황금 = @", party.gold);

		console.write(L"");

		CONSOLE_WRITE(L"마법의 횃불 : @", party.ability.magic_torch);
		CONSOLE_WRITE(L"공중 부상   : @", party.ability.levitation);
		CONSOLE_WRITE(L"물위를 걸음 : @", party.ability.walk_on_water);
		CONSOLE_WRITE(L"늪위를 걸음 : @", party.ability.walk_on_swamp);

		console.display();
	}

	void showCharacterStatus(const yunjr::PcPlayer& selected)
	{
		assert(selected.isValid());

		if (!selected.isValid())
			return;

		wchar_t sz_temp[256];

		yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

		console.clear();

		console.setTextColorIndex(11);

		CONSOLE_WRITE_SS(L"# 이름 : ", selected.getName())
		CONSOLE_WRITE_SS(L"# 성별 : ", selected.getGenderName())
		CONSOLE_WRITE_SS(L"# 계급 : ", selected.getClassName())

		console.write(L"");

		console.setTextColorIndex(3);
		CONSOLE_WRITE(L"체력   : @", selected.strength)
		CONSOLE_WRITE(L"정신력 : @", selected.mentality)
		CONSOLE_WRITE(L"집중력 : @", selected.concentration)
		CONSOLE_WRITE(L"인내력 : @", selected.endurance)
		CONSOLE_WRITE(L"저항력 : @", selected.resistance)
		CONSOLE_WRITE(L"민첩성 : @", selected.agility)
		CONSOLE_WRITE(L"행운   : @", selected.luck)

		console.display();
		yunjr::game::updateScreen();

		yunjr::game::pressAnyKey();

		console.clear();

		// 이 부분은 한 번 더 반복
		console.setTextColorIndex(11);
		CONSOLE_WRITE_SS(L"# 이름 : ", selected.getName())
		CONSOLE_WRITE_SS(L"# 성별 : ", selected.getGenderName())
		CONSOLE_WRITE_SS(L"# 계급 : ", selected.getClassName())

		console.write(L"");

		console.setTextColorIndex(3);
		CONSOLE_WRITE2(L"무기의 정확성   : @    전투 레벨   : @", selected.accuracy[0], selected.level[0])
		CONSOLE_WRITE2(L"정신력의 정확성 : @    마법 레벨   : @", selected.accuracy[1], selected.level[1])
		CONSOLE_WRITE2(L"초감각의 정확성 : @    초감각 레벨 : @", selected.accuracy[2], selected.level[2])
		CONSOLE_WRITE(L"## 경험치   : @", selected.experience)

		console.write(L"");

		console.setTextColorIndex(2);
		CONSOLE_WRITE_SS(L"사용 무기 - ", selected.getWeaponName())
		CONSOLE_WRITE_SSSS(L"방패 - ", selected.getShieldName(), L"           갑옷 - ", selected.getArmorName())

		console.display();
	}

	void showCharacterStatus(void)
	{
		int selected = selectPlayer(L"능력을 보고싶은 인물을 선택하시오");
		if (selected >= 0)
		{
			assert(selected < MAX_PLAYER);

			sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

			showCharacterStatus(*player_list[selected]);
		}
	}

	void showQuickView(void)
	{
		yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

		console.clear();

		console.setTextColorIndex(15);
		console.write(L"                이름    중독  의식불명    죽음");
		console.write(L"");

		console.setTextColorIndex(7);

		sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

		sena::vector<yunjr::shared::PcPlayer>::iterator obj = player_list.begin();

		for ( ; obj != player_list.end(); ++obj)
		{
			if ((*obj)->isValid())
			{
				wchar_t sz_temp[256];

				smutil::string s((*obj)->getName());
				smutil::composeString(sz_temp, L"        @   @   @", (*obj)->poison, (*obj)->unconscious, (*obj)->dead);
				s += sz_temp;

				console.write(s);
			}
		}

		console.display();
	}

	void castSpell(void)
	{
		int selected = selectPlayer();

		if (selected < 0)
			return;

		sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

		if (!player_list[selected]->isConscious())
		{
			yunjr::game::console::writeConsole(7, 2, player_list[selected]->get3rdPersonName(), L"는 마법을 사용할 수 있는 상태가 아닙니다.");
			return;
		}

		yunjr::MenuList menu;

		menu.reserve(4);
		menu.push_back(L"사용할 마법의 종류 ===>");
		menu.push_back(L"공격 마법");
		menu.push_back(L"치료 마법");
		menu.push_back(L"변화 마법");

		switch (yunjr::MenuSelection(menu)())
		{
		case 1:
			player_list[selected]->castAttackSpell();
			break;
		case 2:
			player_list[selected]->castCureSpell();
			break;
		case 3:
			player_list[selected]->castPhenominaSpell();
			break;
		default:
			break;
		}
	}

	void useExtrasense(void)
	{
		yunjr::LoreConsole::getConsole().clear();

		int selected = selectPlayer();
		if (selected < 0)
			return;

		sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

		if (!player_list[selected]->isConscious())
		{
			yunjr::game::console::writeConsole(7, 2, player_list[selected]->get3rdPersonName(), L"는 초감각을 사용할수있는 상태가 아닙니다.");
			return;
		}

		player_list[selected]->useESP();
	}

	void restHere(void)
	{
		yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

		console.clear();

		yunjr::PcParty& party = yunjr::game::object::getParty();
		sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

		sena::for_each(player_list.begin(), player_list.end(), FnRestHere<yunjr::shared::PcPlayer>());

		if (party.ability.magic_torch > 0)
			--party.ability.magic_torch;

		party.ability.levitation  = 0;
		party.ability.walk_on_water = 0;
		party.ability.walk_on_swamp = 0;
		party.ability.mind_control = 0;

		console.display();

		INVALIDATE_STATUS;

		yunjr::game::updateScreen();
		//@@ 필요한가?
		yunjr::game::pressAnyKey();
	}

	bool loadGame(int index)
	{
		yunjr::SerializedStream stream(GetSaveFileName(index), yunjr::SerializedStream::STREAMTYPE_READ);

		sena::vector<yunjr::Serialized*>& save_list = yunjr::game::object::getSaveList();

		for (sena::vector<yunjr::Serialized*>::iterator obj = save_list.begin(); obj != save_list.end(); ++obj)
		{
			stream >> *(*obj);
		}
/*??
		map_event::registerEventFileName(game_option.script_file);
*/
		INVALIDATE_MAP;
		INVALIDATE_SUB_MAP;
		INVALIDATE_STATUS;

		return true;
	}

	bool saveGame(int index)
	{
		yunjr::SerializedStream stream(GetSaveFileName(index), yunjr::SerializedStream::STREAMTYPE_WRITE);

		sena::vector<yunjr::Serialized*>& save_list = yunjr::game::object::getSaveList();

		for (sena::vector<yunjr::Serialized*>::iterator obj = save_list.begin(); obj != save_list.end(); ++obj)
		{
			stream << *(*obj);
		}

		return true;
	}

	bool selectLoadMenu(void)
	{
		yunjr::MenuList menu;

		menu.push_back(L"불러 내고 싶은 게임을 선택하십시오.");
		menu.push_back(L"없습니다");
		menu.push_back(L"본 게임 데이타");
		menu.push_back(L"게임 데이타 1 (부)");
		menu.push_back(L"게임 데이타 2 (부)");
		menu.push_back(L"게임 데이타 3 (부)");

		int selected = yunjr::MenuSelection(menu)();

		if (selected <= 1)
			return false;

		{
			yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

			console.clear();
			console.setTextColorIndex(11);
			console.write(L"저장했던 게임을 다시 불러옵니다");
			console.display();

			// 0~3 사의 값
			if (!loadGame(selected-2))
			{
				// 파일이 존재 하지 않음
				//?? 관련된 메시지를 보내고 다시 메뉴를 선택하게 해야 함
				//?? 또는 자동으로 데이터가 있는지를 확인하여 메뉴를 하이라이트를 시켜야 함
				return false;
			}

			INVALIDATE_MAP;
			INVALIDATE_SUB_MAP;
			INVALIDATE_STATUS;

			console.clear();
			console.display();
		}

		return true;
	}

	bool selectSaveMenu(void)
	{
		yunjr::MenuList menu;

		menu.push_back(L"게임의 저장 장소를 선택하십시오.");
		menu.push_back(L"없습니다");
		menu.push_back(L"본 게임 데이타");
		menu.push_back(L"게임 데이타 1 (부)");
		menu.push_back(L"게임 데이타 2 (부)");
		menu.push_back(L"게임 데이타 3 (부)");

		int selected = yunjr::MenuSelection(menu)();

		if (selected <= 1)
			return false;

		{
			yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

			console.clear();
			console.setTextColorIndex(12);
			console.write(L"현재의 게임을 저장합니다");
			console.display();

			// 0~3 사의 값
			if (!saveGame(selected-2))
			{
				//?? 실패 했음을 알려 줘야 하나?
				return false;
			}

			console.setTextColorIndex(7);
			console.write(L"");
			console.write(L"성공했습니다");
			console.display();

			yunjr::game::pressAnyKey();
		}

		return true;
	}

	void proccessGameOver(yunjr::EXITCODE code)
	{
		switch (code)
		{
		case yunjr::EXITCODE_BY_USER:
			{
				yunjr::MenuList menu;

				menu.push_back(L"정말로 끝내겠습니까 ?"); // 원래는 10번 색
				menu.push_back(L"       << 아니오 >>");
				menu.push_back(L"       <<   예   >>");

				if (yunjr::MenuSelection(menu)() != 2)
					return;
			}
			break;
		case yunjr::EXITCODE_BY_ACCIDENT:
			{
				yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

				console.clear();
				console.setTextColorIndex(13);
				console.write(L"일행은 모험중에 모두 목숨을 잃었다.");
				console.display();

				INVALIDATE_MAP;
				INVALIDATE_SUB_MAP;
				INVALIDATE_STATUS;

				yunjr::game::updateScreen();

				yunjr::game::pressAnyKey();

				if (selectLoadMenu())
					return;
			}
			break;
		case yunjr::EXITCODE_BY_ENEMY:
			{
				yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

				console.clear();
				console.setTextColorIndex(13);
				console.write(L"일행은 모두 전투에서 패했다 !!");
				console.display();

				yunjr::game::pressAnyKey();

				console.clear();

				yunjr::MenuList menu;
				menu.reserve(8);
				menu.push_back(L"    @A어떻게 하시겠습니까 ?@@");
				menu.push_back(L"   이전의 게임을 재개한다");
				menu.push_back(L"       게임을 끝낸다");

				int selected = yunjr::MenuSelection(menu)();

				if (selected == 1)
				{
					if (selectLoadMenu())
						return;
				}
			}
			break;
		case yunjr::EXITCODE_BY_FORCE:
			// 특별하게 취해야할 동작이 없음
			break;
		default:
			assert(false);
		}

		yunjr::GameState::getMutableInstance().terminate();
	}

	void selectGameOption(void)
	{
		yunjr::PcParty& party = yunjr::game::object::getParty();

		yunjr::MenuList menu;

		menu.reserve(7);
		menu.push_back(L"게임 선택 상황");
		menu.push_back(L"난이도 조절");
		menu.push_back(L"정식 일행의 순서 정렬");
		menu.push_back(L"일행에서 제외 시킴");
		menu.push_back(L"이전의 게임을 재개");
		menu.push_back(L"현재의 게임을 저장");
		menu.push_back(L"게임을 마침");

		int selected = yunjr::MenuSelection(menu)();

		switch (selected)
		{
		case 0:
			break;
		case 1:
			// 전투시 출현하는 적의 최대치 기입
			{
				menu.clear();
				menu.push_back(L"한번에 출현하는 적들의 최대치를 기입하십시오");
				menu.push_back(L"3명의 적들");
				menu.push_back(L"4명의 적들");
				menu.push_back(L"5명의 적들");
				menu.push_back(L"6명의 적들");
				menu.push_back(L"7명의 적들");

				int selected = yunjr::MenuSelection(menu, -1, party.max_enemy - 2)();

				if (selected == 0)
					break;

				party.max_enemy = selected + 2;
			}

			// 전투시 출현하는 적의 최대치 기입
			{
				menu.clear();
				menu.push_back(L"일행들의 지금 성격은 어떻습니까 ?");
				menu.push_back(L"일부러 전투를 피하고 싶다");
				menu.push_back(L"너무 잦은 전투는 원하지 않는다");
				menu.push_back(L"마주친 적과는 전투를 하겠다");
				menu.push_back(L"보이는 적들과는 모두 전투하겠다");
				menu.push_back(L"그들은 피에 굶주려 있다");

				int selected = yunjr::MenuSelection(menu, -1, 6 - party.encounter)();

				if (selected == 0)
					break;

				party.encounter = 6 - selected;
			}
			break;
/*??
		case 2:
			{
				int  ix_player;
				int  indexTable[6] = {0, };
				int* pIndex = &indexTable[1];

				yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

				console.clear();

				console.setTextColorIndex(12);
				console.write(L"현재의 일원의 전투 순서를 정렬 하십시오.");
				console.write(L"");
				console.display();

				menu.clear();
				menu.push_back(L"@B순서를 바꿀 일원 1@@");

				for (ix_player = 1; ix_player < 5; ix_player++)
				{
					if (player[ix_player]->isValid())
					{
						menu.push_back(player[ix_player]->getName());
						*pIndex++ = ix_player;
					}
				}

				// 주인공을 제외한 일행이 1명 이하라면 이 기능을 수행할 필요가 없다.
				if (menu.size() < 3)
				{
					LoreConsole& console = LoreConsole::getConsole();

					console.clear();
					console.setTextColorIndex(7);
					console.write("전투 순서를 정렬 할 필요가 없습니다.");
					console.display();

					game::pressAnyKey();

					break;
				}

				yunjr::ControlConsole* p_console = (yunjr::ControlConsole*)yunjr::resource::getMainWindow()->findControl("CONSOLE");

				int saved_x1, saved_y1, saved_x2, saved_y2;
				int saved_left, saved_top, saved_right, saved_bottom;

				// 위의 글자를 출력하기 위해서 임시로 console 창의 사이즈를 줄인다.
				{
					p_console->getRegion(saved_x1, saved_y1, saved_x2, saved_y2);
					p_console->getMargin(saved_left, saved_top, saved_right, saved_bottom);

					int reduced_size = 3 * yunjr::DEFAULT_FONT_BTBD + saved_top;

					p_console->setRegion(saved_x1, saved_y1 + reduced_size, saved_x2, saved_y2);
					p_console->setMargin(saved_left, 0, saved_right, saved_bottom);
				}

				do
				{
					int selected1 = MenuSelection(menu)();
					if (selected1 == 0)
						break;

					menu[0] = "@B순서를 바꿀 일원 2@@";

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

						// 실제 등록 순서를 바꾸기
						sena::swap(player[ixPlayer1], player[ixPlayer2]);
						// order는 등록 번호이므로 이전과 같아야 한다
						sena::swap(player[ixPlayer1]->order, player[ixPlayer2]->order);
					}

					window[WINDOWTYPE_STATUS]->setUpdateFlag();
					window[WINDOWTYPE_STATUS]->display();
				} while(0);

				// console 창의 사이즈를 복귀시킨다.
				{
					p_console->setMargin(saved_left, saved_top, saved_right, saved_bottom);
					p_console->setRegion(saved_x1, saved_y1, saved_x2, saved_y2);
				}

				console.clear();
			}
			break;
		case 3:
			{
				int  ix_player;
				int  indexTable[6] = {0, };
				int* pIndex = &indexTable[1];

				menu.clear();
				menu.push_back("@C일행에서 제외 시키고 싶은 사람을 고르십시오.@@");

				for (ix_player = 1; ix_player < 6; ix_player++)
				{
					if (player[ix_player]->isValid())
					{
						menu.push_back(player[ix_player]->getName());
						*pIndex++ = ix_player;
					}
				}

				// 주인공뿐이라면 이 기능을 수행할 필요가 없다.
				if (menu.size() < 2)
				{
					LoreConsole& console = LoreConsole::getConsole();

					console.clear();
					console.setTextColorIndex(7);
					console.write("제외시킬 일행이 없습니다.");
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
*/
		case 4:
			selectLoadMenu();
			break;
		case 5:
			selectSaveMenu();
			break;
		case 6:
			proccessGameOver(yunjr::EXITCODE_BY_USER);
			break;
		}
	}

	void selectMainMenu(void)
	{
		yunjr::MenuList menu;

		menu.reserve(8);
		menu.push_back(L"당신의 명령을 고르시오 ===>");
		menu.push_back(L"일행의 상황을 본다");
		menu.push_back(L"개인의 상황을 본다");
		menu.push_back(L"일행의 건강 상태를 본다");
		menu.push_back(L"마법을 사용한다");
		menu.push_back(L"초능력을 사용한다");
		menu.push_back(L"여기서 쉰다");
		menu.push_back(L"게임 선택 상황");

		int selected = yunjr::MenuSelection(menu)();

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
			break;
		}
	}

	struct MapCallback
	{
		static void actBlock(int x1, int y1, bool bUseless)
		{
			yunjr::PcParty& party = yunjr::game::object::getParty();

			party.face(x1, y1);
		}
		static void actMove(int x1, int y1, bool bEncounter)
		{
			yunjr::PcParty& party = yunjr::game::object::getParty();

			party.move(x1, y1);

			INVALIDATE_MAP;

			int num_updated;

			sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

			num_updated = sena::for_each(player_list.begin(), player_list.end(), FnTimeGoes<yunjr::shared::PcPlayer>()).Result();

			// player의 디스플레이 된 수치에 변화가 생겼다면
			if (num_updated > 0)
			{
				INVALIDATE_STATUS;
			}

			detectGameOver();
			
			// 독심술을 사용 중이라면 사용 시간을 감소 시킴
			if (party.ability.mind_control > 0)
				--party.ability.mind_control;

			if (bEncounter)
			{
				//?? 이 이벤트는 tile 출발 시 발생하고 있으나 실제로는 타일 도착 시 발생해야 하는 이벤트이다.
				if (smutil::random(party.encounter*20) == 0)
					encounterEnemy();
			}
		}
		static void actEvent(int x1, int y1, bool bUseless)
		{
			yunjr::PcParty& party = yunjr::game::object::getParty();

			party.move(x1, y1);

			INVALIDATE_MAP;

			yunjr::LoreConsole::getConsole().clear();

			yunjr::map_event::occur(yunjr::map_event::TYPE_EVENT, 0, party.x, party.y);
		}
		static void actEnter(int x1, int y1, bool bUseless)
		{
			yunjr::PcParty& party = yunjr::game::object::getParty();

			yunjr::LoreConsole::getConsole().clear();

			yunjr::map_event::occur(yunjr::map_event::TYPE_ENTER, 0, party.x + x1, party.y + y1);
		}
		static void actSign(int x1, int y1, bool bUseless)
		{
			yunjr::PcParty& party = yunjr::game::object::getParty();

			party.face(x1, y1);

			yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

			console.clear();
			console.setTextColorIndex(7);
			console.write(L"표지판에는 다음과 같이 적혀 있었다.");
			console.write(L"");

			yunjr::map_event::occur(yunjr::map_event::TYPE_SIGN, 0, party.x + x1, party.y + y1);
		}
		static void actWater(int x1, int y1, bool bUseless)
		{
			yunjr::PcParty& party = yunjr::game::object::getParty();

			if (party.ability.walk_on_water > 0)
			{
				--party.ability.walk_on_water;
				party.move(x1, y1);

				INVALIDATE_MAP;

				if (smutil::random(party.encounter*30) == 0)
					encounterEnemy();
			}
			else
			{
				party.face(x1, y1);
			}
		}
		static void actSwamp(int x1, int y1, bool bUseless)
		{
			yunjr::PcParty& party = yunjr::game::object::getParty();

			actMove(x1, y1, false);

			// walkOnSwamp에 대한 처리
			if (party.ability.walk_on_swamp > 0)
			{
				--party.ability.walk_on_swamp;
			}
			else
			{
				yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();

				console.clear();
				console.setTextColorIndex(13);
				console.write(L"일행은 독이 있는 늪에 들어갔다 !!!");
				console.write(L"");

				sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

				int num_updated = sena::for_each(player_list.begin(), player_list.end(), FnEnterSwamp<yunjr::shared::PcPlayer>()).Result();

				console.display();

				// player의 디스플레이 된 수치에 변화가 생겼다면
				if (num_updated > 0)
					INVALIDATE_STATUS;
			}

			detectGameOver();
		}

		static void actLava(int x1, int y1, bool bUseless)
		{
			yunjr::PcParty& party = yunjr::game::object::getParty();

			actMove(x1, y1, false);

			{
				yunjr::LoreConsole& console = yunjr::LoreConsole::getConsole();
				console.clear();
				console.setTextColorIndex(12);
				console.write(L"일행은 용암지대로 들어섰다 !!!");
				console.write(L"");

				sena::vector<yunjr::shared::PcPlayer>& player_list = yunjr::game::object::getPlayerList();

				int num_updated = sena::for_each(player_list.begin(), player_list.end(), FnEnterLava<yunjr::shared::PcPlayer>()).Result();

				console.display();

				INVALIDATE_STATUS;
			}

			detectGameOver();
		}
		static void actTalk(int x1, int y1, bool bUseless)
		{
			yunjr::PcParty& party = yunjr::game::object::getParty();

			party.face(x1, y1);
/*
			window[WINDOWTYPE_MAP]->setUpdateFlag();
			window[WINDOWTYPE_MAP]->display();
			window[WINDOWTYPE_SUBMAP]->display();
*/
			yunjr::LoreConsole::getConsole().clear();

			yunjr::map_event::occur(yunjr::map_event::TYPE_TALK, 0, party.x + x1, party.y + y1);
		}
	};
}

void yunjr::init(const char* sz_id, const char* sz_data_path)
{
	// for Android (regarding the life cycle of Activity)
	{
		delete s_p_main_window;
		deleteVector(resource::getCharaList());

		yunjr::GameState::getMutableInstance().reset();
	}

	strcpy(_SAVE_DATA_PATH, sz_data_path);

	resource::init();

	{
		s_p_main_window = ControlWindow::newInstance(720, 1280);

		s_p_main_window->addChild(std::make_pair("WAKU",    ControlWaku::newInstance()));
		s_p_main_window->addChild(std::make_pair("MAP",     ControlMap::newInstance(8*2, 30*2, 216*2, 241*2)));
		s_p_main_window->addChild(std::make_pair("BATTLE",  ControlBattle::newInstance(8*2, 30*2, 216*2, 241*2)));
		s_p_main_window->addChild(std::make_pair("CONSOLE", ControlConsole::newInstance(8*2, 285*2, 345*2, 202*2, 8*2, 8*2, 8*2, 8*2)));
		s_p_main_window->addChild(std::make_pair("STATUS",  ControlStatus::newInstance(238*2, 30*2, 115*2, 241*2)));
		s_p_main_window->addChild(std::make_pair("NOTI",    ControlNoti::newInstance(0*2, 0*2, 720, 24 * 2)));
		s_p_main_window->addChild(std::make_pair("PANEL",   ControlPanel::newInstance()));

		resource::setMainWindow(s_p_main_window);

		ControlBattle* p_battle = (ControlBattle*)resource::getMainWindow()->findControl("BATTLE");
		p_battle->hide();
	}

	{
		ControlConsole* p_console = (ControlConsole*)resource::getMainWindow()->findControl("CONSOLE");

		if (p_console)
		{
			Typeface typeface;

			Text text1(typeface, L"April is the cruelest month, breeding");
			Text text2(typeface, L"Lilacs out of the dead land, mixing");
			Text text3(typeface, L"Memory and desire, sirring");

			p_console->clear();

			p_console->add(text1);
			p_console->add(text2);
			p_console->add(text3);
		}
	}

	{
		Map& map = game::object::getMap();

		map.act_func[Map::ACT_BLOCK] = &MapCallback::actBlock;
		map.act_func[Map::ACT_MOVE]  = &MapCallback::actMove;
		map.act_func[Map::ACT_EVENT] = &MapCallback::actEvent;
		map.act_func[Map::ACT_ENTER] = &MapCallback::actEnter;
		map.act_func[Map::ACT_SIGN]  = &MapCallback::actSign;
		map.act_func[Map::ACT_WATER] = &MapCallback::actWater;
		map.act_func[Map::ACT_SWAMP] = &MapCallback::actSwamp;
		map.act_func[Map::ACT_LAVA]  = &MapCallback::actLava;
		map.act_func[Map::ACT_TALK]  = &MapCallback::actTalk;
	}

	{
		//game::map::loadFromFile("hadar/K_DEN2.MAP");
		yunjr::map_event::load("sample");
	}

	{
		std::vector<yunjr::Chara*>& chara_list = resource::getCharaList();

		chara_list.push_back(Playable::newInstance());
	}

	// party 초기화
	{
		yunjr::PcParty& party = yunjr::game::object::getParty();

		// 외부에서 이 값이 수정되어져야 함
		party.x = -1;
		party.y = -1;

		party.food = 99;

		party.ability.can_use_ESP = false;
		party.ability.can_use_special_magic = false;
	}

	// player 초기화
	{
		sena::vector<shared::PcPlayer>& player_list = game::object::getPlayerList();

		int order = 0;

		{
			shared::PcPlayer p_player(new PcPlayer());

			p_player->setDefault(0);

			p_player->setName(L"쭈니쭈니");
			p_player->class_ = 1;
			p_player->level[0] = 19;
			p_player->level[1] = 19;
			p_player->level[2] = 19;
			p_player->hp = 190;
			p_player->level[0] = 19;
			p_player->weapon = 4;

			p_player->order = order++;
			player_list.push_back(p_player);
		}

		// 임시 party 멤버들
		{
			shared::PcPlayer p_player(new PcPlayer());

			p_player->setDefault(0);

			p_player->setName(L"헤라클레스");
			p_player->hp = 17;
			p_player->sp = 5;
			p_player->esp = 5;
			p_player->level[0] = 20;
			p_player->level[1] = 20;
			p_player->level[2] = 20;

			p_player->order = order++;
			player_list.push_back(p_player);
		}

		{
			shared::PcPlayer p_player(new PcPlayer());

			p_player->setDefault(0);

			p_player->setName(L"베텔규스");
			p_player->gender = PcPlayer::GENDER_FEMALE;
			p_player->hp = 7;
			p_player->sp = 17;
			p_player->esp = 5;
			p_player->level[0] = 20;
			p_player->level[1] = 20;
			p_player->level[2] = 20;

			p_player->order = order++;
			player_list.push_back(p_player);
		}

		{
			shared::PcPlayer p_player(new PcPlayer());

			p_player->setDefault(0);

			p_player->setName(L"카미너스");
			p_player->hp = 10;
			p_player->sp = 11;
			p_player->esp = 3;
			p_player->class_ = 2;
			p_player->level[0] = 20;
			p_player->level[1] = 20;
			p_player->level[2] = 20;

			p_player->order = order++;
			player_list.push_back(p_player);
		}

	}
}

void yunjr::done()
{
	delete s_p_main_window;
	s_p_main_window = 0;

	deleteVector(resource::getCharaList());
}

bool yunjr::loop(const BufferDesc& buffer_desc)
{
	yunjr::GameState& game_state = yunjr::GameState::getMutableInstance();

	if (game_state.isTerminating())
		return false;

	if (buffer_desc.bits_per_pixel != 32)
		return false;

	yunjr::gfx::beginDraw(buffer_desc);

	shared::FlatBoard32 dest_board = yunjr::gfx::getFrameBuffer();

	// Galaxy Note I problem: The default filling value of Android bitmap buffer is not defined.
	{
		static bool is_first = true;

		if (is_first)
		{
			dest_board->fillRect(0, 0, buffer_desc.width, buffer_desc.height, 0xFF000000);
			is_first = false;
		}
	}

	// update
	{
		unsigned long current_tick = 0;

		game_state.update(current_tick);

		// key processing
		if (KeyBuffer::getKeyBuffer().isKeyPressed())
		{
			KeyBuffer::Key key = KeyBuffer::getKeyBuffer().getKey();

			// command processing
			switch (key)
			{
/*??
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
*/
			case KEY_SYS_SELECT:
				selectMainMenu();
				break;
			default:
				break;
			}
		}

		s_p_main_window->update(current_tick);
	}

	// render
	{
		s_p_main_window->render(dest_board);
	}

	yunjr::gfx::endDraw();

	return true;
}
