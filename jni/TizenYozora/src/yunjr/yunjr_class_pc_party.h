
#ifndef __YUNJR_CLASS_PC_PARTY_H__
#define __YUNJR_CLASS_PC_PARTY_H__

#include "yunjr_base_serialized.h"

namespace yunjr
{
	struct PcParty: public Serialized
	{
		enum POS
		{
			POS_PREV,
			POS_ABS,
			POS_REL,
		};

		union
		{
			struct
			{
				long x, y;
				long x_prev, y_prev;
				long faced;
				long faced_x1, faced_y1;
				long encounter;
				long max_enemy;

				long food;
				long gold;

				struct
				{
					long magic_torch;
					long levitation;
					long walk_on_water;
					long walk_on_swamp;
					long mind_control;
					bool can_use_ESP; // etc[39] and bit1 > 0
					bool can_use_special_magic; // etc[38] and bit1 > 0
				} ability;
			};

			char save[20 * sizeof(long)];
		};

		PcParty(void);

		void move(int x1, int y1);
		void warp(POS pos, int x = 0, int y = 0);
		void face(int x1, int y1);

		void igniteTorch(void);
		void levitate(void);
		void walkOnWater(void);
		void walkOnSwamp(void);

	protected:
		bool _load(const smutil::ReadStream& stream);
		bool _save(const smutil::WriteStream& stream) const;
	};

} // namespace yunjr

#endif // #ifndef __YUNJR_CLASS_PC_PARTY_H__
