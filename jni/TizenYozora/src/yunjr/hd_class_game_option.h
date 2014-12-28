
#ifndef __HD_CLASS_GAME_OPTION_H__
#define __HD_CLASS_GAME_OPTION_H__

#include "util/sm_util_string.h"

#include "hd_base_serialized.h"

namespace hadar
{
	struct GameOption: public Serialized
	{
		enum
		{
			MAX_FLAG     = 256,
			MAX_VARIABLE = 256
		};

		// save
		sena::boolflag<MAX_FLAG> flag;
		sena::intflag<unsigned char, MAX_VARIABLE> variable;
		smutil::string script_file;

		// unsave
		bool is_dubug_mode;

		GameOption();

	protected:
		bool _load(const smutil::ReadStream& stream);
		bool _save(const smutil::WriteStream& stream) const;
	};

} // namespace hadar

#endif // #ifndef __HD_CLASS_GAME_OPTION_H__
