
#include "yunjr_class_control_lv1.h"

#include "yunjr_base.h"
#include "yunjr_base_font.h"
#include "yunjr_base_gfx.h"

#include "yunjr_class.h"
#include "yunjr_class_chara.h"
#include "yunjr_class_game_state.h"
#include "yunjr_util.h"

//#include "yunjr_res_string.h"

#include <algorithm>
#include <string.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// local

namespace yunjr
{
	// for playable updating
	template <>
	void Operator<Chara*, unsigned long>::operator()(Chara* p_obj)
	{
		(p_obj) ? p_obj->update(param) : (void)0;
	}

	// for playable rendering
	template <>
	void Operator<Chara*, shared::FlatBoard32>::operator()(Chara* p_obj)
	{
		if (p_obj)
		{
			p_obj->invalidate();
			p_obj->render(param);
		}
	}

}

////////////////////////////////////////////////////////////////////////////////
// class ControlWindow

namespace yunjr
{
	struct ControlWindow::Attribute: public Visible::Attribute
	{
		std::vector<ControlId> child_list;

		Control::Pos pos;
		Control::Size size;

		Attribute(int width, int height)
		{
			pos.x = 0;
			pos.y = 0;

			size.width = width;
			size.height = height;
		}
	};
}

yunjr::ControlWindow::ControlWindow()
{
}

yunjr::ControlWindow::~ControlWindow()
{
	Attribute* p_attribute = (Attribute*)this->getAttribute();
	
	deleteVectorForPair<ResId, Control>(p_attribute->child_list);
}

void yunjr::ControlWindow::addChild(ControlId control_id)
{
	Attribute* p_attribute = (Attribute*)this->getAttribute();

	p_attribute->child_list.push_back(control_id);
}

yunjr::Control* yunjr::ControlWindow::findControl(ResId res_id)
{
	assert((res_id.native_id & ResId::TAG_VERIFICATION_MASK) == ResId::TAG_VERIFICATION_STR);

	Attribute* p_attribute = (Attribute*)this->getAttribute();

	std::vector<ControlId>::iterator p_control = p_attribute->child_list.begin();

	for ( ; p_control != p_attribute->child_list.end(); ++p_control)
	{
		if (res_id == p_control->first)
			return p_control->second;
	}

	return 0;
}

void yunjr::ControlWindow::invalidateBounds(Pos pos, Size size)
{
	Attribute* p_attribute = (Attribute*)this->getAttribute();

	//?? should apply the specified rectangle
	std::vector<ControlId>::iterator p_control = p_attribute->child_list.begin();

	for ( ; p_control != p_attribute->child_list.end(); ++p_control)
		p_control->second->invalidate();
}

yunjr::ControlWindow* yunjr::ControlWindow::newInstance(int width, int height)
{
	struct ShapeWindow: public Visible::Shape
	{
		virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const
		{
			ControlWindow* p_window = (ControlWindow*)p_this;
			Attribute* p_attribute = (Attribute*)p_window->getAttribute();

			std::for_each(p_attribute->child_list.begin(), p_attribute->child_list.end(), Operator<ControlId, shared::FlatBoard32>(dest_board));
		}
	};

	struct UpdateWindow: public Visible::Update
	{
		virtual bool update(Visible* p_this, unsigned long tick)
		{
			ControlWindow* p_window = (ControlWindow*)p_this;
			Attribute* p_attribute = (Attribute*)p_window->getAttribute();

			std::for_each(p_attribute->child_list.rbegin(), p_attribute->child_list.rend(), Operator<ControlId, unsigned long>(tick));

			return true;
		}
	};

	ControlWindow* p_window = new ControlWindow();

	*p_window << new Attribute(width, height) << new ShapeWindow() << new UpdateWindow();

	return p_window;
}

////////////////////////////////////////////////////////////////////////////////
// class ControlWaku

namespace
{
	static unsigned long* s_p_dest_buffer;
	static int s_dest_ppl;

	void baitFillRect(unsigned long* p_dest, int w, int h, int dest_pitch, unsigned long color, unsigned long alpha)
	{
		s_p_dest_buffer = p_dest;
		s_dest_ppl = dest_pitch;
	}
}

namespace yunjr
{
	struct ControlWaku::Attribute: public Visible::Attribute
	{
		Control::Pos pos;
		const shared::FlatBoard32 src_ui;

