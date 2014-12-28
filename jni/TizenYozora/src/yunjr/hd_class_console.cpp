
#include "hd_class_console.h"

#include "util/sm_util_sena.h"

#include "hd_base_config.h"
#include "hd_base_gfx.h"

#include "hd_class_extern.h"

////////////////////////////////////////////////////////////////////////////////
//

namespace
{
	//?? �̱��浵 �ƴѵ�?
	int m_y_line  = 0;
	int m_ix_line = 0;

	typedef sena::vector<hadar::gfx::BltParam> BltParamList;

	BltParamList s_blit_list;

	void m_renderText(int x_dst, int y_dst, int width, int height, int x_src, int y_src, unsigned long color)
	{
		s_blit_list.push_back(hadar::gfx::BltParam(x_dst, y_dst, width, height, x_src, y_src, color));
	}

	unsigned long s_renderText(int x, int y, const char* sz_text_begin, const char* sz_text_end, unsigned long init_color, unsigned long default_color)
	{
		s_blit_list.clear();

//??		unsigned long last_color = hadar::gfx::drawFormatedText(x, y, sz_text_begin, sz_text_end, init_color, default_color, m_renderText);

		s_blit_list.push_back(hadar::gfx::BltParam());

//??		return last_color;
		return 0;
	}
}

namespace
{
	template <class type>
	class FnDeleteArray
	{
	public:
		void operator()(type obj)
		{
			delete[] obj;
		}
	};
}

////////////////////////////////////////////////////////////////////////////////
//

hadar::LoreConsole::LoreConsole(void)
	: m_is_modified(true)
	, m_text_color(0xFFFFFFFF)
	, m_bg_color(config::DEFAULT_BG_COLOR)
	, m_align(TEXTALIGN_LEFT)
	, m_x_offset(260)
	, m_y_offset(30)
	, m_width(360)
	, m_height(192)
{
}

hadar::LoreConsole::~LoreConsole(void)
{
	sena::for_each(m_line.begin(), m_line.end(), FnDeleteArray<gfx::BltParam*>());
}

bool hadar::LoreConsole::isModified(void)
{
	return m_is_modified;
}

void hadar::LoreConsole::clear(void)
{
	sena::for_each(m_line.begin(), m_line.end(), FnDeleteArray<gfx::BltParam*>());

	m_line.clear();

	m_y_line  = 0;
	m_ix_line = 0;

	gfx::fillRect(nullptr, m_bg_color, m_x_offset, m_y_offset, m_width, m_height);

	m_is_modified = true;
}

void hadar::LoreConsole::setBgColor(unsigned long color)
{
	m_bg_color = color;
}

unsigned long hadar::LoreConsole::getBgColor(void)
{
	return m_bg_color;
}

void hadar::LoreConsole::setTextColor(unsigned long color)
{
	m_text_color = color;
}

void hadar::LoreConsole::setTextColorIndex(unsigned long index)
{
	m_text_color = gfx::getColorFromIndexedColor(index, 0xFFFFFFFF);
}

void hadar::LoreConsole::setTextAlign(TEXTALIGN align)
{
	m_align = align;
}

void hadar::LoreConsole::write(const smutil::string text)
{
	this->write(text.operator const char*());
}

