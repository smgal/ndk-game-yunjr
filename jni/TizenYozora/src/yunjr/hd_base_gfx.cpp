
#include "hd_base_gfx.h"

//??#include "util/util_render_text.h"
//??#include "util/util_convert_to_ucs.h"
#include "util/sm_util_sena.h"

#include "hd_base_config.h"

//??#include "hd_res_instance.h"

#include <cstring>

//??using hadar::util::widechar;


unsigned long hadar::gfx::getColorFromIndexedColor(unsigned long index, unsigned long default_color)
{
	enum { MAX_COLOR_TABLE = 16 };

	static const unsigned long COLOR_TABLE[MAX_COLOR_TABLE] =
	{
		0xFF000000, 0xFF000080, 0xFF008000, 0xFF008080,
		0xFF800000, 0xFF800080, 0xFF808000, 0xFF808080,
		0xFF404040, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
		0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF
	};

	return (index < sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0])) ? COLOR_TABLE[index] : default_color;
}

namespace
{
	template <typename pixel>
	void s_drawBltList(int x, int y, hadar::gfx::BltParam rect[], int num_rect)
	{
		static bool s_is_first = true;
		static unsigned long s_white;

		if (s_is_first)
		{
//??			ASSERT(hadar::p_back_buffer);
			s_white = 0xFFFFFFFF;
			s_is_first = false;
		}

		for (int i = 0; i < num_rect; i++)
		{
			if (!rect[i].is_available)
				break;

			unsigned long color[4] = { rect[i].color, rect[i].color, rect[i].color, rect[i].color };

//??			hadar::p_back_buffer->bitBlt(x + rect[i].dst_bound.x1, y + rect[i].dst_bound.y1, hadar::p_font_image.get(), rect[i].src_bound.x1, rect[i].src_bound.y1, rect[i].src_bound.x2 - rect[i].src_bound.x1, rect[i].src_bound.y2 - rect[i].src_bound.y1, color);
		}
	}

	void renderText(int x_dest, int y_dest, int width, int height, int x_sour, int y_sour, unsigned long color)
	{
		unsigned long color_list[4] = { color, color, color, color };
//??		hadar::p_back_buffer->bitBlt(x_dest, y_dest, hadar::p_font_image.get(), x_sour, y_sour, width, height, color_list);
	}

}

void hadar::gfx::drawBltList(int x, int y, hadar::gfx::BltParam rect[], int num_rect)
{
	s_drawBltList<hadar::Pixel>(x, y, rect, num_rect);
}

void hadar::gfx::drawText(int x, int y, const char* sz_text, unsigned long color)
{
	const char* sz_text_end = sz_text + strlen(sz_text);
//??	gfx::drawFormatedText(x, y, sz_text, sz_text_end, color, color, renderText);
}
/* //??
unsigned long hadar::gfx::drawFormatedText(int x, int y, const char* sz_text_begin, const char* sz_text_end, unsigned long init_color, unsigned long default_color, hadar::util::TextRenderer::FnBitBlt fn_bitblt)
{
	// �׻� UCS-2 ������ wchar_t�� ������ �Ǿ�� �Ѵ�.
	CT_ASSERT(sizeof(widechar) == sizeof(wchar_t), QAZ);

	static const hadar::util::TextRenderer& ref_render_text = *hadar::util::TextRenderer::getTextInstance();

	if (sz_text_begin == NULL || sz_text_end == NULL || sz_text_begin >= sz_text_end )
		return init_color;

	//?? ����� �� ������� ����
	static widechar w_str[256];

	// KSC-5601�� UCS-2�� ����
	{
		widechar* w_str_end = hadar::util::convertUhcToUcs2(w_str, 256, sz_text_begin, sz_text_end - sz_text_begin);
		*w_str_end = 0;
	}

	unsigned long current_color = init_color;

	{
		const int DEFAULT_INDEX_COLOR = 15;

		widechar* p_str_begin  = w_str;
		widechar* p_str_end    = w_str;
		int       x_end        = x;

		while (*p_str_end)
		{
			if (*p_str_end == widechar('@'))
			{
				// NULL ���ڷ� ������, '@'�� NULL�� ġȯ�Ǵ� ���̱� ������ �����ص� ���� ����
				*p_str_end = 0;

				// blitting list �����
				ref_render_text.renderText(x, y, p_str_begin, current_color, fn_bitblt);

				// x ���۰� ����
				x = x_end;

				// current_color ����
				{
					widechar index_char = *(++p_str_end);
					int      index_color = -1;

					if      (index_char >= widechar('0') && index_char <= widechar('9'))
						index_color = index_char - '0';
					else if (index_char >= widechar('A') && index_char <= widechar('Z'))
						index_color = index_char - 'A' + 10;
					else if (index_char >= widechar('a') && index_char <= widechar('z'))
						index_color = index_char - widechar('A') + 10;
					else if (index_char == widechar('@'))
						current_color = default_color; // ����

					if (index_color >= 0)
						current_color = gfx::getColorFromIndexedColor(index_color, default_color);
				}

				// ���ڿ��� ������ ����
				p_str_begin = ++p_str_end;
			}
			else
			{
				x_end += (*p_str_end > 0x00FF) ? config::DEFAULT_FONT_WIDTH*2 : config::DEFAULT_FONT_WIDTH;
				++p_str_end;
			}
		}

		// ���� ���ڿ��� ���� blitting list �����
		ref_render_text.renderText(x, y, p_str_begin, current_color, fn_bitblt);
	}

	return current_color;
}
*/
////////////////////////////////////////////////////////////////////////////////

namespace hadar
{
//??	extern sena::vector<sena::shared_ptr<avej::GfxSurface> > font_res_list;
}

void hadar::gfx::setFont(FONT font)
{
	int ix_font = -1;

	switch (font)
	{
	case FONT_SERIF:
		ix_font = 0;
		break;
	case FONT_SANS:
		ix_font = 1;
		break;
	case FONT_DECO:
		ix_font = 2;
		break;
	}

//??	if ((ix_font >= 0) && (ix_font < hadar::font_res_list.size()))
//??		hadar::p_font_image = hadar::font_res_list[ix_font];
}

void hadar::gfx::fillRect(void*, unsigned long color, int x, int y, int width, int height)
{
//??	hadar::p_back_buffer->fillRect(color, x, y, width, height);
}
