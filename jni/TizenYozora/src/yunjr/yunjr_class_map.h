
#ifndef __YUNJR_CLASS_MAP_H__
#define __YUNJR_CLASS_MAP_H__

#include "util/sm_util_set.h"

#include "yunjr_base_serialized.h"

namespace yunjr
{
	struct Map: public Serialized
	{
		enum
		{
			MAX_MAP_TILE = 56
		};

		enum ACT
		{
			ACT_BLOCK = 0,
			ACT_MOVE,
			ACT_EVENT,
			ACT_ENTER,
			ACT_SIGN,
			ACT_WATER,
			ACT_SWAMP,
			ACT_LAVA,
			ACT_TALK,
			ACT_MAX,
			ACT_DWORD = 0x7FFFFFFF
		};

		typedef void (*FnAct)(int x1, int y1, bool);

		enum TYPE
		{
			TYPE_TOWN   = 0,
			TYPE_KEEP   = 1,
			TYPE_GROUND = 2,
			TYPE_DEN    = 3,
			TYPE_DWORD  = 0x7FFFFFFF
		};

		enum HANDICAP
		{
			HANDICAP_TILECHANGING = 0, // ���� ��ȭ 
			HANDICAP_TELEPORT,         // ���� �̵�
			HANDICAP_SEETHROUGH,       // ����
			HANDICAP_CLAIRVOYANCE,     // õ����
			HANDICAP_MAX
		};

		// save; begin
		smutil::SmSet jumpable;
		smutil::SmSet teleportable;

		union
		{
			struct
			{
				TYPE  type;
				long  width, height;
				long  encounter_offset, encounter_range;
				long  tile4way;
				unsigned char data[256][256];
				unsigned char handicap_data[HANDICAP_MAX];
			};

			char save[(256*256)*1 + sizeof(long)*10];
		};
		// save; end

		FnAct act_func[ACT_MAX];
		FnAct act_list[MAX_MAP_TILE];

		inline unsigned char operator()(int x, int y) const	{ return data[y][x] & 0x3F; }

		void clearData(void);

		void changeMap(int x, int y, unsigned char tile);
		void changeToWay(int x, int y);
		void setType(TYPE _type);

		int  getEncounteredEnemy(void);

		bool isJumpable(int x, int y);
		bool isEventPos(int x, int y);
		bool isTeleportable(int x, int y);

		void setHandicap(HANDICAP handicap);
		bool hasHandicap(HANDICAP handicap);

		void setLight(int x, int y);
		void resetLight(int x, int y);
		bool hasLight(int x, int y) const;

	protected:
		bool _load(const smutil::ReadStream& stream);
		bool _save(const smutil::WriteStream& stream) const;
	};

} // namespace yunjr

#endif // #ifndef __YUNJR_CLASS_MAP_H__
