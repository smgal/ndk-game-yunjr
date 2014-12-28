
#ifndef __YUNJR_CLASS_MAP_EVENT_H__
#define __YUNJR_CLASS_MAP_EVENT_H__

namespace yunjr
{
	namespace map_event
	{
		enum TYPE
		{
			TYPE_TALK,
			TYPE_SIGN,
			TYPE_EVENT,
			TYPE_ENTER
		};

		void load(const char* sz_file_name);
		void registerEventFileName(const char* sz_file_name);

		void occur(TYPE type, int position, int x = 0, int y = 0);
	}

} // namespace yunjr

#endif // #ifndef __YUNJR_CLASS_MAP_EVENT_H__
