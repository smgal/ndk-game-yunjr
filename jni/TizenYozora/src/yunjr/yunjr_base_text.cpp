
#include "yunjr_base.h"
#include "yunjr_base_text.h"
#include "yunjr_base_text_impl.h"

#include "util/sm_util_sena.h"

#include <new>
#include <string.h>

yunjr::Text::Text()
	: p_impl(new Impl())
{
}

yunjr::Text::Text(const Typeface& typeface, const char* sz_text_utf8)
{
	new (this) Text(typeface, sz_text_utf8, 0x7FFFFFFF);
}

yunjr::Text::Text(const Typeface& typeface, const char* sz_text_utf8, size_t length)
	: p_impl(0)
{
	// revise string legnth parameter
	{
		size_t real_length = 0;

		const char* p = sz_text_utf8;

		while (*p++ && real_length < length)
			++real_length;

		length = real_length;
	}

	sena::auto_ptr<wchar_t[]> wchar_buffer(new wchar_t[length+1]);

	const char* p_src     = sz_text_utf8;
	wchar_t*    p_dst     = wchar_buffer.get();
	wchar_t*    p_dst_end = p_dst + length;

	while (*p_src && p_dst < p_dst_end)
		*p_dst++ = wchar_t(*p_src++);

	*p_dst = 0;

	p_impl = new Impl(typeface, wchar_buffer.get(), length);
}

yunjr::Text::Text(const Typeface& typeface, const wchar_t* sz_text)
	: p_impl(0)
{
	size_t length = 0;

	const wchar_t* p = sz_text;

	while (*p++)
		++length;

	p_impl = new Impl(typeface, sz_text, length);
}

yunjr::Text::Text(const Typeface& typeface, const wchar_t* sz_text, size_t length)
	: p_impl(new Impl(typeface, sz_text, length))
{
}

yunjr::Text::~Text()
{
	delete p_impl;
}

bool yunjr::Text::isEmpty(void)
{
	return (this->p_impl->text_length == 0);
}

void yunjr::Text::setColor(unsigned long color)
{
	std::vector<GlyphInfo*>::iterator pp_glyph_info = this->p_impl->glyph_info.begin();

	for ( ; pp_glyph_info != this->p_impl->glyph_info.end(); ++pp_glyph_info)
	{
		(*pp_glyph_info)->user_data = color;
	}
}

void yunjr::Text::split(int width, Text& remaining_text, ALIGNMETHOD align)
{
	remaining_text.p_impl->reset();

	std::vector<GlyphInfo*> stream1_1, stream1_2;
	std::vector<GlyphInfo*> stream2_1, stream2_2;

	std::vector<GlyphInfo*>::iterator pp_glyph_info1 = this->p_impl->glyph_info.begin();
	std::vector<GlyphInfo*>::iterator pp_glyph_info2 = this->p_impl->glyph_shadow.begin();

	bool has_shadow = (this->p_impl->glyph_info.size() <= this->p_impl->glyph_shadow.size());

	int x = 0;

	for ( ; pp_glyph_info1 != this->p_impl->glyph_info.end(); ++pp_glyph_info1)
	{
		int x2 = x + (*pp_glyph_info1)->x_advance;

		if (x2 < width)
		{
			stream1_1.push_back(*pp_glyph_info1);
			if (has_shadow)
				stream1_2.push_back(*pp_glyph_info2);
		}
		else
		{
			stream2_1.push_back(*pp_glyph_info1);
			if (has_shadow)
				stream2_2.push_back(*pp_glyph_info2);
		}

		x = x2;

		if (has_shadow)
			++pp_glyph_info2;
	}

	int text_length1 = stream1_1.size();
	int text_length2 = this->p_impl->text_length - text_length1;

	// remaining_text
	{
		remaining_text.p_impl->typeface = this->p_impl->typeface;
		remaining_text.p_impl->text_length = text_length2;
		remaining_text.p_impl->p_text.bind(new wchar_t[text_length2 + 1]);

		// include NULL
		memcpy(remaining_text.p_impl->p_text.get(), &this->p_impl->p_text[0] + text_length1, sizeof(wchar_t) * (text_length2 + 1));

		remaining_text.p_impl->glyph_info.swap(stream2_1);
		remaining_text.p_impl->glyph_shadow.swap(stream2_2);
	}

	// splited text
	{
		wchar_t* p_temp = new wchar_t[text_length1 + 1];
		memcpy(p_temp, &this->p_impl->p_text[0], sizeof(wchar_t) * text_length1);
		p_temp[text_length1] = 0;

		this->p_impl->text_length = text_length1;
		this->p_impl->p_text.reset(p_temp);

		this->p_impl->glyph_info.swap(stream1_1);
		this->p_impl->glyph_shadow.swap(stream1_2);
	}

	if (text_length2 > 0)
	{
		align = ALIGNMETHOD_FIT;

		switch (align)
		{
		case ALIGNMETHOD_FIT:
			{
				std::vector<GlyphInfo*>::iterator ppGlyphInfo = this->p_impl->glyph_info.begin();

				int x = 0;
				int x_fit_offset = 0;

				for ( ; ppGlyphInfo != this->p_impl->glyph_info.end(); ++ppGlyphInfo)
				{
					x += (*ppGlyphInfo)->x_advance;
					x_fit_offset = ((*ppGlyphInfo)->left + (*ppGlyphInfo)->glyph.width) - (*ppGlyphInfo)->x_advance;
				}

				x += x_fit_offset;

				int remaining_pixel = width - x;

				if (this->p_impl->glyph_info.size() > 0)
				{
					//?? temporary
					int adding_pixel = remaining_pixel / this->p_impl->glyph_info.size();

					//
					{
						ppGlyphInfo = this->p_impl->glyph_info.begin();

						for ( ; ppGlyphInfo != this->p_impl->glyph_info.end(); ++ppGlyphInfo)
							(*ppGlyphInfo)->x_advance += adding_pixel;
					}

					//
					if (has_shadow)
					{
						ppGlyphInfo = this->p_impl->glyph_shadow.begin();

						for ( ; ppGlyphInfo != this->p_impl->glyph_shadow.end(); ++ppGlyphInfo)
							(*ppGlyphInfo)->x_advance += adding_pixel;
					}
				}
			}
			break;
		case ALIGNMETHOD_RIGHT:
		case ALIGNMETHOD_CENTER:
			break;
		case ALIGNMETHOD_LEFT:
		default:
			break;
		}
	}

}

int yunjr::Text::getExtents(void)
{
	std::vector<GlyphInfo*>::iterator pp_glyph_info1 = this->p_impl->glyph_info.begin();

	int x = 0;

	for ( ; pp_glyph_info1 != this->p_impl->glyph_info.end(); ++pp_glyph_info1)
	{
		x += (*pp_glyph_info1)->x_advance;
	}

	return x;
}
