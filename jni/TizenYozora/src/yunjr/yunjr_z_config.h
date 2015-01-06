
#ifndef __YUNJR_Z_CONFIG_H__
#define __YUNJR_Z_CONFIG_H__

#include "../flat_board/flat_board.h"
#include "../flat_board/target_dep.h"

namespace yunjr
{
	struct BufferDesc
	{
		void* p_start_address;
		int   width;
		int   height;
		int   bytes_per_line;
		int   bits_per_pixel;
	};

	void init(const char* sz_id, const char* sz_data_path);
	void done(void);
	bool loop(const BufferDesc& buffer_desc);

	typedef unsigned long TargetPixel;
}

namespace yunjr
{
	const int MAP_SCALE_RATIO = 2;

	/*
	 * MAP_SCROLL_IN_PIXELS * m = TILE_W
	 * MAP_SCROLL_IN_PIXELS * n = TILE_H
	 * (m and n must be are integers)
	 */
	const int MAP_SCROLL_IN_PIXELS = 4;
// 28 -> 34  1.214
	const int DEFAULT_FONT_SIZE = 32;
	const int DEFAULT_FONT_ASCENDER = 30;
	const int DEFAULT_FONT_BTBD = (21 * 2); // Baseline to Baseline Distance
	const int DEFAULT_BATTLE_BTBD = (21 * 2);

	const int TILE_W = 24;
	const int TILE_H = 24;
}

#endif
