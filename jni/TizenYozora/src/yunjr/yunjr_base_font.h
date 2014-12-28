
#ifndef __YUNJR_BASE_FONT_H__
#define __YUNJR_BASE_FONT_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include "yunjr_base.h"

#include <vector>

namespace yunjr
{
	struct GlyphInfo
	{
		int left;
		int top;
		int x_advance;
		int y_advance;

		unsigned long user_data;

		struct
		{
			int width;
			int height;
			int bytes_per_line;
			unsigned char* p_buffer;
		} glyph;
	};

	struct FontAttrib
	{
		unsigned int size;
		bool is_italic;
		bool is_bold;
	};

	struct KerningPair
	{
		wchar_t first;
		wchar_t second;
	};
}

namespace yunjr
{
	class Font
	{
	public:
		Font(const char* sz_file_name);
		Font(const void* p_buffer, int buffer_size);
		~Font();

		void set(const FontAttrib& font_attrib);
		void get(FontAttrib& out_font_attrib) const;
		void get(const KerningPair& kerning_pair, int& current_x) const;

		bool load(unsigned long code, GlyphInfo*& p_out_glyph_info) const;
		void unload(GlyphInfo*& p_glyph_info) const;

		bool load(const wchar_t* sz_text, int text_length, std::vector<GlyphInfo*>& out_glyph_info_list) const;
		void unload(std::vector<GlyphInfo*>& glyph_info_list) const;

	private:
		struct Impl;
		Impl* p_impl;
	};

	namespace shared
	{
		typedef sena::shared_ptr<yunjr::Font> Font;
	}

	struct Typeface
	{
		shared::Font font;
		FontAttrib   attribute;

		inline void apply(void)
		{
			font->set(attribute);
		}

		Typeface()
		{
			attribute.size = DEFAULT_FONT_SIZE;
			attribute.is_bold = false;
			attribute.is_italic = false;
		}

		Typeface(int font_size)
		{
			attribute.size = (font_size >= 0) ? font_size : DEFAULT_FONT_SIZE;
			attribute.is_bold = false;
			attribute.is_italic = false;
		}
	};

} // namespace yunjr


#endif // #ifndef __YUNJR_BASE_FONT_H__