		Attribute()
			: src_ui(resource::getResimage(ResId(ResId::TAG_TYPE_IMAGE, ResId::TAG_TYPE_IMAGE_UI, 0)))
		{
			pos.x = 0;
			pos.y = 0;
		}
	};
}

yunjr::ControlWaku::ControlWaku()
{
}

yunjr::ControlWaku* yunjr::ControlWaku::newInstance(void)
{
	struct ShapeWaku: public Visible::Shape
	{
		virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const
		{
			Attribute& attribute = *((Attribute*)p_this->getAttribute());
			//dest_board.bitBlt(attribute.pos.x, attribute.pos.y, const_cast<FlatBoard32*>(&attribute.src_ui), 0, 0, attribute.src_ui.getBufferDesc().width, attribute.src_ui.getBufferDesc().height);

			const BufferDesc* p_buffer_desc = &gfx::getFrameBuffer()->buffer_desc;

			if (p_buffer_desc)
			{
				int src_w = attribute.src_ui->getBufferDesc().width;
				int src_h = attribute.src_ui->getBufferDesc().height;
				int src_p = src_w; // packed buffer

				int dst_w = p_buffer_desc->width;
				int dst_h = p_buffer_desc->height;
				int dst_p = (p_buffer_desc->bytes_per_line << 3) / p_buffer_desc->bits_per_pixel;

				// ura waza
				{
					*(const_cast<FlatBoard32*>(attribute.src_ui.get())) << baitFillRect;
					(const_cast<FlatBoard32*>(attribute.src_ui.get()))->fillRect(0, 0, 1, 1, 0x0);
					*(const_cast<FlatBoard32*>(attribute.src_ui.get())) << attribute.src_ui->getDefaultFillRect();
				}

				FlatBoard32::Pixel* p_src = s_p_dest_buffer;
				FlatBoard32::Pixel* p_dst = (FlatBoard32::Pixel*)p_buffer_desc->p_start_address;

				p_dst += attribute.pos.y * dst_p + attribute.pos.x;

				// assert(...);

				const int SCALE = 2;

				for (int y = 0; y < src_h; y++)
				{
					FlatBoard32::Pixel* p_s = p_src + y * src_p;
					FlatBoard32::Pixel* p_d = p_dst + SCALE * y * dst_p;
					FlatBoard32::Pixel* p_m = p_d;

					for (int x = 0; x < src_w; x++)
					{
						*p_d++ = *p_s;
						*p_d++ = *p_s++;
					}

					memcpy(p_m + dst_p, p_m, dst_p * sizeof(unsigned long) * (SCALE-1));
				}
			}
		}
	};

	struct UpdateWaku: public Visible::Update
	{
		virtual bool update(Visible* p_this, unsigned long tick)
		{
			return false;
		}
	};

	ControlWaku* p_waku = new ControlWaku();

	*p_waku << new Attribute() << new ShapeWaku() << new UpdateWaku();

	return p_waku;
}

////////////////////////////////////////////////////////////////////////////////
// class ControlConsole

namespace yunjr
{
	struct ControlConsole::Attribute: public Visible::Attribute
	{
		Control::Pos  pos;
		Control::Size size;

		struct { int left, right, top, bottom; } margin;

		std::vector<sena::shared_ptr<Text> > text_line;

		Attribute(int x, int y, int width, int height, int margin_left, int margin_right, int margin_top, int margin_bottom)
		{
			pos.x = x;
			pos.y = y;
			size.width = width;
			size.height = height;

			margin.left = margin_left;
			margin.right = margin_right;
			margin.top = margin_top;
			margin.bottom = margin_bottom;
		}
	};
}

yunjr::ControlConsole::ControlConsole()
{
}

yunjr::ControlConsole::~ControlConsole()
{
}

void yunjr::ControlConsole::clear(void)
{
	Attribute* p_attribute = (Attribute*)this->getAttribute();
	
	p_attribute->text_line.clear();

	this->invalidate();
}

void yunjr::ControlConsole::add(Text& text)
{
	Text remaining;

	//?? 700 is a temporary constant
	text.split(620, remaining);

	{
		Attribute* p_attribute = (Attribute*)this->getAttribute();

		sena::shared_ptr<Text> p_text(new Text());
		text.split(0, *p_text);

		p_attribute->text_line.push_back(p_text);
	}

	if (!remaining.isEmpty())
		add(remaining);

	this->invalidate();
}

