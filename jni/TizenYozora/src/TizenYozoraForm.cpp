
#include "TizenYozoraForm.h"
#include "AppResourceId.h"

using namespace Tizen::Base;
using namespace Tizen::App;
using namespace Tizen::Graphics;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;

////////////////////////////////////////////////////////////////////////////////

namespace yozora
{
	struct BufferDesc
	{
		void* p_start_address;
		int   width;
		int   height;
		int   bytes_per_line;
		int   bits_per_pixel;
	};

	void init(const char* sz_id);
	void done();
	bool loop(const BufferDesc& buffer_desc);
}

namespace
{
	bool s_has_touch_event = false;
	int  s_touch_x = -1;
	int  s_touch_y = -1;

	Bitmap* s_p_frame_buffer = 0;

	Tizen::Base::Runtime::Mutex s_lock;
	Tizen::Base::Runtime::Monitor prepared;

}

extern bool g_is_terminating;
extern Tizen::Ui::Controls::Form* s_p_ref_form;
extern void target_setTouchPos(int x, int y);

void UpdateScreen(void);

////////////////////////////////////////////////////////////////////////////////

 class MyThread: public Tizen::Base::Runtime::Thread
 {
	Tizen::Base::Runtime::Monitor& m_prepared;

 public:
	MyThread(Tizen::Base::Runtime::Monitor& prepared)
		: m_prepared(prepared)
	{
	}

	virtual ~MyThread(void)
	{
	}

	result Construct(void)
	{
		return Thread::Construct();
	}

	Object* Run(void)
	{
		m_prepared.Enter();

		yozora::init("lore20th");

		BufferInfo buffer_info;

		Bitmap& bitmap = *s_p_frame_buffer;

		if (bitmap.Lock(buffer_info) == E_SUCCESS)
		{
			yozora::BufferDesc buffer_desc;

			buffer_desc.p_start_address = buffer_info.pPixels;
			buffer_desc.width           = buffer_info.width;
			buffer_desc.height          = buffer_info.height;
			buffer_desc.bytes_per_line  = buffer_info.pitch;
			buffer_desc.bits_per_pixel  = buffer_info.bitsPerPixel;

			g_is_terminating = false;

			m_prepared.Notify();
			m_prepared.Exit();

			while (!g_is_terminating)
			{
				{
					s_lock.Acquire();

					g_is_terminating = !yozora::loop(buffer_desc);

					s_lock.Release();
				}

				Tizen::Base::Runtime::Thread::Sleep(10);
			}
		}
		bitmap.Unlock();

		yozora::done();

		return null;
	}
 };

////////////////////////////////////////////////////////////////////////////////

TizenYozoraForm::TizenYozoraForm(void)
{
	s_lock.Create();
}

TizenYozoraForm::~TizenYozoraForm(void)
{
	delete s_p_frame_buffer;
	s_p_frame_buffer = 0;
}

bool
TizenYozoraForm::Initialize(void)
{
	Construct(FORM_STYLE_NORMAL);

	return true;
}
result
TizenYozoraForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	// TODO: Add your initialization code here

	// Setup back event listener
	SetFormBackEventListener(this);

	s_p_frame_buffer = new Bitmap;
	s_p_frame_buffer->Construct(Dimension(CoordinateSystem::ConvertToLogicalX(1280), CoordinateSystem::ConvertToLogicalY(720)), BITMAP_PIXEL_FORMAT_ARGB8888);


	prepared.Construct();

	MyThread myThread(prepared);

	prepared.Enter();

	if (myThread.Construct() == E_SUCCESS)
	{
		myThread.Start();
		prepared.Wait();
		prepared.Exit();
	}
	else
	{
		AppLog("[SMGAL] Woker thread not created");
	}

	this->AddTouchEventListener(*this);

	return r;
}

result
TizenYozoraForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	return r;
}

void
TizenYozoraForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source)
{
	UiApp* pApp = UiApp::GetInstance();
	AppAssert(pApp);
	pApp->Terminate();
}

result
TizenYozoraForm::OnDraw(void)
{
	Tizen::Base::Runtime::MutexGuard m(s_lock);

	if (g_is_terminating)
		return E_SUCCESS;

	if (s_has_touch_event)
	{
		target_setTouchPos(s_touch_x, s_touch_y);
		s_has_touch_event = false;
	}

	Canvas* p_canvas = GetCanvasN();

	if (p_canvas)
	{
		prepared.Enter();

		p_canvas->SetBackgroundColor(Color(0xFF000000));
		p_canvas->Clear();
		p_canvas->DrawBitmap(p_canvas->GetBounds(), *s_p_frame_buffer);

		prepared.NotifyAll();
		prepared.Exit();
	}

	delete p_canvas;

	return E_SUCCESS;
}

void
TizenYozoraForm::OnTouchPressed(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo & touchInfo)
{
	AppLog("[SMGAL] OnTouchPressed(%d, %d)", currentPosition.x, currentPosition.y);
	s_has_touch_event = true;
	s_touch_x = currentPosition.x;
	s_touch_y = currentPosition.y;
}

void
TizenYozoraForm::OnTouchReleased(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo)
{
	s_has_touch_event = true;
	s_touch_x = -1;
	s_touch_y = -1;
}

void
TizenYozoraForm::OnTouchMoved(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo)
{
	s_has_touch_event = true;
	s_touch_x = currentPosition.x;
	s_touch_y = currentPosition.y;
}

void
TizenYozoraForm::OnTouchFocusIn(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo) {}

void
TizenYozoraForm::OnTouchFocusOut(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo) {}


void UpdateScreen(void)
{
	if (s_p_ref_form == 0)
		return;

	s_lock.Release();

	{
		prepared.Enter();
		s_p_ref_form->Invalidate	(false);
		prepared.Wait();
		prepared.Exit();
	}

	s_lock.Acquire();
}

