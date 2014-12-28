#include <new>
#include "TizenYozoraApp.h"
#include "TizenYozoraFrame.h"

using namespace Tizen::App;
using namespace Tizen::System;

////////////////////////////////////////////////////////////////////////////////

namespace
{
	enum { TIME_OUT = 10 };

	Tizen::Base::Runtime::Timer* s_p_timer = null;
}

Tizen::Ui::Controls::Form* s_p_ref_form = null;

bool g_is_terminating = false;
extern void* g_h_native_window;

#define CHECK_COND(cond) if ((cond) == 0) return false;
#define CHECK_RESULT(r)  if (IsFailed(r)) return false;

////////////////////////////////////////////////////////////////////////////////

TizenYozoraApp::TizenYozoraApp(void)
{
	s_p_timer = null;
}

TizenYozoraApp::~TizenYozoraApp(void)
{
	if (s_p_timer)
	{
		s_p_timer->Cancel();
		delete s_p_timer;
		s_p_timer = null;
	}

	s_p_ref_form = null;
}

UiApp*
TizenYozoraApp::CreateInstance(void)
{
	// TODO: Create the application instance through the constructor.
	return new (std::nothrow) TizenYozoraApp();
}

bool
TizenYozoraApp::OnAppInitializing(AppRegistry& appRegistry)
{
	// TODO: Initialize application-specific data.
	// The permanent data and context of the application can be obtained from the application registry (appRegistry).
	//
	// If this method is successful, return true; otherwise, return false and the application is terminated.


	// Uncomment the following statement to listen to the screen on and off events:
	// PowerManager::SetScreenEventListener(*this);

	return true;
}

bool
TizenYozoraApp::OnAppInitialized(void)
{
	// TODO: Create the application frame.

	TizenYozoraFrame* pTizenYozoraFrame = new (std::nothrow) TizenYozoraFrame;
	TryReturn(pTizenYozoraFrame != null, false, "The memory is insufficient.");

	pTizenYozoraFrame->Construct();
	pTizenYozoraFrame->SetName(L"TizenYozora");

	AddFrame(*pTizenYozoraFrame);

	pTizenYozoraFrame->SetOrientation(Tizen::Ui::ORIENTATION_LANDSCAPE);

	s_p_timer = new Tizen::Base::Runtime::Timer;
	CHECK_COND(s_p_timer);

	result r = s_p_timer->Construct(*this);
	CHECK_RESULT(r);

	return true;
}

bool
TizenYozoraApp::OnAppWillTerminate(void)
{
	// TODO: Deallocate or release resources in devices that have the END key.
	return true;
}

bool
TizenYozoraApp::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	// TODO: Deallocate all resources allocated by the application.
	// The permanent data and context of the application can be saved through the application registry (appRegistry).
	return true;
}

void
TizenYozoraApp::OnForeground(void)
{
	if (s_p_timer)
	{
		s_p_timer->Start(TIME_OUT);
	}
}

void
TizenYozoraApp::OnBackground(void)
{
	if (s_p_timer)
	{
		s_p_timer->Cancel();
	}
}

void
TizenYozoraApp::OnLowMemory(void)
{
	// TODO: Free unnecessary resources or close the application.
}

void
TizenYozoraApp::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
	// TODO: Handle all battery level changes here.
	// Stop using multimedia features (such as camera and mp3 playback) if the battery level is CRITICAL.
}

void
TizenYozoraApp::OnScreenOn(void)
{
	// TODO: Retrieve the released resources or resume the operations that were paused or stopped in the OnScreenOff() event handler.
}

void
TizenYozoraApp::OnScreenOff(void)
{
	// TODO: Release resources (such as 3D, media, and sensors) to allow the device to enter the sleep mode 
	// to save the battery (unless you have a good reason to do otherwise).
	// Only perform quick operations in this event handler. Any lengthy operations can be risky; 
	// for example, invoking a long asynchronous method within this event handler can cause problems
	// because the device can enter the sleep mode before the callback is invoked.

}

void
TizenYozoraApp::OnTimerExpired(Tizen::Base::Runtime::Timer& timer)
{
	if (g_is_terminating)
	{
		this->Terminate();
		return;
	}

	if (!s_p_timer)
	{
		return;
	}

	s_p_timer->Start(TIME_OUT);

	s_p_ref_form = (Tizen::Ui::Controls::Form*)g_h_native_window;

	s_p_ref_form->Draw();
	s_p_ref_form->Show();
}
