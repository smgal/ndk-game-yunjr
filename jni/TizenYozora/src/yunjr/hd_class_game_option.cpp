
#include "hd_class_game_option.h"

////////////////////////////////////////////////////////////////////////////////
// class GameOption

hadar::GameOption::GameOption()
	: is_dubug_mode(false)
{
}

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool hadar::GameOption::_load(const smutil::ReadStream& stream)
{
	bool result = (stream.read((void*)&flag, sizeof(flag)) == sizeof(flag)) &&
	              (stream.read((void*)&variable, sizeof(variable)) == sizeof(variable)) &&
	              (stream.read((void*)&script_file, sizeof(script_file)) == sizeof(script_file));
	return result;
}

bool hadar::GameOption::_save(const smutil::WriteStream& stream) const
{
	bool result = (stream.write((void*)&flag, sizeof(flag)) == sizeof(flag)) &&
	              (stream.write((void*)&variable, sizeof(variable)) == sizeof(variable)) &&
	              (stream.write((void*)&script_file, sizeof(script_file)) == sizeof(script_file));
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// public method

