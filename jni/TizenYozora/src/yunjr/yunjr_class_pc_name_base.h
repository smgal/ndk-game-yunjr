
#ifndef __YUNJR_CLASS_PC_NAME_BASE_H__
#define __YUNJR_CLASS_PC_NAME_BASE_H__

#include "util/sm_util_string.h"
#include "yunjr_base_serialized.h"

namespace yunjr
{

	class PcNameBase: public Serialized
	{
	public:
		enum JOSA
		{
			JOSA_NONE = 0, // �̸� �״��
			JOSA_SUB,      // '��' �Ǵ� '��'
			JOSA_SUB2,     // '��' �Ǵ� '��'
			JOSA_OBJ,      // '��' �Ǵ� '��'
		};

	public:
		PcNameBase(void);
		virtual ~PcNameBase(void);

		bool isValid(void) const;

		void setName(const wchar_t* sz_name);
		const wchar_t* getName(JOSA method = JOSA_NONE) const;

	protected:
		wchar_t _name[24];

		bool _load(const smutil::ReadStream& stream);
		bool _save(const smutil::WriteStream& stream) const;

	private:
		// ���� ���ϴ� �κ�
		smutil::string m_name_subject1;
		smutil::string m_name_subject2;
		smutil::string m_name_object;

	};

} // namespace yunjr

#endif // #ifndef __YUNJR_CLASS_PC_NAME_BASE_H__
