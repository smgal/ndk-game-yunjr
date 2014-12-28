
#include "util/sm_util_sena.h"
#include "hd_class_pc_name_base.h"

#include "hd_res_string.h"

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool hadar::PcNameBase::_load(const smutil::ReadStream& stream)
{
	if (stream.read((void*)_name, sizeof(_name)) == sizeof(_name))
	{
		// 다른 멤버 변수의 값도 생성하기 위함
		setName(_name);
		return true;
	}
	else
	{
		return false;
	}
}

bool hadar::PcNameBase::_save(const smutil::WriteStream& stream) const
{
	return (stream.write((void*)_name, sizeof(_name)) == sizeof(_name));
}

////////////////////////////////////////////////////////////////////////////////
// public method

hadar::PcNameBase::PcNameBase(void)
{
	_name[0] = 0;
}

hadar::PcNameBase::~PcNameBase(void)
{
}

bool hadar::PcNameBase::isValid(void) const
{
	return (_name[0] != 0);
}

void hadar::PcNameBase::setName(const char* sz_name)
{
	sena::strncpy(_name, sz_name, sizeof(_name)-1);

	bool has_jongsung = resource::hasJongsung(resource::getLastMultiByte(_name));

	m_name_subject1  = _name;
	m_name_subject1 += (has_jongsung) ? "은" : "는";

	m_name_subject2  = _name;
	m_name_subject2 += (has_jongsung) ? "이" : "가";

	m_name_object    = _name;
	m_name_object   += (has_jongsung) ? "을" : "를";
}

const char* hadar::PcNameBase::getName(JOSA method) const
{
	static const char* sz_none = "";

	switch (method)
	{
	case JOSA_NONE:
		return _name;
	case JOSA_SUB:
		return m_name_subject1;
	case JOSA_SUB2:
		return m_name_subject2;
	case JOSA_OBJ:
		return m_name_object;
	default:
		assert(false);
		return sz_none;
	}
}
