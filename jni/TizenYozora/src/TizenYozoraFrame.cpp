#include <new>
#include "TizenYozoraFrame.h"
#include "TizenYozoraForm.h"

using namespace Tizen::Base;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;


extern void* g_h_native_window;


TizenYozoraFrame::TizenYozoraFrame(void)
{
}

TizenYozoraFrame::~TizenYozoraFrame(void)
{
}

result
TizenYozoraFrame::OnInitializing(void)
{
	result r = E_SUCCESS;

	// Create a form
	TizenYozoraForm* pTizenYozoraForm = new (std::nothrow) TizenYozoraForm();
	TryReturn(pTizenYozoraForm != null, false, "The memory is insufficient.");
	pTizenYozoraForm->Initialize();

	g_h_native_window = static_cast<void*>(pTizenYozoraForm);

	// Add the form to the frame
	AddControl(pTizenYozoraForm);

	// Set the current form
	SetCurrentForm(pTizenYozoraForm);

	// Draw the form
	pTizenYozoraForm->Invalidate(true);

	// TODO: Add your frame initialization code here.

	return r;
}

result
TizenYozoraFrame::OnTerminating(void)
{
	result r = E_SUCCESS;

	// TODO: Add your frame termination code here.
	return r;
}


