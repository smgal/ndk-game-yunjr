
#ifndef __YUNJR_CLASS_CONTROL_LV2_H__
#define __YUNJR_CLASS_CONTROL_LV2_H__

#include "yunjr_base.h"
#include "yunjr_base_gfx.h"

#include "yunjr_class.h"
#include "yunjr_class_control_lv1.h"
#include "yunjr_class_chara.h"

#include <algorithm>
#include <string.h>

namespace yunjr
{
	struct ControlMap: public Control
	{
		struct Attribute;

	public:
		static ControlMap* newInstance(int x, int y, int width, int height);

	private:
		ControlMap();

	};

	struct ControlBattle: public Control
	{
		struct Attribute;

	public:
		void display(bool need_to_clear, int ix_inverted = -1);

		static ControlBattle* newInstance(int x, int y, int width, int height);

	private:
		ControlBattle();

	};
}

#endif // #ifndef __YUNJR_CLASS_CONTROL_LV2_H__
