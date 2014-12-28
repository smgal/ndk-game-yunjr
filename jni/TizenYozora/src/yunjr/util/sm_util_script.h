
#ifndef __SM_UTIL_SCRIPT_H__
#define __SM_UTIL_SCRIPT_H__

#pragma warning(disable: 4786)

#include "util/sm_util_string.h"

#include <string>
#include <map>
#include <stack>
#include <assert.h>
#include <stdio.h>
#include <string.h>

namespace smutil
{
	struct SmResult
	{
		char type;
		int  data;
		smutil::string sz_str;
	};

	struct SmParam
	{
		char*    type;   // pointer as array
		int*     data;   // pointer as array
		char**   string; // pointer as array
		SmResult result;
	};

	////////////////////////////////////

	template <class _type>
	class SmScriptFunction
	{
	public:
		typedef void (_type::*FnLink)(SmParam*);
		typedef std::map<std::string, FnLink> SmFunctionMap;

		SmScriptFunction(void)
			: m_p_this(0)
		{
		}

		void setScript(_type* p_this)
		{
			m_p_this = p_this;
		}

		bool isNotInitialized(void)
		{
			return m_function_map.empty();
		}

		bool registerFunction(const char* sz_name, FnLink function)
		{
			if (sz_name == 0 || function == 0)
				return false;

			m_function_map[sz_name] = function;

			return true;
		}

		bool unregisterFunction(const char* sz_name)
		{
			if (sz_name == 0)
				return false;

			typename SmFunctionMap::iterator i = m_function_map.find(sz_name);

			if (i == m_function_map.end())
				return false;

			m_function_map.erase(i);

			return true;
		}

		bool executeFunction(const char* sz_name, SmParam* p_param)
		{
			if ((m_p_this == 0) || (sz_name == 0))
				return false;

			typename SmFunctionMap::iterator i = m_function_map.find(sz_name);

			if (i == m_function_map.end())
				return false;

			(m_p_this->*(i->second))(p_param);

			return true;
		}

		SmFunctionMap m_function_map;
		_type*        m_p_this;
	};

	////////////////////////////////////

	template <class _type>
	class SmScript
	{
		enum
		{
			MAX_STRING = 256 * 8,
			MAX_PARAM  = 10
		};

		typedef std::map<std::string, int> SmVariableMap;

		char* searchDelimiter(char* sz_src, char delimiter)
		{
			char* sz_dst = sz_src;
			bool  in_qm = false;

			while ((*sz_dst) && (*sz_dst != delimiter))
			{
				char ch = *sz_dst;

				if (ch == '"')
					in_qm = !in_qm;
				else if ((ch == delimiter) && (!in_qm))
					break;

				++sz_dst;
			}

			return (*sz_dst == delimiter) ? sz_dst : 0;
		}

		char* searchDelimiter2(char* sz_src, char delimiter)
		{
			char* sz_dst = sz_src;
			bool  in_qm = false;
			int   in_pr = 0;

			while (*sz_dst)
			{
				char ch = *sz_dst;

				if (ch == '"')
					in_qm = !in_qm;
				else if (ch == '(')
					++in_pr;
				else if (ch == ')')
					--in_pr;
				else if ((ch == delimiter) && (!in_qm) && (in_pr == 0))
					break;

				++sz_dst;
			}

			return (*sz_dst == delimiter) ? sz_dst : 0;
		}

		int getLevel(const char* sz_src)
		{
			int level = 0;

			--sz_src;

			while (*(sz_src++ + 1) == 9)
				++level;

			return level;
		}
		
		bool parse(const char* sz_src, char* sz_dst, int& out_level)
		{
			const char* const sz_dst_begin = sz_dst;
			out_level = 0;

			--sz_src;

			while (*(sz_src++ + 1) == 9)
				++out_level;

			bool in_qm = false;
			unsigned short ch;

			while ((ch = (unsigned short)(*sz_src++)) > 26)
			{
				if (ch == '"')
					in_qm = !in_qm;
				else if ((ch == ' ') && (!in_qm))
					continue;

				*sz_dst++ = static_cast<char>(ch);
			}

			*sz_dst = 0;

			// 마지막의 ';' 제거
			while (sz_dst_begin < sz_dst)
			{
				if (*(sz_dst-1) == ';') 
				{
					*(--sz_dst) = 0;
					continue;
				}
				break;
			}

			return (sz_dst_begin < sz_dst);
		}

