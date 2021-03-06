
#ifndef __HD_CLASS_PC_NAME_BASE_H__
#define __HD_CLASS_PC_NAME_BASE_H__

#include "util/sm_util_string.h"
#include "hd_base_serialized.h"

namespace hadar
{

	class PcNameBase: public Serialized
	{
	public:
		enum JOSA
		{
			JOSA_NONE = 0, // 이름 그대로
			JOSA_SUB,      // '은' 또는 '는'
			JOSA_SUB2,     // '이' 또는 '가'
			JOSA_OBJ,      // '을' 또는 '를'
		};

	public:
		PcNameBase(void);
		virtual ~PcNameBase(void);

		bool isValid(void) const;

		void setName(const char* sz_name);
		const char* getName(JOSA method = JOSA_NONE) const;

	protected:
		char _name[24];

		bool _load(const smutil::ReadStream& stream);
		bool _save(const smutil::WriteStream& stream) const;

	private:
		// 저장 안하는 부분
		smutil::string m_name_subject1;
		smutil::string m_name_subject2;
		smutil::string m_name_object;

	};

} // namespace hadar

#endif // #ifndef __HD_CLASS_PC_NAME_BASE_H__
