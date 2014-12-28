
#ifndef __YUNJR_UTIL_H__
#define __YUNJR_UTIL_H__

#include "yunjr_res.h"


#define SET_FONT_SIZE(size) ::yunjr::FontSizeTemporarily<size> _
#define GET_RESIZED_FONT _()

#define INVALIDATE_MAP     yunjr::resource::getMainWindow()->findControl("MAP")->invalidate()
#define INVALIDATE_SUB_MAP ; //??
#define INVALIDATE_STATUS  yunjr::resource::getMainWindow()->findControl("STATUS")->invalidate()
#define INVALIDATE_BATTLE  yunjr::resource::getMainWindow()->findControl("BATTLE")->invalidate()

namespace yunjr
{
	template <int font_size>
	class FontSizeTemporarily
	{
		enum { temporary_font_size = font_size };

	public:
		FontSizeTemporarily()
		{
			m_font = resource::getCurrentFont();
			m_old_size = m_setFontSize(temporary_font_size);
		}

		~FontSizeTemporarily()
		{
			m_setFontSize(m_old_size);
		}

		shared::Font operator()(void) const
		{
			return m_font;
		}

	private:
		shared::Font m_font;
		int m_old_size;

		int m_setFontSize(int new_size)
		{
			FontAttrib font_attrib;

			m_font->get(font_attrib);

			int old_size = font_attrib.size;
			font_attrib.size = new_size;

			m_font->set(font_attrib);

			return old_size;
		}
	};

} // namespace yunjr


#endif // #ifndef __YUNJR_UTIL_H__
