
#include <memory>
#include <cassert>
#include <string.h>

namespace
{
	namespace tga_decoder
	{
		struct TImage32Bits
		{
			long            width;
			long            height;
			long            bytes_per_line;
			unsigned long   depth;
			unsigned char*  p_bitmap;

			TImage32Bits()
			:	width(0), height(0), bytes_per_line(0), depth(0), p_bitmap(NULL) {};
		};

		#define IMAGETYPE_MODE_MASK  0x07
		#define IMAGETYPE_RLE_MASK   0x08

		#define IMAGETYPE_COLORMAP   0x01
		#define IMAGETYPE_TRUECOLOR  0x02
		#define IMAGETYPE_BLACKWHITE 0x03

		struct TTgaHeader
		{
			unsigned char  id_length;
			unsigned char  color_map_type;
			unsigned char  image_type;
			unsigned short color_map_start;
			unsigned short color_map_length;
			unsigned char  color_map_depth;
			unsigned short x_offset;
			unsigned short y_offset;
			unsigned short width;
			unsigned short height;
			unsigned char  pixel_depth;
			unsigned char  image_descriptor;
		};

	} // tga_decoder

	template <typename T>
	inline void s_ReadFieldLE(const unsigned char*& p_stream, T& read)
	{
		read = *p_stream++;
	}

	template <>
	inline void s_ReadFieldLE<unsigned short>(const unsigned char*& p_stream, unsigned short& read)
	{
		read  = (unsigned short)(*p_stream++);
		read |= (unsigned short)(*p_stream++) << 8;
	}

	template <>
	inline void s_ReadFieldLE<unsigned long>(const unsigned char*& p_stream, unsigned long& read)
	{
		read  = (unsigned long)(*p_stream++);
		read |= (unsigned long)(*p_stream++) <<  8;
		read |= (unsigned long)(*p_stream++) << 16;
		read |= (unsigned long)(*p_stream++) << 24;
	}

	#define CHECK_FILE_BROKEN(_begin, _end, required_bytes) if (int(_end - _begin) < required_bytes) return false;

