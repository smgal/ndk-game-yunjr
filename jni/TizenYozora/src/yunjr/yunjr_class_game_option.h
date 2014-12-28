
#ifndef __YUNJR_CLASS_GAME_OPTION_H__
#define __YUNJR_CLASS_GAME_OPTION_H__

#include "util/sm_util_string.h"

#include "yunjr_base_serialized.h"

namespace yunjr
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

} // namespace yunjr

#endif // #ifndef __YUNJR_CLASS_GAME_OPTION_H__