		void extract(char* sz_src, char* sz_param[], int max_param)
		{
			sz_param[0] = sz_src;

			sz_src = searchDelimiter(sz_src, '(');

			if (sz_src == 0)
			{
				sz_param[1] = 0;
				return;
			}

			sz_src[strlen(sz_src)-1] = 0;

			*sz_src++ = 0;

			int index = 1;

			// aaa.bbb 형태를 파싱하여 bbb는 함수 이름으로 aaa는 첫 번째 파라미터로 변경
			{
				char* sz_temp = sz_param[0];

				sz_temp = searchDelimiter(sz_temp, '.');

				if (sz_temp)
				{
					*sz_temp++ = 0;
					sz_param[1] = sz_param[0];
					sz_param[0] = sz_temp; 
					++index;
				}
			}

			do
			{
				sz_param[index++] = sz_src;
				assert(index < max_param);

				sz_src = searchDelimiter2(sz_src, ',');

				if (sz_src == 0)
					break;

				*sz_src++ = 0;
			} while (1);

			sz_param[index] = 0;
		}

		SmResult executeNative(char type[], int data[], char* sz_string[])
		{
			SmParam param;

			param.type = type;
			param.data = data;
			param.string = sz_string;
			param.result.type = 'i';
			param.result.data = 0;

			if (strcmp(sz_string[0], "if") == 0)
			{
				param.result.data = data[1];

				return param.result;
			}

			if (strcmp(sz_string[0], "while") == 0)
			{
				param.result.data = data[1];

				return param.result;
			}

			if (strcmp(sz_string[0], "assign") == 0)
			{
				SmVariableMap::iterator i = m_variable_map.find(sz_string[1]);
				assert(i != m_variable_map.end());

				i->second = data[2];

				param.result.data = 1;

				return param.result;
			}

			if (strcmp(sz_string[0], "add") == 0)
			{
				SmVariableMap::iterator i = m_variable_map.find(sz_string[1]);
				assert(i != m_variable_map.end());

				i->second += data[2];

				param.result.data = 1;

				return param.result;
			}

			if (strcmp(sz_string[0], "halt") == 0)
			{
				m_is_not_terminated = false;

				return param.result;
			}

			if (!m_script_function.executeFunction(sz_string[0], &param))
			{
				SmVariableMap::iterator i = m_variable_map.find(sz_string[0]);

				if (i == m_variable_map.end())
				{
					printf("## syntax error: %s(%d) - '%s'\n", m_sz_file_name, m_line, sz_string[0]);
					assert(false);

					m_variable_map[sz_string[0]] = 0;
					i = m_variable_map.find(sz_string[0]);
				}

				param.result.data = i->second;
				param.result.sz_str = sz_string[0];
			}

			return param.result;
		}

		SmResult executeCommand(char* sz_param[])
		{
			char param_type[MAX_PARAM];
			int  param_data[MAX_PARAM];
			smutil::string stringTemplate[MAX_PARAM];

			memset(param_type, 0, sizeof(param_type));
			memset(param_data, 0, sizeof(param_data));

			if (strcmp(sz_param[0], "include") == 0)
			{
				if (sz_param[1])
				{
					int len = strlen(sz_param[1]);

					if ((len > 2) && (sz_param[1][0] == '"') && (sz_param[1][len-1] == '"'))
					{
						// 따옴표 제거
						sz_param[1][len-1] = 0;

						SmResult result;

						result.type = 'x';
						result.sz_str = &sz_param[1][1]; 

						return result;
					}
				}
			}

			if (strcmp(sz_param[0], "variable") == 0)
			{
				m_variable_map[sz_param[1]] = 0;

				SmResult result;

				result.type = 'i';
				result.data = 1;

				return result;
			}

			for (int n = 1; sz_param[n]; n++)
			{
				int len = strlen(sz_param[n]);

				if ((sz_param[n][0] == '"') && (sz_param[n][len-1] == '"'))
				{
					param_type[n] = 's';
					sz_param[n][len-1] = 0;
					++sz_param[n];
				}
				else
				{
					int index = 0;
					int sign  = 1;

					char ch = sz_param[n][index];

					if ((ch == '+') || (ch == '-'))
					{
						if (ch == '-')
						sign = -1;
						++index;
					}

					int sum   = 0;

					while ((ch = sz_param[n][index]))
					{
						if ((ch < '0') || (ch > '9'))
						{
							sign = 0;
							break;
						}
						sum *= 10;
						sum += ch - '0';
						++index;
					}

					if (sign)
					{
						param_type[n] = 'i';
						param_data[n] = sum * sign;
					}
					else
					{
						SmResult result   = this->excuteStream(sz_param[n]);
						param_type[n]     = result.type;
						param_data[n]     = result.data;
						stringTemplate[n] = result.sz_str;
						//!!
						sz_param[n]       = (char*)(const char*)stringTemplate[n];
					}
				}

			}
			return this->executeNative(param_type, param_data, sz_param);
		}

