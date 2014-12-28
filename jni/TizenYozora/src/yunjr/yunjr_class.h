
#ifndef __YUNJR_CLASS_H__
#define __YUNJR_CLASS_H__

#include "yunjr_base.h"
#include "yunjr_base_gfx.h"

namespace yunjr
{
	struct Object
	{
	};

	struct Persist: public Object
	{
		//assign
		//assignTo
	};

	struct Visible: public Persist
	{
		struct Attribute
		{
			virtual ~Attribute() { }
		};

		struct Shape
		{
			virtual ~Shape() { }
			virtual void render(Visible* p_this, shared::FlatBoard32 dest_board) const = 0;
		};

		struct Update
		{
			virtual ~Update() { }
			virtual bool update(Visible* p_this, unsigned long tick) = 0;
		};

		Attribute* p_attribute;
		Shape* p_shape;
		Update* p_update;

		bool is_visible;
		bool is_damaged;

		Visible()
			: p_attribute(0)
			, p_shape(0)
			, p_update(0)
			, is_visible(true)
			, is_damaged(true)
		{
		}

		virtual ~Visible()
		{
			delete p_update;
			delete p_shape;
			delete p_attribute;
		}

		Visible& operator<<(Attribute* in_p_attribute)
		{
			delete p_attribute;
			p_attribute = in_p_attribute;

			return *this;
		}

		Visible& operator<<(Shape* in_p_shape)
		{
			delete p_shape;
			p_shape = in_p_shape;

			return *this;
		}

		Visible& operator<<(Update* in_p_update)
		{
			delete p_update;
			p_update = in_p_update;

			return *this;
		}

		Attribute* getAttribute(void) const
		{
			return p_attribute;
		}

		void invalidate(void)
		{
			is_damaged = true;
		}

		void show(void)
		{
			is_visible = true;
			is_damaged = true;
		}

		void hide(void)
		{
			is_visible = false;
		}

		void update(unsigned long tick)
		{
			if (p_update)
				is_damaged |= p_update->update(this, tick);
		}

		void render(shared::FlatBoard32 dest_board)
		{
			if (p_shape && is_visible && is_damaged)
			{
				p_shape->render(this, dest_board);
				is_damaged = false;
			}
		}
	};
}

#endif // #ifndef __YUNJR_CLASS_H__
