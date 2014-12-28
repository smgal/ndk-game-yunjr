
#ifndef __PIXEL_FORMAT_H__
#define __PIXEL_FORMAT_H__

namespace flat_board
{
	enum PIXELFORMAT
	{
		PIXELFORMAT_INVALID = 0,
		PIXELFORMAT_RGB565,
		PIXELFORMAT_ARGB8888,
	};

	struct Color32
	{
		Color32(unsigned long argb8888)
			: argb(argb8888)
		{
		#if defined(PIXELFORMAT_ABGR)
			argb = (argb & 0xFF00FF00) | ((argb >> 16) & 0x000000FF) | ((argb << 16) & 0x00FF0000);
		#endif
		}

		unsigned long argb;
	};
}

#endif // #ifndef __PIXEL_FORMAT_H__
