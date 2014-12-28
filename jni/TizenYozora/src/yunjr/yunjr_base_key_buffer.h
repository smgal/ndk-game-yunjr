
#ifndef __YUNJR_CLASS_KEY_BUFFER_H__
#define __YUNJR_CLASS_KEY_BUFFER_H__

#include "util/sm_util.h"

namespace yunjr
{
	enum KEY
	{
		KEY_UNKNOWN = 0,
		KEY_SYS_START,
		KEY_SYS_SELECT,
		KEY_DIR_UP,
		KEY_DIR_DOWN,
		KEY_DIR_LEFT,
		KEY_DIR_RIGHT,
		KEY_BTN_L1,
		KEY_BTN_L2,
		KEY_BTN_R1,
		KEY_BTN_R2,
		KEY_BTN_A,
		KEY_BTN_B,
		KEY_BTN_C,
		KEY_BTN_D,
		KEY_VOL_UP,
		KEY_VOL_DOWN,
		KEY_A,
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z,
		KEY_MAX
	};

	//! Key event를 DOS 때의 key buffer 형식으로 만들어 주는 class
	/*!
	 * \ingroup AVEJ library utilities
	*/
	class KeyBuffer
	{
	public:
		//! Key의 타입
		typedef unsigned short Key;

		//! KeyBuffer의 생성자
		KeyBuffer(void);
		//! KeyBuffer의 소멸자
		~KeyBuffer(void);

		void clear(void);

		//! Key가 눌려졌다는 것을 알려준다.
		bool setKeyDown(Key key);
		//! Key가 떨어졌다는 것을 알려준다.
		bool setKeyUp(Key key);
		//! 현재 Key buffer에 key가 남아 있는지 알려 준다.
		bool isKeyPressed(void);
		//! Key buffer에 남아 있는 key를 돌려 준다.
		Key getKey(void);
		//! 현재 특정 Key가 눌려진 상태인지를 판별한다.
		bool isKeyPressing(Key key);

		void processAutoRepeat(void);

		static KeyBuffer& getKeyBuffer(void);

	private:
		//! 버퍼의 최대 queue 크기
		enum
		{
			MAX_KEY_BUFFER = 100
		};

		int  m_key_head_ptr;
		int  m_key_tail_ptr;
		Key  m_key_buffer[MAX_KEY_BUFFER];
		int  m_key_map[KEY_MAX];

		void m_processMessage(void);

		int  m_increasePtr(int ptr);
		bool m_pushKeyChar(Key key);

	};

} // namespace yunjr

#endif // #ifndef __YUNJR_CLASS_KEY_BUFFER_H__
