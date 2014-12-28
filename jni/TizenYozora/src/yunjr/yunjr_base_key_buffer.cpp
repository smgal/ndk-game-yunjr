
#include "yunjr_base_key_buffer.h"

#include "yunjr_class_game_state.h"

#include "util/sm_util_sena.h"

#include "../flat_board/target_dep.h"

////////////////////////////////////////////////////////////////////////////////

namespace
{
	// KeyBuffer는 싱글톤이므로 그에 맞는 위치가 필요할지도..
	static bool           s_is_key_pressed = false;
	static unsigned short s_pressed_key;
	static unsigned long  s_repeat_time;
	const  unsigned long  c_delay_time = 75;
}

namespace
{
	#define CASE(key1, key2) \
		case target::KEY_##key1: key = yunjr::KEY_##key2; break

	yunjr::KEY convertTargetToYunjr(target::KEY avejKey)
	{
		yunjr::KEY key = yunjr::KEY_UNKNOWN;

		switch (avejKey)
		{
			CASE(MENU,    SYS_START);
			CASE(SELECT,  SYS_SELECT);
			CASE(UP,      DIR_UP);
			CASE(LEFT,    DIR_LEFT);
			CASE(DOWN,    DIR_DOWN);
			CASE(RIGHT,   DIR_RIGHT);
			CASE(A,       BTN_A);
			CASE(B,       BTN_B);
			CASE(X,       BTN_C);
			CASE(Y,       BTN_D);
			CASE(L,       BTN_L1);
			CASE(R,       BTN_R1);
			CASE(VOLUP,   VOL_UP);
			CASE(VOLDOWN, VOL_DOWN);

			default:
			{
				if (avejKey >= 'A' && avejKey <= 'Z')
					key = yunjr::KEY(int(yunjr::KEY_A) + (avejKey - 'A'));
				else if (avejKey >= 'a' && avejKey <= 'z')
					key = yunjr::KEY(int(yunjr::KEY_A) + (avejKey - 'a'));
			}
		}

		return key;
	}
}

////////////////////////////////////////////////////////////////////////////////

//?? 싱글톤임이 보장되어야 함
yunjr::KeyBuffer::KeyBuffer(void)
	: m_key_head_ptr(0)
	, m_key_tail_ptr(0)
{
	sena::memclear(m_key_buffer, sizeof(m_key_buffer));
	sena::memclear(m_key_map, sizeof(m_key_map));
}

yunjr::KeyBuffer::~KeyBuffer(void)
{
}

void yunjr::KeyBuffer::clear(void)
{
	m_key_head_ptr = 0;
	m_key_tail_ptr = 0;

	sena::memclear(m_key_buffer, sizeof(m_key_buffer));
	sena::memclear(m_key_map, sizeof(m_key_map));

	s_is_key_pressed = false;
}

bool yunjr::KeyBuffer::setKeyDown(Key key)
{
	// auto pressed key 구현
	if (!s_is_key_pressed)
	{
		s_is_key_pressed = true;
		s_pressed_key   = key;
		s_repeat_time   = target::system::getTicks() + c_delay_time*3;
	}

	if (key < KEY_MAX)
		m_key_map[key] = 1;

	return m_pushKeyChar(key);
}

bool yunjr::KeyBuffer::setKeyUp(Key key)
{
	// auto pressed key 구현
	if (s_is_key_pressed)
	{
		s_is_key_pressed = false;
	}

	if (key < KEY_MAX)
		m_key_map[key] = 0;

	return true;
}

bool yunjr::KeyBuffer::isKeyPressed(void)
{
	this->processAutoRepeat();

	this->m_processMessage();

	return  (m_key_head_ptr != m_key_tail_ptr);
}

yunjr::KeyBuffer::Key yunjr::KeyBuffer::getKey()
{
	Key key = -1;

	if (isKeyPressed())
	{
		key = m_key_buffer[m_key_head_ptr];
		m_key_head_ptr = m_increasePtr(m_key_head_ptr);
	}

	return key;
}

bool yunjr::KeyBuffer::isKeyPressing(Key key)
{
	if (key >= KEY_MAX)
		return false;

	return (m_key_map[key] > 0);
}

void yunjr::KeyBuffer::processAutoRepeat(void)
{
	unsigned long current_tick = target::system::getTicks();

	if (s_is_key_pressed)
	{
		if (s_repeat_time <= current_tick)
		{
			yunjr::KeyBuffer::getKeyBuffer().setKeyUp(s_pressed_key);
			yunjr::KeyBuffer::getKeyBuffer().setKeyDown(s_pressed_key);
			s_repeat_time = current_tick + c_delay_time;
		}
	}
}

void yunjr::KeyBuffer::m_processMessage(void)
{
	//?? update key event forcingly in Android
	target::updateScreen();

	#define PROCESS_KEY(target_key) \
		pressing = (game_state.checkKeyPressed(target_key)) ? target_key : 0; \
		if ((rescent_pressed & target_key) && (pressing == 0)) \
		{ \
			setKeyUp(convertTargetToYunjr(target::KEY(target_key))); \
			rescent_pressed &= ~target_key; \
		} \
		if (!(rescent_pressed & target_key) && pressing) \
		{ \
			setKeyDown(convertTargetToYunjr(target::KEY(target_key))); \
			rescent_pressed |= target_key; \
		}

	static unsigned long rescent_pressed = 0;

	yunjr::GameState& game_state = yunjr::GameState::getMutableInstance();

	game_state.update(0);

	unsigned long pressing = 0;

	PROCESS_KEY(target::KEY_SELECT);
	PROCESS_KEY(target::KEY_A);
	PROCESS_KEY(target::KEY_B);
	PROCESS_KEY(target::KEY_UP);
	PROCESS_KEY(target::KEY_DOWN);
	PROCESS_KEY(target::KEY_LEFT);
	PROCESS_KEY(target::KEY_RIGHT);

	#undef PROCESS_KEY
}

int yunjr::KeyBuffer::m_increasePtr(int ptr)
{
	if (++ptr >= 100)
		ptr -= 100;

	return ptr;
}

bool yunjr::KeyBuffer::m_pushKeyChar(Key key)
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
	sena::auto_ptr<yunjr::KeyBuffer> s_p_key_buffer;
}

yunjr::KeyBuffer& yunjr::KeyBuffer::getKeyBuffer(void)
{
	if (s_p_key_buffer.get() == 0)
	{
		s_p_key_buffer.bind(new yunjr::KeyBuffer);
	}

	return *s_p_key_buffer.get();
}
