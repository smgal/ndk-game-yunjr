
#include "util/sm_util_sena.h"
#include "util/sm_util_string.h"
#include "util/sm_util.h"

#include "hd_class_extern.h"
#include "hd_class_pc_enemy.h"
#include "hd_class_pc_player.h"

#include <map>

namespace
{
	using hadar::PcPlayer;
	using hadar::PcEnemy;

	enum PLAYERSTATUS
	{
		PLAYERSTATUS_CONSCIOUS, // �ǽ��� �ִ� ����
		PLAYERSTATUS_NOT_DEAD,  // ���� ���� ����
		PLAYERSTATUS_ALL,       // ��ȿ�� ��� ����
	};

	PcPlayer* getRandomPlayer(PLAYERSTATUS status)
	{
		PcPlayer* p_player = NULL;
		int num_player = 0;

		sena::vector<PcPlayer*>& player = hadar::game::object::getPlayerList();

		for (sena::vector<PcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
		{
			if (!(*obj)->isValid())
				continue;

			if (status == PLAYERSTATUS_CONSCIOUS)
				if (((*obj)->unconscious > 0) || ((*obj)->dead > 0) || ((*obj)->hp == 0))
					continue;

			if (status == PLAYERSTATUS_NOT_DEAD)
				if ((*obj)->dead > 0)
					continue;

			++num_player;
		}

		if (num_player > 0)
		{
			int order = smutil::random(num_player);

			for (sena::vector<PcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
			{
				if (!(*obj)->isValid())
					continue;

				if (status == PLAYERSTATUS_CONSCIOUS)
					if (((*obj)->unconscious > 0) || ((*obj)->dead > 0) || ((*obj)->hp == 0))
						continue;

				if (status == PLAYERSTATUS_NOT_DEAD)
					if ((*obj)->dead > 0)
						continue;

				if (order-- == 0)
				{
					p_player = *obj;
					break;
				}
			}
		}
		else
		{
			// ���� �ش�Ǵ� ���ǿ� �´� ĳ���Ͱ� ���ٸ� ���Ƿ� �����Ѵ�.
			p_player = player[smutil::random(player.size())];

			if (!p_player->isValid())
				p_player = NULL;
		}

		return p_player;
	}

	void setEnemyName(PcEnemy& enemy, const char* sz_name)
	{
		enemy.setName(sz_name);
	}

	void getEnemyName(PcEnemy& enemy, smutil::string& ref_name)
	{
		const char* sz_name = enemy.getName();
		ref_name = sz_name;
	}

	/* ȣ�� ������
	Attack -+-> enemyAttackWithWeapon
	        |
	        +-> enemyCastSpell -+-> enemyCastAttackSpellToAll -,
	        |                |                           |
	        |                +-> enemyCastAttackSpellToOne -+-> enemyCastAttackSpellSub
	        |                |
	        |                `-> enemyCastCureSpell
	        |
	        +-> enemyAttackWithSpecialAbility
	        |
	        `-> enemyCastSpellWithSpecialAbility
	*/
	void enemyAttackWithWeapon(PcEnemy* p_enemy);
	void enemyCastSpell(PcEnemy* p_enemy);
	void enemyCastAttackSpellToAll(PcEnemy* p_enemy);
	void enemyCastAttackSpellToOne(PcEnemy* p_enemy, PcPlayer* p_player);
	void enemyCastAttackSpellSub(PcEnemy* p_enemy, PcPlayer* p_player, int damage);
	void enemyCastCureSpell(PcEnemy* p_enemy, PcEnemy* p_target, int recovery);
	void enemyAttackWithSpecialAbility(PcEnemy* p_enemy);
	void enemyCastSpellWithSpecialAbility(PcEnemy* p_enemy);
}

// local variables
namespace
{
	typedef std::map<smutil::string, int> AttribMapInt;
	AttribMapInt s_m_attrib_map_int_list;

	typedef void (*FnSet)(PcEnemy&, const char*);
	typedef void (*FnGet)(PcEnemy&, smutil::string&);
	typedef sena::pair<void*, void*> FnSetGet;
	typedef std::map<smutil::string, FnSetGet> AttribMapStr;
	AttribMapStr s_m_attrib_map_str_list;
}

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool hadar::PcEnemy::_load(const smutil::ReadStream& stream)
{
	return false;
}

bool hadar::PcEnemy::_save(const smutil::WriteStream& stream) const
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// public method

hadar::PcEnemy::PcEnemy(void)
{
	// static data�� �ʱ�ȭ
	if (s_m_attrib_map_int_list.empty())
	{
		s_m_attrib_map_int_list["ed_number"]          = int(&this->ed_number) - int(this);
		s_m_attrib_map_int_list["strength"]           = int(&this->strength) - int(this);
		s_m_attrib_map_int_list["mentality"]          = int(&this->mentality) - int(this);
		s_m_attrib_map_int_list["endurance"]          = int(&this->endurance) - int(this);
		s_m_attrib_map_int_list["resistance"]         = int(&this->resistance) - int(this);
		s_m_attrib_map_int_list["agility"]            = int(&this->agility) - int(this);
		s_m_attrib_map_int_list["accuracy[0]"]        = int(&this->accuracy[0]) - int(this);
		s_m_attrib_map_int_list["accuracy[1]"]        = int(&this->accuracy[1]) - int(this);
		s_m_attrib_map_int_list["ac"]                 = int(&this->ac) - int(this);
		s_m_attrib_map_int_list["special"]            = int(&this->special) - int(this);
		s_m_attrib_map_int_list["cast_level"]         = int(&this->cast_level) - int(this);
		s_m_attrib_map_int_list["special_cast_level"] = int(&this->special_cast_level) - int(this);
		s_m_attrib_map_int_list["level"]              = int(&this->level) - int(this);
	}

	if (s_m_attrib_map_str_list.empty())
	{
		s_m_attrib_map_str_list["_name"] = sena::make_pair((void*)setEnemyName, (void*)getEnemyName);
	}
}

hadar::PcEnemy::~PcEnemy(void)
{
}

void hadar::PcEnemy::operator<<(const EnemyData& data)
{
	CT_ASSERT(sizeof(_name) >= sizeof(data._name), name_is_too_small);

	// �̸� ����
	setName(data._name);

	// ��Ÿ �Ӽ� ����
	strength           = data.strength;
	mentality          = data.mentality;
	endurance          = data.endurance;
	resistance         = data.resistance;
	agility            = data.agility;
	accuracy[0]        = data.accuracy[0];
	accuracy[1]        = data.accuracy[1];
	ac                 = data.ac;
	special            = data.special;
	cast_level         = data.cast_level;
	special_cast_level = data.special_cast_level;
	level              = data.level;

	// �Ӽ����� ��� ������ ��
	hp                 = endurance * level;
	poison             = 0;
	unconscious        = 0;
	dead               = 0;

	ed_number          = 1;
}

void hadar::PcEnemy::operator<<(const PcPlayer& player)
{
	setName(player.getName(JOSA_NONE));

	strength           = player.strength;
	mentality          = player.mentality;
	endurance          = player.endurance;
	resistance         = player.resistance;
	agility            = player.agility;
	accuracy[0]        = player.accuracy[0];
	accuracy[1]        = player.accuracy[1];
	ac                 = player.ac;
	special            = (player.class_ == 7) ? 2 : 0;
	cast_level         = player.level[1] / 4;
	special_cast_level = 0;
	level              = player.level[0];

	hp                 = endurance * level;
	poison             = 0;
	unconscious        = 0;
	dead               = 0;

	ed_number          = 1;
}

bool hadar::PcEnemy::isConscious(void) const
{
	return this->isValid() && (unconscious == 0) && (dead == 0) && (hp > 0);
}

void hadar::PcEnemy::attack(void)
{
	PcEnemy* p_enemy = this;

	if (p_enemy->special_cast_level > 0)
		enemyCastSpellWithSpecialAbility(p_enemy);

	int agility = p_enemy->agility;

	if (agility > 20)
		agility = 20;

	if ((p_enemy->special > 0) && (smutil::random(50) < agility))
	{
		int num_alive = game::player::getNumOfConsciousPlayer();

		if (num_alive > 3)
		{
			enemyAttackWithSpecialAbility(p_enemy);
			return;
		}
	}

	if ((smutil::random(p_enemy->accuracy[0]*1000) > smutil::random(p_enemy->accuracy[1]*1000)) && (p_enemy->strength > 0))
	{
		enemyAttackWithWeapon(p_enemy);
	}
	else
	{
		if (p_enemy->cast_level > 0)
			enemyCastSpell(p_enemy);
		else
			enemyAttackWithWeapon(p_enemy);
	}
}

bool hadar::PcEnemy::changeAttribute(const char* sz_attribute, int value)
{
	AttribMapInt::iterator attrib = s_m_attrib_map_int_list.find(sz_attribute);

	if (attrib == s_m_attrib_map_int_list.end())
		return false;

	int* p_int = reinterpret_cast<int*>(int(this) + int(attrib->second));

	*p_int = value;

	return true;
}

bool hadar::PcEnemy::changeAttribute(const char* sz_attribute, const char* sz_data)
{
	AttribMapStr::iterator attrib = s_m_attrib_map_str_list.find(sz_attribute);

	if (attrib == s_m_attrib_map_str_list.end())
		return false;

	FnSet((attrib->second).first)(*this, sz_data);

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// private method

namespace
{
	void enemyAttackWithWeapon(PcEnemy* p_enemy)
	{
		if (smutil::random(20) >= p_enemy->accuracy[0])
		{
			hadar::game::console::writeConsole(7, 2, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " �����߾���");
			return;
		}

		PcPlayer* p_player = getRandomPlayer(PLAYERSTATUS_CONSCIOUS);

		if (p_player == NULL)
			return;

		int damage = p_enemy->strength * p_enemy->level * (smutil::random(10)+1) / 10;

		if (p_player->isConscious())
		{
			if (smutil::random(50) < p_player->resistance)
			{
				hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ", p_player->getName(hadar::PcEnemy::JOSA_OBJ), " �����ߴ�");
				hadar::game::console::writeConsole(7, 3, "�׷���, ", p_player->getName(hadar::PcEnemy::JOSA_SUB), " ���� ������ �����ߴ�");
				return;
			}
			damage -= (p_player->ac * p_player->level[0] * (smutil::random(10)+1) / 10);
		}

		if (damage <= 0)
		{
			hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ", p_player->getName(hadar::PcEnemy::JOSA_OBJ), " �����ߴ�");
			hadar::game::console::writeConsole(7, 3, "�׷���, ", p_player->getName(hadar::PcEnemy::JOSA_SUB), " ���� ������ ����ߴ�");
			return;
		}

		if (p_player->dead > 0)
			p_player->dead += damage;
		if ((p_player->unconscious > 0) && (p_player->dead == 0))
			p_player->unconscious += damage;
		if (p_player->hp > 0)
			p_player->hp -= damage;

		hadar::game::console::writeConsole(13, 4, p_player->getName(hadar::PcEnemy::JOSA_SUB), " ", p_enemy->getName(hadar::PcEnemy::JOSA_NONE), "���� ���� �޾Ҵ�");
		hadar::game::console::writeConsole(5, 4, p_player->getName(hadar::PcEnemy::JOSA_SUB), " @D", smutil::IntToStr(damage)(), "@@��ŭ�� ���ظ� �Ծ���");
	}

	void enemyCastSpell(PcEnemy* p_enemy)
	{
		PcPlayer* p_player = NULL;

		sena::vector<PcEnemy*>& enemy = hadar::game::object::getEnemyList();
		sena::vector<PcPlayer*>& player = hadar::game::object::getPlayerList();

		switch (p_enemy->cast_level)
		{
		case 0:
			break;
		case 1:
			{
				p_player = player[smutil::random(player.size())];
				if (!p_player->isConscious())
					p_player = player[smutil::random(player.size())];

				enemyCastAttackSpellToOne(p_enemy, p_player);
			}
			break;
		case 2:
			{
				enemyCastAttackSpellToOne(p_enemy, getRandomPlayer(PLAYERSTATUS_CONSCIOUS));
			}
			break;
		case 3:
			{
				if (smutil::random(hadar::game::player::getNumOfConsciousPlayer()) < 2)
					enemyCastAttackSpellToOne(p_enemy, getRandomPlayer(PLAYERSTATUS_CONSCIOUS));
				else
					enemyCastAttackSpellToAll(p_enemy);
			}
			break;
		case 4:
			{
				if ((p_enemy->hp < p_enemy->endurance * p_enemy->level / 3) && (smutil::random(2) == 0))
				{
					enemyCastCureSpell(p_enemy, p_enemy, p_enemy->level * p_enemy->mentality / 4);
				}
				else
				{
					if (smutil::random(hadar::game::player::getNumOfConsciousPlayer()) < 2)
						enemyCastAttackSpellToOne(p_enemy, getRandomPlayer(PLAYERSTATUS_CONSCIOUS));
					else
						enemyCastAttackSpellToAll(p_enemy);
				}
			}
			break;
		case 5:
			{
				if ((p_enemy->hp < p_enemy->endurance * p_enemy->level / 3) && (smutil::random(3) == 0))
				{
					enemyCastCureSpell(p_enemy, p_enemy, p_enemy->level * p_enemy->mentality / 4);
				}
				else
				{
					if (smutil::random(hadar::game::player::getNumOfConsciousPlayer()) < 2)
					{
						int total_hp = 0;
						int total_max_hp = 0;

						for (sena::vector<PcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
						{
							if ((*obj)->isValid())
							{
								total_hp += (*obj)->hp;
								total_max_hp += (*obj)->endurance * (*obj)->level;
							}
						}

						if ((enemy.size() > 2) && (total_hp*3 < total_max_hp) && (smutil::random(2) == 0))
						{
							for (sena::vector<PcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
							{
								if ((*obj)->isValid())
									enemyCastCureSpell(p_enemy, (*obj), p_enemy->level * p_enemy->mentality / 6);
							}
						}
						else
						{
							// party���� �ǽ��� �ִ� ��� �߿� ���� hp�� ���� ����� ����
							p_player = *player.begin();
							for (sena::vector<PcPlayer*>::iterator obj = player.begin()+1; obj != player.end(); ++obj)
							{
								if ((*obj)->isValid())
								{
									if ((*obj)->hp < p_player->hp)
										p_player = (*obj);
								}
							}

							if (p_player->isValid())
								enemyCastAttackSpellToOne(p_enemy, p_player);
						}
					}
					else
					{
						enemyCastAttackSpellToAll(p_enemy);
					}
				}
			}
			break;
		case 6:
			{
				if ((p_enemy->hp < p_enemy->endurance * p_enemy->level / 3) && (smutil::random(3) == 0))
				{
					enemyCastCureSpell(p_enemy, p_enemy, p_enemy->level * p_enemy->mentality / 4);
					return;
				}

				int average_ac = 0;

				{
					int accum_ac = 0;
					int num_player = 0;;
					for (sena::vector<PcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
					{
						if ((*obj) && ((*obj)->isValid()))
						{
							++num_player;
							accum_ac += p_enemy->ac;
						}
					}
					average_ac = (num_player > 0) ? (accum_ac / num_player) : 0;
				}

				if ((average_ac > 4) && (smutil::random(5) == 0))
				{
					for (sena::vector<PcPlayer*>::iterator obj = player.begin()+1; obj != player.end(); ++obj)
					{
						if ((*obj)->isValid())
						{
							hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ", (*obj)->getName(hadar::PcEnemy::JOSA_NONE), "�� �����ı��� �õ��ߴ�");
							if ((*obj)->luck > smutil::random(21))
							{
								hadar::game::console::writeConsole(7, 3, "�׷���, ", p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " �������� ���ߴ�");
							}
							else
							{
								hadar::game::console::writeConsole(5, 2, (*obj)->getName(hadar::PcEnemy::JOSA_NONE), "�� ������ �ı��Ǿ���");
								if ((*obj)->ac > 0)
									--(*obj)->ac;
							}
						}
					}

					hadar::game::window::displayStatus();
				}
				else
				{
					int total_hp = 0;
					int total_max_hp = 0;

					for (sena::vector<PcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
					{
						if ((*obj)->isValid())
						{
							total_hp += (*obj)->hp;
							total_max_hp += (*obj)->endurance * (*obj)->level;
						}
					}

					if ((enemy.size() > 2) && (total_hp*3 < total_max_hp) && (smutil::random(3) == 0))
					{
						for (sena::vector<PcEnemy*>::iterator obj = enemy.begin(); obj != enemy.end(); ++obj)
						{
							if ((*obj)->isValid())
								enemyCastCureSpell(p_enemy, (*obj), p_enemy->level * p_enemy->mentality / 6);
						}
					}
					else
					{
						if (smutil::random(hadar::game::player::getNumOfConsciousPlayer()) < 2)
						{
							// party���� �ǽ��� �ִ� ��� �߿� ���� hp�� ���� ����� ����
							p_player = *player.begin();
							int hp = (p_player->isConscious()) ? p_player->hp : 0x7FFFFFFFL;
							for (sena::vector<PcPlayer*>::iterator obj = player.begin()+1; obj != player.end(); ++obj)
							{
								if ((*obj)->isValid())
								{
									if ((*obj)->isConscious())
									{
										if ((*obj)->hp < hp)
										{
											p_player = (*obj);
											hp = p_player->hp;
										}
									}
								}
							}

							if (p_player->isValid())
								enemyCastAttackSpellToOne(p_enemy, p_player);
						}
						else
						{
							enemyCastAttackSpellToAll(p_enemy);
						}
					}
				}
			}
			break;
		default:
			assert(false);
		}
	}

	void enemyAttackWithSpecialAbility(PcEnemy* p_enemy)
	{
		PcPlayer* p_player = NULL;

		switch (p_enemy->special)
		{
		case 1:
			{
				for (int i = 0; i < 5; i++)
				{
					p_player = getRandomPlayer(PLAYERSTATUS_CONSCIOUS);
					if ((p_player != NULL) && (p_player->poison == 0))
						break;
				}

				if (p_player == NULL)
					return;

				hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ", p_player->getName(hadar::PcEnemy::JOSA_NONE), "���� �� ������ �õ��ߴ�");

				if (smutil::random(40) > p_enemy->agility)
				{
					hadar::game::console::writeConsole(7, 1, "�� ������ �����ߴ�");
					return;
				}

				if (smutil::random(20) < p_player->luck)
				{
					hadar::game::console::writeConsole(7, 3, "�׷���, ", p_player->getName(hadar::PcEnemy::JOSA_SUB), " �� ������ ���ߴ�");
					return;
				}

				hadar::game::console::writeConsole(4, 2, p_player->getName(hadar::PcEnemy::JOSA_SUB), " �ߵ� �Ǿ��� !!");

				++p_player->poison;
			}
			break;
		case 2:
			{
				p_player = getRandomPlayer(PLAYERSTATUS_CONSCIOUS);

				if (p_player == NULL)
					return;

				hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ", p_player->getName(hadar::PcEnemy::JOSA_NONE), "���� ġ���� ������ �õ��ߴ�");

				if (smutil::random(50) > p_enemy->agility)
				{
					hadar::game::console::writeConsole(7, 1, "ġ���� ������ �����ߴ�");
					return;
				}

				if (smutil::random(20) < p_player->luck)
				{
					hadar::game::console::writeConsole(7, 3, "�׷���, ", p_player->getName(hadar::PcEnemy::JOSA_SUB), " ġ���� ������ ���ߴ�");
					return;
				}

				hadar::game::console::writeConsole(4, 2, p_player->getName(hadar::PcEnemy::JOSA_SUB), " �ǽĺҸ��� �Ǿ��� !!");

				if (p_player->unconscious == 0)
				{
					p_player->unconscious = 1;
					if (p_player->hp > 0)
						p_player->hp = 0;
				}
			}
			break;
		case 3:
			{
				p_player = getRandomPlayer(PLAYERSTATUS_NOT_DEAD);

				if (p_player == NULL)
					return;

				hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ", p_player->getName(hadar::PcEnemy::JOSA_NONE), "���� ������ ������ �õ��ߴ�");

				if (smutil::random(60) > p_enemy->agility)
				{
					hadar::game::console::writeConsole(7, 1, "������ ������ �����ߴ�");
					return;
				}

				if (smutil::random(20) < p_player->luck)
				{
					hadar::game::console::writeConsole(7, 3, "�׷���, ", p_player->getName(hadar::PcEnemy::JOSA_SUB), " ������ ������ ���ߴ�");
					return;
				}

				hadar::game::console::writeConsole(4, 2, p_player->getName(hadar::PcEnemy::JOSA_SUB), " �׾��� !!");

				if (p_player->dead == 0)
				{
					p_player->dead = 1;
					if (p_player->hp > 0)
						p_player->hp = 0;
				}
			}
			break;
		}
	}

	void enemyCastSpellWithSpecialAbility(PcEnemy* p_enemy)
	{
		//?? ��ü �ڵ� �ٽ� ���� ��
		if (p_enemy->special_cast_level <= 0)
			return;

		// Ư���� �����, ������ �ڵ�� ���۵� ��
		if (p_enemy->ed_number <= 1)
			return;

		sena::vector<PcEnemy*>& enemy = hadar::game::object::getEnemyList();
		sena::vector<PcPlayer*>& player = hadar::game::object::getPlayerList();

		int num_enemy = enemy.size();

		{
			// ���� ���� ���� ��
			int num_not_dead = 0;
			// ���� �� �߿��� ���� �տ� �ִ� ��
			int ix_enemy = num_enemy - 1;

			for (int i = num_enemy - 1; i >= 0; --i)
			{
				if (enemy[i]->dead == 0)
					++num_not_dead;
				else
					ix_enemy = i;
			}

			if ((num_not_dead < smutil::random(3)+2) && (smutil::random(3) == 0))
			{
				if (num_enemy < int(enemy.capacity()))
				{
					hadar::game::battle::registerEnemy(p_enemy->ed_number + smutil::random(4) - 20);
					num_enemy = enemy.size();
				}
				else
				{
					int index = p_enemy->ed_number + smutil::random(4) - 20;
					//@@ enemy[ix_enemy]�� ��ȿ���� �����Ǿ���?
					*enemy[ix_enemy] << hadar::getEnemyDataFromEnemyTable(index);
					enemy[ix_enemy]->ed_number = index;
				}

				hadar::game::window::displayBattle(0); //@@ 0 �³�?
				hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ", enemy[ix_enemy]->getName(hadar::PcEnemy::JOSA_OBJ), " �������״�");
			}

			if (p_enemy->special_cast_level > 1)
			{
				PcPlayer* p_last_player = *(player.end() - 1);
				if ((p_last_player->isValid()) && (num_not_dead < 7) && (smutil::random(5) == 0))
				{
					if (num_enemy < int(enemy.capacity()))
						ix_enemy = num_enemy++;

					*enemy[ix_enemy] << *p_last_player;
					p_last_player->setName("");

					hadar::game::window::displayStatus();
					hadar::game::window::displayBattle(0); //@@ 0�� �´��� Ȯ�� �ʿ�

					hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB2), " ���ɼ��� ����Ͽ� ", enemy[ix_enemy]->getName(hadar::PcEnemy::JOSA_OBJ), " �ڱ������� ����鿴��");
				}
			}

			if (p_enemy->special_cast_level > 2)
			{
				int ix_player;

				if (p_enemy->special == 0)
					return;

				if (smutil::random(5) == 0)
				for (ix_player = 0; ix_player < 6; ix_player++)
				if ((player[ix_player]->dead == 0) && (player[ix_player]->isValid()))
				{
					hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ", player[ix_player]->getName(hadar::PcEnemy::JOSA_NONE), "���� ������ ������ �õ��ߴ�");

					if (smutil::random(60) > p_enemy->agility)
						hadar::game::console::writeConsole(7, 1, "������ ������ �����ߴ�");
					else if (smutil::random(20) < player[ix_player]->luck)
						hadar::game::console::writeConsole(7, 3, "�׷���, ", player[ix_player]->getName(hadar::PcEnemy::JOSA_SUB), "������ ������ ���ߴ�");
					else
					{
						hadar::game::console::writeConsole(4, 2, player[ix_player]->getName(hadar::PcEnemy::JOSA_SUB), " �׾���!!");
						if (player[ix_player]->dead == 0)
						{
							player[ix_player]->dead = 1;
							if (player[ix_player]->hp > 0)
								player[ix_player]->hp = 0;
						}
					}
				}
			}
			
		}
	}

	void enemyCastAttackSpellToAll(PcEnemy* p_enemy)
	{
		//@@ ������ ���ҽ� ���� ������ �Ѿ� ���� ��
		struct MagicTable
		{
			const char* sz_magic_name;
			int damage;
		};
		
		static const MagicTable MAGIC_TABLE[] = 
		{
			{ "����",     1},  // 0
			{ "������",   2},  // 2
			{ "������",   3},  // 3
			{ "Ȥ�ѱ�",   5},  // 4
			{ "ȭ����ǳ", 8},  // 5
		};

		const int MAX_CONVERT_TABLE = 21;
		static const int CONVERT_TABLE[MAX_CONVERT_TABLE+1] =
		{
		//  0  1  2  3  4  5  6  7  8  9
			0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
			1, 1, 1, 2, 2, 2, 2, 3, 3, 3,
			3, 4
		};

		int mentality = p_enemy->mentality;

		if (mentality < 0)
			mentality = 0;

		if (mentality > MAX_CONVERT_TABLE)
			mentality = MAX_CONVERT_TABLE;

		const MagicTable& magic = MAGIC_TABLE[CONVERT_TABLE[mentality]];

		hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ���� ��ο��� '", magic.sz_magic_name, "'������ ����ߴ�");

		sena::vector<PcPlayer*>& player = hadar::game::object::getPlayerList();

		for (sena::vector<PcPlayer*>::iterator obj = player.begin(); obj != player.end(); ++obj)
			enemyCastAttackSpellSub(p_enemy, *obj, magic.damage * p_enemy->level);
	}

	void enemyCastAttackSpellToOne(PcEnemy* p_enemy, PcPlayer* p_player)
	{
		//@@ ������ ���ҽ� ���� ������ �Ѿ� ���� ��
		struct MagicTable
		{
			const char* sz_magic_name;
			int damage;
		};
		
		static const MagicTable MAGIC_TABLE[] = 
		{
			{ "���", 1},  // 0
			{ "�ñ�", 2},  // 2
			{ "����", 4},  // 3
			{ "Ȥ��", 6},  // 4
			{ "ȭ��", 7},  // 5
			{ "����", 10}, // 6
		};

		const int MAX_CONVERT_TABLE = 20;
		static const int CONVERT_TABLE[MAX_CONVERT_TABLE+1] =
		{
		//  0  1  2  3  4  5  6  7  8  9
			0, 0, 0, 0, 1, 1, 1, 1, 1, 2,
			2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
			5
		};

		if (p_player == NULL)
			return;

		int mentality = p_enemy->mentality;

		if (mentality < 0)
			mentality = 0;

		if (mentality > MAX_CONVERT_TABLE)
			mentality = MAX_CONVERT_TABLE;

		const MagicTable& magic = MAGIC_TABLE[CONVERT_TABLE[mentality]];

		hadar::game::console::writeConsole(13, 6, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ", p_player->getName(hadar::PcEnemy::JOSA_NONE), "���� '", magic.sz_magic_name, "'������ ����ߴ�");

		enemyCastAttackSpellSub(p_enemy, p_player, magic.damage * p_enemy->level);
	}

	void enemyCastCureSpell(PcEnemy* p_enemy, PcEnemy* p_target, int recovery)
	{
		if (p_enemy == p_target)
			hadar::game::console::writeConsole(13, 2, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " �ڽ��� ġ���ߴ�");
		else
			hadar::game::console::writeConsole(13, 4, p_enemy->getName(hadar::PcEnemy::JOSA_SUB), " ", p_target->getName(hadar::PcEnemy::JOSA_OBJ), " ġ���ߴ�");

		if (p_target->dead > 0)
		{
			p_target->dead = 0;
		}
		else if (p_target->unconscious > 0)
		{
			p_target->unconscious = 0;
			if (p_target->hp <= 0)
				p_target->hp = 1;
		}
		else
		{
			p_target->hp += recovery;
			if (p_target->hp > p_target->endurance * p_target->level)
				p_target->hp = p_target->endurance * p_target->level;
		}
	}

	void enemyCastAttackSpellSub(PcEnemy* p_enemy, PcPlayer* p_player, int damage)
	{
		if (smutil::random(20) >= p_enemy->accuracy[1])
		{
			hadar::game::console::writeConsole(7, 2, p_enemy->getName(hadar::PcEnemy::JOSA_NONE), "�� ���������� ��������");
			return;
		}

		if (p_player->isConscious())
		if (smutil::random(50) < p_player->resistance)
		{
			hadar::game::console::writeConsole(7, 3, "�׷���, ", p_player->getName(hadar::PcEnemy::JOSA_SUB), " ���� ������ �����ߴ�");
			return;
		}

		damage -= smutil::random(damage / 2);

		if (p_player->isConscious())
			damage -= (p_player->ac * p_player->level[0] * (smutil::random(10)+1) / 10);

		if (damage <= 0)
		{
			hadar::game::console::writeConsole(7, 3, "�׷���, ", p_player->getName(hadar::PcEnemy::JOSA_SUB), " ���� ������ ���Ƴ´�");
			return;
		}

		if (p_player->dead > 0)
			p_player->dead += damage;

		if ((p_player->unconscious > 0) && (p_player->dead == 0))
			p_player->unconscious += damage;

		if (p_player->hp > 0)
			p_player->hp -= damage;

		hadar::game::console::writeConsole(5, 4, p_player->getName(hadar::PcEnemy::JOSA_SUB), " @D", smutil::IntToStr(damage)(), "@@��ŭ�� ���ظ� �Ծ���");
	}

}

////////////////////////////////////////////////////////////////////////////////
// global function

const hadar::EnemyData& hadar::getEnemyDataFromEnemyTable(int index)
{
	static bool s_is_first = true;
	static hadar::EnemyData s_enemy_data[75] =
	{	// _name                 str men end res agi accuracy   ac spe clv scl  lv
		{"Orc",                   8,  0,  8,  0,  8,{  8,  0},  1,  0,  0,  0,  1},
		{"Troll",                 9,  0,  6,  0,  9,{  9,  0},  1,  0,  0,  0,  1},
		{"Serpent",               7,  3,  7,  0, 11,{ 11,  6},  1,  1,  1,  0,  1},
		{"Earth Worm",            3,  5,  5,  0,  6,{ 11,  7},  1,  0,  1,  0,  1},
		{"Dwarf",                10,  0, 10,  0, 10,{ 10,  0},  2,  0,  0,  0,  2},
		{"Giant",                15,  0, 13,  0,  8,{  8,  0},  2,  0,  0,  0,  2},
		{"Phantom",               0, 12, 12,  0,  0,{  0, 13},  0,  0,  2,  0,  2},
		{"Wolf",                  7,  0, 11,  0, 15,{ 15,  0},  1,  0,  0,  0,  2},
		{"Imp",                   8,  8, 10, 20, 18,{ 18, 10},  2,  0,  2,  0,  3},
		{"Goblin",               11,  0, 13,  0, 13,{ 13,  0},  3,  0,  0,  0,  3},
		{"Python",                9,  5, 10,  0, 13,{ 13,  6},  1,  1,  1,  0,  3},
		{"Insects",               6,  4,  8,  0, 14,{ 14, 15},  2,  1,  1,  0,  3},
		{"Giant Spider",         10,  0,  9,  0, 20,{ 13,  0},  2,  1,  0,  0,  4},
		{"Gremlin",              10,  0, 10,  0, 20,{ 20,  0},  2,  0,  0,  0,  4},
		{"Buzz Bug",             13,  0, 11,  0, 15,{ 15,  0},  1,  1,  0,  0,  4},
		{"Salamander",           12,  2, 13,  0, 12,{ 12, 10},  3,  1,  1,  0,  4},
		{"Blood Bat",            11,  0, 10,  0,  5,{ 15,  0},  1,  0,  0,  0,  5},
		{"Giant Rat",            13,  0, 18,  0, 10,{ 10,  0},  2,  0,  0,  0,  5},
		{"Skeleton",             10,  0, 19,  0, 12,{ 12,  0},  3,  0,  0,  0,  5},
		{"Kelpie",                8, 13,  8,  0, 14,{ 15, 17},  2,  0,  3,  0,  5},
		{"Gazer",                15,  8, 11,  0, 20,{ 15, 15},  3,  0,  2,  0,  6},
		{"Ghost",                 0, 15, 10,  0,  0,{  0, 15},  0,  0,  3,  0,  6},
		{"Slime",                 5, 13,  5,  0, 19,{ 19, 19},  2,  0,  2,  0,  6},
		{"Rock-Man",             19,  0, 15,  0, 10,{ 10,  0},  5,  0,  0,  0,  6},
		{"Kobold",                9,  9,  9,  0,  9,{  9,  9},  2,  0,  3,  0,  7},
		{"Mummy",                10, 10, 10,  0, 10,{ 10, 10},  3,  1,  2,  0,  7},
		{"Devil Hunter",         13, 10, 10,  0, 10,{ 10, 18},  3,  2,  2,  0,  7},
		{"Crazy One",             9,  9, 10,  0,  5,{  5, 13},  1,  0,  3,  0,  7},
		{"Ogre",                 19,  0, 19,  0,  9,{ 12,  0},  4,  0,  0,  0,  8},
		{"Headless",             10,  0, 15,  0, 10,{ 10,  0},  3,  2,  0,  0,  8},
		{"Mud-Man",              10,  0, 15,  0, 10,{ 10,  0},  7,  0,  0,  0,  8},
		{"Hell Cat",             10, 15, 11,  0, 18,{ 18, 16},  2,  2,  3,  0,  8},
		{"Wisp",                  5, 16, 10,  0, 20,{ 20, 20},  2,  0,  4,  0,  9},
		{"Basilisk",             10, 15, 12,  0, 20,{ 20, 10},  2,  1,  2,  0,  9},
		{"Sprite",                0, 20,  2, 80, 20,{ 20, 20},  0,  3,  5,  0,  9},
		{"Vampire",              15, 13, 14, 20, 17,{ 17, 15},  3,  1,  2,  0,  9},
		{"Molten Monster",        8,  0, 20, 50,  8,{ 16,  0},  3,  0,  0,  0, 10},
		{"Great Lich",           10, 10, 11, 10, 18,{ 10, 10},  4,  2,  3,  0, 10},
		{"Rampager",             20,  0, 19,  0, 19,{ 19,  0},  3,  0,  0,  0, 10},
		{"Mutant",                0, 10, 15,  0,  0,{  0, 20},  3,  0,  3,  0, 10},
		{"Rotten Corpse",        15, 15, 15, 60, 15,{ 15, 15},  2,  2,  3,  0, 11},
		{"Gagoyle",              10,  0, 20, 10, 10,{ 10,  0},  6,  0,  0,  0, 11},
		{"Wivern",               10, 10,  9, 30, 20,{ 20,  9},  3,  2,  3,  0, 11},
		{"Grim Death",           16, 16, 16, 50, 16,{ 16, 16},  2,  2,  3,  0, 12},
		{"Griffin",              15, 15, 15,  0, 15,{ 14, 14},  3,  2,  3,  0, 12},
		{"Evil Soul",             0, 20, 10,  0,  0,{  0, 15},  0,  3,  4,  0, 12},
		{"Cyclops",              20,  0, 20, 10, 20,{ 20,  0},  4,  0,  0,  0, 13},
		{"Dancing-Swd",          15, 20,  6, 20, 20,{ 20, 20},  0,  2,  4,  0, 13},
		{"Hydra",                15, 10, 20, 40, 18,{ 18, 12},  8,  1,  3,  0, 13},
		{"Stheno",               20, 20, 20,255, 10,{ 10, 10},255,  1,  3,  0, 14},
		{"Euryale",              20, 20, 15,255, 10,{ 15, 10},255,  2,  3,  0, 14},
		{"Medusa",               15, 10, 16, 50, 15,{ 15, 10},  4,  3,  3,  0, 14},
		{"Minotaur",             15,  7, 20, 40, 20,{ 20, 15}, 10,  0,  3,  0, 15},
		{"Dragon",               15,  7, 20, 50, 18,{ 20, 15},  9,  2,  4,  0, 15},
		{"Dark Soul",             0, 20, 40, 60,  0,{  0, 20},  0,  0,  5,  0, 15},
		{"Hell Fire",            15, 20, 30, 30, 15,{ 15, 15},  0,  3,  5,  0, 16},
		{"Astral Mud",           13, 20, 25, 40, 19,{ 19, 10},  9,  3,  4,  0, 16},
		{"Reaper",               15, 20, 33, 70, 20,{ 20, 20},  5,  1,  3,  0, 17},
		{"Crab God",             20, 20, 30, 20, 18,{ 18, 19},  7,  2,  4,  0, 17},
		{"Wraith",                0, 24, 35, 50, 15,{  0, 20},  2,  3,  4,  0, 18},
		{"Death Skull",           0, 20, 40, 80,  0,{  0, 20},  0,  2,  5,  0, 18},
		{"Draconian",            30, 20, 30, 60, 18,{ 18, 18},  7,  2,  5,  1, 19},
		{"Death Knight",         35,  0, 35, 50, 20,{ 20,  0},  6,  3,  0,  1, 19},
		{"Guardian-Lft",         25,  0, 40, 70, 20,{ 18,  0},  5,  2,  0,  0, 20},
		{"Guardian-Rgt",         25,  0, 40, 40, 20,{ 20,  0},  7,  2,  0,  0, 20},
		{"Mega-Robo",            40,  0, 50,  0, 19,{ 19,  0}, 10,  0,  0,  0, 21},
		{"Ancient Evil",          0, 20, 60,100, 18,{  0, 20},  5,  0,  6,  2, 22},
		{"Lord Ahn",             40, 20, 60,100, 35,{ 20, 20}, 10,  3,  5,  3, 23},
		{"Frost Dragon",         15,  7, 20, 50, 18,{ 20, 15},  9,  2,  4,  0, 24},
		{"ArchiDraconian",       30, 20, 30, 60, 18,{ 18, 18},  7,  2,  5,  1, 25},
		{"Panzer Viper",         35,  0, 40, 80, 20,{ 20,  0},  9,  1,  0,  0, 26},
		{"Black Knight",         35,  0, 35, 50, 20,{ 20,  0},  6,  3,  0,  1, 27},
		{"ArchiMonk",            20,  0, 50, 70, 20,{ 20,  0},  5,  0,  0,  0, 28},
		{"ArchiMage",            10, 19, 30, 70, 10,{ 10, 19},  4,  0,  6,  0, 29},
		{"Neo-Necromancer",      40, 20, 60,100, 30,{ 20, 20}, 10,  3,  6,  3, 30}
	};

	CT_ASSERT(sizeof(s_enemy_data[0]) == 32, TEnemyData_size_is_changed);

	if (s_is_first)
	{
		// ���� ���丮�� foedata0.dat ������ �ִ� ���� �� ������ �̿��ϰ�,
		// �׷��� ���� ��쿡�� ����� �̸� ������ ����Ʈ �����͸� ����Ѵ�.
		FILE* p_file = fopen("./foedata0.dat", "rb");
		if (p_file)
		{
			for (int i = 0; i < sizeof(s_enemy_data) / sizeof(s_enemy_data[0]); i++)
			{
				int read = fread(&s_enemy_data[i], sizeof(EnemyData), 1, p_file);

				assert(read == 1);

				if (read != 1)
					break;

				// pascal�� string�� C�� string���� ��ȯ
				{
					unsigned char strLength = (unsigned char)s_enemy_data[i]._name[0];
					memcpy(&s_enemy_data[i]._name[0], &s_enemy_data[i]._name[1], strLength);
					s_enemy_data[i]._name[strLength] = 0;
				}
			}

			fclose(p_file);

#if 0 // ������ ���忡 ���� code
			{
				FILE* p_file = fopen("./foedata0.cpp", "wt");
				if (p_file)
				{
					int num_enemy = sizeof(s_enemy_data) / sizeof(s_enemy_data[0]);

					fprintf(p_file, "static EnemyData s_enemy_data[%d] =\n", num_enemy);
					fprintf(p_file, "{\t// _name                 str men end res agi accuracy   ac spe clv scl  lv\n");
	
					for (int i = 0; i < num_enemy; i++)
					{
						const EnemyData& data = s_enemy_data[i];
						fprintf(p_file, "\t{");
						fprintf(p_file, "\"%s\",", data._name);
						for (int j = 0; j < sizeof(data._name)-strlen(data._name); j++)
							fprintf(p_file, " ");
						fprintf(p_file, "%3d,", data.strength);
						fprintf(p_file, "%3d,", data.mentality);
						fprintf(p_file, "%3d,", data.endurance);
						fprintf(p_file, "%3d,", data.resistance);
						fprintf(p_file, "%3d,", data.agility);
						fprintf(p_file, "{%3d,%3d},", data.accuracy[0], data.accuracy[1]);
						fprintf(p_file, "%3d,", data.ac);
						fprintf(p_file, "%3d,", data.special);
						fprintf(p_file, "%3d,", data.cast_level);
						fprintf(p_file, "%3d,", data.special_cast_level);
						fprintf(p_file, "%3d},\n", data.level);
					}

					fprintf(p_file, "};\n");

					fclose(p_file);
				}
			}
#endif
		}
		else
		{
			// ����Ʈ �����͸� ���
		}

		s_is_first = false;
	}

	// 1-base�� 0-base��
	--index;

	if ((index < 0) || (index >= sizeof(s_enemy_data) / sizeof(s_enemy_data[0])))
		index = 0;

	return s_enemy_data[index];
}
