
#include "yunjr_class_select.h"

#include "yunjr_base_gfx.h"
#include "yunjr_base_key_buffer.h"

#include "yunjr_class_control_lv1.h"
#include "yunjr_class_console.h"
#include "yunjr_class_extern.h"


void yunjr::MenuSelection::m_display(const MenuList& menu, int num_menu, int num_enabled, int selected)
{
	assert(num_menu > 0);
	assert(num_enabled <= num_menu);
	assert(selected > 0 && selected <= num_menu);

	LoreConsole& console = LoreConsole::getConsole();

	console.clear();
	console.setTextColor(0xFFFF0000);
	console.write(menu[0]);
	console.write(L"");

	for (int i = 1; i <= num_menu; ++i)
	{
		console.setTextColor((i == selected) ? 0xFFFFFFFF : ((i <= num_enabled) ? 0xFF808080 : 0xFF000000));
		console.write(menu[i]);
	}

	console.display();
	game::updateScreen();
}

yunjr::MenuSelection::MenuSelection(const MenuList& menu, int num_enabled, int ix_initial)
	: m_currrent(-1)
	, m_selected(0)
{
	int num_menu = menu.size() - 1;

	assert(num_menu > 0);

	if (num_enabled < 0)
		num_enabled = num_menu;

	if (ix_initial < 0)
		ix_initial = 1;
	if (ix_initial > num_menu)
		ix_initial = num_menu;

	MenuList::const_iterator p_item = menu.begin();
	for ( ; p_item != menu.end(); ++p_item)
		m_menu.push_back(*p_item);

	m_currrent = ix_initial;
	m_num_enabled = num_enabled;

	m_display(menu, num_menu, num_enabled, m_currrent);

	KeyBuffer::getKeyBuffer().getKey();

	while (this->loop())
		;
}

bool yunjr::MenuSelection::loop(void)
{
	bool has_been_updated = false;

	unsigned short key;
	while (short(key = KeyBuffer::getKeyBuffer().getKey()) < 0L)
		yunjr::game::window::displayNoti();

	switch (key)
	{
	case KEY_DIR_UP:
	case KEY_DIR_DOWN:
		{
			int dy = (key == KEY_DIR_UP) ? -1 : +1;
			m_currrent += dy;

			if (m_currrent <= 0)
				m_currrent = m_num_enabled;
			if (m_currrent > m_num_enabled)
				m_currrent = 1;

			has_been_updated = true;
		}     
		break;
	case KEY_BTN_B:
		m_currrent = 0;
		// pass through
	case KEY_BTN_A:
		{
			LoreConsole& console = LoreConsole::getConsole();
			console.clear();
			console.display();
		}

		m_selected = m_currrent;

		return false;
	}

	if (has_been_updated)
		m_display(m_menu, m_menu.size() - 1, m_num_enabled, m_currrent);

	return true;
}

//////////////////////////////////////////////////////
// MenuSelectionUpDown

yunjr::MenuSelectionUpDown::MenuSelectionUpDown(int x, int y, int min, int max, int step, int init, unsigned long fgColor, unsigned long bgColor)
	: m_value(init)
{
	ControlConsole* p_console = (ControlConsole*)resource::getMainWindow()->findControl("CONSOLE");

	do
	{
		//? smutil::IntToStr를 2번하니 비효율적
//??	gfx::fillRect(nullptr, bgColor, x, y, 6*strlen(smutil::IntToStr(m_value)()), 12);
		p_console->drawText(x, y, smutil::IntToStr<wchar_t>(m_value)(), fgColor);
		game::updateScreen();

		bool has_been_updated = false;

		do
		{
			unsigned short key;
			while ((key = KeyBuffer::getKeyBuffer().getKey()) < 0)
				;

			switch (key)
			{
			case KEY_DIR_UP:
			case KEY_DIR_DOWN:
			case KEY_DIR_LEFT:
			case KEY_DIR_RIGHT:
				{
					int dy = ((key == KEY_DIR_DOWN) || (key == KEY_DIR_LEFT)) ? -1 : +1;
					m_value += dy * step;

					if (m_value < min)
						m_value = min;
					if (m_value > max)
						m_value = max;

					has_been_updated = true;
				}
				break;
			case KEY_BTN_B:
				m_value = min-1;
				// pass through
			case KEY_BTN_A:
				{
					LoreConsole& console = LoreConsole::getConsole();
					console.clear();
					console.display();
				}
				return;
			}
		} while (!has_been_updated);
		
	} while (1);
}
