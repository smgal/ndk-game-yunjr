
#ifndef __YUNJR_BASE_GFX_H__
#define __YUNJR_BASE_GFX_H__

#include "yunjr_base.h"
#include "yunjr_base_font.h"
#include "yunjr_base_text.h"

#include "util/sm_util_sena.h"

namespace yunjr
{
	// FlatBoard32
	struct FlatBoard32: public flat_board::FlatBoard<flat_board::PIXELFORMAT_ARGB8888>
	{
	public:
		FlatBoard32(Pixel* p_buffer, int width, int height, int pitch)
			: FlatBoard<flat_board::PIXELFORMAT_ARGB8888>(p_buffer, width, height, pitch)
		{
			buffer_desc.p_start_address = (void*)p_buffer;
			buffer_desc.width           = width;
			buffer_desc.height          = height;
			buffer_desc.bytes_per_line  = pitch * sizeof(Pixel);
			buffer_desc.bits_per_pixel  = sizeof(Pixel) * 8;
		}

		void renderText(int pos_x, int pos_y, const Text& text, Color32 color1, Color32 color2);
		void renderTextFx(int pos_x, int pos_y, const Text& text, Color32 color1, Color32 color2);

		BufferDesc buffer_desc;
	};

	namespace shared
	{
		typedef sena::shared_ptr<yunjr::FlatBoard32> FlatBoard32;
	}

	// frame buffer
	namespace gfx
	{
		void beginDraw(const BufferDesc& buffer_desc);
		void endDraw(void);

		shared::FlatBoard32 getFrameBuffer(void);
	}

	namespace gfx
	{
		void fillRect(unsigned long* p_dest_32, int w, int h, int dest_pitch, unsigned long color, unsigned long alpha);
		void bitBlt(unsigned long* p_dest_32, int w, int h, int dest_pitch, unsigned long* p_sour_32, int sour_pitch, unsigned long alpha, const flat_board::Optional<unsigned long>& chroma_key);

		void drawTile(shared::FlatBoard32& dest_board, int x, int y, TileId tile_id, int id_offset = 0);
	}

	namespace gfx
	{
		struct TextBoard
		{
			typedef unsigned long Pixel;

			Pixel* m_p_buffer;
			int    m_width;
			int    m_height;
			int    m_pitch;

			TextBoard(Pixel* p_buffer, int width, int height, int pitch);

			void renderTextFx(int pos_x, int pos_y, const Text& text, Color32 color1, Color32 color2);
			void renderText(int pos_x, int pos_y, const Text& text, Color32 color1, Color32 color2);
			void renderText(int pos_x, int pos_y, const std::vector<yunjr::GlyphInfo*>& glyph_info, Color32 color1, Color32 color2);
		};
	}
}

#endif // #ifndef __YUNJR_BASE_GFX_H__
