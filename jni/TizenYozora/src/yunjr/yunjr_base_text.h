
#ifndef __YUNJR_BASE_TEXT_H__
#define __YUNJR_BASE_TEXT_H__

namespace yunjr
{
	struct Typeface;

	struct Text
	{
	public:
		enum ALIGNMETHOD
		{
			ALIGNMETHOD_LEFT,
			ALIGNMETHOD_RIGHT,
			ALIGNMETHOD_CENTER,
			ALIGNMETHOD_FIT
		};

		Text();

		Text(const Typeface& typeface, const char* sz_text_utf8);
		Text(const Typeface& typeface, const char* sz_text_utf8, size_t length);

		Text(const Typeface& typeface, const wchar_t* sz_text);
		Text(const Typeface& typeface, const wchar_t* sz_text, size_t length);

		~Text();

		bool isEmpty(void);

		void setColor(unsigned long color);
		void split(int width, Text& remaining_text, ALIGNMETHOD align = ALIGNMETHOD_LEFT);

		struct Impl;

	private:
		Impl* p_impl;
	};

}

#endif // #ifndef __YUNJR_BASE_TEXT_H__
