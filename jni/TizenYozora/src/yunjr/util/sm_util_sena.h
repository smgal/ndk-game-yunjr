
#ifndef __SM_UTIL_SENA_H__
#define __SM_UTIL_SENA_H__

//#define USE_STL

////////////////////////////////////////////////////////////////////////////////
// uses

#include <cassert>
#include <cstring>
#include <new>

#if defined(USE_STL)
#include <map>
#endif

////////////////////////////////////////////////////////////////////////////////
// definition

#if defined(_MSC_VER)
#pragma warning(4: 4018)
#pragma warning(disable: 4284 4786)
#endif

////////////////////////////////////////////////////////////////////////////////
// interface

namespace sena
{
	template<typename BaseType>
	class optional
	{
	public:
		optional()
			: m_initialized(false)
		{
		}
		optional(const BaseType& t)
			: m_initialized(false)
		{
			reset(t);
		}
		~optional()
		{
			reset();
		}

		BaseType& operator*() 
		{ 
			assert(m_initialized);
			return *static_cast<BaseType*>(address()); 
		}
		const BaseType& operator*() const
		{ 
			assert(m_initialized);
			return *static_cast<const BaseType*>(address()); 
		}
		BaseType* operator->() 
		{ 
			assert(m_initialized);
			return static_cast<BaseType*>(address()); 
		}
		const BaseType* operator->() const
		{ 
			assert(m_initialized);
			return static_cast<const BaseType*>(address()); 
		}
		BaseType* get() 
		{ 
			assert(m_initialized);
			return static_cast<BaseType*>(address()); 
		}
		const BaseType* get() const
		{ 
			assert(m_initialized);
			return static_cast<const BaseType*>(address()); 
		}
		void reset() 
		{
			if (m_initialized)
			{ 
	#if 1
				static_cast<BaseType*>(address())->BaseType::~BaseType();
	#else
				BaseType* t = static_cast<BaseType*>(address());
				t->~BaseType();
	#endif
				m_initialized = false;
			}
		}
		void reset(const BaseType& t) 
		{
			reset();
			new (address()) BaseType(t); 
			m_initialized = true;
		}
		bool valid() const
		{
			return m_initialized;
		}
		bool invalid() const
		{
			return !m_initialized;
		}

	private:
		bool      m_initialized;
		BaseType  m_storage;

		optional(const optional&);
		optional& operator=(const optional&);

		void* address()
		{
			return &m_storage;
		}
		const void* address() const
		{
			return &m_storage;
		}
	};

	template<typename BaseType>
	class auto_ptr
	{
	public:
		explicit auto_ptr(BaseType* ptr = 0)
			: m_owns(ptr != 0)
			, m_ptr((BaseType*)ptr)
		{
		}
		~auto_ptr()
		{
			if (m_owns)
				delete (BaseType*)m_ptr;
		}
		void bind(BaseType* ptr)
		{
			assert(m_ptr == 0);

			m_owns = (ptr != 0);
			m_ptr  = (BaseType*)ptr;
		}
		BaseType* operator ->() const
		{
			return m_ptr;
		}
		BaseType* get() const
		{
			return m_ptr;
		}
		BaseType* release() const
		{
			((auto_ptr<BaseType>*)this)->m_owns = false;
			return m_ptr;
		}

	private:
		bool m_owns;
		BaseType* m_ptr;
	};

	template<typename BaseType>
	class auto_ptr<BaseType[]>
	{
	public:
		explicit auto_ptr(BaseType* ptr = 0)
			: m_owns(ptr != 0)
			, m_ptr((BaseType*)ptr)
		{
		}
		~auto_ptr()
		{
			if (m_owns)
				delete[] (BaseType*)m_ptr;
		}
		BaseType& operator[](unsigned int index) const
		{
			return m_ptr[index];
		}
		void bind(BaseType* ptr)
		{
			assert(m_ptr == 0);

			m_owns = (ptr != 0);
			m_ptr  = (BaseType*)ptr;
		}
		void reset(BaseType* ptr = 0)
		{
			if (m_owns)
				delete[] (BaseType*)m_ptr;

			m_owns = (ptr != 0);
			m_ptr  = (BaseType*)ptr;
		}
		BaseType* get() const
		{
			return m_ptr;
		}
		BaseType* release() const
		{
			((auto_ptr<BaseType[]>*)this)->m_owns = false;
			return m_ptr;
		}

	private:
		bool m_owns;
		BaseType* m_ptr;
	};

