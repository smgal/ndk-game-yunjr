
#ifndef __HD_CLASS_MAP_EVENT_H__
#define __HD_CLASS_MAP_EVENT_H__

//#include ""

namespace hadar
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

} // namespace hadar

#endif // #ifndef __HD_CLASS_MAP_EVENT_H__
