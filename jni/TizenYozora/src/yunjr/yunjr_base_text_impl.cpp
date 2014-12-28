
#include "yunjr_base.h"
#include "yunjr_base_text_impl.h"

#include "yunjr_res.h"

#include <string.h>

// goto to config file
const int EXPANDING_PIXEL = 2;

yunjr::Text::Impl::Impl()
	: p_text(new wchar_t[1])
	, text_length(0)
{
	this->typeface.font = resource::getCurrentFont();
	p_text[0] = 0;
}

yunjr::Text::Impl::Impl(const Typeface& typeface, const wchar_t* sz_text, size_t length)
	: p_text(new wchar_t[length+1])
	, text_length(length)
{
	this->typeface = typeface;
	
	if (this->typeface.font.isNull())
		this->typeface.font = resource::getCurrentFont();

	memcpy(p_text.get(), sz_text, sizeof(wchar_t) * length);
	p_text[length] = 0;

	this->typeface.apply();
	this->typeface.font->load(p_text.get(), length, glyph_info);

	{
		std::vector<GlyphInfo*>::const_iterator pp_glyph = glyph_info.begin();

		for ( ; pp_glyph != glyph_info.end(); ++pp_glyph)
		{
			int buffer_width = (*pp_glyph)->glyph.width + EXPANDING_PIXEL * 2;
			int buffer_height = (*pp_glyph)->glyph.height + EXPANDING_PIXEL * 2;
			int buffer_size = buffer_width * buffer_height;

			GlyphInfo* p_glyph_info = (GlyphInfo*)new char[sizeof(GlyphInfo) + buffer_size];
			memset(p_glyph_info, 0, sizeof(GlyphInfo) + buffer_size);

			p_glyph_info->glyph.p_buffer = (unsigned char*)(p_glyph_info + 1);

			p_glyph_info->left      = (*pp_glyph)->left - EXPANDING_PIXEL;
			p_glyph_info->top       = (*pp_glyph)->top - EXPANDING_PIXEL;
			p_glyph_info->x_advance = (*pp_glyph)->x_advance;
			p_glyph_info->y_advance = (*pp_glyph)->y_advance;

			p_glyph_info->user_data = 0;

			p_glyph_info->glyph.width          = buffer_width;
			p_glyph_info->glyph.height        = buffer_height;
			p_glyph_info->glyph.bytes_per_line = buffer_width;

			struct _util
			{
				static void mergeGlyphByAdd(int dst_x, int dst_y, GlyphInfo& dst_glyph, const GlyphInfo& src_glyph, int alpha)
				{
					typedef unsigned char Pixel;

					dst_x += EXPANDING_PIXEL;
					dst_y += EXPANDING_PIXEL;

					Pixel* p_src = src_glyph.glyph.p_buffer;
					Pixel* p_dst = dst_glyph.glyph.p_buffer;

					int copy_bytes  = src_glyph.glyph.width;
					int src_padding = 0;
					int dst_padding = dst_glyph.glyph.width - copy_bytes;

					// 'dst_x' and 'dst_y' are always valid.
					p_dst += (dst_y * dst_glyph.glyph.width) + dst_x;

					alpha += (alpha >> 7);
					alpha  = (alpha >= 256) ? 256 : alpha;

					for (int y = 0; y < src_glyph.glyph.height; y++)
					{
						int num_copy = copy_bytes;

						for ( ; --num_copy >= 0; p_src++, p_dst++)
						{
							int decided_alpha = (*p_src * alpha) >> 8;

							if (decided_alpha > *p_dst)
								*p_dst = Pixel(decided_alpha);
						}

						p_src += src_padding;
						p_dst += dst_padding;
					}
				}
			};

			_util::mergeGlyphByAdd(-2,  0, *p_glyph_info, *(*pp_glyph), 0x60);
			_util::mergeGlyphByAdd( 2,  0, *p_glyph_info, *(*pp_glyph), 0x60);
			_util::mergeGlyphByAdd( 0, -2, *p_glyph_info, *(*pp_glyph), 0x60);
			_util::mergeGlyphByAdd( 0, +2, *p_glyph_info, *(*pp_glyph), 0x60);

			_util::mergeGlyphByAdd(-1,  0, *p_glyph_info, *(*pp_glyph), 0xFF);
			_util::mergeGlyphByAdd(-1, -1, *p_glyph_info, *(*pp_glyph), 0xA0);
			_util::mergeGlyphByAdd(-1, +1, *p_glyph_info, *(*pp_glyph), 0xA0);
			_util::mergeGlyphByAdd( 0, -1, *p_glyph_info, *(*pp_glyph), 0xFF);
			_util::mergeGlyphByAdd( 0, +1, *p_glyph_info, *(*pp_glyph), 0xFF);
			_util::mergeGlyphByAdd( 1, -1, *p_glyph_info, *(*pp_glyph), 0xA0);
			_util::mergeGlyphByAdd( 1,  0, *p_glyph_info, *(*pp_glyph), 0xFF);
			_util::mergeGlyphByAdd( 1, +1, *p_glyph_info, *(*pp_glyph), 0xA0);

			glyph_shadow.push_back(p_glyph_info);
		}
	}
}

yunjr::Text::Impl::~Impl()
{
	this->typeface.font->unload(glyph_shadow);
	this->typeface.font->unload(glyph_info);
}

void yunjr::Text::Impl::reset(void)
{
	this->typeface.font->unload(glyph_shadow);
	this->typeface.font->unload(glyph_info);

	this->typeface.~Typeface();
	new (&this->typeface) Typeface();

	this->p_text.reset();
	this->text_length = 0;
}
