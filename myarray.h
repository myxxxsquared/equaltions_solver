#pragma once

namespace mylib{
	template<typename T>
	class array_val
	{
	public:
		inline array_val(){ data = nullptr; len = 0; }
		inline array_val(int length){ data = new T[length]; len = length; }
		inline ~array_val(){ if (data)delete[] data; }

		array_val(const array_val& obj)
		{
			len = obj.len;
			data = new T[len];
			for (int i = 0; i < len; i++)
				data[i] = obj.data[i];
		}

		array_val(array_val&& obj)
		{
			len = obj.len;
			data = obj.data;
			obj.data = nullptr;
		}

		array_val& operator=(const array_val& obj)
		{
			if (this == &obj)
				return *this;

			if (data)
				delete[]data;

			len = obj.len;
			data = new T[len];
			for (int i = 0; i < len; i++)
				data[i] = obj.data[i];
		}

		array_val& operator=(array_val&& obj)
		{
			if (this == &obj)
				return *this;

			if (data)
				delete[]data;

			len = obj.len;
			data = obj.data;
			obj.data = nullptr;
		}

		inline T& operator[](int index){ return data[index]; }
		inline const T& operator[](int index) const{ return data[index]; }
		inline T* Data(){ return data; }
		inline const T* Data() const{ return data };
		inline void ReNew(int length){ if (data)delete data; data = new T[length]; len = length; }
		inline operator T*(){ return data; }
		inline operator const T*() const{ return data; }
		inline int size() const{ return len; }

		int search_in_order(T val)
		{
			int a = 0, b = len - 1, c;
			while (b > a)
			{
				c = (a + b) / 2;
				if (val == data[c])
					return c;
				if (val < data[c])
					b = c - 1;
				else
					a = c + 1;
			}
			return -1;
		}

	private:
		T* data;
		int len;
	};


	class array_val_writter
	{
	public:
		inline array_val_writter() { data = nullptr; len = 0; cur = 0; }
		inline array_val_writter(int length){ data = new char[length]; memset(data, 0, length); len = length; cur = 0; }
		inline ~array_val_writter(){ if (data)delete[] data; }

		inline array_val_writter(const array_val_writter& obj)
		{
			len = obj.len;
			data = new char[len];
			cur = obj.cur;
			for (int i = 0; i < len; i++)
				data[i] = obj.data[i];
		}

		inline array_val_writter(array_val_writter&& obj)
		{
			len = obj.len;
			data = obj.data;
			cur = obj.cur;
			obj.data = nullptr;
		}

		inline array_val_writter& operator=(const array_val_writter& obj)
		{
			if (this == &obj)
				return *this;

			if (data)
				delete[]data;

			cur = obj.cur;
			len = obj.len;
			data = new char[len];
			for (int i = 0; i < len; i++)
				data[i] = obj.data[i];
		}

		inline array_val_writter& operator=(array_val_writter&& obj)
		{
			if (this == &obj)
				return *this;

			if (data)
				delete[]data;

			cur = obj.cur;
			len = obj.len;
			data = obj.data;
			obj.data = nullptr;
		}

		inline char& operator[](int index){ return data[index]; }
		inline const char& operator[](int index) const{ return data[index]; }
		inline char* Data(){ return data; }
		inline const char* Data() const{ return data; }
		inline void ReNew(int length){ if (data)delete data; data = new char[length]; len = length; }
		inline operator char*(){ return data; }
		inline operator const char*() const{ return data; }
		inline int size() const{ return len; }
		inline int current() const{ return cur; }
		inline int setcur(int c){ return (cur = c); }
		inline int seek(int c){ return (cur += c); }
		inline void write(const void* src, int len){
			if (cur + len > this->len)
				throw std::logic_error("no much memory");
			memcpy(data + cur, src, len); cur += len;
		}
		inline void read(void* dst, int len)
		{
			if (cur + len > this->len)
				throw std::logic_error("no much memory");
			memcpy(dst, data + cur, len); cur += len;
		}
		inline mylib::string readstring()
		{
			int i;
			for (i = cur; i < len; i++)
			{
				if (data[i] == '\0')
				{
					int curold = cur;
					cur = i + 1;
					return mylib::string(data+curold);
				}
			}
			throw std::logic_error("no string end.");
		}
	private:
		int cur;
		char* data;
		int len;
	};

	template<class T>
	class myptr
	{
	public:
		myptr(T * ptr){ this->ptr = ptr; }
		~myptr(){ delete ptr; }
		myptr(const myptr<T>&) = delete;
		myptr<T> & operator=(const myptr<T>&) = delete;

		T* operator*(){ return ptr; }
		const T* operator*() const{ return ptr; }
		T* operator->(){ return ptr; }
		const T* operator->() const{ return ptr; }
		operator T* (){ return ptr; }
		operator const T* ()const{ return ptr; }
	private:
		T * ptr;
	};

	template<class T>
	class myptr_shared
	{
	public:
		myptr_shared(T* p)
		{
			ptr = p;
			ref = new int(1);
		}

		myptr_shared()
		{
			ptr = nullptr;
			ref = nullptr;
		}

		~myptr_shared()
		{
			if (ref)
			{
				(*ref)--;
				if (!(*ref))
				{
					delete ptr;
					delete ref;
					ref = nullptr;
				}
			}
		}

		myptr_shared(const myptr_shared& obj)
		{
			ptr = obj.ptr;
			ref = obj.ref;

			if (ref)
				(*ref)++;
		}

		myptr_shared& operator=(const myptr_shared&)
		{
			if (ref)
			{
				(*ref)--;
				if (!(*ref))
				{
					delete ptr;
					delete ref;
					ref = nullptr;
				}
			}

			ptr = obj.ptr;
			ref = obj.ref;

			if (ref)
				(*ref)++;
		}

		T& operator*(){ return *ptr; }
		const T& operator*() const{ return *ptr; }
		T* operator->(){ return ptr; }
		const T* operator->() const{ return ptr; }
		operator T* (){ return ptr; }
		operator const T* ()const{ return ptr; }
	private:
		T * ptr;
		int * ref;
	};
}