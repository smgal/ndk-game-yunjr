
#include "util/sm_util_sena.h"

#include "hd_base_config.h"
#include "hd_base_key_buffer.h"

hadar::KeyBuffer::KeyBuffer(void)
	: m_key_head_ptr(0)
	, m_key_tail_ptr(0)
{
	sena::memclear(m_key_buffer, sizeof(m_key_buffer));
	sena::memclear(m_key_map, sizeof(m_key_map));
}

hadar::KeyBuffer::~KeyBuffer(void)
{
}

hadar::KeyBuffer::Key hadar::KeyBuffer::getKey()
{
	return -1;
}

hadar::KeyBuffer& hadar::KeyBuffer::getKeyBuffer(void)
{
	static hadar::KeyBuffer s_temp;

	return s_temp;
}

#if 0 //??
#include "util/sm_util_sena.h"
#include "util/sm_util.h"

#include "hd_base_config.h"
#include "hd_base_key_buffer.h"

#include <avej.h>

#define CLEAR_MEMORY(var) memset(var, 0, sizeof(var));

namespace
{
	#define CASE(key1, key2) \
		case avej::input::KEY_##key1: key = hadar::KEY_##key2; break

	hadar::KEY convertAvejToHadar(avej::input::KEY avejKey)
	{
		hadar::KEY key = hadar::KEY_UNKNOWN;

		switch (avejKey)
		{
			CASE(START,    SYS_START);
			CASE(SELECT,   SYS_SELECT);
			CASE(UP,       DIR_UP);
			CASE(LEFT,     DIR_LEFT);
			CASE(DOWN,     DIR_DOWN);
			CASE(RIGHT,    DIR_RIGHT);
			CASE(BUTTON_A, BTN_A);
			CASE(BUTTON_B, BTN_B);
			CASE(BUTTON_C, BTN_C);
			CASE(BUTTON_D, BTN_D);
			CASE(BUTTON_L, BTN_L1);
			CASE(BUTTON_R, BTN_R1);
			CASE(VOL_UP,   VOL_UP);
			CASE(VOL_DOWN, VOL_DOWN);

			default:
			{
				if (avejKey >= 'A' && avejKey <= 'Z')
					key = hadar::KEY(int(hadar::KEY_A) + (avejKey - 'A'));
				else if (avejKey >= 'a' && avejKey <= 'z')
					key = hadar::KEY(int(hadar::KEY_A) + (avejKey - 'a'));
			}
		}

		return key;
	}

	bool OnKeyDown(unsigned short avej_key, unsigned long state)
	{
		hadar::KEY key = convertAvejToHadar(avej::input::KEY(avej_key));

		return hadar::KeyBuffer::getKeyBuffer().setKeyDown(key);
	}

	bool OnKeyUp(unsigned short avej_key, unsigned long state)
	{
		hadar::KEY key = convertAvejToHadar(avej::input::KEY(avej_key));

		return hadar::KeyBuffer::getKeyBuffer().setKeyUp(key);
	}

	avej::input::EventCallback s_callback =
	{
		OnKeyDown,
		OnKeyUp,
	};

}

////////////////////////////////////////////////////////////////////////////////

namespace
{
	// KeyBuffer는 싱글톤이므로 그에 맞는 위치가 필요할지도..
	static bool           s_isKeyPressed = false;
	static unsigned short s_pressedKey;
	static unsigned long  s_repeatTime;
	const  unsigned long  c_delayTime = 75;
}

////////////////////////////////////////////////////////////////////////////////

//?? 싱글톤임이 보장되어야 함
hadar::KeyBuffer::KeyBuffer(void)
	: m_key_head_ptr(0)
	, m_key_tail_ptr(0)
{
	sena::memclear(m_key_buffer, sizeof(m_key_buffer));
	sena::memclear(m_key_map, sizeof(m_key_map));
}

hadar::KeyBuffer::~KeyBuffer(void)
{
}

bool hadar::KeyBuffer::setKeyDown(Key key)
{
	// auto pressed key 구현
	if (!s_isKeyPressed)
	{
		s_isKeyPressed = true;
		s_pressedKey   = key;
		s_repeatTime   = smutil::getTicks() + c_delayTime*3;
	}

	if (key < KEY_MAX)
		m_key_map[key] = 1;

	return m_pushKeyChar(key);
}

bool hadar::KeyBuffer::setKeyUp(Key key)
{
	// auto pressed key 구현
	if (s_isKeyPressed)
	{
		s_isKeyPressed = false;
	}

	if (key < KEY_MAX)
		m_key_map[key] = 0;

	return true;
}

bool hadar::KeyBuffer::isKeyPressed(void)
{
	this->processAutoRepeat();

	avej::input::processMessage(s_callback);

	return  (m_key_head_ptr != m_key_tail_ptr);
}

hadar::KeyBuffer::Key hadar::KeyBuffer::getKey()
{
	Key key = -1;

	if (isKeyPressed())
	{
		key = m_key_buffer[m_key_head_ptr];
		m_key_head_ptr = m_increasePtr(m_key_head_ptr);
	}

	return key;
}

bool hadar::KeyBuffer::isKeyPressing(Key key)
{
	if (key >= KEY_MAX)
		return false;

	return (m_key_map[key] > 0);
}

void hadar::KeyBuffer::processAutoRepeat(void)
{
	unsigned long current_tick = smutil::getTicks();

	if (s_isKeyPressed)
	{
		if (s_repeatTime <= current_tick)
		{
			hadar::KeyBuffer::getKeyBuffer().setKeyUp(s_pressedKey);
			hadar::KeyBuffer::getKeyBuffer().setKeyDown(s_pressedKey);
			s_repeatTime = current_tick + c_delayTime;
		}
	}
}

int hadar::KeyBuffer::m_increasePtr(int ptr)
{
	if (++ptr >= 100)
		ptr -= 100;

	return ptr;
}

bool hadar::KeyBuffer::m_pushKeyChar(Key key)
{
	if (m_increasePtr(m_key_tail_ptr) != m_key_head_ptr)
	{
		m_key_buffer[m_key_tail_ptr] = key;
		m_key_tail_ptr = m_increasePtr(m_key_tail_ptr);
		return true;
	}
	else
	{
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////////

namespace
{
	sena::auto_ptr<hadar::KeyBuffer> s_p_key_buffer;
}

hadar::KeyBuffer& hadar::KeyBuffer::getKeyBuffer(void)
{
	if (s_p_key_buffer.get() == nullptr)
	{
		s_p_key_buffer.bind(new hadar::KeyBuffer);
	}

	return *s_p_key_buffer.get();
}

#endif