void hadar::LoreConsole::write(const char* sz_text)
{
	if (sz_text == NULL)
		return;

	if (*sz_text == 0)
	{
		m_line.push_back(NULL);
		return;
	}

	unsigned long real_color = m_text_color;

	const char* sz_temp = sz_text;

	while (*sz_text)
	{
		int x_curr_pos = 0;
		int num_char = 0;

		while (*sz_temp)
		{
			if (*sz_temp == '@')
			{
				if (*(++sz_temp))
					++sz_temp;
			}
			else
			{
				int add = (*sz_temp & 0x80) ? config::DEFAULT_FONT_WIDTH*2 : config::DEFAULT_FONT_WIDTH;
				if ((x_curr_pos+add) > m_width)
					break;
				x_curr_pos += add;
				sz_temp += (add / config::DEFAULT_FONT_WIDTH);
				++num_char;
			}
		}

		const char* sz_back = sz_temp;
		// ������ ���� ����
		{
			while ((sz_back > sz_text) && (*(sz_back-1) == ' '))
			{
				--sz_back;
				--num_char;
			}
		}

		if (num_char <= 0)
			break;

#if 0
		// Left aligned word-wrap
		s_renderText(0, 0, sz_text, sz_temp, real_color);
#else
		// Center aligned word-wrap
		real_color = s_renderText(0, 0, sz_text, sz_back, real_color, m_text_color);

		// align
		if (num_char > 1)
		{
			if (*sz_temp)
			{
				int ix_last_char = s_blit_list.size() - 2;

				// ���ڿ��� �¿� ����
				int padding = m_width - (s_blit_list[ix_last_char].dst_bound.x1 + (s_blit_list[ix_last_char].src_bound.x2 - s_blit_list[ix_last_char].src_bound.x1));
				if (padding > 0)
				{
					int ix_blit_list = 0;

					for (int i = 1; i < num_char-1; i++)
					{
						int guide_x1 = s_blit_list[ix_blit_list].dst_bound.x1;
						int pixel_adding = padding * i / num_char;

						while (s_blit_list[ix_blit_list].dst_bound.x1 == guide_x1)
							s_blit_list[ix_blit_list++].dst_bound.x1 += pixel_adding;
					}

					while (ix_blit_list <= ix_last_char)
						s_blit_list[ix_blit_list++].dst_bound.x1 += padding;
				}
			}
			else
			{
				// ���ڿ��� ������ ���̱� ������ ������ align ����, �ƹ� �͵� ���� ������ TEXTALIGN_LEFT
				switch (m_align)
				{
				case TEXTALIGN_CENTER:
					{
						int x_offset = (m_width - x_curr_pos) / 2;

						BltParamList::iterator blit_param = s_blit_list.begin();
						BltParamList::iterator blit_param_end = s_blit_list.end();

						while (blit_param != blit_param_end)
							(blit_param++)->dst_bound.x1 += x_offset;
					}
					break;
				case TEXTALIGN_RIGHT:
					{
						int x_offset = (m_width - x_curr_pos);

						BltParamList::iterator blit_param = s_blit_list.begin();
						BltParamList::iterator blit_param_end = s_blit_list.end();

						while (blit_param != blit_param_end)
							(blit_param++)->dst_bound.x1 += x_offset;
					}
					break;
				default:
					break;
				}
			}
		}
#endif

		gfx::BltParam* p_blt_list = new gfx::BltParam[s_blit_list.size()];
		//?? ����
		sena::memcpy(p_blt_list, &s_blit_list[0], sizeof(gfx::BltParam) * s_blit_list.size());

		m_line.push_back(p_blt_list);

		while ((*sz_temp) && (*sz_temp == ' '))
			++sz_temp;

		sz_text = sz_temp;
	}

	m_is_modified = true;
}

void hadar::LoreConsole::display(void)
{
	if ((m_ix_line < 0) || (m_ix_line >= int(m_line.size())))
	{
		game::updateScreen();
		return;
	}

	while (m_ix_line < int(m_line.size()))
	{
		gfx::BltParam* p_line = m_line[m_ix_line];

		if (p_line)
			gfx::drawBltList(m_x_offset, m_y_offset + m_y_line, p_line);

		m_y_line += config::DEFAULT_FONT_HEIGHT;

		if (m_y_line >= m_height)
		{
			game::updateScreen();
			game::waitForAnyKey();

#if 0
			//?? ���� ����
			p_back_buffer->bitBlt(m_x_offset, m_y_offset, p_back_buffer, m_x_offset, m_y_offset+config::DEFAULT_FONT_HEIGHT, m_width, m_height-config::DEFAULT_FONT_HEIGHT);
			p_back_buffer->fillRect(m_bg_color, m_x_offset, m_y_offset+m_height-config::DEFAULT_FONT_HEIGHT, m_width, config::DEFAULT_FONT_HEIGHT);
			m_y_line -= config::DEFAULT_FONT_HEIGHT;
#else
			gfx::fillRect(nullptr, m_bg_color, m_x_offset, m_y_offset, m_width, m_height);
			m_ix_line -= (m_height / config::DEFAULT_FONT_HEIGHT - 1);
			m_y_line   = 0;
#endif
		}
		++m_ix_line;
	}
	game::updateScreen();
}

bool hadar::LoreConsole::setRegion(int x, int y, int w, int h)
{
	if ((x < 0) || (y < 0))
		return false;

	m_x_offset = x;
	m_y_offset = y;
	m_width   = w;
	m_height  = h;

	return true;
}

void hadar::LoreConsole::getRegion(int* p_out_x, int* p_out_y, int* p_out_width, int* p_out_height) const
{
	if (p_out_x)
		*p_out_x = m_x_offset;
	if (p_out_y)
		*p_out_y = m_y_offset;
	if (p_out_width)
		*p_out_width = m_width;
	if (p_out_height)
		*p_out_height = m_height;
}

hadar::LoreConsole& hadar::LoreConsole::getConsole(void)
{
	return sena::singleton<LoreConsole>::get();
}
