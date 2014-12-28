
#include "hd_base_config.h"
#include "hd_base_gfx.h"
#include "hd_base_key_buffer.h"

#include "hd_class_extern.h"
#include "hd_class_select.h"
#include "hd_class_console.h"

#include <vector>

void hadar::MenuSelection::m_display(const MenuList& menu, int num_menu, int num_enabled, int selected)
{
	assert(num_menu > 0);
	assert(num_enabled <= num_menu);
	assert(selected > 0 && selected <= num_menu);

	LoreConsole& console = LoreConsole::getConsole();

	console.clear();
	console.setTextColor(0xFFFF0000);
	console.write(menu[0]);
	console.write("");

	for (int i = 1; i <= num_menu; ++i)
	{
		console.setTextColor((i == selected) ? 0xFFFFFFFF : ((i <= num_enabled) ? 0xFF808080 : 0xFF000000));
		console.write(menu[i]);
	}

	console.display();
	game::updateScreen();
}

hadar::MenuSelection::MenuSelection(const MenuList& menu, int num_enabled, int ix_initial)
	: m_selected(0)
{
	int num_menu = menu.size() - 1;

	assert(num_menu > 0);

	if (num_enabled < 0)
		num_enabled = num_menu;

	if (ix_initial < 0)
		ix_initial = 1;
	if (ix_initial > num_menu)
		ix_initial = num_menu;

	int selected = ix_initial;

	do
	{
		m_display(menu, num_menu, num_enabled, selected);

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
				{
					int dy = (key == KEY_DIR_UP) ? -1 : +1;
					selected += dy;

					if (selected <= 0)
						selected = num_enabled;
					if (selected > num_enabled)
						selected = 1;

					has_been_updated = true;
				}
				break;
			case KEY_BTN_A:
				selected = 0;
				// pass through
			case KEY_BTN_B:
				{
					LoreConsole& console = LoreConsole::getConsole();
					console.clear();
					console.display();
				}

				m_selected = selected;

				return;
			}
		} while (!has_been_updated);
		
	} while (1);
}

//////////////////////////////////////////////////////
// MenuSelectionUpDown

hadar::MenuSelectionUpDown::MenuSelectionUpDown(int x, int y, int min, int max, int step, int init, unsigned long fgColor, unsigned long bgColor)
	: m_value(init)
{
	do
	{
		//@@ smutil::IntToStr를 2번하니 비효율적
		gfx::fillRect(nullptr, bgColor, x, y, 6*strlen(smutil::IntToStr(m_value)()), 12);
		gfx::drawText(x, y, smutil::IntToStr(m_value)(), fgColor);
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
			case KEY_BTN_A:
				m_value = min-1;
				// pass through
			case KEY_BTN_B:
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
