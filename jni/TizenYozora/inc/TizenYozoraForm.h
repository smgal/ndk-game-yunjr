#ifndef _TIZEN_YOZORA_FORM_H_
#define _TIZEN_YOZORA_FORM_H_

#include <FApp.h>
#include <FBase.h>
#include <FSystem.h>
#include <FUi.h>
#include <FUiIme.h>
#include <FGraphics.h>
#include <gl.h>

class TizenYozoraForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::ITouchEventListener
{
public:
	TizenYozoraForm(void);
	virtual ~TizenYozoraForm(void);
	bool Initialize(void);

private:
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void   OnFormBackRequested(Tizen::Ui::Controls::Form& source);
	virtual result OnDraw(void);

	virtual void   OnTouchPressed(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo & touchInfo);
	virtual void   OnTouchReleased(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo);
	virtual void   OnTouchMoved(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo);
	virtual void   OnTouchFocusIn(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo);
	virtual void   OnTouchFocusOut(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo);
};

#endif	//_TIZEN_YOZORA_FORM_H_
