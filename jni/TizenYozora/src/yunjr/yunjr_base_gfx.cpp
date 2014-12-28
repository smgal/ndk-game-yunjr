
#include "yunjr_base_gfx.h"

#include "yunjr_base.h"
#include "yunjr_base_text_impl.h"

#include "yunjr_res.h"

////////////////////////////////////////////////////////////////////////////////

namespace
{
	yunjr::shared::FlatBoard32 _s_flat_board;
	yunjr::shared::FlatBoard32 _s_flat_board_null;
}

void yunjr::gfx::beginDraw(const BufferDesc& buffer_desc)
{
	int dest_buffer_pitch = (buffer_desc.bytes_per_line << 3) / buffer_desc.bits_per_pixel;

	assert(_s_flat_board.isNull() || _s_flat_board.unique());

	_s_flat_board.setNull();
	_s_flat_board.bind(new yunjr::FlatBoard32((FlatBoard32::Pixel*)buffer_desc.p_start_address, buffer_desc.width, buffer_desc.height, dest_buffer_pitch));
}

void yunjr::gfx::endDraw(void)
{
	_s_flat_board.setNull();
}

yunjr::shared::FlatBoard32 yunjr::gfx::getFrameBuffer(void)
{
	if (!_s_flat_board.isNull())
		return _s_flat_board;

	if (_s_flat_board_null.isNull())
		_s_flat_board_null.bind(new yunjr::FlatBoard32(0, 0, 0, 0));

	return _s_flat_board_null;
}

////////////////////////////////////////////////////////////////////////////////

void yunjr::gfx::fillRect(unsigned long* p_dest_32, int w, int h, int dest_pitch, unsigned long color, unsigned long alpha)
{
	alpha += (alpha >> 7);
	alpha  = ((alpha * (color >> 24)) << 16) & 0xFF000000;
	color &= 0x00FFFFFF;

	int gray = int((alpha >> 24) + (alpha >> 31));
	int s_r  = int(color >> 16) & 0xFF;
	int s_g  = int(color >> 8) & 0xFF;
	int s_b  = int(color) & 0xFF;

	while (--h >= 0)
	{
		int copy = w;

		while (--copy >= 0)
		{
			int r = (*p_dest_32 >> 16) & 0xFF;
			r += (((s_r - r) * gray) >> 8);

			int g = (*p_dest_32 >>  8) & 0xFF;
			g += (((s_g - g) * gray) >> 8);

			int b = (*p_dest_32 >>  0) & 0xFF;
			b += (((s_b - b) * gray) >> 8);

			*p_dest_32 = alpha | (r << 16) | (g << 8) | b;

			++p_dest_32;
		}

		p_dest_32 += (dest_pitch - w);
	}
}

void yunjr::gfx::bitBlt(unsigned long* p_dest_32, int w, int h, int dest_pitch, unsigned long* p_sour_32, int sour_pitch, unsigned long alpha, const flat_board::Optional<unsigned long>& chroma_key)
{
	typedef unsigned long Pixel;

	alpha += (alpha >> 7);
	alpha  = (alpha >= 256) ? 256 : alpha;

	while (--h >= 0)
	{
		int copy = w;

		while (--copy >= 0)
		{
			int gray = Pixel(alpha * (*p_sour_32 >> 24)) >> 8;
			gray += (gray >> 7);

			int r = (*p_dest_32 >> 16) & 0xFF;
			r += (((int(*p_sour_32 >> 16 & 0xFF) - r) * gray) >> 8);

			int g = (*p_dest_32 >>  8) & 0xFF;
			g += (((int(*p_sour_32 >>  8 & 0xFF) - g) * gray) >> 8);

			int b = (*p_dest_32 >>  0) & 0xFF;
			b += (((int(*p_sour_32 >>  0 & 0xFF) - b) * gray) >> 8);

			*p_dest_32 = (*p_dest_32 & 0xFF000000) | (r << 16) | (g << 8) | b;

			++p_sour_32;
			++p_dest_32;
		}

		p_dest_32 += (dest_pitch - w);
		p_sour_32 += (sour_pitch - w);
	}
}

void yunjr::gfx::drawTile(shared::FlatBoard32& dest_board, int x, int y, TileId tile_id, int id_offset)
{
	const Tile& tile = resource::getTile(tile_id, id_offset);
	const shared::FlatBoard32 src_board = resource::getResimage(tile.id_res);

	dest_board->bitBlt(x, y, const_cast<FlatBoard32*>(src_board.get()), tile.bounds.x1, tile.bounds.y1, tile.bounds.x2 - tile.bounds.x1, tile.bounds.y2 - tile.bounds.y1);
}

///////////

yunjr::gfx::TextBoard::TextBoard(Pixel* p_buffer, int width, int height, int pitch)
	: m_p_buffer(p_buffer)
	, m_width(width)
	, m_height(height)
	, m_pitch((pitch != 0) ? pitch : width)
{
}

