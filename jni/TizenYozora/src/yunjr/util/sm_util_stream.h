
#ifndef __SM_UTIL_STREAM_H__
#define __SM_UTIL_STREAM_H__

#include <stdio.h>

namespace smutil
{
	///////////////////////////////////////////////////////////////////////////////
	// ReadStream class

	class ReadStream
	{ 
	public:
		ReadStream(void)
			: m_is_available(false)
		{
		}
		
		virtual ~ReadStream(void)
		{
		}

		bool isValid(void) const
		{
			return m_is_available;
		}

		virtual long  read(void* p_buffer, long count) const = 0;
		virtual long  seek(long offset, unsigned short origin) const = 0;
		virtual long  getSize(void) const = 0;
		virtual void* getPointer(void) const = 0;
		virtual bool  isValidPos(void) const = 0;

	protected:
		bool m_is_available;
	};

	class FileReadStream: public ReadStream
	{
	public:
		FileReadStream(const char* sz_file_name);
		~FileReadStream(void);

		long  read(void* p_buffer, long count) const;
		long  seek(long offset, unsigned short origin) const;
		long  getSize(void) const;
		void* getPointer(void) const;
		bool  isValidPos(void) const;

	private:
		FILE* m_p_file;
	};

	class MemoryReadStream: public ReadStream
	{
	public:
		MemoryReadStream(void* p_memory, long size);
		~MemoryReadStream(void);

		long  read(void* p_buffer, long count) const;
		long  seek(long offset, unsigned short origin) const;
		long  getSize(void) const;
		void* getPointer(void) const;
		bool  isValidPos(void) const;

	private:
		void* m_p_memory;
		long  m_size;
		mutable long m_position;
	};

	///////////////////////////////////////////////////////////////////////////////
	// WriteStream class

	class WriteStream
	{ 
	public:
		WriteStream(void)
			: m_is_available(false)
		{
		}
		
		virtual ~WriteStream(void)
		{
		}

		bool isValid(void) const
		{
			return m_is_available;
		}
		
		virtual long write(void* p_buffer, long count) const = 0;

	protected:
		bool m_is_available;
	};

	class FileWriteStream : public WriteStream
	{
	public:
		FileWriteStream(const char* sz_file_name);
		~FileWriteStream(void);

		long write(void* p_buffer, long count) const;

	private:
		FILE* m_p_file;
	};

	///////////////////////////////////////////////////////////////////////////////
	// TextFileHolder class

	class TextFileHolder
	{
	public:
		TextFileHolder();
		TextFileHolder(const char* p_buffer, int size);
		~TextFileHolder();

		void reset(const char* p_buffer, int size);
		int  getLineCount(void) const;
		int  getLineLength(int index) const;
		bool getLine(int index, char* out_str, int str_size) const;

	private:
		struct Impl;
		Impl* m_p_impl;
	};
}

#endif // #ifndef __SM_UTIL_STREAM_H__
