
#ifndef __YUNJR_CLASS_SOUND_H__
#define __YUNJR_CLASS_SOUND_H__

namespace yunjr
{
	namespace sound
	{
		enum SOUND
		{
			SOUND_WALK = 0,
			SOUND_HIT,
			SOUND_SCREAM1,
			SOUND_SCREAM2
		};

		void playFx(SOUND ix_sound);
		void muteFx(bool on);

		void playBGM(const char* sz_file_name);
		void stopBGM(void);
	} // namespace sound

} // namespace yunjr

#endif // #ifndef __YUNJR_CLASS_SOUND_H__
