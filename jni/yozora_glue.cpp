
namespace target
{
	void setTouchPos(int x, int y);
}

namespace yozora
{
	struct BufferDesc
	{
		void* p_start_address;
		int   width;
		int   height;
		int   bytes_per_line;
		int   bits_per_pixel;
	};

	void init(const char* sz_id, const char* sz_data_path);
	void done();
	bool loop(const BufferDesc& buffer_desc);
}

extern "C" void yozora_glue_init(const char* sz_id, const char* sz_data_path)
{
	yozora::init(sz_id, sz_data_path);
}

extern "C" void yozora_glue_done(void)
{
	yozora::done();
}

extern "C" int yozora_glue_process(int touch_x, int touch_y)
{
	target::setTouchPos(touch_x, touch_y);

	return 1;
}

extern "C" int yozora_glue_render(void* p_start_address, int width, int height, int bytes_per_line, int bits_per_pixel)
{
	yozora::BufferDesc buffer_desc;

	buffer_desc.p_start_address = p_start_address;
	buffer_desc.width = width;
	buffer_desc.height = height;
	buffer_desc.bytes_per_line = bytes_per_line;
	buffer_desc.bits_per_pixel = bits_per_pixel;

	return (yozora::loop(buffer_desc)) ? 1 : 0;
}

////////////////////////////////////////////////////////////////////////////////
//

#include <stdio.h>

extern "C" int g_fileExists(const char* fname);
extern "C" FILE* g_fileOpen(const char* fname, unsigned int* out_start_offset, unsigned int* out_length);
extern "C" void g_createBufferFromCompressedFile(const char* sz_file_name, unsigned char** out_p_p_buffer, unsigned int* out_buffer_length);

namespace target
{
	namespace file_io
	{
		bool fileExists(const char* sz_file_name)
		{
			return (g_fileExists(sz_file_name) != 0);
		}

		FILE* fopen(const char* sz_file_name, unsigned int* out_start_offset, unsigned int* out_length)
		{
			return g_fileOpen(sz_file_name, out_start_offset, out_length);
		}

		void createBufferFromCompressedFile(const char* sz_file_name, unsigned char** out_p_p_buffer, unsigned int* out_buffer_length)
		{
			g_createBufferFromCompressedFile(sz_file_name, out_p_p_buffer, out_buffer_length);
		}
	}
}
