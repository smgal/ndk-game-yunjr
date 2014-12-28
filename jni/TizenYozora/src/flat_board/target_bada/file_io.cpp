
#include "../target_dep.h"
#include <FAppApp.h>
#include <FIo.h>

using namespace Tizen::App;

namespace target
{
	namespace file_io
	{
		Tizen::Base::String s_app_path;
	}

	void file_io::setResourcePath(const char* sz_path)
	{
		s_app_path  = App::GetInstance()->GetAppRootPath();
		s_app_path += "res/";
		s_app_path += sz_path;
		s_app_path += "/";
	}

	struct file_io::StreamReadFile::Impl
	{
		Impl()
			: file_size(0)
		{
		}
		Tizen::Io::File file;
		int           file_size;
	};

	#define IS_AVAILABLE  (p_impl != 0)

	file_io::StreamReadFile::StreamReadFile(const char* sz_file_name)
	{
		p_impl = new Impl;

		if (p_impl)
		{
			try
			{
				if (s_app_path.IsEmpty())
				{
					s_app_path = App::GetInstance()->GetAppRootPath() + "res/";
				}

				result r;
				Tizen::Base::String fileName(sz_file_name);

				fileName = s_app_path + fileName;
				//fileName = App::GetInstance()->GetAppRootPath() + "res/" + fileName;

				r = p_impl->file.Construct(fileName, L"r");

				if (r == E_SUCCESS)
				{
					p_impl->file_size = this->getSize();
				}
				else
				{
					AppLog("[DOJI] File not found (%s)", sz_file_name);

					throw 1;
				}
			}
			catch (...)
			{
				delete p_impl;
				p_impl = 0;
			}
		}
	}

	file_io::StreamReadFile::~StreamReadFile(void)
	{
		delete p_impl;
	}

	long file_io::StreamReadFile::read(void* p_buffer, long count)
	{
		if (!IS_AVAILABLE)
			return 0;

		return p_impl->file.Read(p_buffer, count);
	}

	long file_io::StreamReadFile::seek(SEEK origin, long offset)
	{
		if (!IS_AVAILABLE)
			return -1;

		switch (origin)
		{
		case Tizen::Io::FILESEEKPOSITION_BEGIN:
			p_impl->file.Seek(Tizen::Io::FileSeekPosition(origin), offset);
			break;
		case Tizen::Io::FILESEEKPOSITION_CURRENT:
			p_impl->file.Seek(Tizen::Io::FileSeekPosition(origin), offset);
			break;
		case Tizen::Io::FILESEEKPOSITION_END:
			p_impl->file.Seek(Tizen::Io::FileSeekPosition(origin), offset);
			break;
		default:
			return -1;
		}

		return p_impl->file.Tell();
	}

	long file_io::StreamReadFile::getSize(void)
	{
		if (!IS_AVAILABLE)
			return -1;

		long res;
		long current_pos = p_impl->file.Tell();

		p_impl->file.Seek(Tizen::Io::FILESEEKPOSITION_END, 0);
		res = p_impl->file.Tell();

		p_impl->file.Seek(Tizen::Io::FILESEEKPOSITION_BEGIN, current_pos);

		return res;
	}

	bool file_io::StreamReadFile::isValidPos(void)
	{
		if (!IS_AVAILABLE)
			return false;

		return (p_impl->file.Tell() < p_impl->file_size);
	}
}
