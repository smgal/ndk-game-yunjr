
#ifndef __YUNJR_CLASS_CHARA_H__
#define __YUNJR_CLASS_CHARA_H__

#include "yunjr_base.h"
#include "yunjr_base_gfx.h"
#include "yunjr_base_sound.h"

#include "yunjr_class.h"
#include "yunjr_class_game_state.h"
#include "yunjr_class_extern.h"
#include "yunjr_class_map.h"
#include "yunjr_class_pc_party.h"

#include "yunjr_res.h"

#include <vector>

namespace yunjr
{
	struct Chara: public Visible
	{
	};

	struct Playable: public Chara
	{
		struct AttributeChara: public Visible::Attribute
		{
			struct { int x, y; }   pos;
			struct { int x1, y1; } dir;

			void move(int x_vec, int y_vec)
			{
				if (x_vec != 0 || y_vec != 0)
				{
					pos.x += x_vec;
					pos.y += y_vec;

					_convertVecToDir(float(x_vec), float(y_vec), dir.x1, dir.y1);
				}
			}

			void _convertVecToDir(float x_vec, float y_vec, int& x_dir, int& y_dir)
			{
				x_dir = (x_vec == 0.0f) ? 0 : ((x_vec > 0.0f) ? 1 : -1);
				y_dir = (y_vec == 0.0f) ? 0 : ((y_vec > 0.0f) ? 1 : -1);
			/*
				if (x_dir != 0 || y_dir != 0)
				{
					x_vec = (x_vec >= 0.0f) ? x_vec : -x_vec;
					y_vec = (y_vec >= 0.0f) ? y_vec : -y_vec;

					const float TAN_67d5 = 2.41421356237f;
					const float TAN_22d5 = 0.41421356237f;

					float tangent = y_vec / x_vec;

					if (tangent > TAN_67d5)
						x_dir = 0;
					if (tangent < TAN_22d5)
						y_dir = 0;
				}
			*/
			}
		};