	//! 참조 카운트가 달린 객체를 담는 템플릿
	/*!
	 * \ingroup gfx2d
	 * \remarks 이 템플릿 코드는 'Ogre3D'의 것을 가져와서 싱글 쓰레드용으로 수정한 것임
	 */
	template<typename BaseType>
	class shared_ptr
	{
	protected:
		BaseType* m_p_instance;
		unsigned int* m_use_count;

	public:
		shared_ptr()
			: m_p_instance(0)
			, m_use_count(0)
		{
		}
		explicit shared_ptr(BaseType* p_instance)
			: m_p_instance(p_instance)
			, m_use_count(new unsigned int(1))
		{
		}
		shared_ptr(const shared_ptr& ref_instance)
			: m_p_instance(0)
			, m_use_count(0)
		{
			m_p_instance = ref_instance.m_p_instance;
			m_use_count  = ref_instance.m_use_count; 

			if (m_use_count)
				++(*m_use_count); 
		}
		virtual ~shared_ptr()
		{
			release();
		}

		shared_ptr& operator=(const shared_ptr& ref_instance)
		{
			if (m_p_instance == ref_instance.m_p_instance)
				return *this;

			release();

			{
				m_p_instance = ref_instance.m_p_instance;
				m_use_count  = ref_instance.m_use_count;
				if (m_use_count)
					++(*m_use_count);
			}

			return *this;
		}

		inline BaseType& operator*() const
		{
			assert(m_p_instance);

			return *m_p_instance;
		}
		inline BaseType* operator->() const
		{
			assert(m_p_instance);

			return m_p_instance;
		}
		inline BaseType* get() const
		{
			return m_p_instance;
		}

		void bind(BaseType* rep)
		{
			assert(!m_p_instance && !m_use_count);

			m_use_count  = new unsigned int(1);
			m_p_instance = rep;
		}

		inline bool unique() const
		{
			assert(m_use_count);

			return (*m_use_count == 1);
		}
		inline unsigned int useCount() const
		{
			assert(m_use_count);

			return *m_use_count;
		}
		inline unsigned int* useCountPointer() const
		{
			return m_use_count;
		}

		inline BaseType* getPointer() const
		{
			return m_p_instance;
		}

		inline bool isNull(void) const
		{
			return (m_p_instance == 0);
		}

		inline void setNull(void)
		{ 
			if (m_p_instance)
			{
				release();
				m_p_instance = 0;
				m_use_count  = 0;
			}
		}

	protected:
		inline void release(void)
		{
			if (m_use_count)
				if (--(*m_use_count) == 0) 
					destroy();
		}

		virtual void destroy(void)
		{
			delete m_p_instance;
			delete m_use_count;
		}
	};

	template<typename TBaseType1, typename TBaseType2> inline bool operator==(shared_ptr<TBaseType1> const& a, shared_ptr<TBaseType2> const& b)
	{
		return a.get() == b.get();
	}

	template<typename TBaseType1, typename TBaseType2> inline bool operator!=(shared_ptr<TBaseType1> const& a, shared_ptr<TBaseType2> const& b)
	{
		return a.get() != b.get();
	}

	//! 포인터 객체에 자동 해제 기능을 부여하는 템플릿
	/*!
	 * \ingroup gfx2d
	 */
	template <typename BaseType>
	class auto_deletor
	{
	public:
		auto_deletor(BaseType* p_object = 0)
			: m_p_object(p_object)
		{
		}
		~auto_deletor(void)
		{
			delete m_p_object;
		}

		void bind(BaseType* p_object)
		{
			assert(m_p_object == 0);
			m_p_object = p_object; 
		}

	private:
		BaseType* m_p_object;
	};

	template<typename BaseType>
	class auto_deletor<BaseType[]>
	{
	public:
		auto_deletor(BaseType* p_object = 0)
			: m_p_object(p_object)
		{
		}
		~auto_deletor(void)
		{
			delete[] m_p_object;
		}

		void bind(BaseType* p_object)
		{
			assert(m_p_object == 0);
			m_p_object = p_object; 
		}

	private:
		BaseType* m_p_object;
	};

	//! 싱글톤 객체를 만들어 주기 위한 템플릿 (single thread 전용)
	/*!
	 * \ingroup gfx2d
	 */
	template <typename BaseType, int kind = 0>
	class singleton
	{
	public:
		static BaseType& get(int param = 0)
		{
			if (m_instance == 0)
			{
				m_instance = new BaseType;
				m_deletor.bind(m_instance);
			}

			return *m_instance;
		}
		// 명시적인 해제 순서가 필요할 때 사용한다.
		static void release(int param = 0)
		{
			if (m_instance)
			{
				delete m_instance;
				m_instance = 0;
				m_deletor.bind(m_instance);
			}
		}
	private:
		singleton(void);

