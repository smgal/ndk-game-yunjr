
#ifndef __HD_SCRIPT_H__
#define __HD_SCRIPT_H__

namespace hadar
{
	namespace script
	{
		struct Callback
		{
			bool (*identify)(const char* sz_map_name);
			bool (*load)(void);
			bool (*unload)(void);
			bool (*talk)(int x, int y);
			bool (*read)(int x, int y);
			bool (*occur)(int x, int y);
			bool (*enter)(int x, int y);

			Callback
			(
				bool (*in_identify)(const char* sz_map_name),
				bool (*in_load)(void),
				bool (*in_unload)(void),
				bool (*in_talk)(int x, int y),
				bool (*in_read)(int x, int y),
				bool (*in_occur)(int x, int y),
				bool (*in_enter)(int x, int y)
			)
				: identify(in_identify)
				, load(in_load)
				, unload(in_unload)
				, talk(in_talk)
				, read(in_read)
				, occur(in_occur)
				, enter(in_enter)
			{
			}
		};
	}
}

#endif // #ifndef __HD_SCRIPT_H__
