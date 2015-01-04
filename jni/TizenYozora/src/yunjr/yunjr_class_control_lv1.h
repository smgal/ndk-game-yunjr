
#ifndef __YUNJR_CLASS_CONTROL_LV1_H__
#define __YUNJR_CLASS_CONTROL_LV1_H__

#include "yunjr_base.h"

#include "yunjr_class.h"
#include "yunjr_class_control.h"

namespace yunjr
{
	struct ControlWindow: public Control
	{
		struct Attribute;

	public:
		virtual ~ControlWindow();

		void     addChild(ControlId control_id);
		Control* findControl(ResId res_id);

		void     invalidateBounds(Pos pos, Size size);

		static   ControlWindow* newInstance(int width, int height);

	private:
		ControlWindow();
	};

	struct ControlWaku: public Control
	{
		struct Attribute;

	public:
		static ControlWaku* newInstance(void);

	private:
		ControlWaku();

	};

	struct ControlConsole: public Control
	{
		struct Attribute;

	public:
		virtual ~ControlConsole();

		void clear(void);
		void add(Text& text);

		void drawText(int x, int y, const wchar_t* sz_text, unsigned long color);

		void getRegion(int& x1, int& y1, int& x2, int& y2) const;
		void setRegion(int x1, int y1, int x2, int y2);

		void getMargin(int& left, int& top, int& right, int& bottom) const;
		void setMargin(int left, int top, int right, int bottom);

		static ControlConsole* newInstance(int x, int y, int width, int height, int margin_left, int margin_right, int margin_top, int margin_bottom);

	private:
		ControlConsole();

	};

	struct ControlStatus: public Control
	{
		struct Attribute;

	public:
		virtual ~ControlStatus();

		static   ControlStatus* newInstance(int x, int y, int width, int height);

	private:
		ControlStatus();

	};

	struct ControlNoti: public Control
	{
		struct Attribute;

	public:
		virtual ~ControlNoti();

		static   ControlNoti* newInstance(int x, int y, int width, int height);

	private:
		ControlNoti();

	};

	struct ControlPanel: public Control
	{
		struct Attribute;

	public:
		static   ControlPanel* newInstance(void);

	private:
		ControlPanel();

	};

}

#endif // #ifndef __YUNJR_CLASS_CONTROL_LV1_H__