		static BaseType* m_instance;
		static auto_deletor<BaseType> m_deletor;
	};

	template <class BaseType, int kind> BaseType* singleton<BaseType, kind>::m_instance = 0;
	template <class BaseType, int kind> auto_deletor<BaseType> singleton<BaseType, kind>::m_deletor;

	//! boolflag
	/*!
	 * \ingroup gfx2d
	 */
	template <int MAX_FLAG_NUM>
	class boolflag
	{
		enum
		{
			MAX_FLAG_BYTE = (MAX_FLAG_NUM + 7) / 8,
		};

		typedef unsigned char Item;

		class boolflag_private
		{
		public:
			boolflag_private(Item& item, int shift)
				: m_item(item)
				, m_shift(shift)
			{
			}
			boolflag_private& operator=(bool flag)
			{
				if (flag)
					m_item |=  (0x80 >> m_shift);
				else
					m_item &= ~(0x80 >> m_shift);

				return (*this);
			}
			operator bool() const
			{
				return (m_item & (0x80 >> m_shift)) > 0;
			}

		private:
			Item& m_item;
			int   m_shift;
		};

	public:
		boolflag(void)
		{
			Item* pTable = m_table;

			for (int nItem = MAX_FLAG_BYTE; nItem > 0; --nItem)
				*pTable++ = 0;
		}
		boolflag_private operator[](int index)
		{
			if ((index < 0) || (index >= MAX_FLAG_NUM))
			{
				static Item item;

				item = 0;

				return (boolflag_private(item, 0));
			}

			return (boolflag_private(m_table[index/8], index%8));
		}

	private:
		boolflag(const boolflag&);
		boolflag& operator=(const boolflag<MAX_FLAG_NUM>& ref);

		Item m_table[MAX_FLAG_BYTE];
	};

	////////////////////////////////////////////////////////////////////////////////
	// intflag

	template <typename Item, int MAX_FLAG_NUM>
	class intflag
	{
		enum
		{
			MAX_FLAG_BYTE = (MAX_FLAG_NUM + 7) / 8,
		};

		class intflag_private
		{
		public:
			intflag_private(Item& item)
				: m_item(item)
			{
			}
			intflag_private& operator=(Item value)
			{
				m_item = value;

				return (*this);
			}
			operator Item() const
			{
				return m_item;
			}
			intflag_private& operator++()
			{
				++m_item;

				return (*this);
			}

		private:
			Item& m_item;
		};

	public:
		intflag(void)
		{
			Item* pTable = m_table;

			for (int nItem = MAX_FLAG_BYTE; nItem > 0; --nItem)
				*pTable++ = 0;
		}
		intflag_private operator[](int index)
		{
			if ((index < 0) || (index >= MAX_FLAG_NUM))
			{
				static Item item;

				item = 0;

				return (intflag_private(item));
			}

			return (intflag_private(m_table[index]));
		}

	private:
		intflag(const intflag& ref);
		intflag& operator=(const intflag<Item, MAX_FLAG_NUM>& ref);

		Item m_table[MAX_FLAG_BYTE];
	};

#if defined(USE_STL)

	//! 팩토리용 template
	/*!
	 * \ingroup AVEJ utility
	*/
	template <class BaseClass, typename Id>
	class factory
	{
		typedef BaseClass*(*FnCreate)(int);
		typedef std::map<Id, FnCreate> Map;

	public:
		//! 객체를 생성한다.
		BaseClass* createObject(const Id& id, int level = 0)
		{
			typename Map::iterator i = m_map.find(id);

			return (i != m_map.end()) ? (i->second)(level) : 0;
		}
		//! 객체 생성 함수를 등록한다.
		bool registerfunction(const Id& id, FnCreate fn_create)
		{
			typename Map::iterator i = m_map.find(id);

			if (i != m_map.end())
				return false;

			m_map[id] = fn_create;

			return true;
		}
		//! 팩토리 자체를 리셋한다..
		void reset(void)
		{
			Map temp;
			m_map.swap(temp);
		}

	private:
		Map m_map;
	};

#endif // #if defined(USE_STL)

} // namespace sena

namespace sena
{
	template <typename _type>
	_type max(_type val1, _type val2)
	{
		return (val1 > val2) ? val1 : val2;
	}

	template <typename _type>
	_type min(_type val1, _type val2)
	{
		return (val1 < val2) ? val1 : val2;
	}

	template<typename TData>
	inline void swap(TData& a, TData& b)
	{
		TData temp = a;
		a = b, b = temp;
	}

