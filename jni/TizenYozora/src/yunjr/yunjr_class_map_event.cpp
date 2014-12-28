
#include "yunjr_class_map_event.h"
#include "yunjr_class_extern.h"
#include "yunjr_script.h"

/////////

namespace yunjr
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
	}
}

/////////

namespace
{
	const yunjr::script::Callback* g_scipt_callback_list[] =
	{
		&yunjr::script::sample::callback
	};

	const yunjr::script::Callback* const g_p_null_script = &yunjr::script::dummy::callback;

	const yunjr::script::Callback* g_p_current_script = g_p_null_script;
}

/////////

void yunjr::map_event::load(const char* sz_file_name)
{
	registerEventFileName(sz_file_name);

	//??Script script(Script::MODE_MAP, 0);

	const yunjr::script::Callback** pp_callback = &g_scipt_callback_list[0];
	const yunjr::script::Callback** pp_callback_end = pp_callback + sizeof(g_scipt_callback_list) / sizeof(g_scipt_callback_list[0]);

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

void yunjr::map_event::registerEventFileName(const char* sz_file_name)
{
	//??Script::registerScriptFileName(sz_file_name);
}

void yunjr::map_event::occur(yunjr::map_event::TYPE type, int position, int x, int y)
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
