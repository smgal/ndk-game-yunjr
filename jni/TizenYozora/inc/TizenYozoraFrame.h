#ifndef _TIZEN_YOZORA_FRAME_H_
#define _TIZEN_YOZORA_FRAME_H_

#include <FApp.h>
#include <FBase.h>
#include <FSystem.h>
#include <FUi.h>
#include <FUiIme.h>
#include <FGraphics.h>
#include <gl.h>

class TizenYozoraFrame
	: public Tizen::Ui::Controls::Frame
{
public:
	TizenYozoraFrame(void);
	virtual ~TizenYozoraFrame(void);

private:
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
};

#endif	//_TIZEN_YOZORAFRAME_H_
