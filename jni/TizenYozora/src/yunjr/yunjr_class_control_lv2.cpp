
#include "yunjr_base.h"
#include "yunjr_base_gfx.h"

#include "yunjr_class.h"
#include "yunjr_class_control_lv2.h"
#include "yunjr_class_chara.h"

#include <algorithm>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// class ControlMap

namespace yunjr
{
	// ScaleTraits
	template <int Scale>
	struct ScaleTraits
	{
		template <typename Pixel>
		void expandW(Pixel*& p_d, Pixel*& p_s);

		template <typename Pixel>
		void expandH(Pixel*& p_d, int src_ppl, int dst_ppl);
	};

	template <>
	struct ScaleTraits<2>
	{
		enum { SCALE = 2 };
		enum { GUIDE_WIDTH  = 9 * TILE_W };
		enum { GUIDE_HEIGHT = 11 * TILE_H };

		template <typename Pixel>
		static inline void expandW(Pixel*& p_d, Pixel*& p_s)
		{
			*p_d++ = *p_s;
			*p_d++ = *p_s++;
		}

		template <typename Pixel>
		static inline void expandH(Pixel* p_d, int src_ppl, int dst_ppl)
		{
			memcpy(p_d + dst_ppl, p_d, src_ppl * sizeof(Pixel) * SCALE);
		}
	};

	template <>
	struct ScaleTraits<3>
	{
		enum { SCALE = 3 };
		enum { GUIDE_WIDTH  = 9 * TILE_W };
		enum { GUIDE_HEIGHT = 11 * TILE_H };

		template <typename Pixel>
		static inline void expandW(Pixel*& p_d, Pixel*& p_s)
		{
			*p_d++ = *p_s;
			*p_d++ = *p_s;
			*p_d++ = *p_s++;
		}

		template <typename Pixel>
		static inline void expandH(Pixel* p_d, int src_ppl, int dst_ppl)
		{
			memcpy(p_d + dst_ppl, p_d, src_ppl * sizeof(Pixel) * SCALE);
			// if you apply alpha attenuation to this line, interlace effect occurs.
			memcpy(p_d + dst_ppl*2, p_d, src_ppl * sizeof(Pixel) * SCALE);
		}
	};
}

namespace yunjr
{
	enum { SCALE = MAP_SCALE_RATIO };

	struct ControlMap::Attribute: public Visible::Attribute
	{
		struct { int x, y; } pos;
		struct { int width, height; } size;

		sena::auto_ptr<FlatBoard32::Pixel[]> auto_buffer;
		shared::FlatBoard32 map_board;

		Attribute(int x, int y, int width, int height)
		{
			pos.x = x;
			pos.y = y;
			size.width = width;
			size.height = height;

			int buffer_w = ScaleTraits<SCALE>::GUIDE_WIDTH;
			int buffer_h = ScaleTraits<SCALE>::GUIDE_HEIGHT;

			auto_buffer.bind(new FlatBoard32::Pixel[buffer_w * buffer_h]);
			map_board.bind(new FlatBoard32(auto_buffer.get(), buffer_w, buffer_h, buffer_w));
		}
	};
}

yunjr::ControlMap::ControlMap()
{
}

