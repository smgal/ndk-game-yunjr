
#include "hd_class_map_event.h"

#include "hd_class_extern.h"
//??#include "hd_class_script.h"

#include "hd_script.h"

/////////

namespace hadar
{
	namespace script
	{
		namespace dummy
		{
			extern Callback callback;
		}

		namespace sample
		{
			extern Callback callback;
		}

		namespace lore
		{
			extern Callback callback;
		}

		namespace vesper
		{
			extern Callback callback;
		}
	}
}

/////////

namespace
{
	const hadar::script::Callback* g_scipt_callback_list[] =
	{
		&hadar::script::sample::callback,
		&hadar::script::lore::callback,
		&hadar::script::vesper::callback
	};

	const hadar::script::Callback* const g_p_null_script = &hadar::script::dummy::callback;

	const hadar::script::Callback* g_p_current_script = g_p_null_script;
}

/////////

void hadar::map_event::load(const char* sz_file_name)
{
	registerEventFileName(sz_file_name);

	//??Script script(Script::MODE_MAP, 0);

	const hadar::script::Callback** pp_callback = &g_scipt_callback_list[0];
	const hadar::script::Callback** pp_callback_end = pp_callback + sizeof(g_scipt_callback_list) / sizeof(g_scipt_callback_list[0]);

	while (pp_callback < pp_callback_end)
	{
		if ((*pp_callback)->identify(sz_file_name))
		{
			g_p_current_script = *pp_callback;
			break;
		}

		++pp_callback;
	}

	g_p_current_script->load();
}

void hadar::map_event::registerEventFileName(const char* sz_file_name)
{
	//??Script::registerScriptFileName(sz_file_name);
}

void hadar::map_event::occur(hadar::map_event::TYPE type, int position, int x, int y)
{
	switch (type)
	{
	case TYPE_TALK:
		{
			if (!g_p_current_script->talk(x, y))
			{
				//??Script script(Script::MODE_TALK, 0, x, y);
			}
		}
		break;

	case TYPE_SIGN:
		{
			if (!g_p_current_script->read(x, y))
			{
				//??Script script(Script::MODE_SIGN, 0, x, y);
			}
		}
		break;

	case TYPE_EVENT:
		{
			if (!g_p_current_script->occur(x, y))
			{
				//??Script script(Script::MODE_EVENT, 0, x, y);
			}
		}
		break;

	case TYPE_ENTER:
		{
			if (!g_p_current_script->enter(x, y))
			{
				//??Script script(Script::MODE_EVENT, 0, x, y);
			}
		}
		break;
	}
}
