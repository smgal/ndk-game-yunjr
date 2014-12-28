
#include "../target_dep.h"
#include <FSystem.h>
#include <FUiControl.h>

extern void* g_h_native_window;
extern void UpdateScreen(void);


unsigned long target::system::getTicks(void)
{
	static double s_guide = 0;

	if (s_guide == 0)
	{
		long long tick;
		Tizen::System::SystemTime::GetTicks(tick);
		s_guide = tick;
	}

	long long tick;
	Tizen::System::SystemTime::GetTicks(tick);
	double current_tick = tick;

	return (unsigned long)((int)(current_tick - s_guide));
}

void target::system::wait(int msec)
{
	//??
}

int target::getScreenWidth(void)
{
	Tizen::Ui::Control* p_control = (Tizen::Ui::Control*)g_h_native_window;
	return (p_control) ? p_control->GetWidth() : 0;
}

int target::getScreenHeight(void)
{
	Tizen::Ui::Control* p_control = (Tizen::Ui::Control*)g_h_native_window;
	return (p_control) ? p_control->GetHeight() : 0;
}

void target::updateScreen(void)
{
	UpdateScreen();
}
