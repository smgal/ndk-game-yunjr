
#include <string>
#include <map>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
// macro

#define DECLARE_BUFFER_DESC \
	struct BufferDesc \
	{ \
		void* p_start_address; \
		int   width; \
		int   height; \
		int   bytes_per_line; \
		int   bits_per_pixel; \
	}

#define DECLARE_APP(app_name) \
	namespace app_name \
	{ \
		DECLARE_BUFFER_DESC; \
	  \
		void init(const char* sz_id, const char* sz_data_path); \
		void done(void); \
		bool loop(const BufferDesc& buffer_desc); \
	} \

#define REGISTER_APP(app_name) \
	DECLARE_APP(app_name) \
	AppList app_name##_cb(#app_name, app_name::init, app_name::done, (AppList::FnLoop)app_name::loop);

////////////////////////////////////////////////////////////////////////////////
// type definition

DECLARE_BUFFER_DESC;

////////////////////////////////////////////////////////////////////////////////
// local

struct AppList
{
	typedef void (*FnInit)(const char* sz_id, const char* sz_data_path);
	typedef void (*FnDone)(void);
	typedef bool (*FnLoop)(const BufferDesc& buffer_desc);

	void (*init)(const char* sz_id, const char* sz_data_path);
	void (*done)(void);
	bool (*loop)(const BufferDesc& buffer_desc);

	AppList()
		: init(0)
		, done(0)
		, loop(0)
	{
	}

	AppList(const char* sz_app_name, FnInit fn_init, FnDone fn_done, FnLoop fn_loop)
	{
		m_registerApp(sz_app_name, fn_init, fn_done, fn_loop);
	}

	static const AppList* getAppCallback(const char* sz_app_name)
	{
		std::map<std::string, AppList>::iterator i = m_app_list.find(sz_app_name);

		return (i != m_app_list.end()) ? &(i->second) : 0;
	}

	static const AppList* getDefaultAppCallback(void)
	{
		std::map<std::string, AppList>::iterator i = m_app_list.begin();

		return (i != m_app_list.end()) ? &(i->second) : 0;
	}

private:
	static std::map<std::string, AppList> m_app_list;

	void m_registerApp(std::string app_name, FnInit fn_init, FnDone fn_done, FnLoop fn_loop)
	{
		AppList app_callbacks;

		app_callbacks.init = fn_init;
		app_callbacks.done = fn_done;
		app_callbacks.loop = fn_loop;

		m_app_list[app_name] = app_callbacks;
	}
};

std::map<std::string, AppList> AppList::m_app_list;

////////////////////////////////////////////////////////////////////////////////
// external

REGISTER_APP(yunjr)

////////////////////////////////////////////////////////////////////////////////
// local

namespace
{
	AppList::FnInit s_init = 0;
	AppList::FnDone s_done = 0;
	AppList::FnLoop s_loop = 0;

	void setCurrentApp(const char* sz_app_name)
	{
		const AppList* p_app = AppList::getAppCallback(sz_app_name);
		p_app = (p_app) ? p_app : AppList::getDefaultAppCallback();

		assert(p_app);

		s_init = p_app->init;
		s_done = p_app->done;
		s_loop = p_app->loop;
	}
}

////////////////////////////////////////////////////////////////////////////////
// object

DECLARE_APP(yozora)

////////////////////////////////////////////////////////////////////////////////
// main

void yozora::init(const char* sz_id, const char* sz_data_path)
{
	setCurrentApp(sz_id);

	s_init(sz_id, sz_data_path);
}

void yozora::done()
{
	s_done();
}

bool yozora::loop(const yozora::BufferDesc& buffer_desc)
{
	return s_loop(*(reinterpret_cast<const ::BufferDesc*>(&buffer_desc)));
}
