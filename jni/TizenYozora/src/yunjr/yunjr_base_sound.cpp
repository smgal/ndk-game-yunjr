
#include "yunjr_base_sound.h"

//??#include <avej.h>

namespace
{
	bool s_is_first = true;
//??	avej::sound::Mixer s_fx_mixer;
}

void yunjr::sound::playFx(SOUND ix_sound)
{
/* ??
	if (s_is_first)
	{
		s_fx_mixer.load(0, "s_bang.wav");
		s_fx_mixer.load(1, "extend.wav");
		s_is_first = false;
	}

	switch (ix_sound)
	{
	case SOUND_WALK:
		s_fx_mixer.play(0, 0, 255, 255);
		break;
	case SOUND_HIT:
		s_fx_mixer.play(0, 1, 255, 255);
		break;
	case SOUND_SCREAM1:
	case SOUND_SCREAM2:
	default:
		break;
	}

	return;
*/
}

void yunjr::sound::muteFx(bool on)
{
	//?? 언젠가는 처리해야 함
}

void yunjr::sound::playBGM(const char* sz_file_name)
{
//??	avej::sound::playBGM(sz_file_name);
}

void yunjr::sound::stopBGM(void)
{
//??	avej::sound::stopBGM();
}
