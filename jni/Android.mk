LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#####################################################

SRC_LIBZIP_ROOT := libzip
SRC_LIBZIP      := \
	               $(SRC_LIBZIP_ROOT)/zip_add.c \
	               $(SRC_LIBZIP_ROOT)/zip_add_dir.c \
	               $(SRC_LIBZIP_ROOT)/zip_close.c \
	               $(SRC_LIBZIP_ROOT)/zip_delete.c \
	               $(SRC_LIBZIP_ROOT)/zip_dirent.c \
	               $(SRC_LIBZIP_ROOT)/zip_entry_free.c \
	               $(SRC_LIBZIP_ROOT)/zip_entry_new.c \
	               $(SRC_LIBZIP_ROOT)/zip_err_str.c \
	               $(SRC_LIBZIP_ROOT)/zip_error.c \
	               $(SRC_LIBZIP_ROOT)/zip_error_clear.c \
	               $(SRC_LIBZIP_ROOT)/zip_error_get.c \
	               $(SRC_LIBZIP_ROOT)/zip_error_get_sys_type.c \
	               $(SRC_LIBZIP_ROOT)/zip_error_strerror.c \
	               $(SRC_LIBZIP_ROOT)/zip_error_to_str.c \
	               $(SRC_LIBZIP_ROOT)/zip_fclose.c \
	               $(SRC_LIBZIP_ROOT)/zip_file_error_clear.c \
	               $(SRC_LIBZIP_ROOT)/zip_file_error_get.c \
	               $(SRC_LIBZIP_ROOT)/zip_file_get_offset.c \
	               $(SRC_LIBZIP_ROOT)/zip_file_strerror.c \
	               $(SRC_LIBZIP_ROOT)/zip_filerange_crc.c \
	               $(SRC_LIBZIP_ROOT)/zip_fopen.c \
	               $(SRC_LIBZIP_ROOT)/zip_fopen_index.c \
	               $(SRC_LIBZIP_ROOT)/zip_fread.c \
	               $(SRC_LIBZIP_ROOT)/zip_free.c \
	               $(SRC_LIBZIP_ROOT)/zip_get_archive_comment.c \
	               $(SRC_LIBZIP_ROOT)/zip_get_archive_flag.c \
	               $(SRC_LIBZIP_ROOT)/zip_get_file_comment.c \
	               $(SRC_LIBZIP_ROOT)/zip_get_num_files.c \
	               $(SRC_LIBZIP_ROOT)/zip_get_name.c \
		           $(SRC_LIBZIP_ROOT)/zip_memdup.c \
		           $(SRC_LIBZIP_ROOT)/zip_name_locate.c \
		           $(SRC_LIBZIP_ROOT)/zip_new.c \
		           $(SRC_LIBZIP_ROOT)/zip_open.c \
		           $(SRC_LIBZIP_ROOT)/zip_rename.c \
		           $(SRC_LIBZIP_ROOT)/zip_replace.c \
		           $(SRC_LIBZIP_ROOT)/zip_set_archive_comment.c \
		           $(SRC_LIBZIP_ROOT)/zip_set_archive_flag.c \
		           $(SRC_LIBZIP_ROOT)/zip_set_file_comment.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_buffer.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_file.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_filep.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_free.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_function.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_zip.c \
		           $(SRC_LIBZIP_ROOT)/zip_set_name.c \
		           $(SRC_LIBZIP_ROOT)/zip_stat.c \
		           $(SRC_LIBZIP_ROOT)/zip_stat_index.c \
		           $(SRC_LIBZIP_ROOT)/zip_stat_init.c \
		           $(SRC_LIBZIP_ROOT)/zip_strerror.c \
		           $(SRC_LIBZIP_ROOT)/zip_unchange.c \
		           $(SRC_LIBZIP_ROOT)/zip_unchange_all.c \
		           $(SRC_LIBZIP_ROOT)/zip_unchange_archive.c \
		           $(SRC_LIBZIP_ROOT)/zip_unchange_data.c

LOCAL_CFLAGS    := -DPNG_NO_READ_EXPAND

#####################################################

