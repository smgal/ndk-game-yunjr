
#include "hd_base_config.h"

#if   (SCREEN_BASE_WIDTH == 480)
	#define CONFIG_NUM 1
#elif (SCREEN_BASE_WIDTH == 320)
	#define CONFIG_NUM 2
#elif (SCREEN_BASE_WIDTH == 800)
	#define CONFIG_NUM 0
#elif (SCREEN_BASE_WIDTH == 640)
	#define CONFIG_NUM 3
#endif

#if !defined(CONFIG_NUM)
	#error cannot support current platform
#endif

const int hadar::config::DEFAULT_FONT_HEIGHT = 16;
const int hadar::config::DEFAULT_FONT_WIDTH = hadar::config::DEFAULT_FONT_HEIGHT / 2;

const int hadar::config::DEFAULT_TILE_DISPLAY_WIDTH = 24;
const int hadar::config::DEFAULT_TILE_DISPLAY_HEIGHT = 24;

const unsigned long hadar::config::DEFAULT_BG_COLOR = 0xFF525252;

#if (CONFIG_NUM == 0)

	const int SCREEN_DISPLAY_WIDTH = 800;
	const int SCREEN_DISPLAY_HEIGHT = 480+100;

	// 실제로 사용되지는 않는 디폴트 값
	const hadar::config::Rect hadar::config::REGION_MAP_WINDOW =
	{
		0,
		0,
		hadar::config::DEFAULT_TILE_DISPLAY_WIDTH * 9,
		hadar::config::DEFAULT_TILE_DISPLAY_HEIGHT * 10 
	};

	const int CONSOLE_WINDOW_MARGIN_LEFT = 16;
	const int CONSOLE_WINDOW_MARGIN_TOP = 16;
	const int CONSOLE_WINDOW_MARGIN_RIGHT = 16;
	const int CONSOLE_WINDOW_MARGIN_BOTTOM = 16;

	const hadar::config::Rect hadar::config::REGION_CONSOLE_WINDOW =
	{
		CONSOLE_WINDOW_MARGIN_LEFT + hadar::config::REGION_MAP_WINDOW.x + hadar::config::REGION_MAP_WINDOW.w,
		CONSOLE_WINDOW_MARGIN_TOP + hadar::config::REGION_MAP_WINDOW.y,
		SCREEN_DISPLAY_WIDTH - (hadar::config::REGION_MAP_WINDOW.x + hadar::config::REGION_MAP_WINDOW.w) - (CONSOLE_WINDOW_MARGIN_LEFT + CONSOLE_WINDOW_MARGIN_RIGHT),
		hadar::config::DEFAULT_FONT_HEIGHT * 15
	};

	const int STATUS_Y = hadar::config::REGION_MAP_WINDOW.y + hadar::config::REGION_MAP_WINDOW.h + (0);

	const hadar::config::Rect hadar::config::REGION_STATUS_WINDOW =
	{
		hadar::config::DEFAULT_TILE_DISPLAY_WIDTH,
		STATUS_Y,
		SCREEN_DISPLAY_WIDTH,
		SCREEN_DISPLAY_HEIGHT - STATUS_Y
	};

#elif (CONFIG_NUM == 1)

const hadar::config::Rect hadar::config::REGION_MAP_WINDOW     = {0, 0, 24*9, 24*9};
const hadar::config::Rect hadar::config::REGION_CONSOLE_WINDOW = {24*9, 0, 480-24*9, 24*9};
const hadar::config::Rect hadar::config::REGION_STATUS_WINDOW  = {0, 24*10, 480, 360-24*10};

#elif (CONFIG_NUM == 2)

const hadar::config::Rect hadar::config::REGION_MAP_WINDOW     = {0, 0, 24*6, 24*7};
const hadar::config::Rect hadar::config::REGION_CONSOLE_WINDOW = {24*6, 0, S_SCREEN_WIDTH-24*6, 24*7};
const hadar::config::Rect hadar::config::REGION_STATUS_WINDOW  = {0, 24*7, S_SCREEN_WIDTH, S_SCREEN_HEIGHT-24*7};

#elif (CONFIG_NUM == 3)

	const int SCREEN_DISPLAY_WIDTH = 640;
	const int SCREEN_DISPLAY_HEIGHT = 480;

	const hadar::config::Rect hadar::config::REGION_MAP_WINDOW =
	{
		24,
		24,
		hadar::config::DEFAULT_TILE_DISPLAY_WIDTH * 9,
		hadar::config::DEFAULT_TILE_DISPLAY_HEIGHT * 11 
	};

	const hadar::config::Rect hadar::config::REGION_MAP_SUB_WINDOW =
	{
		24,
		24 * 13 - 12,
		hadar::config::DEFAULT_TILE_DISPLAY_WIDTH * 9,
		hadar::config::DEFAULT_TILE_DISPLAY_HEIGHT * 7 
	};

	const int CONSOLE_WINDOW_MARGIN_LEFT = 24;
	const int CONSOLE_WINDOW_MARGIN_TOP = 24;
	const int CONSOLE_WINDOW_MARGIN_RIGHT = 24*2;
	const int CONSOLE_WINDOW_MARGIN_BOTTOM = 24;

	const hadar::config::Rect hadar::config::REGION_CONSOLE_WINDOW =
	{
		CONSOLE_WINDOW_MARGIN_LEFT + hadar::config::REGION_MAP_WINDOW.x + hadar::config::REGION_MAP_WINDOW.w,
		CONSOLE_WINDOW_MARGIN_TOP + hadar::config::REGION_MAP_WINDOW.y,
		SCREEN_DISPLAY_WIDTH - (hadar::config::REGION_MAP_WINDOW.x + hadar::config::REGION_MAP_WINDOW.w) - (CONSOLE_WINDOW_MARGIN_LEFT + CONSOLE_WINDOW_MARGIN_RIGHT),
		hadar::config::DEFAULT_FONT_HEIGHT * 15
	};

	const int STATUS_Y = hadar::config::REGION_CONSOLE_WINDOW.y + hadar::config::REGION_CONSOLE_WINDOW.h + 24;

	const hadar::config::Rect hadar::config::REGION_STATUS_WINDOW =
	{
		hadar::config::REGION_CONSOLE_WINDOW.x,
		STATUS_Y,
		hadar::config::REGION_CONSOLE_WINDOW.w,
		SCREEN_DISPLAY_HEIGHT - STATUS_Y
	};

#else

	#error cannot support current platform

#endif

////////////////////////////////////////////////////////////////////////////////
// mutable
