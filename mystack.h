#pragma once

namespace mylib{

	//自定义栈类型来提高效率
	template<typename T, int defaultlen = 100>
	class stack
	{
	public:
		inline stack()
		{
			cur = 0;
			len = defaultlen;
			data = new T[defaultlen];
		}

		~stack()
		{
			delete data;
		}

		stack(const stack&) = delete;
		stack& operator=(const stack&) = delete;

		inline void push(T value)
		{
			if (cur >= len)
				expand();
			data[cur++] = value;
		}

		inline T pop() throw(std::logic_error)
		{
			if (empty())
				throw std::logic_error("Pop from an empty stack!");
			return data[--cur];
		}

		inline bool empty() const { return cur == 0; }

		inline T& top() { return data[cur - 1]; }
		inline void clear() { len = 0; }

	private:
		void expand()
		{
			T* newdata = new T[len + defaultlen];
			memcpy(newdata, data, sizeof(T)*len);
			len += defaultlen;
			delete data;
			data = newdata;
		}

		int cur;
		int len;
		T* data;
	};

	template<typename T>
	class stack_size_var
	{
	public:
		stack_size_var(unsigned int n)
		{
			if (n == 0)
				throw std::invalid_argument("invild length");
			data = new T[len];
		}

		stack_size_var(const stack_size_var& obj)
		{
			cur = obj.cur;
			size = obj.size;
			len = obj.len;
			data = new T[len];
			for (int i = 0; i < len; i++)
				data[i] = obj.data[i];
		}

		stack_size_var(stack_size_var&& obj)
		{
			cur = obj.cur;
			size = obj.size;
			len = obj.len;
			data = obj.data;
			obj.data = nullptr;
		}

		stack_size_var& operator=(const stack_size_var& obj)
		{
			if (*obj == this)
				return *this;

			if (data)
				delete[] data;

			cur = obj.cur;
			size = obj.size;
			len = obj.len;
			data = new T[len];
			for (int i = 0; i < len; i++)
				data[i] = obj.data[i];

			return *this;
		}

		stack_size_var& operator=(stack_size_var&& obj)
		{
			if (*obj == this)
				return *this;

			if (data)
				delete[] data;

			cur = obj.cur;
			size = obj.size;
			len = obj.len;
			data = obj.data;
			obj.data = nullptr;

			return *this;
		}

		~stack_size_var()
		{
			if (data)
				delete[] data;
			data = nullptr;
			cur = size = 0;
		}

		void push(T val)
		{
			data[cur] = val;
			cur = (cur + 1) % size;
			if (size < len)
				size++;
		}

		T pop()
		{
			if (size <= 0)
				throw std::logic_error("Pop from an empty stack.");
			cur--;
			size--;
			if (cur == -1)
				cur = size - 1;
			return data[cur];
		}

		T& top()
		{
			if (size <= 0)
				throw std::logic_error("Pop from an empty stack.");
			cur2 = cur - 1;
			if (cur2 == -1)
				cur2 = size - 1;
			return data[cur];
		}

		int getsize()
		{
			return size;
		}

	private:
		int cur;
		int size;
		int len;
		T* data;
	};

	template<typename T>
	class stack_size
	{
	public:
		stack_size(int n)
		{
			if (n <= 0)
				throw std::invalid_argument("invild length");
			len = n;
			data = new T*[len];
			memset(data, 0, sizeof(T*) * len);
		}

		stack_size(const stack_size& obj)
		{
			cur = obj.cur;
			size = obj.size;
			len = obj.len;
			data = new T*[len];
			memset(data, 0, sizeof(T*) * len);
			for (int i = 0; i < len; i++)
				if (obj.data[i])
					data[i] = new T(*obj.data[i]);
		}

		stack_size(stack_size&& obj)
		{
			cur = obj.cur;
			size = obj.size;
			len = obj.len;
			data = obj.data;
			obj.data = nullptr;
		}

		stack_size& operator=(const stack_size& obj)
		{
			if (*obj == this)
				return *this;

			if (data)
			{
				for (int i = 0; i < len; i++)
					if (data[i])
						delete data[i];
				delete[] data;
				data = nullptr;
			}

			cur = obj.cur;
			size = obj.size;
			len = obj.len;
			data = new T*[len];
			memset(data, 0, sizeof(T*) * len);
			for (int i = 0; i < len; i++)
				if (obj.data[i])
					data[i] = new T(*obj.data[i]);

			return *this;
		}

		stack_size& operator=(stack_size&& obj)
		{
			if (*obj == this)
				return *this;

			if (data)
			{
				for (int i = 0; i < len; i++)
					if (data[i])
						delete data[i];
				delete[] data;
				data = nullptr;
			}

			cur = obj.cur;
			size = obj.size;
			len = obj.len;
			data = obj.data;
			obj.data = nullptr;

			return *this;
		}

		~stack_size()
		{
			if (data)
			{
				for (int i = 0; i < len; i++)
					if (data[i])
						delete data[i];
				delete[] data;
				data = nullptr;
			}
		}

		void push(T val)
		{
			if (size == len)
				delete data[cur];
			data[cur] = new T(val);
			cur = (cur + 1) % size;
			if (size < len)
				size++;
		}

		T pop()
		{
			if (size <= 0)
				throw std::logic_error("Pop from an empty stack.");
			cur--;
			size--;
			if (cur == -1)
				cur = size - 1;
			T result = *data[cur];
			delete data[cur];
			return T;
		}

		T& top()
		{
			if (size <= 0)
				throw std::logic_error("Pop from an empty stack.");
			cur2 = cur - 1;
			if (cur2 == -1)
				cur2 = size - 1;
			return *data[cur];
		}

		int getsize()
		{
			return size;
		}

	private:
		int cur;
		int size;
		int len;
		T** data;
	};

}