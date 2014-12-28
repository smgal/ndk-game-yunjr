
#include "yunjr_class_console.h"
#include "yunjr_class_extern.h"

#include "util/sm_util_sena.h"

//#include "hd_base_config.h"
//#include "hd_base_gfx.h"


////////////////////////////////////////////////////////////////////////////////
//

namespace
{
	//?? 싱글톤도 아닌데?
	int m_y_line  = 0;
	int m_ix_line = 0;
/*
	typedef sena::vector<yunjr::gfx::BltParam> BltParamList;

	BltParamList s_blit_list;

	void m_renderText(int x_dst, int y_dst, int width, int height, int x_src, int y_src, unsigned long color)
	{
		s_blit_list.push_back(yunjr::gfx::BltParam(x_dst, y_dst, width, height, x_src, y_src, color));
	}

	unsigned long s_renderText(int x, int y, const char* sz_text_begin, const char* sz_text_end, unsigned long init_color, unsigned long default_color)
	{
		s_blit_list.clear();

//??		unsigned long last_color = yunjr::gfx::drawFormatedText(x, y, sz_text_begin, sz_text_end, init_color, default_color, m_renderText);

		s_blit_list.push_back(yunjr::gfx::BltParam());

//??		return last_color;
		return 0;
	}
*/
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

	unsigned long getColorFromIndexedColor(unsigned long index, unsigned long default_color)
	{
		enum { MAX_COLOR_TABLE = 16 };

		static const unsigned long COLOR_TABLE[MAX_COLOR_TABLE] =
		{
			0xFF000000, 0xFF000080, 0xFF008000, 0xFF008080,
			0xFF800000, 0xFF800080, 0xFF808000, 0xFFA0A0A0,
			0xFF404040, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
			0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF
		};

		return (index < sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0])) ? COLOR_TABLE[index] : default_color;
	}
}

namespace yunjr
{
	namespace config
	{
		const long DEFAULT_FONT_WIDTH = 16;
		const unsigned long DEFAULT_BG_COLOR = 0xFF525252;
	}
}

////////////////////////////////////////////////////////////////////////////////
//

yunjr::LoreConsole::LoreConsole(void)
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

yunjr::LoreConsole::~LoreConsole(void)
{
//??	sena::for_each(m_line.begin(), m_line.end(), FnDeleteArray<gfx::BltParam*>());
}

bool yunjr::LoreConsole::isModified(void)
{
	return m_is_modified;
}

void yunjr::LoreConsole::clear(void)
{
	yunjr::game::console::clear();

//??	sena::for_each(m_line.begin(), m_line.end(), FnDeleteArray<gfx::BltParam*>());

	m_line.clear();

	m_y_line  = 0;
	m_ix_line = 0;

//??	gfx::fillRect(nullptr, m_bg_color, m_x_offset, m_y_offset, m_width, m_height);

	m_is_modified = true;
}

void yunjr::LoreConsole::setBgColor(unsigned long color)
{
	m_bg_color = color;
}

unsigned long yunjr::LoreConsole::getBgColor(void)
{
	return m_bg_color;
}

void yunjr::LoreConsole::setTextColor(unsigned long color)
{
	m_text_color = color;
}

void yunjr::LoreConsole::setTextColorIndex(unsigned long index)
{
	m_text_color = getColorFromIndexedColor(index, 0xFFFFFFFF);
}

void yunjr::LoreConsole::setTextAlign(TEXTALIGN align)
{
	m_align = align;
}

void yunjr::LoreConsole::write(const smutil::string& text)
{
	this->write(text.operator const wchar_t*());
}

void yunjr::LoreConsole::write(const wchar_t* sz_text)
{
	yunjr::game::console::writeLine(sz_text, m_text_color);
}

void yunjr::LoreConsole::write(const char* sz_text, int dummy)
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
		// 마지막 공백 제거
		{
			while ((sz_back > sz_text) && (*(sz_back-1) == ' '))
			{
				--sz_back;
				--num_char;
			}
		}

		if (num_char <= 0)
			break;

#if 0 //??

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

				// 문자열을 좌우 정렬
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
				// 문자열의 마지막 행이기 때문에 지정된 align 적용, 아무 것도 하지 않으면 TEXTALIGN_LEFT
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
		//?? 위험
		sena::memcpy(p_blt_list, &s_blit_list[0], sizeof(gfx::BltParam) * s_blit_list.size());

		m_line.push_back(p_blt_list);

		while ((*sz_temp) && (*sz_temp == ' '))
			++sz_temp;

		sz_text = sz_temp;
#endif
	}

	m_is_modified = true;
}

void yunjr::LoreConsole::display(void)
{
	if ((m_ix_line < 0) || (m_ix_line >= int(m_line.size())))
	{
		game::updateScreen();
		return;
	}
#if 0 //??
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
			//?? 문제 있음
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
#endif
	game::updateScreen();
}

//??
bool yunjr::LoreConsole::setRegion(int x, int y, int w, int h)
{
	if ((x < 0) || (y < 0))
		return false;

	m_x_offset = x;
	m_y_offset = y;
	m_width   = w;
	m_height  = h;

	return true;
}

void yunjr::LoreConsole::getRegion(int* p_out_x, int* p_out_y, int* p_out_width, int* p_out_height) const
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

yunjr::LoreConsole& yunjr::LoreConsole::getConsole(void)
{
	return sena::singleton<LoreConsole>::get();
}