void yunjr::gfx::TextBoard::renderTextFx(int pos_x, int pos_y, const Text& text, Color32 color1, Color32 color2)
{
	const yunjr::Text::Impl* p_text_impl = yunjr::Text::Impl::getImpl(text);

	const Color32 shadow_color1(0xFF000000); // 0xFF800000
	const Color32 shadow_color2(0xFF000000); // 0xFF204060

	this->renderText(pos_x, pos_y, p_text_impl->glyph_shadow, shadow_color1, shadow_color2);
	this->renderText(pos_x, pos_y, p_text_impl->glyph_info, color1, color2);
}

void yunjr::gfx::TextBoard::renderText(int pos_x, int pos_y, const Text& text, Color32 color1, Color32 color2)
{
	const yunjr::Text::Impl* p_text_impl = yunjr::Text::Impl::getImpl(text);

	this->renderText(pos_x, pos_y, p_text_impl->glyph_info, color1, color2);
}

void yunjr::gfx::TextBoard::renderText(int pos_x, int pos_y, const std::vector<yunjr::GlyphInfo*>& glyph_info, Color32 color1, Color32 color2)
{
	//?? 좀 더 정확한 clipping 계산이 필요
	if (pos_y > this->m_height)
		return;

	typedef unsigned long DstPixel;
	typedef unsigned char SrcPixel;
	typedef signed long   TgtPixel;

	std::vector<GlyphInfo*>::const_iterator pp_glyph = glyph_info.begin();

	//?? temporary
	if (pp_glyph != glyph_info.end())
	{
		unsigned long glyph_color = (*pp_glyph)->user_data;
		if (glyph_color)
			color1 = color2 = glyph_color;
	}

	TgtPixel s_a1 = (color1.argb >> 24) & 0xFF;
	TgtPixel s_r1 = (color1.argb >> 16) & 0xFF;
	TgtPixel s_g1 = (color1.argb >> 8) & 0xFF;
	TgtPixel s_b1 = (color1.argb >> 0) & 0xFF;

	TgtPixel s_a2 = (color2.argb >> 24) & 0xFF;
	TgtPixel s_r2 = (color2.argb >> 16) & 0xFF;
	TgtPixel s_g2 = (color2.argb >> 8) & 0xFF;
	TgtPixel s_b2 = (color2.argb >> 0) & 0xFF;

	for ( ; pp_glyph != glyph_info.end(); ++pp_glyph)
	{
		int offset_x = (*pp_glyph)->left;
		int offset_y = (*pp_glyph)->top;

		SrcPixel* p_src = (*pp_glyph)->glyph.p_buffer;
		DstPixel* p_dst = m_p_buffer + (pos_y + offset_y) * m_pitch + (pos_x + offset_x);

		for (int y = 0; y < (*pp_glyph)->glyph.height; y++)
		{
			TgtPixel gradation = (y << 8) / ((*pp_glyph)->glyph.height - 1);

			TgtPixel s_a = s_a1 + (((s_a2 - s_a1) * gradation) >> 8);
			TgtPixel s_r = s_r1 + (((s_r2 - s_r1) * gradation) >> 8);
			TgtPixel s_g = s_g1 + (((s_g2 - s_g1) * gradation) >> 8);
			TgtPixel s_b = s_b1 + (((s_b2 - s_b1) * gradation) >> 8);

			s_a += (s_a >> 7);

			for (int x = 0; x < (*pp_glyph)->glyph.width; x++)
			{
				// in case, (*pp_glyph)->glyph.bytes_per_line == (*pp_glyph)->glyph.width
				TgtPixel gray = *p_src++;

				if (gray > 254)
				{
					*p_dst = (*p_dst & 0xFF000000) | (s_r << 16) | (s_g << 8) | (s_b);
				}
				else if (gray > 0)
				{
					gray += (gray >> 7);
					TgtPixel mul = (s_a * gray) >> 8;

					TgtPixel d_a = (*p_dst) & 0xFF000000;
					TgtPixel d_r = (*p_dst >> 16) & 0xFF;
					TgtPixel d_g = (*p_dst >> 8) & 0xFF;
					TgtPixel d_b = (*p_dst >> 0) & 0xFF;

					d_r += ((s_r - d_r) * mul) >> 8;
					d_g += ((s_g - d_g) * mul) >> 8;
					d_b += ((s_b - d_b) * mul) >> 8;

					*p_dst = d_a | (d_r << 16) | (d_g << 8) | (d_b);
				}

				++p_dst;
			}
			
			p_dst += (m_pitch - (*pp_glyph)->glyph.width);
		}

		pos_x += (*pp_glyph)->x_advance;
		pos_y += (*pp_glyph)->y_advance;
	}
}
