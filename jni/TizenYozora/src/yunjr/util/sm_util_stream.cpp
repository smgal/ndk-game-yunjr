
#pragma warning(disable: 4996)

#include "sm_util_sena.h"
#include "sm_util_stream.h"

// for debugging
namespace target
{
	namespace system
	{
		void log(const char* sz_message);
	}
}

/***************************************************
                   FileReadStream
***************************************************/

smutil::FileReadStream::FileReadStream(const char* sz_file_name)
{
	m_p_file = fopen(sz_file_name, "rb");

	{
		char message[512];
		sprintf(message, "[SMGAL] File reading: %s %s", sz_file_name, (m_p_file) ? "found" : "NOT found");
		target::system::log(message);
	}

	this->m_is_available = (m_p_file != 0);
}

smutil::FileReadStream::~FileReadStream(void)
{
	if (this->m_is_available)
		fclose(m_p_file);
}

long smutil::FileReadStream::read(void* p_buffer, long count) const
{
	if (!this->m_is_available)
		return 0;

	return fread(p_buffer, 1, count, m_p_file);
}

long smutil::FileReadStream::seek(long offset, unsigned short origin) const
{
	if (!this->m_is_available)
		return -1;

	switch (origin)
	{
	case SEEK_SET:
	case SEEK_CUR:
	case SEEK_END:
		fseek(m_p_file, offset, origin);
		break;
	default:
		return -1;
	}

	return ftell(m_p_file);
}

long smutil::FileReadStream::getSize(void) const
{
	if (!this->m_is_available)
		return -1;

	long  result;
	long  current_pos = ftell(m_p_file);

	fseek(m_p_file, 0, SEEK_END);
	result = ftell(m_p_file);

	fseek(m_p_file, current_pos, SEEK_SET);

	return result;
}

void* smutil::FileReadStream::getPointer(void) const
{
	return NULL;
}

bool smutil::FileReadStream::isValidPos(void) const
{
	if (!this->m_is_available)
		return false;

	return (feof(m_p_file) == 0);
}

/***************************************************
                   MemoryReadStream
***************************************************/

smutil::MemoryReadStream::MemoryReadStream(void* p_memory, long size)
{
	m_p_memory  = p_memory;
	m_size      = size;
	m_position  = 0;
	m_is_available = (m_p_memory != NULL);
}

smutil::MemoryReadStream::~MemoryReadStream()
{
}

long smutil::MemoryReadStream::read(void* p_buffer, long count) const
{
	if (!this->m_is_available)
		return -1;

	long result;

	if ((m_position >= 0) && (count >= 0 ))
	{
		result = m_size - m_position;

		if (result > 0)
		{
			if (result > count)
				result = count;

			sena::memcpy(p_buffer, (char*)m_p_memory + m_position, result);

			m_position += result;

			return result;
		}
	}

	return 0;
}

long smutil::MemoryReadStream::seek(long offset, unsigned short origin) const
{
	if (!this->m_is_available)
		return -1;

	switch (origin)
	{
	case SEEK_SET:
		m_position = offset;
		break;
	case SEEK_CUR:
		m_position += offset;
		break;
	case SEEK_END:
		m_position = m_size + offset;
		break;
	default:
		return -1;
	}

	// 특별한 범위 체크 안함
	return m_position;
}

long smutil::MemoryReadStream::getSize(void) const
{
	if (!this->m_is_available)
		return -1;

	return m_size;
}

void* smutil::MemoryReadStream::getPointer(void) const
{
	if (!this->m_is_available)
		return NULL;

	return m_p_memory;
}

bool smutil::MemoryReadStream::isValidPos(void) const
{
	if (!this->m_is_available)
		return false;

	return ((m_position >= 0) && (m_position < m_size));
}

/***************************************************
                   FileWriteStream
***************************************************/

smutil::FileWriteStream::FileWriteStream(const char* sz_file_name)
{
	m_p_file = fopen(sz_file_name, "wb");

	{
		char message[512];
		sprintf(message, "[SMGAL] File writing: %s %s", sz_file_name, (m_p_file) ? "created" : "NOT created");
		target::system::log(message);
	}

	this->m_is_available = (m_p_file != 0);
}

smutil::FileWriteStream::~FileWriteStream(void)
{
	if (this->m_is_available)
		fclose(m_p_file);
}

long smutil::FileWriteStream::write(void* p_buffer, long count) const
{
	if (!this->m_is_available)
		return 0;

	return fwrite(p_buffer, 1, count, m_p_file);
}


////////////////////////////////////////////////////////////////////////////////
// class TextFileHolder

struct smutil::TextFileHolder::Impl
{
public:
	Impl(const char* p_buffer, int size)
		: m_p_buffer(0)
		, m_p_buffer_end(0)
	{
		if (size > 0)
		{
			m_p_buffer = new char[size+2];

			if (m_p_buffer)
			{
				sena::copy(p_buffer, p_buffer+size, m_p_buffer);
				m_p_buffer[size+0] = 0x0D;
				m_p_buffer[size+1] = 0x0A;

				m_p_buffer_end = m_p_buffer + (size+2);

				char* p_current = m_p_buffer;

				while (p_current < m_p_buffer_end)
				{
					m_line.push_back(p_current);

					while (p_current < m_p_buffer_end && *p_current != 0x0D)
						++p_current;

					if (p_current < m_p_buffer_end)
						*p_current = 0;

					while (p_current < m_p_buffer_end && *p_current != 0x0A)
						++p_current;

					if (p_current < m_p_buffer_end)
						*p_current++ = 0;
				}
			}
		}
	}

	~Impl()
	{
		delete m_p_buffer;
	}

	int getLineCount(void) const
	{
		return m_line.size()-1;
	}

	int getLineLength(int index) const
	{
		if (index < 0 || index >= (m_line.size()-1))
			return 0;

		return (m_line[index+1] - m_line[index]) - 2;
	}

	bool getLine(int index, char* p_out_str, int str_size) const
	{
		if (index < 0 || index >= (m_line.size()-1))
			return false;

		sena::strncpy(p_out_str, m_line[index], str_size);
		
		return true;
	}

private:
	char* m_p_buffer;
	char* m_p_buffer_end;

	sena::vector<char*> m_line;
};

smutil::TextFileHolder::TextFileHolder()
	: m_p_impl(0)
{
}

smutil::TextFileHolder::TextFileHolder(const char* p_buffer, int size)
	: m_p_impl(new Impl(p_buffer, size))
{
}

smutil::TextFileHolder::~TextFileHolder()
{
	delete m_p_impl;
}

void smutil::TextFileHolder::reset(const char* p_buffer, int size)
{
	delete m_p_impl;

	m_p_impl = new Impl(p_buffer, size);
}

int smutil::TextFileHolder::getLineCount(void) const
{
	return (m_p_impl) ? m_p_impl->getLineCount() : 0;
}

int smutil::TextFileHolder::getLineLength(int index) const
{
	return (m_p_impl) ? m_p_impl->getLineLength(index) : 0;
}

bool smutil::TextFileHolder::getLine(int index, char* p_out_str, int str_size) const
{
	return (m_p_impl) ? m_p_impl->getLine(index, p_out_str, str_size) : false;
}
