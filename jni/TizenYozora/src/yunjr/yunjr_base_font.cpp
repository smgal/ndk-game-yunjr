
#include "yunjr_base_font.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include <math.h>

////////////////////////////////////////////////////////////////////////////////
//

namespace
{
	int to26d6(int i)
	{
		return (i << 6);
	}

	int from26d6(int i26d6)
	{
		return (i26d6 >> 6);
	}
}

////////////////////////////////////////////////////////////////////////////////
// struct Font::Impl

struct yunjr::Font::Impl
{
	struct FontInfo
	{
		double ascender;
		double descender;
		double height;
		double leading;
		double max_height;
	};

	Impl()
		: m_p_font_library(0)
		, m_p_font_face(0)
		, m_p_font_buffer(0)
	{
		m_font_attrib.size = 36;
		m_font_attrib.is_italic = false;
		m_font_attrib.is_bold = false;
	}

	friend class Font;

private:
	bool m_setSize(int size)
	{
		if (this->m_p_font_face == 0)
			return false;

		FT_Set_Char_Size(this->m_p_font_face, to26d6(size) * 72, to26d6(size) * 72, 1, 1);

		this->m_font_info.ascender = this->m_p_font_face->size->metrics.ascender * 1.0 / 64.0;
		this->m_font_info.descender = this->m_p_font_face->size->metrics.descender * 1.0 / 64.0;
		this->m_font_info.height = this->m_p_font_face->size->metrics.height * 1.0 / 64.0;

		{
			int min_26d6 = (this->m_p_font_face->bbox.yMin * this->m_p_font_face->size->metrics.y_ppem) / this->m_p_font_face->units_per_EM;
			int max_26d6 = (this->m_p_font_face->bbox.yMax * this->m_p_font_face->size->metrics.y_ppem) / this->m_p_font_face->units_per_EM;

			this->m_font_info.leading = max_26d6 - m_font_info.ascender;
			this->m_font_info.max_height = max_26d6 - min_26d6;
		}

		return true;
	}

	void m_unload(GlyphInfo*& p_glyph_info) const
	{
		delete[] (char*)p_glyph_info;
		p_glyph_info = 0;
	}