	template<class TInIter, class TOutIter>
	inline TOutIter copy(TInIter first, TInIter last, TOutIter dest)
	{
		for (; first != last; ++dest, ++first)
			*dest = *first;

		return dest;
	}

	template<class TInIter, class TOutIter>
	inline TOutIter copy_backward(TInIter first, TInIter last, TOutIter dest)
	{
		while (first != last)
			*--dest = *--last;
		
		return dest;
	}

	template<class TIter, class TData>
	inline void fill(TIter first, TIter last, const TData& data)
	{
		for (; first != last; ++first)
			*first = data;
	}

	template<class TInIter, class TOutIter>
	inline TOutIter strncpy(TOutIter dest, TInIter first, int size)
	{
		TInIter last = first + size;
		for (; first != last; ++dest, ++first)
		{
			*dest = *first;
			if (*first == 0)
				break;
		}

		return dest;
	}

	template<typename BaseType>
	inline int strlen(const BaseType* src)
	{
		const BaseType* p = src;
		while (*p++)
			;

		return (p - src) - 1;
	}

	// from 'man strncat'
	template<typename BaseType>
	inline BaseType* strncat(BaseType* dest, const BaseType* src, int n)
	{
		int dest_len = strlen(dest);
		int i = 0;

		for (; i < n && src[i]; i++)
			dest[dest_len + i] = src[i];
		
		dest[dest_len + i] = 0;

		return dest;
	}

	template<typename BaseType>
	inline int strncmp(const BaseType* sz_str1, const BaseType* sz_str2, int n)
	{
		for (; n > 0; sz_str1++, sz_str2++, --n)
		{
			if (*sz_str1 != *sz_str2)
				return ((*(unsigned char *)sz_str1 < *(unsigned char *)sz_str2) ? -1 : +1);
			else if (*sz_str1 == BaseType(0))
				return 0;
		}

		return 0;
	}

	template<typename BaseType>
	inline bool strndiff(const BaseType* sz_str1, const BaseType* sz_str2, int n)
	{
		for (; n > 0; sz_str1++, sz_str2++, --n)
		{
			if (*sz_str1 == *sz_str2)
				if (*sz_str1 != BaseType(0))
					continue;

			return (*sz_str1 == *sz_str2);
		}

		return true;
	}

	inline void memclear(void* p_dst, int size)
	{
		char* p_dst_begin = (char*)p_dst;
		char* p_dst_end = p_dst_begin + size;

		while (p_dst_begin < p_dst_end)
			*p_dst_begin++ = 0;
	}

	inline void memcpy(void* p_dst, const void* p_src, int size)
	{
		char* p_dst_begin = (char*)p_dst;
		char* p_dst_end = p_dst_begin + size;

		const char* p_src_begin = (char*)p_src;

		while (p_dst_begin < p_dst_end)
			*p_dst_begin++ = *p_src_begin++;
	}

	template<typename Iterator, typename Function>
	inline Function for_each(Iterator first, Iterator last, Function fn)
	{
		for ( ; first != last; ++first)
			fn(*first);

		return fn;
	}

	template<typename T1, typename T2>
	struct pair
	{
		pair()
			: first(T1()), second(T2()) {}
		pair(const T1& val1, const T2& val2)
			: first(val1), second(val2) {}
		template<class U, class V> pair(const pair<U, V>& ref)
			: first(ref.first), second(ref.second) {}

		T1 first;
		T2 second;
	};

	template<typename T1, typename T2>
	inline pair<T1, T2> make_pair(const T1& val1, const T2& val2)
	{
		return (pair<T1, T2>(val1, val2));
	}

	template<typename T1, typename T2, typename T3>
	struct triple
	{
		triple()
			: first(T1()), second(T2()), third(T3()) {}
		triple(const T1& val1, const T2& val2, const T3& val3)
			: first(val1), second(val2), third(val3) {}
		template<class U, class V, class W> triple(const triple<U, V, W>& ref)
			: first(ref.first), second(ref.second), third(ref.third) {}

		T1 first;
		T2 second;
		T3 third;
	};

	template<typename T1, typename T2, typename T3>
	inline triple<T1, T2, T3> make_triple(const T1& val1, const T2& val2, const T3& val3)
	{
		return (triple<T1, T2, T3>(val1, val2, val3));
	}

	template<typename TIter, typename TProc>
	inline TIter find_if(TIter first, TIter last, TProc Proc)
	{
		for (; first != last; ++first)
			if (Proc(*first))
				break;

		return (first);
	}

