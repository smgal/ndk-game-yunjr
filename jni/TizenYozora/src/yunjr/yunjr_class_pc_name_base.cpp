
#include "yunjr_class_pc_name_base.h"
#include "yunjr_res_string.h"

#include "util/sm_util_sena.h"

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool yunjr::PcNameBase::_load(const smutil::ReadStream& stream)
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

bool yunjr::PcNameBase::_save(const smutil::WriteStream& stream) const
{
	return (stream.write((void*)_name, sizeof(_name)) == sizeof(_name));
}

////////////////////////////////////////////////////////////////////////////////
// public method

yunjr::PcNameBase::PcNameBase(void)
{
	_name[0] = 0;
}

yunjr::PcNameBase::~PcNameBase(void)
{
}

bool yunjr::PcNameBase::isValid(void) const
{
	return (_name[0] != 0);
}

void yunjr::PcNameBase::setName(const wchar_t* sz_name)
{
	sena::strncpy(_name, sz_name, sizeof(_name) / sizeof(_name[0])-1);

	bool has_jongsung = resource::hasJongsung(resource::getLastMultiByte(_name));

	m_name_subject1  = _name;
	m_name_subject1 += (has_jongsung) ? L"은" : L"는";

	m_name_subject2  = _name;
	m_name_subject2 += (has_jongsung) ? L"이" : L"가";

	m_name_object    = _name;
	m_name_object   += (has_jongsung) ? L"을" : L"를";
}

const wchar_t* yunjr::PcNameBase::getName(JOSA method) const
{
	static const wchar_t* sz_none = L"";

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
