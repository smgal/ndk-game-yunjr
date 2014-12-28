
#if defined(YOZORA_ANDROID)

#include "../target_dep.h"

extern "C" void g_printLog(const char* sz_log);
extern "C" unsigned long g_getTicks(void);
extern "C" void g_waitMillisecond(int msec);
extern "C" void g_updateScreen(void);

#include <stdio.h>

unsigned long target::system::getTicks(void)
{
	return g_getTicks();
}

void target::system::wait(int msec)
{
	g_waitMillisecond(msec);
}

void target::system::log(const char* sz_message)
{
	g_printLog(sz_message);
}

void target::updateScreen(void)
{
	g_updateScreen();
}

#endif // #if defined(YOZORA_ANDROID)
