
#ifndef __HD_CLASS_PC_PLAYER_H__
#define __HD_CLASS_PC_PLAYER_H__

#pragma warning(disable: 4786)

#include "hd_class_pc_name_base.h"
#include "hd_base_serialized.h"

namespace hadar
{
	struct EnemyData;
	struct PcEnemy;

	/*!
	 * \file
	 * \ingroup game main
	 * \brief ���ӿ� ���Ǵ� player ĳ������ class
	 * \author 
	 * \date 2006-12-31
	*/
	struct PcPlayer: public PcNameBase
	{
		enum GENDER
		{
			GENDER_MALE,
			GENDER_FEMALE,
			GENDER_MAX = 0x7FFFFFFF
		};

		union
		{
			struct
			{
				int   order;
				GENDER gender;
				int   class_;

				int   strength;
				int   mentality;
				int   concentration;
				int   endurance;
				int   resistance;
				int   agility;
				int   luck;

				int   ac;

				int   hp;
				int   sp;
				int   esp;
				int   experience;
				int   accuracy[3];
				int   level[3];

				int   poison;
				int   unconscious;
				int   dead;

				int   weapon;
				int   shield;
				int   armor;

				int   pow_of_weapon;
				int   pow_of_shield;
				int   pow_of_armor;
			};

			char save[30 * sizeof(int)];
		};

	public:
		PcPlayer(void);
		~PcPlayer(void);

		//! �Ӽ� ���� kind�� �´� ����Ʈ ���� ä��
		void setDefault(int kind);
		//! ������ ���¸� �����ϰ� �̻��� �ִ� ���� ����
		void checkCondition(void);

		//! ������ ���¸� ��Ÿ���� ���ڿ��� ������
		const char* getConditionString(void) const;
		//! ������ ���¸� 32-bit �÷� ������ ������
		unsigned long getConditionColor(void) const;
		//! 3��Ī ���縦 ������
		const char* get3rdPersonName(void) const;
		//! ������ ��Ÿ���� ���ڿ��� ������
		const char* getGenderName(void) const;
		//! class�� ��Ÿ���� ���ڿ��� ������
		const char* getClassName(void) const;
		//! ������ ���⸦ ��Ÿ���� ���ڿ��� ������
		const char* getWeaponName(void) const;
		//! ������ ���и� ��Ÿ���� ���ڿ��� ������
		const char* getShieldName(void) const;
		//! ������ ������ ��Ÿ���� ���ڿ��� ������
		const char* getArmorName(void) const;

		//! ���ڿ��� ���� ������ �Ӽ��� ����
		bool changeAttribute(const char* sz_attribute, int value);
		bool changeAttribute(const char* sz_attribute, const char* sz_data);
		bool getAttribute(const char* sz_attribute, int& value);
		bool getAttribute(const char* sz_attribute, smutil::string& data);

		//! ���յǴ� �Ӽ�(hp��)�� ������ �� ������ �ִ����� Ȯ���ϰ�, ������ ������ �����Ѵ�.
		void reviseAttribute(void);
		//! ������ �Ӽ����� �´� hp������ �����Ѵ�.
		void applyAttribute(void);

		//! �ǽ��� �ִ� �������� �ƴ����� �Ǻ�
		bool isConscious(void) const
		{
			return this->isValid() && (unconscious == 0) && (dead == 0) && (hp > 0);
		}

		//! �� �����ͷκ��� player�� ����
		void operator<<(const struct EnemyData& data);

		/*
		runBattleMode -+-> attackWithWeapon -------,
					|                           |
					+-> castSpellToAll          |
					|      |                    |
					+------+-> castSpellToOne --+
					|                           |
					+-> useESPForBattle --------+-> m_plusExperience
					|
					+-> castCureSpell -+-> m_healAll
					|              |        |
					|              +--------+-> m_healOne
					|              |
					|              +-> m_antidoteAll
					|              |        |
					|              +--------+-> m_antidoteOne
					|              |
					|              +-> m_recoverConsciousnessAll
					|              |        |
					|              +--------+-> m_recoverConsciousnessOne
					|              |
					|              +-> m_revitalizeAll
					|              |        |
					|              `--------+-> m_revitalizeOne
					|
					+-> castSpellWithSpecialAbility
					|
					`-> tryToRunAway
		*/

		//! ���� ���� ���
		void castAttackSpell(void);
		//! ġ�� ���� ���
		void castCureSpell(void);
		//! ��ȭ ���� ���
		void castPhenominaSpell(void);

		//! ������ �̿��� ����
		void attackWithWeapon(int ix_object, int ix_enemy);
		//! �� ��ü�� ���� ���� ����
		void castSpellToAll(int ix_object);
		//! �� �ϳ��� ���� ���� ����
		void castSpellToOne(int ix_object, int ix_enemy);
		//! ���� esp ���
		void useESP(void);
		//! ������ esp ���
		void useESPForBattle(int ix_object, int ix_enemy);
		//! Ư�� ���� ������ ���
		void castSpellWithSpecialAbility(int ix_object, int ix_enemy);
		//! ������ �õ�
		bool tryToRunAway(void);

	private:
		//! �� ���� ġ��
		void m_healOne(PcPlayer* p_target);
		//! �� ���� �ص�
		void m_antidoteOne(PcPlayer* p_target);
		//! �� ���� �ǽ��� ����
		void m_recoverConsciousnessOne(PcPlayer* p_target);
		//! �� ���� ��Ȱ ��Ŵ
		void m_revitalizeOne(PcPlayer* p_target);
		//! ��θ� ġ��
		void m_healAll(void);
		//! ��θ� �ص�
		void m_antidoteAll(void);
		//! ��θ� �ǽ��� ����
		void m_recoverConsciousnessAll(void);
		//! ��θ� ��Ȱ ��Ŵ
		void m_revitalizeAll(void);
		//! Ư�� ������ ��� �������� Ȯ��
		bool m_canUseSpecialMagic(void);
		//! ESP ��� �������� Ȯ��
		bool m_canUseESP(void);
		//! ����ģ ���� ���� ����ġ�� �߰�
		void m_plusExperience(PcEnemy* p_enemy);
		//! Spell point�� �����ϴٴ� �޽��� ���
		void m_printSpNotEnough(void);
		//! ESP point�� �����ϴٴ� �޽��� ���
		void m_printESPNotEnough(void);

	protected:
		bool _load(const smutil::ReadStream& stream);
		bool _save(const smutil::WriteStream& stream) const;
	};

	smutil::string getBattleMessage(const PcPlayer& who, int how, int what, const PcEnemy& whom);

} // namespace hadar

#endif // #ifndef __HD_CLASS_PC_PLAYER_H__
