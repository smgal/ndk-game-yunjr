
#ifndef __HD_CLASS_PC_ENEMY_H__
#define __HD_CLASS_PC_ENEMY_H__

#pragma warning(disable: 4786)

#include "hd_class_pc_name_base.h"

namespace hadar
{
	struct PcPlayer;

	#pragma pack(push, 1)
	struct EnemyData
	{
		char _name[20];
		unsigned char strength;
		unsigned char mentality;
		unsigned char endurance;
		unsigned char resistance;
		unsigned char agility;
		unsigned char accuracy[2];
		unsigned char ac;
		unsigned char special;
		unsigned char cast_level;
		unsigned char special_cast_level;
		unsigned char level;
	};
	#pragma pack(pop)

	struct PcEnemy: public PcNameBase
	{
		int  strength;
		int  mentality;
		int  endurance;
		int  resistance;
		int  agility;
		int  accuracy[2];
		int  ac;
		int  special;
		int  cast_level;
		int  special_cast_level;
		int  level;

		int  hp;
		int  poison;
		int  unconscious;
		int  dead;

		int  ed_number;

		PcEnemy(void);
		~PcEnemy(void);

		void operator<<(const EnemyData& data);
		void operator<<(const PcPlayer& player);

		bool isConscious(void) const;

		void attack(void);

		//! ���ڿ��� ���� ������ �Ӽ��� ����
		bool changeAttribute(const char* sz_attribute, int value);
		bool changeAttribute(const char* sz_attribute, const char* sz_data);

	protected:
		bool _load(const smutil::ReadStream& stream);
		bool _save(const smutil::WriteStream& stream) const;
	};

	// ���� ���ӿ��� �� �����͸� �̾� ���� �Լ� 1~75���̰� ��ȿ ��
	const EnemyData& getEnemyDataFromEnemyTable(int index);

} // namespace hadar

#endif // #ifndef __HD_CLASS_PC_ENEMY_H__
