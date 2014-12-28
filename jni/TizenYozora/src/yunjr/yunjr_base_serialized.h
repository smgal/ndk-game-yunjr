
#ifndef __YUNJR_BASE_SERIALIZED_H__
#define __YUNJR_BASE_SERIALIZED_H__

#include "util/sm_util_stream.h"

namespace yunjr
{
	class Serialized
	{
		friend class SerializedStream;

	protected:
		virtual bool _load(const smutil::ReadStream& stream) = 0;
		virtual bool _save(const smutil::WriteStream& stream) const = 0;
	};

	class SerializedStream
	{
	public:
		enum STREAMTYPE
		{
			STREAMTYPE_READ,
			STREAMTYPE_WRITE
		};

		SerializedStream(const char* sz_file_name, STREAMTYPE stream_type);
		~SerializedStream(void);

		virtual void operator<<(const Serialized& stream);
		virtual void operator>>(Serialized& stream);

	private:
		smutil::ReadStream*  p_read_stream;
		smutil::WriteStream* p_write_stream;
	};

} // namespace yunjr

#endif // #ifndef __YUNJR_BASE_SERIALIZED_H__
