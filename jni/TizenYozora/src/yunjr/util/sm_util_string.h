
#ifndef __SM_UTIL_STRING_H
#define __SM_UTIL_STRING_H

#pragma warning(disable: 4996)

#include "sm_util_sena.h"

//#include <string.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// definition

#if 1
#define CT_ASSERT(x, msg) \
	typedef int __CT_ASSERT ## msg ## __ [(x) ? 1 : -1];
#else
#define CT_ASSERT(x, msg)
#endif

#if defined(_WIN32)
#	define SPRINTF _snprintf
#else
#	define SPRINTF snprintf
#endif

////////////////////////////////////////////////////////////////////////////////
// namespace

namespace smutil
{

template <typename T>
class IntToStr
{
	T m_s[32];
public:
	IntToStr(int value)
	{
		if (sizeof(T) == sizeof(char))
		{
			SPRINTF((char*)m_s, 32, "%d", value);
		}
		else
		{
			char s[32];
			SPRINTF(s, 32, "%d", value);

			memset(&m_s[0], 0, sizeof(m_s));

			for (int i = 0; i < 32; i++)
			{
				m_s[i] = T(s[i]);
			}
		}
	}
	const T* operator()(void)
	{
		return m_s;
	}
};

template <typename CHAR>
void convertIntToStr(CHAR **pp_buffer, int value)
{
    if (value == 0)
    {
        *((*pp_buffer)++) = CHAR('0');
    }
    else
    {
        if (value < 0)
        {
            *((*pp_buffer)++) = CHAR('-');
            value = -value;
        }

        int n_num = 0;
        int temp = value;
        while (temp)
        {
            temp = temp / 10;
            ++n_num;
        }

        int n_loop = n_num;
        while (n_loop-- > 0)
        {
            (*pp_buffer)[n_loop] = (value % 10) + CHAR('0');
            value = value / 10;
        }
        *pp_buffer += n_num;
    }
}

template <typename CHAR>
void composeString(CHAR p_buffer[], const CHAR* sz_format, const int data...)
{
    if (p_buffer == 0)
        return;

    const int* p_data = &data;

    while (*sz_format)
    {
        if (*sz_format != CHAR('@'))
        {
            *p_buffer++ = *sz_format++;
            continue;
        }
        ++sz_format;
        convertIntToStr(&p_buffer, *p_data++);
    }

    *p_buffer = 0;
}

template <typename CHAR, unsigned int _MAX_STRLEN>
class basic_string
{
	enum { MAX_STRLEN = _MAX_STRLEN };

private:
	CHAR m_string[MAX_STRLEN+1];

public:
	basic_string(void)
	{
		m_string[0] = 0;
	}
	basic_string(const CHAR* lpsz)
	{      
		sena::strncpy(m_string, lpsz, MAX_STRLEN);
	}

	const CHAR* c_str(void) const
	{
		return m_string;
	}

	operator const CHAR*() const
	{
		return m_string;
	}

	const basic_string& operator=(const CHAR* lpsz)
	{      
		sena::strncpy(m_string, lpsz, MAX_STRLEN);
		return *this;
	}

	const basic_string& operator+=(const CHAR* lpsz)
	{      
		sena::strncat(m_string, lpsz, MAX_STRLEN);
		return *this;
	}

	template <typename T, unsigned int MAX_STRLEN>
	friend bool operator==(const basic_string& str1, const basic_string& str2)
	{      
		return sena::strndiff(str1.m_string, str2.m_string, MAX_STRLEN);
	}

	template <typename T, unsigned int MAX_STRLEN>
	friend basic_string operator+(const basic_string& str1, const basic_string& str2)
	{
		basic_string s(str1);
		s += str2;
		return s;
	}

	template <typename T, unsigned int MAX_STRLEN>
	bool operator==(const basic_string<T, MAX_STRLEN>& str) const
	{      
		return sena::strndiff(m_string, str.m_string, MAX_STRLEN);
	}

	bool operator<(const basic_string& lpsz) const
	{
		return (sena::strncmp(m_string, lpsz.c_str(), MAX_STRLEN) < 0);
	}

	void copyToFront(const basic_string& lpsz)
	{      
		CHAR m_temp[MAX_STRLEN+1];
		sena::strncpy(m_temp, m_string, MAX_STRLEN);
		sena::strncpy(m_string, lpsz.m_string, MAX_STRLEN);
		sena::strncat(m_string, m_temp, MAX_STRLEN);
	}
};

typedef basic_string<wchar_t, 255> string;

typedef basic_string<char, 255> string8;
typedef basic_string<char, 511> long_string;
typedef basic_string<char, 1023> long_long_string;

}

#endif // #ifndef __SM_UTIL_STRING_H