		SmResult excuteStream(char* sz_src)
		{
			char* paramList[MAX_PARAM];

			this->extract(sz_src, paramList, MAX_PARAM);

			return this->executeCommand(paramList);
		}

	public:
		SmScript(SmScriptFunction<_type>& scriptFunction, const char* sz_file_name)
			: m_is_not_terminated(true)
			, m_sz_file_name(sz_file_name)
			, m_line(0)
			, m_script_function(scriptFunction)
		{
			typedef std::pair<smutil::string, int> FileStackData;
			typedef std::stack<FileStackData> FileStack;

			FileStack file_stack;

			file_stack.push(std::make_pair(sz_file_name, 0));

			while (!file_stack.empty())
			{
				FileStackData data = file_stack.top();
				file_stack.pop();

				smutil::FileReadStream file_stream(data.first);

				if (!file_stream.isValid())
					continue;

				smutil::TextFileHolder text_holder;

				{
					int   file_size = file_stream.getSize();
					char* p_buffer  = new char[file_size];

					file_stream.read(p_buffer, file_size);

					text_holder.reset(p_buffer, file_size);

					delete[] p_buffer;
				}

				int current_line = data.second;

/*
				FILE* file = fopen(data.first, "rt");
				if (file == 0)
					continue;

				fseek(file, data.second, SEEK_SET);
*/
				char src_buf[MAX_STRING];
				char mod_buf[MAX_STRING];
				int  curr_level = 0;
				int  skip_level = -1;
				int  level;

				--current_line; 

				while (current_line < text_holder.getLineCount() && m_is_not_terminated)
				{
					++m_line;
					++current_line; 

					if (!text_holder.getLine(current_line, src_buf, MAX_STRING))
						continue;

					if ((skip_level > 0) && (getLevel(src_buf) >= skip_level))
						continue;

					if (!this->parse(src_buf, mod_buf, level))
						continue;

					if (mod_buf[0] == '#')
						continue;

					if (skip_level > 0)
					{
						if (strcmp(mod_buf, "else") == 0)
						{
							curr_level = level+1;
							skip_level = (level == skip_level-1) ? -1 : level+1;
							continue;
						}
						curr_level  = level;
						skip_level = -1;
					}
					else if (level < curr_level)
					{
						if (strcmp(mod_buf, "else") == 0)
						{
							skip_level = level+1;
							continue;
						}
						curr_level = level;
					}

					SmResult result = this->excuteStream(mod_buf);

					if (result.type == 'i')
					{
						if (result.data)
						{
							++curr_level;
						}
						else
						{
							skip_level = curr_level+1;
						}
					}
					if (result.type == 'x')
					{
						// include
						file_stack.push(std::make_pair(data.first, current_line + 1));
						file_stack.push(std::make_pair(result.sz_str, 0));

						break;
					}
				}
			}
		}

		bool           m_is_not_terminated;
		const char*    m_sz_file_name;
		int            m_line;
		SmVariableMap  m_variable_map;
		SmScriptFunction<_type>& m_script_function;
	};
}

#endif // #ifndef __SM_UTIL_SCRIPT_H__
