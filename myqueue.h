#pragma once

namespace mylib
{
	//值语义队列
	template<typename T>
	class queue_var
	{
		typedef queue_var<T> _MyT;
	public:
		queue_var(int capacity = 10)
		{
			_capacity = capacity;
			_n = 0;
			data = new T[capacity];
			_first = 0;
			_end = 0;
		}

		~queue_var()
		{
			if (data)
			{
				delete[] data;
				data = nullptr;
			}
		}

		queue_var(const _MyT& obj)
		{
			_capacity = obj._capacity;
			_n = obj._n;
			_end = 0;
			_first = _n;
			data = new T[_capacity];
			for (int i = 0; i < _n; i++)
				data[i] = obj.data[(i + obj._end) % _capacity];
		}

		queue_var(_MyT&& obj)
		{
			_capacity = obj._capacity;
			_n = obj._n;
			_first = obj._first;
			_end = obj._end;
			data = obj.data;
			obj.data = nullptr;
		}

		_MyT& operator=(const _MyT& obj)
		{
			if (this == &obj)
				return *this;

			if (data)
				delete[] data;

			_capacity = obj._capacity;
			_n = obj._n;
			_end = 0;
			_first = _n;
			data = new T[_capacity];
			for (int i = 0; i < _n; i++)
				data[i] = obj.data[(i + obj._end) % _capacity];

			return *this;
		}

		_MyT& operator=(_MyT&& obj)
		{
			if (this == &obj)
				return *this;

			if (data)
				delete[] data;

			_capacity = obj._capacity;
			_n = obj._n;
			_first = obj._first;
			_end = obj._end;
			data = obj.data;
			obj.data = nullptr;
		}

		void in(T val)
		{
			if (_n == _capacity)
				_expand();

			data[_first] = val;
			_first = (_first + 1) % _capacity;
			_n++;
		}

		T out()
		{
			if (_n == 0)
				throw std::logic_error("队列为空，无法出队列。");

			T result = data[_end];
			_end = (_end + 1) % _capacity;
			_n--;
			return result;
		}

		int size() { return _n; }
		int capacity() { return _capacity; }

	private:
		void _expand()
		{
			T* newdata = new T[2 * _capacity];
			for (int i = 0; i < _n; i++)
				newdata[i] = data[(i + _end) % _capacity];
			_capacity = 2 * _capacity;
			_first = _n;
			_end = 0;
			delete[] data;
			data = newdata;
		}

	private:
		int _capacity;
		int _n;
		T* data;
		int _first;
		int _end;
	};
}