	bool m_load(unsigned long glyph_index, GlyphInfo*& p_out_glyph_info) const
	{
		{
			FT_Matrix matrix;

			matrix.xx = (FT_Fixed) 0x10000;
			matrix.xy = (FT_Fixed) 0;
			matrix.yx = (FT_Fixed) 0;
			matrix.yy = (FT_Fixed) 0x10000;

			if (m_font_attrib.is_italic)
			{
				const double ITALIC_ANGLE = 10.619655;
				matrix.xy = (FT_Fixed)(sin(ITALIC_ANGLE * 3.14159265358979323846 / 180.0) * 0x10000);
			}

			FT_Vector pen = {0, 0};

			FT_Set_Transform(m_p_font_face, &matrix, &pen);
		}

		FT_Error error = 1;

		if (m_font_attrib.is_bold)
		{
			error = FT_Load_Glyph(m_p_font_face, glyph_index, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);

			if (error == 0)
			{
				if (m_p_font_face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
				{
					FT_Pos x_strength = FT_MulFix(m_p_font_face->units_per_EM, m_p_font_face->size->metrics.y_scale ) / 24;
					FT_Outline_Embolden(&m_p_font_face->glyph->outline, x_strength);
				}
			}
		}
		else
		{
			error = FT_Load_Glyph(m_p_font_face, glyph_index, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT);
		}

		if (error)
			return false;

		error = FT_Render_Glyph(m_p_font_face->glyph, FT_RENDER_MODE_NORMAL);

		if (error)
			return false;

		FT_GlyphSlot pSlot = m_p_font_face->glyph;

		{
			int buffer_size = pSlot->bitmap.width * pSlot->bitmap.rows;

			GlyphInfo* p_glyph_info = (GlyphInfo*)new char[sizeof(GlyphInfo) + buffer_size];
			p_glyph_info->glyph.p_buffer = (unsigned char*)(p_glyph_info + 1);

			p_glyph_info->left      = pSlot->bitmap_left;
			p_glyph_info->top       = - pSlot->bitmap_top;
			p_glyph_info->x_advance = from26d6(pSlot->advance.x);
			p_glyph_info->y_advance = from26d6(pSlot->advance.y);

			p_glyph_info->user_data = 0;

			p_glyph_info->glyph.width          = pSlot->bitmap.width;
			p_glyph_info->glyph.height         = pSlot->bitmap.rows;
			p_glyph_info->glyph.bytes_per_line = pSlot->bitmap.width;

			memcpy(p_glyph_info->glyph.p_buffer, pSlot->bitmap.buffer, buffer_size);

			p_out_glyph_info = p_glyph_info;
		}

		return true;
	}

private:
	FT_Library  m_p_font_library;
	FT_Face     m_p_font_face;
	FontAttrib  m_font_attrib;
	FontInfo    m_font_info;
	FT_Byte*    m_p_font_buffer;
};

////////////////////////////////////////////////////////////////////////////////
// class Font

yunjr::Font::Font(const void* p_buffer, int buffer_size)
	: p_impl(new Impl)
{
	if (p_buffer == 0 || buffer_size <= 0)
		return;

	do
	{
		int error = FT_Init_FreeType(&p_impl->m_p_font_library);

		if (error)
			break;

		p_impl->m_p_font_buffer = new FT_Byte[buffer_size];

		memcpy(p_impl->m_p_font_buffer, p_buffer, buffer_size);

		error = FT_New_Memory_Face(p_impl->m_p_font_library, p_impl->m_p_font_buffer, buffer_size, 0, &p_impl->m_p_font_face);

		if (error)
			break;

		if ((p_impl->m_p_font_face->charmap == 0) && (p_impl->m_p_font_face->num_charmaps > 0))
			FT_Set_Charmap(p_impl->m_p_font_face, p_impl->m_p_font_face->charmaps[0]);

		p_impl->m_setSize(p_impl->m_font_attrib.size);

		return;

	} while (0);

	if (p_impl->m_p_font_face != 0)
		FT_Done_Face(p_impl->m_p_font_face);

	if (p_impl->m_p_font_library != 0)
		FT_Done_FreeType(p_impl->m_p_font_library);

	delete p_impl;
}

yunjr::Font::Font(const char* sz_file_name)
	: p_impl(new Impl)
{
	if (sz_file_name == 0)
		return;

	do
	{
		int error = FT_Init_FreeType(&p_impl->m_p_font_library);

		if (error)
			break;

		error = FT_New_Face(p_impl->m_p_font_library, sz_file_name, 0, &p_impl->m_p_font_face);

		if (error)
			break;

		if ((p_impl->m_p_font_face->charmap == 0) && (p_impl->m_p_font_face->num_charmaps > 0))
			FT_Set_Charmap(p_impl->m_p_font_face, p_impl->m_p_font_face->charmaps[0]);

		p_impl->m_setSize(p_impl->m_font_attrib.size);

		return;

	} while(0);

	if (p_impl->m_p_font_face != 0)
		FT_Done_Face(p_impl->m_p_font_face);

	if (p_impl->m_p_font_library != 0)
		FT_Done_FreeType(p_impl->m_p_font_library);

	delete p_impl;
}

yunjr::Font::~Font()
{
	if (p_impl->m_p_font_face != 0)
		FT_Done_Face(p_impl->m_p_font_face);

	if (p_impl->m_p_font_library != 0)
		FT_Done_FreeType(p_impl->m_p_font_library);

	delete[] p_impl->m_p_font_buffer;

	delete p_impl;
}

void
yunjr::Font::set(const yunjr::FontAttrib& font_attrib)
{
	p_impl->m_setSize(font_attrib.size);

	p_impl->m_font_attrib = font_attrib;
}

void
yunjr::Font::get(yunjr::FontAttrib& out_font_attrib) const
{
	out_font_attrib = p_impl->m_font_attrib;
}

void
yunjr::Font::get(const yunjr::KerningPair& kerning_pair, int& current_x) const
{
	bool has_kerning = (FT_HAS_KERNING(p_impl->m_p_font_face) != 0);

	if (has_kerning)
	{
		FT_UInt ix_glyph_1 = FT_Get_Char_Index(p_impl->m_p_font_face, kerning_pair.first);
		FT_UInt ix_glyph_2 = FT_Get_Char_Index(p_impl->m_p_font_face, kerning_pair.second);

		FT_Vector delta;

		if (FT_Get_Kerning(p_impl->m_p_font_face, ix_glyph_1, ix_glyph_2, ft_kerning_default, &delta) == 0)
			current_x += from26d6(delta.x);
	}
}

bool
yunjr::Font::load(unsigned long code, yunjr::GlyphInfo*& p_out_glyph_info) const
{
	FT_Face m_p_font_face = p_impl->m_p_font_face;

	FT_UInt glyph_index = code;

	if (m_p_font_face->charmap != 0)
	{
		glyph_index = FT_Get_Char_Index(m_p_font_face, code);

		if (glyph_index == 0)
			return false;
	}

	p_impl->m_load(glyph_index, p_out_glyph_info);

	return true;
}

void
yunjr::Font::unload(yunjr::GlyphInfo*& p_glyph_info) const
{
	p_impl->m_unload(p_glyph_info);
}

bool
yunjr::Font::load(const wchar_t* sz_text, int text_length, std::vector<yunjr::GlyphInfo*>& out_glyph_info_list) const
{
	// not clear 'out_glyph_info_list'

	if (sz_text == NULL)
		return false;

	if (text_length <= 0)
		return true;

	FT_Face face = p_impl->m_p_font_face;

	if (face->charmap == 0)
		return false;

	bool has_kerning = (FT_HAS_KERNING(p_impl->m_p_font_face) != 0);

	{
		const wchar_t* sz_text_end = sz_text + text_length;

		for ( ; sz_text < sz_text_end; ++sz_text)
		{
			int glyph_index = FT_Get_Char_Index(face, *sz_text);

			if (glyph_index == 0)
				continue;

			GlyphInfo* p_glyph_info = 0;

			p_impl->m_load(glyph_index, p_glyph_info);

			if (has_kerning && (sz_text+1) < sz_text_end)
			{
				FT_UInt ix_glyph_1 = glyph_index;
				FT_UInt ix_glyph_2 = FT_Get_Char_Index(p_impl->m_p_font_face, *(sz_text+1));

				FT_Vector delta;

				if (FT_Get_Kerning(p_impl->m_p_font_face, ix_glyph_1, ix_glyph_2, ft_kerning_default, &delta) == 0)
					p_glyph_info->x_advance += from26d6(delta.x);
			}

			out_glyph_info_list.push_back(p_glyph_info);
		}
	}

	return true;
}

void
yunjr::Font::unload(std::vector<yunjr::GlyphInfo*>& glyph_info_list) const
{
	for (std::vector<yunjr::GlyphInfo*>::iterator i = glyph_info_list.begin(); i != glyph_info_list.end(); ++i)
	{
		p_impl->m_unload(*i);
	}

	glyph_info_list.clear();
}
