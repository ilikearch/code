#pragma once
#include<functional>
using namespace std;

namespace bit
{
	template<class T>
	class shared_ptr
	{
	public:
		shared_ptr(T* ptr = nullptr)
			:_ptr(ptr)
			,_pcount(new int(1))
		{}

		template<class D>//删除器
		shared_ptr(T* ptr, D del)
			: _ptr(ptr)
			, _pcount(new int(1))
			,_del(del)
		{}

		// sp2(sp1)
		shared_ptr(const shared_ptr<T>& sp)
			:_ptr(sp._ptr)
			,_pcount(sp._pcount)
		{
			(*_pcount)++;
		}
			
		void release()
		{
			if(--(*_pcount)==0){
				del(_ptr);
				delete _pcount;
				_pcount=nullptr;
				_ptr=nullptr;
			}
		}

		// sp1 = sp3
		// sp3 = sp3
		shared_ptr<T>& operator=(const shared_ptr<T>& sp)
		{
			if (_ptr != sp._ptr)
			{
				release();
				_ptr = sp._ptr;
				_pcount = sp._pcount;
				++(*_pcount);
			}

			return *this;
		}

		~shared_ptr()
		{
			release();
		}

		T* get() const
		{
			return _ptr;
		}

		int use_count() const
		{
			return *_pcount;
		}

		T& operator*()
		{ 
			return *_ptr;
		}

		T* operator->()
		{
			return _ptr;
		}

	private:
		T* _ptr;
		int* _pcount;

		function<void(T*ptr)> _del=[](T*ptr){delete ptr;};
	};

	template<class T>
	class weak_ptr
	{
	public:
		weak_ptr()
		{}

		weak_ptr(const shared_ptr<T>& sp)
			:_ptr(sp.get())
		{}
		weak_ptr<T>& operator=(const shared_ptr<T>& sp)
		{
			_ptr = sp.get();

			return *this;
		}

	private:
		T* _ptr = nullptr;

		//T* _pcount
	};
}