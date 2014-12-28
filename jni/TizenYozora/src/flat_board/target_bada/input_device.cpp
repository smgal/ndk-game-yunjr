
#include "../target_dep.h"
#include <FUiTouch.h>

void* g_h_native_window = 0;

namespace
{
	struct TRect
	{
		int x1, y1, x2, y2;
	};

	bool s_InRect(int x, int y, const TRect& rect)
	{
		return (x >= rect.x1 && x < rect.x2 && y >= rect.y1 && y < rect.y2);
	}

	const int X1 = 1280 * 0 / 3;
	const int X2 = 1280 * 1 / 3;
	const int X3 = 1280 * 2 / 3;
	const int X4 = 1280 * 3 / 3;

	const int Y1 = 720 * 0 / 3;
	const int Y2 = 720 * 1 / 3;
	const int Y3 = 720 * 2 / 3;
	const int Y4 = 720 * 3 / 3;

	struct TRectInfo
	{
		TRect        rect;
		target::KEY key;
	} HIT_RECT[] =
	{
		// up
		{ { X2, Y1, X3, Y2 }, target::KEY_UP },
		// down
		{ { X2, Y3, X3, Y4 }, target::KEY_DOWN },
		// left
		{ { X1, Y2, X2, Y3 }, target::KEY_LEFT },
		// right
		{ { X3, Y2, X4, Y3 }, target::KEY_RIGHT },
	};

	bool s_is_pressed = false;
	int  s_pressed_x  = 0;
	int  s_pressed_y  = 0;

	unsigned long s_GetButtons(void)
	{
		s_is_pressed = false;

		Tizen::Ui::Touch g_touch;

		static bool s_is_first = true;

		if (s_is_first)
		{
			g_touch.SetMultipointEnabled(*((Tizen::Ui::Control*)g_h_native_window), true);
			s_is_first = false;
		}

		unsigned long flags = 0;

		Tizen::Base::Collection::IList* pList = g_touch.GetTouchInfoListN();

		if (pList)
		{
			for (int i = 0; i < pList->GetCount(); i++)
			{
				Tizen::Ui::TouchInfo* p_touch_info = (Tizen::Ui::TouchInfo*)pList->GetAt(i);

				if (p_touch_info && p_touch_info->status == Tizen::Ui::TOUCH_PRESSED)
				{

					int x = p_touch_info->position.x;
					int y = p_touch_info->position.y;

					if (!s_is_pressed)
					{
						s_is_pressed = true;
						s_pressed_x  = x;
						s_pressed_y  = y;
					}

					for (int key = 0; key < int(sizeof(HIT_RECT)/sizeof(HIT_RECT[0])); key++)
					{
						if (s_InRect(x, y, HIT_RECT[key].rect))
						{
							flags |= HIT_RECT[key].key;
						}
					}
				}
			}

			pList->RemoveAll(true);
			delete pList;
		}

		return flags;
	}
}

void target_setTouchPos(int x, int y)
{
	s_is_pressed = (s_pressed_x >= 0);
	s_pressed_x  = x;
	s_pressed_y  = y;
}

namespace target
{
	class CInputDevice: public InputDevice
	{
	public:
		CInputDevice()
		{
			m_update_info.time_stamp       = 0;
			m_update_info.is_touched       = false;
			m_update_info.touch_pos.x      = 0;
			m_update_info.touch_pos.y      = 0;
			m_update_info.key_down_flag    = 0;
			m_update_info.key_pressed_flag = 0;
		}
		virtual ~CInputDevice()
		{
		}

		virtual const InputUpdateInfo& update(void)
		{
			//m_update_info.key_pressed_flag = s_GetButtons();
			m_update_info.is_touched       = s_is_pressed;
			m_update_info.touch_pos.x      = s_pressed_x;
			m_update_info.touch_pos.y      = s_pressed_y;

			return m_update_info;
		}
		virtual const InputUpdateInfo& getUpdateInfo(void) const
		{
			return m_update_info;
		}
		virtual bool  checkKeyDown(KEY key)
		{
			return false;
		}
		virtual bool  checkKeyPressed(KEY key)
		{
			return ((m_update_info.key_pressed_flag & key) != 0);
		}

	private:
		InputUpdateInfo m_update_info;

	};
}

target::InputDevice* target::getInputDevice(void)
{
	static target::CInputDevice s_input_device;

	return &s_input_device;
}
