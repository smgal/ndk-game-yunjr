
#ifndef __HD_Z_CONFIG_H__
#define __HD_Z_CONFIG_H__

#pragma warning(disable: 4996)

#include <assert.h>

#define CT_ASSERT(x, msg) typedef int __CT_ASSERT ## msg ## __ [(x) ? 1 : -1];
#define ASSERT(cond) assert(cond)

#define nullptr 0

#define SCREEN_BASE_WIDTH 640

namespace hadar
{
	const int S_SCREEN_WIDTH  = SCREEN_BASE_WIDTH;
	const int S_SCREEN_HEIGHT = 480;

	typedef unsigned short Pixel;
}

#endif
