
#ifndef __HD_CLASS_KEY_BUFFER_H__
#define __HD_CLASS_KEY_BUFFER_H__

#include "util/sm_util.h"

namespace hadar
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

	//! Key event�� DOS ���� key buffer �������� ����� �ִ� class
	/*!
	 * \ingroup AVEJ library utilities
	*/
	class KeyBuffer
	{
	public:
		//! Key�� Ÿ��
		typedef unsigned short Key;

		//! KeyBuffer�� ������
		KeyBuffer(void);
		//! KeyBuffer�� �Ҹ���
		~KeyBuffer(void);

		//! Key�� �������ٴ� ���� �˷��ش�.
		bool setKeyDown(Key key);
		//! Key�� �������ٴ� ���� �˷��ش�.
		bool setKeyUp(Key key);
		//! ���� Key buffer�� key�� ���� �ִ��� �˷� �ش�.
		bool isKeyPressed(void);
		//! Key buffer�� ���� �ִ� key�� ���� �ش�.
		Key getKey(void);
		//! ���� Ư�� Key�� ������ ���������� �Ǻ��Ѵ�.
		bool isKeyPressing(Key key);

		void processAutoRepeat(void);

		static KeyBuffer& getKeyBuffer(void);

	private:
		//! ������ �ִ� queue ũ��
		enum
		{
			MAX_KEY_BUFFER = 100
		};

		int  m_key_head_ptr;
		int  m_key_tail_ptr;
		Key  m_key_buffer[MAX_KEY_BUFFER];
		int  m_key_map[KEY_MAX];

		int  m_increasePtr(int ptr);
		bool m_pushKeyChar(Key key);

	};

} // namespace hadar

#endif // #ifndef __HD_CLASS_KEY_BUFFER_H__