	template<typename TDataType>
	class vector
	{
	public:
		typedef vector<TDataType> TMyVector;
		typedef size_t            size_type;
		typedef       TDataType*  iterator;
		typedef const TDataType*  const_iterator;
		typedef       TDataType&  reference;
		typedef const TDataType&  const_reference;

		explicit vector()
		: m_first(0), m_last(0), m_end(0)
		{
		}
		~vector()
		{
			_Destroy(m_first, m_last);
			operator delete(m_first);
			m_first = 0, m_last = 0, m_end = 0;
		}
		void reserve(int num_alloc)
		{
			assert(m_first == 0 && m_last == 0 && m_end == 0);

			if (num_alloc > 0)
			{
				m_first = (TDataType*)operator new(num_alloc * sizeof(TDataType));
				m_last = m_first;
				m_end = m_first + num_alloc;
			}
		}
		iterator begin()
		{
			return (m_first);
		}
		const_iterator begin() const
		{
			return ((const_iterator)m_first);
		}
		iterator end()
		{
			return (m_last);
		}
		const_iterator end() const
		{
			return ((const_iterator)m_last);
		}
		size_type size() const
		{
			return (m_first == 0) ? 0 : m_last - m_first;
		}
		size_type capacity() const
		{
			return (m_first == 0) ? 0 : m_end - m_first;
		}
		bool empty() const
		{
			return (size() == 0);
		}
		const_reference at(size_type index) const
		{
			if (size() <= index)
				_Throw_OutOfRange();
			return (*(begin() + index));
		}
		reference at(size_type index)
		{
			if (size() <= index)
				_Throw_OutOfRange();
			return (*(begin() + index));
		}
		const_reference operator[](size_type index) const
		{
			return (*(begin() + index));
		}
		reference operator[](size_type index)
		{
			return (*(begin() + index));
		}
		void push_back(const TDataType& data)
		{
			insert(end(), 1, data);
		}
		void pop_back()
		{
			erase(end() - 1);
		}
		void insert(iterator iter, size_type num_insert, const TDataType& data)
		{
			if (m_end - m_last < ptrdiff_t(num_insert))
			{
				size_type num_alloc       = size() + (num_insert < size() ? size() : num_insert);
				iterator  p_allocated     = ((TDataType *)operator new(num_alloc * sizeof(TDataType)));
				iterator  p_allocated_end = _copy(m_first, iter, p_allocated);

				_fill(p_allocated_end, num_insert, data);
				_copy(iter, m_last, p_allocated_end + num_insert);

				_Destroy(m_first, m_last);
				operator delete(m_first);

				m_end   = p_allocated + num_alloc;
				m_last  = p_allocated + size() + num_insert;
				m_first = p_allocated;
			}
			else if (m_last - iter < ptrdiff_t(num_insert))
			{
				_copy(iter, m_last, iter + num_insert);
				_fill(m_last, num_insert - (m_last - iter), data);
				fill(iter, m_last, data);
				m_last += num_insert;
			}
			else if (0 < num_insert)
			{
				_copy(m_last - num_insert, m_last, m_last);
				copy_backward(iter, m_last - num_insert, m_last);
				fill(iter, iter + num_insert, data);
				m_last += num_insert;
			}
		}
		iterator erase(iterator iter)
		{
			copy(iter + 1, end(), iter);
			_Destroy(m_last - 1, m_last);
			--m_last;
			return (iter);
		}
		iterator erase(iterator iter_first, iterator iter_last)
		{
			iterator temp = copy(iter_last, end(), iter_first);
			_Destroy(temp, end());
			m_last = temp;
			return iter_first;
		}
		void clear()
		{
			erase(begin(), end());
		}
		void swap(TMyVector& ref)
		{
			TMyVector temp = *this; *this = ref, ref = temp;
		}

		friend void swap(TMyVector& ref1, TMyVector& ref2)
		{
			ref1.swap(ref2);
		}

	protected:
		void _Destroy(iterator first, iterator last)
		{
			for (; first != last; ++first)
				(first)->~TDataType();
		}
		iterator _copy(const_iterator first, const_iterator last, iterator p_allocated)
		{
			for (; first != last; ++p_allocated, ++first)
				new ((void*)p_allocated) TDataType(*first);

			return (p_allocated);
		}
		void _fill(iterator first, size_type num_fill, const TDataType &data)
		{
			for (; 0 < num_fill; --num_fill, ++first)
				new ((void*)first) TDataType(data);
		}
		void _Throw_OutOfRange() const
		{
			assert(0);
		}

		iterator m_first, m_last, m_end;

	private:
		TMyVector& operator=(const TMyVector&);

	};

}

#endif // #ifndef __SM_UTIL_SENA_H__
