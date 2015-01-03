
// 리소스 읽는 방법
// http://blog.sephiroth.it/2010/10/24/reading-resource-files-from-native-code/

// 리소스 예제 프로젝트
// http://www.anddev.org/ndk_opengl_-_loading_resources_and_assets_from_native_code-t11978.html

////////////////////////////////////////////////////////////////////////////////
//

#include <zip.h>
#include <zipint.h>
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
//

#define  LOG_TAG    "libhadar"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
//

char g_package_name[1024] = {0};
struct zip* g_p_zip_file;

////////////////////////////////////////////////////////////////////////////////
// callback

static JNIEnv* s_p_env = 0;
static jclass s_native_class = 0;
static jmethodID s_fn_updateScreen = 0;
static jmethodID s_fn_getTicks = 0;
static jmethodID s_fn_waitMillisecond = 0;

////////////////////////////////////////////////////////////////////////////////
//

void g_printLog(const char* sz_log)
{
	LOGI("%s", sz_log);
}

unsigned long g_getTicks(void)
{
	if (s_fn_getTicks)
	{
		return (*s_p_env)->CallStaticIntMethod(s_p_env, s_native_class, s_fn_getTicks);
	}
}

void g_waitMillisecond(int msec)
{
	if (s_fn_waitMillisecond)
	{
		(*s_p_env)->CallStaticVoidMethod(s_p_env, s_native_class, s_fn_waitMillisecond, msec);
	}
}

static void _invalidate_source_bitmap();

void g_updateScreen(void)
{
	if (s_fn_updateScreen)
	{
		_invalidate_source_bitmap();
		(*s_p_env)->CallStaticVoidMethod(s_p_env, s_native_class, s_fn_updateScreen);
	}
}

int g_fileExists(const char* sz_file_name)
{
	if (g_p_zip_file != NULL)
	{
		int result = zip_name_locate(g_p_zip_file, sz_file_name, 0);
		return (result != -1) ? 1 : 0;
	}

	return 0;
}

FILE* g_fileOpen(const char* sz_file_name, unsigned int* out_start_offset, unsigned int* out_length)
{
	struct zip_file* p_zip_file = zip_fopen(g_p_zip_file, sz_file_name, 0);

	if (p_zip_file)
	{
		FILE* p_file = NULL;

		if (out_start_offset)
			*out_start_offset = p_zip_file->fpos;

		if (out_length)
			*out_length = p_zip_file->bytes_left;

		zip_fclose(p_zip_file);

		p_file = fopen(g_package_name, "rb");
		fseek(p_file, p_zip_file->fpos, SEEK_SET);

		return p_file;
	}
	else
	{
		return NULL;
	}

}

typedef struct zip_file zip_file;

void g_createBufferFromCompressedFile(const char* sz_file_name, unsigned char** out_p_buffer, unsigned int* out_buffer_length)
{
	if (out_p_buffer)
	{
		*out_p_buffer = NULL;

		zip_file* p_file = zip_fopen(g_p_zip_file, sz_file_name, 0);

		if (p_file)
		{
			unsigned int length = p_file->bytes_left;

			unsigned char* p_buffer = malloc(length);

			if (p_buffer)
			{
				zip_fread(p_file, p_buffer, length);

				*out_p_buffer = p_buffer;

				if (out_buffer_length)
					*out_buffer_length = length;
			}

			zip_fclose(p_file);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//

extern void yozora_glue_init(const char* sz_id);
extern void yozora_glue_done(void);
extern int  yozora_glue_process(int touch_x, int touch_y);
extern int  yozora_glue_render(void* p_start_address, int width, int height, int bytes_per_line, int bits_per_pixel);

////////////////////////////////////////////////////////////////////////////////
//

JNIEXPORT void JNICALL Java_com_avej_game_hadar_YozoraView_initYozora(JNIEnv* p_env, jobject obj, jstring sj_package_name, jstring sj_app_name)
{
	s_p_env = p_env;

	s_native_class = (*p_env)->FindClass(p_env, "com/avej/game/hadar/GameTask");

	if (s_native_class)
	{
		s_fn_getTicks = (*p_env)->GetStaticMethodID(p_env, s_native_class, "getTicks", "()I");
		s_fn_waitMillisecond = (*p_env)->GetStaticMethodID(p_env, s_native_class, "waitMillisecond", "(I)V");
		s_fn_updateScreen = (*p_env)->GetStaticMethodID(p_env, s_native_class, "updateScreen", "()V");

		if (s_fn_getTicks)
			g_printLog("[SMGAL] Callback function 'int getTicks()' found");
		if (s_fn_waitMillisecond)
			g_printLog("[SMGAL] Callback function 'void waitMillisecond(int)' found");
		if (s_fn_updateScreen)
			g_printLog("[SMGAL] Callback function 'void updateScreen()' found");
	}

	// open package file as zip file
	{
		const char* p_buffer = (*p_env)->GetStringUTFChars(p_env, sj_package_name, 0);

		int error;
		g_p_zip_file = zip_open(p_buffer, 0, &error);

		strcpy(g_package_name, p_buffer);

		if (g_p_zip_file == NULL)
		{
			LOGE("Failed to open apk: %i", error);
		}

		(*p_env)->ReleaseStringUTFChars(p_env, sj_package_name, p_buffer);
	}

	// initialize with app_name
	{
		const char* p_app_name = (*p_env)->GetStringUTFChars(p_env, sj_app_name, 0);
		yozora_glue_init(p_app_name);
		(*p_env)->ReleaseStringUTFChars(p_env, sj_app_name, p_app_name);
	}
}

JNIEXPORT void JNICALL Java_com_avej_game_hadar_YozoraView_doneYozora(JNIEnv* p_env, jobject obj)
{
	yozora_glue_done();
}

JNIEXPORT int JNICALL Java_com_avej_game_hadar_YozoraView_processYozora(JNIEnv* p_env, jobject obj, jlong time_ms, jint motion_x, jint motion_y)
{
	int result = yozora_glue_process(motion_x, motion_y);

	return result;
}

static JNIEnv* s_p_env2 = 0;
static jobject s_bitmap;

static void _invalidate_source_bitmap()
{
	if (s_p_env2)
	{
		void* pixels;
		AndroidBitmap_unlockPixels(s_p_env2, s_bitmap);
		AndroidBitmap_lockPixels(s_p_env2, s_bitmap, &pixels);
	}
	else
	{
		LOGE("Invalid _invalidate_source_bitmap() called");
	}
}

JNIEXPORT int JNICALL Java_com_avej_game_hadar_YozoraView_renderYozora(JNIEnv* p_env, jobject obj, jobject bitmap)
{
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;
    static int         is_first = 1;

    if ((ret = AndroidBitmap_getInfo(p_env, bitmap, &info)) < 0)
    {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    if (is_first)
	{
        if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
        {
            LOGE("Bitmap format is not RGBA_8888 !");
            return;
        }

		is_first = 0;
    }

    if ((ret = AndroidBitmap_lockPixels(p_env, bitmap, &pixels)) < 0)
    {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return;
    }

    s_p_env2 = p_env;
    s_bitmap = bitmap;

	int result = yozora_glue_render(pixels, info.width, info.height, info.stride, 32);

    s_p_env2 = NULL;

    AndroidBitmap_unlockPixels(p_env, bitmap);

    return result;
}
