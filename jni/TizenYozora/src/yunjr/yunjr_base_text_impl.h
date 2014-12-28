
#ifndef __YUNJR_BASE_TEXT_IMPL_H__
#define __YUNJR_BASE_TEXT_IMPL_H__

#include "yunjr_base.h"
#include "yunjr_base_font.h"
#include "yunjr_base_text.h"

#include <vector>

namespace yunjr
{
	struct yunjr::Text::Impl
	{
		Impl();
		Impl(const Typeface& typeface, const wchar_t* sz_text, size_t length);
		~Impl();

		static const Impl* getImpl(const Text& text)
		{
			return text.p_impl;
		}

		static Impl* getImpl(Text& text)
		{
			return text.p_impl;
		}

		void reset(void);

		Typeface typeface;
		sena::auto_ptr<wchar_t[]> p_text;
		int text_length;
		std::vector<GlyphInfo*> glyph_info;
		std::vector<GlyphInfo*> glyph_shadow;
	};
}

#endif // #ifndef __YUNJR_BASE_TEXT_IMPL_H__
