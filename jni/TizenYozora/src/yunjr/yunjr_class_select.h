
#ifndef __YUNJR_CLASS_SELECT_H__
#define __YUNJR_CLASS_SELECT_H__

#pragma warning( disable: 4786 )

#include "util/sm_util_string.h"

namespace yunjr
{
	typedef sena::vector<smutil::string> MenuList;

	class MenuSelection
	{
	public:
		MenuSelection(const MenuList& menu, int num_enabled = -1, int ix_initial = 1);

		bool loop(void);

		int operator()(void)
		{
			return m_selected;
		}

	private:
		sena::vector<smutil::string> m_menu;
		int  m_currrent;
		int  m_num_enabled;

		int  m_selected;

		void m_display(const MenuList& menu, int num_menu, int num_enabled, int selected);
	};

	class MenuSelectionUpDown
	{
	public:
		/*
			INPUT PARAMETER
			x: ���� x��ǥ
			y: ���� y��ǥ
			min: ���� �ּ� ��
			max: ���� �ִ� ��
			step: ������
			init: �ʱ� ��
			fgColor: ��»� (index color)
			bgColor: ���� (index color)
		*/
		MenuSelectionUpDown(int x, int y, int min, int max, int step, int init, unsigned long fg_color, unsigned long bg_color);
		/*
			REUTRN VALUE: min~max ������ ���� �� ����, min ���� ������ ����
		*/
		int operator()(void)
		{
			return m_value;
		}

	private:
		int  m_value;
	};

} // namespace yunjr

#endif // #ifndef __YUNJR_CLASS_SELECT_H__
