
#include "yunjr_class_game_option.h"

////////////////////////////////////////////////////////////////////////////////
// class GameOption

yunjr::GameOption::GameOption()
	: is_dubug_mode(false)
{
}

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool yunjr::GameOption::_load(const smutil::ReadStream& stream)
{
	bool result = (stream.read((void*)&flag, sizeof(flag)) == sizeof(flag)) &&
	              (stream.read((void*)&variable, sizeof(variable)) == sizeof(variable)) &&
	              (stream.read((void*)&script_file, sizeof(script_file)) == sizeof(script_file));
	return result;
}

bool yunjr::GameOption::_save(const smutil::WriteStream& stream) const
{
	bool result = (stream.write((void*)&flag, sizeof(flag)) == sizeof(flag)) &&
	              (stream.write((void*)&variable, sizeof(variable)) == sizeof(variable)) &&
	              (stream.write((void*)&script_file, sizeof(script_file)) == sizeof(script_file));
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// public method