		static Playable* newInstance(void)
		{
			struct ShapeChara: public Visible::Shape
			{
				TileId tile_id;
				std::vector<int> dir_offset[3][3];

				virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const
				{
					// magic key
					const int NUM_MAP_X = 9;
					const int NUM_MAP_Y = 11;

					const int REVISE_FOR_CENTERING_X = TILE_W * (NUM_MAP_X / 2);
					const int REVISE_FOR_CENTERING_Y = TILE_H * (NUM_MAP_Y / 2);

					int display_offset_x = REVISE_FOR_CENTERING_X;
					int display_offset_y = REVISE_FOR_CENTERING_Y;

					{
						int map_offset_x;
						int map_offset_y;
						
						resource::getCurrentMapPos(map_offset_x, map_offset_y);

						display_offset_x -= map_offset_x;
						display_offset_y -= map_offset_y;
					}

					AttributeChara& attribute = *((AttributeChara*)p_this->getAttribute());

					int current_offset = this->dir_offset[attribute.dir.x1+1][attribute.dir.y1+1][0];
					gfx::drawTile(dest_board, attribute.pos.x + display_offset_x, attribute.pos.y + display_offset_y, this->tile_id, current_offset);
				}
			};

			struct UpdateChara: public Visible::Update
			{
				virtual bool update(Visible* p_this, unsigned long tick)
				{
					AttributeChara& attribute = *((AttributeChara*)p_this->getAttribute());
					PcParty& party = game::object::getParty();

					attribute.dir.x1 = party.faced_x1;
					attribute.dir.y1 = party.faced_y1;

					int x1 = 0;
					int y1 = 0;

					const GameState& game_state = GameState::getInstance();

					if (game_state.checkKeyPressed(target::KEY_LEFT))
						x1 = (attribute.pos.y % TILE_H) ? 0 : -1;
					if (game_state.checkKeyPressed(target::KEY_RIGHT))
						x1 = (attribute.pos.y % TILE_H) ? 0 : 1;
					if (game_state.checkKeyPressed(target::KEY_UP))
						y1 = (attribute.pos.x % TILE_W) ? 0 : -1;
					if (game_state.checkKeyPressed(target::KEY_DOWN))
						y1 = (attribute.pos.x % TILE_W) ? 0 : 1;

					// not allow diagonal moving
					if (x1 != 0 && y1 != 0)
						x1 = 0;

					if (x1 == 0 && y1 == 0)
					{
						x1 = (attribute.pos.x % TILE_W) ? attribute.dir.x1 : 0;
						y1 = (attribute.pos.y % TILE_H) ? attribute.dir.y1 : 0;
					}

					if (x1 != 0 || y1 != 0)
					{
						Map& map = game::object::getMap();

						bool auto_move = (attribute.pos.x % TILE_W) != 0 || (attribute.pos.y % TILE_H) != 0;

						if (auto_move)
						{
							attribute.move(x1 * MAP_SCROLL_IN_PIXELS, y1 * MAP_SCROLL_IN_PIXELS);
						}
						else
						{
							int map_offset_x;
							int map_offset_y;

							resource::getCurrentMapPos(map_offset_x, map_offset_y);

							int party_x = party.x = map_offset_x / TILE_W;
							int party_y = party.y = map_offset_y / TILE_H;

							unsigned char tile = map(party_x + x1, party_y + y1);

							(*map.act_list[tile])(x1, y1, true);

							if (party_x != party.x || party_y != party.y)
							{
								attribute.move(x1 * MAP_SCROLL_IN_PIXELS, y1 * MAP_SCROLL_IN_PIXELS);
								yunjr::sound::playFx(yunjr::sound::SOUND_WALK);
							}
						}
					}
					else if ((party.x != attribute.pos.x / TILE_W) || (party.y != attribute.pos.y / TILE_H))
					{
						if (party.x >= 0 && party.y >= 0)
						{
							attribute.pos.x = party.x * TILE_W;
							attribute.pos.y = party.y * TILE_H;
						}
					}

					return true;
				}
			};

			Playable* p_playable = new Playable;

			{
				AttributeChara* p_attribute = new AttributeChara();

				{
					PcParty& party = game::object::getParty();

					p_attribute->pos.x  = party.x * yunjr::TILE_W;
					p_attribute->pos.y  = party.y * yunjr::TILE_H;

					switch (party.faced)
					{
					case 1:
						p_attribute->dir.x1 = 0;
						p_attribute->dir.y1 = -1;
						break;
					case 2:
						p_attribute->dir.x1 = 1;
						p_attribute->dir.y1 = 0;
						break;
					case 3:
						p_attribute->dir.x1 = -1;
						p_attribute->dir.y1 = 0;
						break;
					case 0:
					default:
						p_attribute->dir.x1 = 0;
						p_attribute->dir.y1 = 1;
						break;
					}
				}

				*p_playable << p_attribute;
			}

			{
				ShapeChara* p_shape = new ShapeChara();

				p_shape->tile_id = TILE_ID_PLAYER;

				const int X__ = 0;
				const int _X_ = 1;
				const int __X = 2;
				const int Y__ = 0;
				const int _Y_ = 1;
				const int __Y = 2;

				p_shape->dir_offset[X__][Y__].push_back(5);
				p_shape->dir_offset[_X_][Y__].push_back(5);
				p_shape->dir_offset[__X][Y__].push_back(5);
				p_shape->dir_offset[X__][_Y_].push_back(7);
				p_shape->dir_offset[_X_][_Y_].push_back(4);
				p_shape->dir_offset[__X][_Y_].push_back(6);
				p_shape->dir_offset[X__][__Y].push_back(4);
				p_shape->dir_offset[_X_][__Y].push_back(4);
				p_shape->dir_offset[__X][__Y].push_back(4);

				*p_playable << p_shape;
			}

			{
				*p_playable << new UpdateChara();
			}

			return p_playable;
		}

		inline int getPosX(void) const
		{
			return ((AttributeChara*)this->p_attribute)->pos.x;
		}

		inline int getPosY(void) const
		{
			return ((AttributeChara*)this->p_attribute)->pos.y;
		}
	};
}

#endif // #ifndef __YUNJR_CLASS_CHARA_H__