yunjr::ControlMap* yunjr::ControlMap::newInstance(int x, int y, int width, int height)
{
	struct ShapeMap: public Visible::Shape
	{
		virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const
		{
			Attribute& attribute = *((Attribute*)p_this->getAttribute());

			*attribute.map_board << gfx::bitBlt;

			int buffer_width = attribute.map_board->getBufferDesc().width;
			int buffer_height = attribute.map_board->getBufferDesc().height;
			int buffer_x = (dest_board->getBufferDesc().width - buffer_width) >> 1;
			int buffer_y = (dest_board->getBufferDesc().height - buffer_height) >> 1;

			{
				// map
				attribute.map_board->fillRect(0, 0, buffer_width, buffer_height, 0xFF103050);

				int NUM_MAP_X = (ScaleTraits<SCALE>::GUIDE_WIDTH + (TILE_W-1)) / TILE_W;
				int NUM_MAP_Y = (ScaleTraits<SCALE>::GUIDE_HEIGHT + (TILE_H-1)) / TILE_H;

				int map_offset_x;
				int map_offset_y;

				resource::getCurrentMapPos(map_offset_x, map_offset_y);

				int display_offset_x = - map_offset_x % TILE_W;
				int display_offset_y = - map_offset_y % TILE_H;

				map_offset_x = map_offset_x / TILE_W - NUM_MAP_X / 2;
				map_offset_y = map_offset_y / TILE_H - NUM_MAP_Y / 2;

				for (int y = 0; y <= NUM_MAP_Y; y++)
				for (int x = 0; x <= NUM_MAP_X; x++)
				{
					int x_pos = x * TILE_W + 0;
					int y_pos = y * TILE_H + 0;

					int ix_map = game::object::getMap()(x + map_offset_x, y + map_offset_y);
					ix_map += NUM_TILE_ID_GROUND_W * game::object::getMap().type;

					gfx::drawTile(attribute.map_board, x_pos + display_offset_x, y_pos + display_offset_y, TILE_ID_GROUND, ix_map);
				}

				// playable
				std::vector<Chara*>& chara_list = yunjr::resource::getCharaList();
				std::for_each(chara_list.begin(), chara_list.end(), Operator<Chara*, shared::FlatBoard32>(attribute.map_board));
			}

			const BufferDesc* p_buffer_desc = &gfx::getFrameBuffer()->buffer_desc;

			if (p_buffer_desc)
			{
				int src_w = attribute.map_board->getBufferDesc().width;
				int src_h = attribute.map_board->getBufferDesc().height;
				int src_p = src_w; // packed buffer

				int dst_w = p_buffer_desc->width;
				int dst_h = p_buffer_desc->height;
				int dst_p = (p_buffer_desc->bytes_per_line << 3) / p_buffer_desc->bits_per_pixel;

				FlatBoard32::Pixel* p_src = attribute.auto_buffer.get();
				FlatBoard32::Pixel* p_dst = (FlatBoard32::Pixel*)p_buffer_desc->p_start_address;

				p_dst += attribute.pos.y * dst_p + attribute.pos.x;

				// assert(...);

				int offset_y = (attribute.size.height - ScaleTraits<SCALE>::SCALE * buffer_height) / 4 * 2;

				for (int y = 0; y < src_h; y++)
				{
					int revised_y = ScaleTraits<SCALE>::SCALE * y + offset_y;

					if (revised_y >= attribute.size.height)
					{
						break;
					}
					if (revised_y >= 0)
					{
						FlatBoard32::Pixel* p_s = p_src + y * src_p;
						FlatBoard32::Pixel* p_d = p_dst + revised_y * dst_p;
						FlatBoard32::Pixel* p_m = p_d;

						for (int x = 0; x < src_w; x++)
							ScaleTraits<SCALE>::expandW(p_d, p_s);

						ScaleTraits<SCALE>::expandH(p_m, src_p, dst_p);
					}
				}
			}
		}
	};

	struct UpdateMap: public Visible::Update
	{
		virtual bool update(Visible* p_this, unsigned long tick)
		{
			std::vector<Chara*>& chara_list = yunjr::resource::getCharaList();
			std::for_each(chara_list.begin(), chara_list.end(), Operator<Chara*, unsigned long>(tick));

			Playable* p_chara = (Playable*)chara_list[0];
			assert(p_chara);
			resource::setCurrentMapPos(p_chara->getPosX(), p_chara->getPosY());

			return true;
		}
	};

	ControlMap* p_map = new ControlMap();

	*p_map << new Attribute(x, y, width, height) << new ShapeMap() << new UpdateMap();

	return p_map;
}

////////////////////////////////////////////////////////////////////////////////
// class ControlBattle

namespace yunjr
{
	struct ControlBattle::Attribute: public Visible::Attribute
	{
		Control::Pos pos;
		Control::Size size;
		struct { int left, top; } margin;

		bool need_to_clear;
		int ix_inverted;

		Attribute(int x, int y, int width, int height)
			: need_to_clear(true)
			, ix_inverted(-1)
		{
			pos.x = x;
			pos.y = y;

			size.width = width;
			size.height = height;

			margin.left = 26 * 2;
			margin.top = 6 * 2;
		}
	};
}

yunjr::ControlBattle::ControlBattle()
{
}