void yunjr::ControlConsole::drawText(int x, int y, const wchar_t* sz_text, unsigned long color)
{
	Attribute& attribute = *((Attribute*)this->getAttribute());

	const int DIALOG_WINDOW_X = attribute.pos.x;
	const int DIALOG_WINDOW_Y = attribute.pos.y;
	const int DIALOG_WINDOW_W = attribute.size.width;
	const int DIALOG_WINDOW_H = attribute.size.height;

	const BufferDesc* p_buffer_desc = &gfx::getFrameBuffer()->buffer_desc;

	unsigned long* p = (unsigned long*)p_buffer_desc->p_start_address;
	int ppl = (p_buffer_desc->bytes_per_line << 3) / p_buffer_desc->bits_per_pixel;

	p += (ppl * DIALOG_WINDOW_Y);
	p += DIALOG_WINDOW_X;

	yunjr::gfx::TextBoard text_board(p, DIALOG_WINDOW_W, DIALOG_WINDOW_H, ppl);

	int text_x = attribute.margin.left;
	int text_y = attribute.margin.top + 26;

	Typeface typeface;
	Text text(typeface, sz_text);
	text_board.renderTextFx(x, y, text, color, color);
}

void yunjr::ControlConsole::getRegion(int& x1, int& y1, int& x2, int& y2) const
{
	const Attribute& attribute = *((const Attribute*)this->getAttribute());

	x1 = attribute.pos.x;
	y1 = attribute.pos.y;
	x2 = attribute.pos.x + attribute.size.width;
	y2 = attribute.pos.y + attribute.size.height;
}

void yunjr::ControlConsole::setRegion(int x1, int y1, int x2, int y2)
{
	Attribute& attribute = *((Attribute*)this->getAttribute());

	attribute.pos.x = x1;
	attribute.pos.y = y1;

	attribute.size.width = x2 - x1;
	attribute.size.height = y2 - y1;

	this->invalidate();
}

void yunjr::ControlConsole::getMargin(int& left, int& top, int& right, int& bottom) const
{
	const Attribute& attribute = *((const Attribute*)this->getAttribute());

	left   = attribute.margin.left;
	top    = attribute.margin.top;
	right  = attribute.margin.right;
	bottom = attribute.margin.bottom;
}

void yunjr::ControlConsole::setMargin(int left, int top, int right, int bottom)
{
	Attribute& attribute = *((Attribute*)this->getAttribute());

	attribute.margin.left   = left;
	attribute.margin.top    = top;
	attribute.margin.right  = right;
	attribute.margin.bottom = bottom;
}

yunjr::ControlConsole* yunjr::ControlConsole::newInstance(int x, int y, int width, int height, int margin_left, int margin_right, int margin_top, int margin_bottom)
{
	struct ShapeConsole: public Visible::Shape
	{
		virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const
		{
			Attribute& attribute = *((Attribute*)p_this->getAttribute());

			dest_board->fillRect(attribute.pos.x, attribute.pos.y, attribute.size.width, attribute.size.height, 0xFF545454);

			/* display client area for debugging
			dest_board.fillRect(attribute.pos.x + attribute.margin.left
				, attribute.pos.y + attribute.margin.top
				, attribute.size.width - (attribute.margin.left + attribute.margin.right)
				, attribute.size.height - (attribute.margin.top + attribute.margin.bottom)
				, 0xFF547054);
			*/

			const int DIALOG_WINDOW_X = attribute.pos.x;
			const int DIALOG_WINDOW_Y = attribute.pos.y;
			const int DIALOG_WINDOW_W = attribute.size.width;
			const int DIALOG_WINDOW_H = attribute.size.height;

			{
				const BufferDesc* p_buffer_desc = &gfx::getFrameBuffer()->buffer_desc;

				unsigned long* p = (unsigned long*)p_buffer_desc->p_start_address;
				int ppl = (p_buffer_desc->bytes_per_line << 3) / p_buffer_desc->bits_per_pixel;

				p += (ppl * DIALOG_WINDOW_Y);
				p += DIALOG_WINDOW_X;

				yunjr::gfx::TextBoard text_board(p, DIALOG_WINDOW_W, DIALOG_WINDOW_H, ppl);

				int text_x = attribute.margin.left;
				int text_y = attribute.margin.top + DEFAULT_FONT_ASCENDER;
				int text_line_gap = DEFAULT_FONT_BTBD;

				ControlConsole* _p_this = (ControlConsole*)p_this;
				Attribute* p_attribute = (Attribute*)_p_this->getAttribute();

				std::vector<sena::shared_ptr<Text> >::iterator p_text = p_attribute->text_line.begin();

				for ( ; p_text != p_attribute->text_line.end(); ++p_text)
				{
					text_board.renderTextFx(text_x, text_y, **p_text, 0xFFFFFFFF, 0xFFFFFFFF);

					text_y += text_line_gap;

				}
/*
				for (int i = 0; i < 3; i++)
				{
					if (_p_this->text[i])
						text_board.renderTextFx(text_x, text_y, *_p_this->text[i], 0xFFFFFFFF, 0xFFFFFFFF);

					text_y += text_line_gap;
				}
*/
			}
		}
	};

	struct UpdateConsole: public Visible::Update
	{
		virtual bool update(Visible* p_this, unsigned long tick)
		{
			return false;
		}
	};

	ControlConsole* p_popup = new ControlConsole();

	*p_popup << new Attribute(x, y, width, height, margin_left, margin_right, margin_top, margin_bottom) << new ShapeConsole() << new UpdateConsole();

	return p_popup;
}

