
#if defined(YOZORA_ANDROID)

#include "../target_dep.h"

#ifdef USE_MEMORY_RESOURCE

namespace res
{
	const unsigned char* getData(void);
	unsigned int getDataSize(void);
}

////////////////////////////////////////////////////////////////////////////////
// file i/o

static const int StreamRead_SEEK_END = target::file_io::StreamRead::SEEK_END;

#include <stdio.h>
#include <string.h>

namespace target
{
	namespace file_io
	{
		void setResourcePath(const char* sz_path)
		{
		}

		struct StreamReadFile::Impl
		{
			const unsigned char* m_p_data_begin;
			const unsigned char* m_p_data_end;
			int m_offset;
			bool m_is_available;

			Impl(const char* sz_file_name)
				: m_p_data_begin(0)
				, m_p_data_end(0)
				, m_offset(0)
				, m_is_available(false)
			{
				const unsigned char* p_res_data_begin = res::getData();
				const unsigned char* p_res_data_end = p_res_data_begin + res::getDataSize();
				
				const unsigned char* p_res_data = p_res_data_begin;

				while (p_res_data < p_res_data_end)
				{
					const char* sz_res_file_name = (const char*)(p_res_data + 4);

					p_res_data = p_res_data_begin + *((unsigned int*)p_res_data);
					const unsigned char* p_data_begin = (p_res_data + 4);
					const unsigned char* p_data_end = p_res_data_begin + *((unsigned int*)p_res_data);

					if (strcmp(sz_res_file_name, sz_file_name) == 0)
					{
						m_p_data_begin = p_data_begin;
						m_p_data_end = p_data_end;
						m_offset = 0;

						m_is_available = true;

						return;
					}
					p_res_data = p_data_end;
				}
			}

			~Impl()
			{
			}
		};

		StreamReadFile::StreamReadFile(const char* sz_file_name)
			: p_impl(new Impl(sz_file_name))
		{
		}

		StreamReadFile::~StreamReadFile(void)
		{
			delete p_impl;
		}

		long  StreamReadFile::read(void* p_buffer, long count)
		{
			if (!p_impl->m_is_available)
				return 0;

			memcpy(p_buffer, &p_impl->m_p_data_begin[p_impl->m_offset], count);

			p_impl->m_offset += count;

			return count;
		}

		long  StreamReadFile::seek(SEEK origin, long offset)
		{
			if (!p_impl->m_is_available)
				return -1;

			switch (origin)
			{
			case StreamRead::SEEK_BEGIN:
				p_impl->m_offset = offset;
				break;
			case StreamRead::SEEK_CURRENT:
				p_impl->m_offset += offset;
				break;
			case StreamRead_SEEK_END:
				p_impl->m_offset = (p_impl->m_p_data_end - p_impl->m_p_data_begin) + offset;
				break;
			default:
				return -1;
			}

			return p_impl->m_offset;
		}

		long  StreamReadFile::getSize(void)
		{
			if (!p_impl->m_is_available)
				return -1;

			return (p_impl->m_p_data_end - p_impl->m_p_data_begin);
		}

		bool  StreamReadFile::isValidPos(void)
		{
			if (!p_impl->m_is_available)
				return false;

			return (p_impl->m_offset >= 0UL && p_impl->m_offset < (p_impl->m_p_data_end - p_impl->m_p_data_begin));
		}
	}
}

#else

extern "C" void g_printLog(const char* sz_log);

static const int StreamRead_SEEK_END = target::file_io::StreamRead::SEEK_END;

#include <stdio.h>
#include <string.h>

namespace target
{
	namespace file_io
	{
		bool fileExists(const char* sz_file_name);
		void createBufferFromCompressedFile(const char* sz_file_name, unsigned char** out_p_p_buffer, unsigned int* out_buffer_length);
	}
}

namespace
{
	template <typename T>
	T getMin(T a, T b)
	{
		return (a < b) ? a : b;
	}
}

namespace target
{
	namespace file_io
	{
		const char* s_app_path1 = "assets/";
		char s_app_path2[256] = {0, };

		void setResourcePath(const char* sz_path)
		{
			strncpy(s_app_path2, sz_path, 256);
			strncat(s_app_path2, "/", 256);
		}

		struct StreamReadFile::Impl
		{
			unsigned char* m_p_buffer;
			unsigned int m_buffer_length;
			unsigned int m_offset;
			bool  m_is_available;

			Impl()
				: m_p_buffer(0)
				, m_buffer_length(0)
				, m_offset(0)
				, m_is_available(false)
			{
			}

			~Impl()
			{
				if (m_p_buffer)
					free(m_p_buffer);
			}
		};

		StreamReadFile::StreamReadFile(const char* sz_file_name)
			: p_impl(new Impl)
		{
			char file_name[256];

			strncpy(file_name, s_app_path1, 256);
			strncat(file_name, s_app_path2, 256);
			strncat(file_name, sz_file_name, 256);

			createBufferFromCompressedFile(file_name, &p_impl->m_p_buffer, &p_impl->m_buffer_length);

			p_impl->m_offset = 0;
			p_impl->m_is_available = (p_impl->m_p_buffer != 0);

			if (p_impl->m_p_buffer)
			{
				// suppress non-error logs
				//
				// char sz_log[256];
				// sprintf(sz_log, "[DOJI] %s found!", file_name);
				// g_printLog(sz_log);
			}
			else
			{
				char sz_log[256];
				sprintf(sz_log, "[DOJI] %s NOT found!", file_name);
				g_printLog(sz_log);
			}
		}

		StreamReadFile::~StreamReadFile(void)
		{
			delete p_impl;
		}

		long  StreamReadFile::read(void* p_buffer, long count)
		{
			if (!p_impl->m_is_available)
				return 0;

			long copied_bytes = getMin(count, long(p_impl->m_buffer_length - p_impl->m_offset));

			if (copied_bytes <= 0)
				return 0;

			memcpy(p_buffer, &p_impl->m_p_buffer[p_impl->m_offset], copied_bytes);

			p_impl->m_offset += copied_bytes;

			return copied_bytes;
		}

		long  StreamReadFile::seek(SEEK origin, long offset)
		{
			if (!p_impl->m_is_available)
				return -1;

			switch (origin)
			{
			case StreamRead::SEEK_BEGIN:
				p_impl->m_offset = offset;
				break;
			case StreamRead::SEEK_CURRENT:
				p_impl->m_offset += offset;
				break;
			case StreamRead_SEEK_END:
				p_impl->m_offset = p_impl->m_buffer_length + offset;
				break;
			default:
				return -1;
			}

			return p_impl->m_offset;
		}

		long  StreamReadFile::getSize(void)
		{
			if (!p_impl->m_is_available)
				return -1;

			return p_impl->m_buffer_length;
		}

		bool  StreamReadFile::isValidPos(void)
		{
			if (!p_impl->m_is_available)
				return false;

			return (p_impl->m_offset >= 0 && p_impl->m_offset < p_impl->m_buffer_length);
		}
	}
}

#endif

#endif // #if defined(YOZORA_ANDROID)
