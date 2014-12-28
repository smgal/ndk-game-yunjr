
#ifndef __YUNJR_RES_H__
#define __YUNJR_RES_H__

#include "yunjr_base.h"
#include "yunjr_base_gfx.h"
#include "yunjr_base_res.h"

#include <vector>

namespace yunjr
{
	class Font;

	struct Control;
	struct ControlWindow;
	struct Chara;

	namespace resource
	{
		void init();

		void setCurrentMapPos(int x, int y);
		void getCurrentMapPos(int& x, int& y);

		void setCurrentFont(shared::Font font);
		shared::Font getCurrentFont(void);

		void setMainWindow(yunjr::ControlWindow* p_window);
		yunjr::ControlWindow* getMainWindow(void);

		const Tile& getTile(TileId tile_id, int id_offset = 0);
		const shared::FlatBoard32& getResimage(ResId res_id);

		std::vector<Chara*>& getCharaList(void);
	}
}

#endif