////////////////////////////////////////////////////////////////////////////////
// class ControlStatus

namespace yunjr
{
	struct ControlStatus::Attribute: public Visible::Attribute
	{
		Control::Pos pos;
		Control::Size size;

		Attribute(int x, int y, int width, int height)
		{
			pos.x = x;
			pos.y = y;
			size.width = width;
			size.height = height;
		}
	};
}

yunjr::ControlStatus::ControlStatus()
{
}

yunjr::ControlStatus::~ControlStatus()
{
}

yunjr::ControlStatus* yunjr::ControlStatus::newInstance(int x, int y, int width, int height)
{
	struct ShapeConsole: public Visible::Shape
	{
		virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const
		{
			Attribute& attribute = *((Attribute*)p_this->getAttribute());

			dest_board->fillRect(attribute.pos.x, attribute.pos.y, attribute.size.width, attribute.size.height, 0xFF545454);

			const int DIALOG_WINDOW_X = attribute.pos.x;
			const int DIALOG_WINDOW_Y = attribute.pos.y;
			const int DIALOG_WINDOW_W = attribute.size.width;
			const int DIALOG_WINDOW_H = attribute.size.height;

			{
				const BufferDesc* p_buffer_desc = &gfx::getFrameBuffer()->buffer_desc;

				unsigned long* p = (unsigned long*)p_buffer_desc->p_start_address;
				int ppl = (p_buffer_desc->bytes_per_line << 3) / p_buffer_desc->bits_per_pixel;

				p += (ppl * DIALOG_WINDOW_Y);
				p += DIALOG_WINDOW_X;

				yunjr::gfx::TextBoard text_board(p, DIALOG_WINDOW_W, DIALOG_WINDOW_H, ppl);

				sena::vector<shared::PcPlayer>& player_list = game::object::getPlayerList();

				{
					//SET_FONT_SIZE(32);
					//shared_ptr<Font> font = GET_RESIZED_FONT;
					Typeface typeface;
					typeface.attribute.size = 32;
					typeface.attribute.is_italic = false;
					typeface.attribute.is_bold = false;

					for (int i = 0; i < 6; i++)
					{
						if (i < player_list.size())
						{
							// rectangle
							int w = attribute.size.width;
							int h = 40*2;
							int x = attribute.pos.x;
							int y = attribute.pos.y + (h-1) * i;

							dest_board->fillRect(x, y, w, h, 0xFFC0C0C0);
							dest_board->fillRect(x+1, y+1, w-2, h-2, 0xFF545454);

							// light-green bar x 2
							const int BAR_W = 40*2;
							const int BAR_H = 12*2;
							const int BAR_X = x+3*2;
							const int BAR_Y = y + h - 3*2 - BAR_H;
							const int BAR_X_GAP = BAR_W+4*2;

							dest_board->fillRect(BAR_X + BAR_X_GAP * 0, BAR_Y, BAR_W, BAR_H, 0xFF20FF20);
							dest_board->fillRect(BAR_X + BAR_X_GAP * 1, BAR_Y, BAR_W, BAR_H, 0xFF20FF20);

							// name
							Text text(typeface, player_list[i]->getName());

							int text_x = (x - attribute.pos.x) + 10;
							int text_y = (y - attribute.pos.y) + 10 + 25;
							text_board.renderTextFx(text_x, text_y, text, 0xFFFFFFFF, 0xFFFFFFFF);

							// HP
							Typeface typeface_status;
							typeface_status.attribute.size = 20;
							typeface_status.attribute.is_italic = false;
							typeface_status.attribute.is_bold = false;

							char sz_status[256];
							sprintf(sz_status, "%d", player_list[i]->hp);

							Text textHP(typeface_status, sz_status);

							text_x += 5*2;
							text_y += 17*2;
							text_board.renderText(text_x, text_y, textHP, 0xFF000000, 0xFF000000);

							// SP
							sprintf(sz_status, "%d", player_list[i]->sp);

							Text textSP(typeface_status, sz_status);

							text_x += BAR_X_GAP;
							text_board.renderText(text_x, text_y, textSP, 0xFF000000, 0xFF000000);
						}
					}
				}
			}
		}
	};

	//?? 삭제 대상
	struct ShapeConsoleOld: public Visible::Shape
	{
		virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const
		{
			Attribute& attribute = *((Attribute*)p_this->getAttribute());

			dest_board->fillRect(attribute.pos.x, attribute.pos.y, attribute.size.width, attribute.size.height, 0xFF545454);

			const int DIALOG_WINDOW_X = attribute.pos.x;
			const int DIALOG_WINDOW_Y = attribute.pos.y;
			const int DIALOG_WINDOW_W = attribute.size.width;
			const int DIALOG_WINDOW_H = attribute.size.height;

			{
				const BufferDesc* p_buffer_desc = &gfx::getFrameBuffer()->buffer_desc;

				unsigned long* p = (unsigned long*)p_buffer_desc->p_start_address;
				int ppl = (p_buffer_desc->bytes_per_line << 3) / p_buffer_desc->bits_per_pixel;

				p += (ppl * DIALOG_WINDOW_Y);
				p += DIALOG_WINDOW_X;

				yunjr::gfx::TextBoard text_board(p, DIALOG_WINDOW_W, DIALOG_WINDOW_H, ppl);

				int text_x = 35;
				int text_y = 22;
				int text_y_gap = 15 * 2;

				sena::vector<shared::PcPlayer>& player_list = game::object::getPlayerList();

				{
					//SET_FONT_SIZE(32);
					//shared_ptr<Font> font = GET_RESIZED_FONT;
					Typeface typeface;
					typeface.attribute.size = 32;
					typeface.attribute.is_italic = false;
					typeface.attribute.is_bold = false;

					for (int i = 0; i < 6; i++)
					{
						if (i < player_list.size())
						{
							Text text(typeface, player_list[i]->getName());

							text_board.renderTextFx(text_x, text_y, text, 0xFFFFFFFF, 0xFFFFFFFF);

							char s[256];
							sprintf(s, "%d,  %d,  %d,  %d,  %d,  "
								, player_list[i]->hp, player_list[i]->sp
								, player_list[i]->esp, player_list[i]->ac
								, player_list[i]->level[0]);

							Text text_aux1(typeface, s);
							text_board.renderTextFx(text_x+150, text_y, text_aux1, 0xFFFFFFFF, 0xFFFFFFFF);

							Text text_aux2(typeface, player_list[i]->getConditionString());
							text_board.renderTextFx(text_x+400, text_y, text_aux2, 0xFFFFFFFF, 0xFFFFFFFF);

							text_y += text_y_gap;
						}
						else
						{
							Text text(typeface, L"reserved");

							text_board.renderTextFx(text_x, text_y, text, 0xFF804020, 0xFF804020);

							text_y += text_y_gap;
						}
					}
				}
/* ?? test case
				{
					Typeface typeface(30);

					Text text1(typeface, L"April is the cruelest month, breeding Lilacs out of the dead land, mixing Memory and desire, sirring");
					Text text2(typeface, L"");
					Text text3(typeface, L"");
					Text text4(typeface, L"");
					Text text5(typeface, L"");

					int guide = 250;
					text1.split(guide, text2);
					if (!text2.isEmpty())
					{
						text2.split(guide, text3);
						if (!text3.isEmpty())
						{
							text3.split(guide, text4);
							if (!text4.isEmpty())
							{
								text4.split(guide, text5);
							}
						}
					}

					text_board.renderTextFx(300,  25, text1, 0xFFFF0000, 0xFFFFFFFF);
					text_board.renderTextFx(300,  50, text2, 0xFFFF0000, 0xFFFFFFFF);
					text_board.renderTextFx(300,  75, text3, 0xFFFF0000, 0xFFFFFFFF);
					text_board.renderTextFx(300, 100, text4, 0xFFFF0000, 0xFFFFFFFF);
					text_board.renderTextFx(300, 125, text5, 0xFFFF0000, 0xFFFFFFFF);
				}
*/
			}
		}
	};

	struct UpdateConsole: public Visible::Update
	{
		virtual bool update(Visible* p_this, unsigned long tick)
		{
			return false;
		}
	};

	ControlStatus* p_status = new ControlStatus();

	*p_status << new Attribute(x, y, width, height) << new ShapeConsole() << new UpdateConsole();

	return p_status;
}