SRC_LIBPNG_ROOT := libpng
SRC_LIBPNG      := \
	               $(SRC_LIBPNG_ROOT)/png.c \
	               $(SRC_LIBPNG_ROOT)/pngerror.c \
	               $(SRC_LIBPNG_ROOT)/pngget.c \
	               $(SRC_LIBPNG_ROOT)/pngmem.c \
	               $(SRC_LIBPNG_ROOT)/pngpread.c \
	               $(SRC_LIBPNG_ROOT)/pngread.c \
	               $(SRC_LIBPNG_ROOT)/pngrio.c \
	               $(SRC_LIBPNG_ROOT)/pngrtran.c \
	               $(SRC_LIBPNG_ROOT)/pngrutil.c \
	               $(SRC_LIBPNG_ROOT)/pngset.c \
	               $(SRC_LIBPNG_ROOT)/pngtrans.c \
	               $(SRC_LIBPNG_ROOT)/pngwio.c \
	               $(SRC_LIBPNG_ROOT)/pngwrite.c \
	               $(SRC_LIBPNG_ROOT)/pngwtran.c \
	               $(SRC_LIBPNG_ROOT)/pngwutil.c 
	
#####################################################

SRC_ROOT        := TizenYozora/src

SRC_FLAT_BOARD  := \
                   $(SRC_ROOT)/flat_board/pcx_decoder.cpp \
                   $(SRC_ROOT)/flat_board/tga_decoder.cpp \
                   $(SRC_ROOT)/flat_board/target_android/file_io.cpp \
                   $(SRC_ROOT)/flat_board/target_android/input_device.cpp \
                   $(SRC_ROOT)/flat_board/target_android/system.cpp

SRC_YOZORA      := \
                   $(SRC_ROOT)/yozora.cpp

SRC_HADAR_FT    := \
                   $(SRC_ROOT)/yunjr/freetype2/src/autofit/autofit.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/bdf/bdf.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/cff/cff.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftbase.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftbitmap.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/cache/ftcache.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftdebug.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftfstype.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftgasp.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftglyph.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/gzip/ftgzip.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftinit.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/lzw/ftlzw.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftstroke.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftsystem.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/smooth/smooth.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftbbox.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftgxval.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftlcdfil.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftmm.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftotval.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftpatent.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftpfr.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftsynth.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/fttype1.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftwinfnt.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/base/ftxf86.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/pcf/pcf.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/pfr/pfr.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/psaux/psaux.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/pshinter/pshinter.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/psnames/psmodule.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/raster/raster.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/sfnt/sfnt.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/truetype/truetype.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/type1/type1.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/cid/type1cid.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/type42/type42.c \
                   $(SRC_ROOT)/yunjr/freetype2/src/winfonts/winfnt.c
                   
SRC_HADAR       := \
                   $(SRC_ROOT)/yunjr/yunjr_base_font.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_base_gfx.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_base_key_buffer.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_base_serialized.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_base_sound.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_base_text.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_base_text_impl.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_console.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_control_lv1.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_control_lv2.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_extern.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_game_option.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_game_state.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_map.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_map_event.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_pc_enemy.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_pc_name_base.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_pc_party.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_pc_player.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_class_select.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_res.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_res_string.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_script_def.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_script_map_a_dummy.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_script_map_a_sample.cpp \
                   $(SRC_ROOT)/yunjr/yunjr_z_main.cpp \
                   $(SRC_ROOT)/yunjr/util/sm_util_random.cpp \
                   $(SRC_ROOT)/yunjr/util/sm_util_set.cpp \
                   $(SRC_ROOT)/yunjr/util/sm_util_stream.cpp \
                   $(SRC_HADAR_FT)

LOCAL_MODULE    := hadar

LOCAL_SRC_FILES := $(SRC_LIBZIP) $(SRC_LIBPNG) $(SRC_FLAT_BOARD) $(SRC_YOZORA) $(SRC_HADAR)
LOCAL_SRC_FILES += yozora.c yozora_glue.cpp

LOCAL_CFLAGS    += -DYOZORA_ANDROID -DPIXELFORMAT_ABGR
LOCAL_CFLAGS    += -DFT_DEBUG_LEVEL_ERROR -DFT_DEBUG_LEVEL_TRACE -DFT2_BUILD_LIBRARY
# LOCAL_CFLAGS    += -finput-charset=USC-2
LOCAL_CFLAGS    += -Ilibzip -ITizenYozora/src/yunjr/freetype2/include

LOCAL_LDLIBS    := -lm -llog -ljnigraphics -lz

include $(BUILD_SHARED_LIBRARY)