	bool DecodeTGA_Native(const unsigned char* p_stream, unsigned int stream_size, tga_decoder::TImage32Bits& image)
	{
		if (p_stream == 0)
			return false;

		if (stream_size == 0)
			return false;

		const unsigned char* p_stream_begin = p_stream;
		const unsigned char* p_stream_end   = p_stream + stream_size;

		tga_decoder::TTgaHeader tga_header;
		unsigned long           palette[256];

		CHECK_FILE_BROKEN(p_stream_begin, p_stream_end, 18);

		s_ReadFieldLE(p_stream_begin, tga_header.id_length);
		s_ReadFieldLE(p_stream_begin, tga_header.color_map_type);
		s_ReadFieldLE(p_stream_begin, tga_header.image_type);
		s_ReadFieldLE(p_stream_begin, tga_header.color_map_start);
		s_ReadFieldLE(p_stream_begin, tga_header.color_map_length);
		s_ReadFieldLE(p_stream_begin, tga_header.color_map_depth);
		s_ReadFieldLE(p_stream_begin, tga_header.x_offset);
		s_ReadFieldLE(p_stream_begin, tga_header.y_offset);
		s_ReadFieldLE(p_stream_begin, tga_header.width);
		s_ReadFieldLE(p_stream_begin, tga_header.height);
		s_ReadFieldLE(p_stream_begin, tga_header.pixel_depth);
		s_ReadFieldLE(p_stream_begin, tga_header.image_descriptor);

		// Image Identification Field 지원 안함
		if (tga_header.id_length != 0)
			return false;

		bool is_color_mapped = (tga_header.image_type & IMAGETYPE_MODE_MASK) == IMAGETYPE_COLORMAP;
		bool is_RGB_color    = (tga_header.image_type & IMAGETYPE_MODE_MASK) == IMAGETYPE_TRUECOLOR;
		bool is_compressed   = (tga_header.image_type & IMAGETYPE_RLE_MASK) > 0;
		bool is_flipped      = (tga_header.image_descriptor & 0x20) > 0;

		bool is_decodable    = (tga_header.pixel_depth == 32 && is_RGB_color)
							|| (tga_header.pixel_depth == 8 && is_color_mapped && !is_compressed);

		if (!is_decodable)
			return false;

		tga_header.image_descriptor &= 0x0F;

		image.width          = tga_header.width;
		image.height         = tga_header.height;
		image.bytes_per_line = tga_header.width * 4;
		image.depth          = 32;
		image.p_bitmap       = new unsigned char[image.bytes_per_line * image.height];

		if (image.p_bitmap == NULL)
			return false;

		std::auto_ptr<unsigned char> mem_holder(image.p_bitmap);

		switch (tga_header.pixel_depth)
		{
		case  8:
			{
				// palette 검증
				bool is_valid_palette = (tga_header.color_map_start == 0) && (tga_header.color_map_length <= 256) && ((tga_header.color_map_depth == 24) || (tga_header.color_map_depth == 32));

				if (!is_valid_palette)
					return false;

				CHECK_FILE_BROKEN(p_stream_begin, p_stream_end, tga_header.color_map_length * tga_header.color_map_depth / 8);

				if (tga_header.color_map_depth == 32)
				{
					//  b, g, r, attribute info
					assert(false);
				}
				else if (tga_header.color_map_depth == 24)
				{
					//  b, g, r
					for (int i = 0; i < tga_header.color_map_length; i++)
					{
					#if !defined(PIXELFORMAT_ABGR)
						unsigned long b = *p_stream_begin++;
						unsigned long g = *p_stream_begin++;
						unsigned long r = *p_stream_begin++;
						unsigned long a = 0xFF;
					#else
						unsigned long r = *p_stream_begin++;
						unsigned long g = *p_stream_begin++;
						unsigned long b = *p_stream_begin++;
						unsigned long a = 0xFF;
					#endif

						palette[tga_header.color_map_start+i] = a << 24 | r << 16 | g << 8 | b;
					}
				}

				//?? 임시. 자동 컬러키 설정
				{
					palette[*(p_stream_begin+(tga_header.height-1)*tga_header.width)] &= 0x00FFFFFF;
				}

				{
					int            w = tga_header.width;
					int            h = tga_header.height;
					unsigned long* p_dest_32;
					unsigned char* p_dest = image.p_bitmap;

					//?? 크기 비교를 한 번 해야 함

					while (--h >= 0)
					{
						p_dest_32 = (unsigned long*)p_dest + h * w;
						for (int i = 0; i < w; i++)
						{
							*p_dest_32++ = palette[*p_stream_begin++];
						}
					}
				}
			}
			break;
		case 32:
			{
				int            w = tga_header.width;
				int            h = tga_header.height;
				signed   char  code;
				unsigned long  color;
				unsigned long* p_dest_32;
				unsigned char* p_dest = image.p_bitmap;

				if (is_compressed)
				{
					while (--h >= 0)
					{
						p_dest_32 = (is_flipped) ? (unsigned long*)(p_dest + image.bytes_per_line * (tga_header.height-h-1)) : (unsigned long*)(p_dest + image.bytes_per_line * h);

						int count = w;
						while (count > 0)
						{
							CHECK_FILE_BROKEN(p_stream_begin, p_stream_end, 1);
							s_ReadFieldLE(p_stream_begin, code);

							if (code & 0x80)
							{
								code  &= 0x7F;
								count -= (++code);

								CHECK_FILE_BROKEN(p_stream_begin, p_stream_end, 4);
								s_ReadFieldLE(p_stream_begin, color);

								while (--code >= 0)
								{
									*p_dest_32++ = color;
								}
							}
							else
							{
								count -= (++code);

								CHECK_FILE_BROKEN(p_stream_begin, p_stream_end, 4 * int(code));

								while (--code >= 0)
								{
									s_ReadFieldLE(p_stream_begin, color);
									*p_dest_32++ = color;
								}
							}
						}
					}
				}
				else
				{
					while (--h >= 0)
					{
						p_dest_32 = (unsigned long*)p_dest + h * w;

						//?? endian 고려 안되어 있음
						memcpy(&p_dest_32, p_stream_begin, w * 4);
						p_stream_begin += w * 4;
					}
				}

			#if defined(PIXELFORMAT_ABGR)
				{
					unsigned long* p_dest_32 = (unsigned long*)p_dest;
					unsigned long* p_dest_end = (unsigned long*)p_dest_32 + tga_header.width * tga_header.height;

					while (p_dest_32 < p_dest_end)
					{
						unsigned long ag = *p_dest_32 & 0xFF00FF00;
						unsigned long r  = *p_dest_32 & 0x00FF0000;
						unsigned long b  = *p_dest_32 & 0x000000FF;

						*p_dest_32++ = ag | b << 16 | r >> 16;
					}
				}
			#endif

			}
			break;
		default:
			assert(false);
			return false;
		}

		mem_holder.release();

		return true;
	}
}

namespace flat_board
{
	namespace
	{
	}

	template <typename Pixel>
	bool loadTGA(const unsigned char* p_stream, unsigned long stream_size, Pixel*& p_out_buffer, int& out_width, int& out_height)
	{
		return false;
	}

	template <>
	bool loadTGA<unsigned long>(const unsigned char* p_stream, unsigned long stream_size, unsigned long*& p_out_buffer, int& out_width, int& out_height)
	{
		tga_decoder::TImage32Bits image;

		if (DecodeTGA_Native(p_stream, stream_size, image))
		{
			p_out_buffer = (unsigned long*)image.p_bitmap;
			out_width    = image.width;
			out_height   = image.height;

			return true;
		}

		return false;
	}
}