////////////////////////////////////////////////////////////////////////////////
// class ControlNoti

namespace yunjr
{
	struct ControlNoti::Attribute: public Visible::Attribute
	{
		Control::Pos pos;
		Control::Size size;

		Attribute(int x, int y, int width, int height)
		{
			pos.x = x;
			pos.y = y;
			size.width = width;
			size.height = height;
		}
	};
}

yunjr::ControlNoti::ControlNoti()
{
}

yunjr::ControlNoti::~ControlNoti()
{
}

yunjr::ControlNoti* yunjr::ControlNoti::newInstance(int x, int y, int width, int height)
{
	struct ShapeConsole: public Visible::Shape
	{
		virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const
		{
			static Typeface typeface;
			typeface.attribute.size = 32;
			typeface.attribute.is_italic = false;
			typeface.attribute.is_bold = false;

			// test
			static Text text(typeface, L">> The year is 2400 A. D. and the Tzorg Empire has enslaved the city of Metropolis for a while in its quest for galactic domination.  But wait!  There's hope! You, with the help of the Underground, a covert resistance movement, can stop them.");

			Attribute& attribute = *((Attribute*)p_this->getAttribute());

			dest_board->fillRect(attribute.pos.x, attribute.pos.y, attribute.size.width, attribute.size.height, Color32(0xFF300000).argb);

			{
				const BufferDesc* p_buffer_desc = &gfx::getFrameBuffer()->buffer_desc;

				unsigned long* p = (unsigned long*)p_buffer_desc->p_start_address;
				int ppl = (p_buffer_desc->bytes_per_line << 3) / p_buffer_desc->bits_per_pixel;

				yunjr::gfx::TextBoard text_board(p, attribute.size.width, attribute.size.height, ppl);

				{
					static int s_text_x = 720;
					int text_x = s_text_x;
					int text_y = 10 + 25;
					text_board.renderTextFx(text_x, text_y, text, 0xFFFFFFFF, 0xFFFF8080);

					s_text_x -= 3;
					if (s_text_x < - text.getExtents())
						s_text_x = 720;
				}
			}
		}
	};

	struct UpdateNoti: public Visible::Update
	{
		virtual bool update(Visible* p_this, unsigned long tick)
		{
			return true;
		}
	};

	ControlNoti* p_noti = new ControlNoti();

	*p_noti << new Attribute(x, y, width, height) << new ShapeConsole() << new UpdateNoti();

	return p_noti;
}

////////////////////////////////////////////////////////////////////////////////
// class ControlPanel

namespace yunjr
{
	struct ControlPanel::Attribute: public Visible::Attribute
	{
		struct { int x, y; } pos;

		Attribute()
		{
			pos.x = 0;
			pos.y = 0;
		}
	};
}

yunjr::ControlPanel::ControlPanel()
{
}

yunjr::ControlPanel* yunjr::ControlPanel::newInstance(void)
{
	struct ShapePanel: public Visible::Shape
	{
		virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const
		{
		}
	};

	struct UpdatePanel: public Visible::Update
	{
		virtual bool update(Visible* p_this, unsigned long tick)
		{
			Attribute& attribute = *((Attribute*)p_this->getAttribute());

			GameState& game_state = GameState::getMutableInstance();
			target::InputUpdateInfo& touch_info = game_state.current_input_info;

			if (game_state.checkKeyPressed(target::KEY_MENU))
				game_state.terminate();

			return false;
		}
	};

	ControlPanel* p_panel = new ControlPanel();

	*p_panel << new Attribute() << new ShapePanel() << new UpdatePanel();

	return p_panel;
}