void yunjr::ControlBattle::display(bool need_to_clear, int ix_inverted)
{
	Attribute& attribute = *((Attribute*)this->getAttribute());

	attribute.need_to_clear = need_to_clear;
	attribute.ix_inverted = ix_inverted;
	{
		const BufferDesc* p_buffer_desc = &gfx::getFrameBuffer()->buffer_desc;
		const BufferDesc& buffer_desc = *p_buffer_desc;
		int dest_buffer_pitch = (buffer_desc.bytes_per_line << 3) / buffer_desc.bits_per_pixel;

		shared::FlatBoard32 dest_board(new FlatBoard32((FlatBoard32::Pixel*)buffer_desc.p_start_address, buffer_desc.width, buffer_desc.height, dest_buffer_pitch));

		this->invalidate();
		this->render(dest_board);
	}
	attribute.need_to_clear = true;
	attribute.ix_inverted = -1;
}

yunjr::ControlBattle* yunjr::ControlBattle::newInstance(int x, int y, int width, int height)
{
	struct ShapeBattle: public Visible::Shape
	{
		void m_displayWithColor(const Attribute& attribute, gfx::TextBoard& text_board, const PcEnemy& enemy, int row) const
		{
			if (!enemy.isValid())
				return;

			int ix_color = 10;

			if (enemy.hp <= 0)
				ix_color = 8;
			else if (enemy.hp <= 20)
				ix_color = 12;
			else if (enemy.hp <= 50)
				ix_color = 4;
			else if (enemy.hp <= 100)
				ix_color = 6;
			else if (enemy.hp <= 200)
				ix_color = 14;
			else if (enemy.hp <= 300)
				ix_color = 2;

			if (enemy.unconscious)
				ix_color = 8;
			if (enemy.dead)
				ix_color = 0;

			assert(ix_color >= 0 && ix_color < 16);

			unsigned long color = game::getRealColor(ix_color);

			Typeface typeface;
			Text text(typeface, enemy.getName());

			{
				int text_x = attribute.margin.left;
				int text_y = attribute.margin.top + DEFAULT_BATTLE_BTBD * (row + 1); // 20*(row++)+(m_y+6) according to old source code

				text_board.renderTextFx(text_x, text_y, text, color, color);
			}
		}

		void m_displayEnemies(const Attribute& attribute) const
		{
			const int BATTLE_WINDOW_X = attribute.pos.x;
			const int BATTLE_WINDOW_Y = attribute.pos.y;
			const int BATTLE_WINDOW_W = attribute.size.width;
			const int BATTLE_WINDOW_H = attribute.size.height;

			{
				const BufferDesc* p_buffer_desc = &gfx::getFrameBuffer()->buffer_desc;

				unsigned long* p = (unsigned long*)p_buffer_desc->p_start_address;
				int ppl = (p_buffer_desc->bytes_per_line << 3) / p_buffer_desc->bits_per_pixel;

				p += (ppl * BATTLE_WINDOW_Y);
				p += BATTLE_WINDOW_X;

				yunjr::gfx::TextBoard text_board(p, BATTLE_WINDOW_W, BATTLE_WINDOW_H, ppl);

				sena::vector<yunjr::shared::PcEnemy>& enemy_list = yunjr::game::object::getEnemyList();

				sena::vector<yunjr::shared::PcEnemy>::iterator p_enemy = enemy_list.begin();

				int row = 0;
				for ( ; p_enemy != enemy_list.end(); ++p_enemy)
				{
					if (p_enemy == 0 || (*p_enemy).get() == 0)
						continue;

					const PcEnemy& enemy = *((*p_enemy).get());

					m_displayWithColor(attribute, text_board, enemy, row++);
				}
			}
		}

		virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const
		{
			Attribute& attribute = *((Attribute*)p_this->getAttribute());

			if (attribute.need_to_clear)
			{
				dest_board->fillRect(attribute.pos.x, attribute.pos.y, attribute.size.width, attribute.size.height, 0xFF000000);
			}

			if (attribute.ix_inverted >= 0)
			{
				int x = 0;
				int y = DEFAULT_BATTLE_BTBD * (attribute.ix_inverted + 1) + attribute.margin.top - DEFAULT_FONT_SIZE;
				int width = attribute.size.width;
				int height = DEFAULT_BATTLE_BTBD;

				dest_board->fillRect(attribute.pos.x + x, attribute.pos.y + y, width, height, 0xFF808080);
			}

			m_displayEnemies(attribute);
		}
	};

	struct UpdateBattle: public Visible::Update
	{
		virtual bool update(Visible* p_this, unsigned long tick)
		{
			return true;
		}
	};

	ControlBattle* p_map = new ControlBattle();

	*p_map << new Attribute(x, y, width, height) << new ShapeBattle() << new UpdateBattle();

	return p_map;
}
