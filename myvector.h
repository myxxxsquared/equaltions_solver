#pragma once

namespace mylib{

	template <typename T>
	class vector
	{
	public:
		inline vector(int deflen = 10)
		{
			defaultlen = deflen;
			memlen = defaultlen;
			data = new T*[defaultlen];
			len = 0;
		}

		inline ~vector()
		{
			for (int i = 0; i < len; i++)
				delete data[i];
			delete data;
		}

		vector(const vector & obj)
		{
			defaultlen = obj.defaultlen;
			memlen = obj.memlen;
			len = obj.len;
			data = new T*[memlen];

			for (int i = 0; i < len; i++)
			{
				data[i] = new T(*obj.data[i]);
			}
		}

		vector& operator=(const vector& obj)
		{
			if (this == &obj)
				return *this;

			for (int i = 0; i < len; i++)
				delete data[i];
			delete data;

			defaultlen = obj.defaultlen;
			memlen = obj.memlen;
			len = obj.len;
			data = new T*[memlen];

			for (int i = 0; i < len; i++)
			{
				data[i] = new T(*(obj.data[i]));
			}

			return *this;
		}

		void push_back(const T & obj)
		{
			if (len >= memlen)
				expand();
			data[len++] = new T(obj);
		}

		void insert(const T& obj, int index)throw(std::invalid_argument)
		{
			if (index == len)
			{
				push_back(obj);
				return;
			}
			if (index <0 || index > len)
				throw std::invalid_argument("Invalid index");

			if (len >= memlen)
				expamd();
			for (int i = len; i >= index; i--)
				data[i] = data[i - 1];
			data[index] = new T(obj);
			len++;
		}

		T pop_back()throw(std::logic_error)
		{
			if (len <= 0)
				throw std::logic_error("Pop from an empty vector.");
			return *(data[--len]);
		}

		void erase(int index)throw(std::invalid_argument)
		{
			if (index == len - 1)
			{
				delete data[index];
				len--;
				return;
			}

			if (index < 0 || index > len - 1)
				throw std::invalid_argument("Invalid index");

			delete data[index];
			len--;
			for (int i = index; i < len; i++)
				data[i] = data[i + 1];
		}

		void clear()
		{
			if (len == 0)
				return;
			for (int i = 0; i < len; i++)
				delete data[i];

			len = 0;
		}

		inline T& operator[](int index)throw(std::invalid_argument) {
			if (index < 0 || index > len - 1)
				throw std::invalid_argument("Invalid index");
			return *(data[index]);
		}

		inline const T& operator[](int index) const throw(std::invalid_argument) {
			if (index < 0 || index > len - 1)
				throw std::invalid_argument("Invalid index");
			return *(data[index]);
		}

		inline int size() const { return len; }

		inline void remove(int index)
		{
			erase(index);
		}
	private:
		void expand()
		{
			T** newdata = new T*[memlen + defaultlen];
			memcpy(newdata, data, sizeof(T*) * len);
			delete[] data;
			data = newdata;
			memlen += defaultlen;
			defaultlen = memlen;
		}

		int defaultlen;
		int memlen;
		int len;
		T**data;
	};

	template<typename T>
	class vector_var
	{
	public:
		inline vector_var(int defaultlen = 100)
		{
			cur = 0;
			len = defaultlen;
			data = new T[defaultlen];
			this->defaultlen = defaultlen;
		}

		~vector_var()
		{
			delete data;
		}

		vector_var(const vector_var& s)
		{
			this->cur = s.cur;
			this->len = s.len;

			this->data = new T[len];
			memcpy(this->data, s.data, sizeof(T)*len);
			this->defaultlen = s.defaultlen;
		}

		vector_var& operator=(const vector_var& s)
		{
			if (this == &s)
				return *this;

			delete data;

			this->cur = s.cur;
			this->len = s.len;

			this->data = new T[len];
			memcpy(this->data, s.data, sizeof(T)*len);
			this->defaultlen = s.defaultlen;
			return *this;
		}

		inline void push_back(T value)
		{
			if (cur >= len)
				expand();
			data[cur++] = value;
		}

		inline T pop_back()
		{
			if (empty())
				throw std::logic_error("Pop from an empty vector.");
			return data[--cur];
		}

		inline T& operator[](int index)throw(std::invalid_argument)
		{
			if (index >= 0 && index < cur)
				return data[index];
			throw std::invalid_argument("Invalid index");
		}

		inline const T& operator[](int index)const throw(std::invalid_argument)
		{
			if (index >= 0 && index < cur)
				return data[index];
			throw std::invalid_argument("Invalid index");
		}

		void remove(int index)
		{
			if (index == cur - 1)
			{
				cur--;
				return;
			}

			if (index < 0 || index > cur)
				throw std::invalid_argument("Invalid index");

			cur--;
			for (int i = index; i < cur; i++)
				data[i] = data[i + 1];
		}

		inline bool empty() const { return cur == 0; }
		inline T& top() { return data[cur - 1]; }

		inline int size() const { return cur; }

		inline void clear(){ cur = 0; }

	private:
		void expand()
		{
			T* newdata = new T[len + defaultlen];
			memcpy(newdata, data, sizeof(T)*len);
			len += defaultlen;
			delete data;
			data = newdata;
			defaultlen = len;
		}

		int cur;
		int len;
		T* data;

		int defaultlen;
	};

	class vector_val_writter
	{
	public:
		inline vector_val_writter() { data = nullptr; len = 0; cur = 0; }
		inline vector_val_writter(int length){ data = new char[length]; memset(data, 0, length); len = length; cur = 0; }
		inline ~vector_val_writter(){ if (data)delete[] data; }

		inline vector_val_writter(const vector_val_writter& obj)
		{
			len = obj.len;
			data = new char[len];
			cur = obj.cur;
			for (int i = 0; i < len; i++)
				data[i] = obj.data[i];
		}

		inline vector_val_writter(vector_val_writter&& obj)
		{
			len = obj.len;
			data = obj.data;
			cur = obj.cur;
			obj.data = nullptr;
		}

		inline vector_val_writter& operator=(const vector_val_writter& obj)
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
			return *this;
		}

		inline vector_val_writter& operator=(vector_val_writter&& obj)
		{
			if (this == &obj)
				return *this;

			if (data)
				delete[]data;

			cur = obj.cur;
			len = obj.len;
			data = obj.data;
			obj.data = nullptr;
			return *this;
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
			while (cur + len > this->len)
				expand();
			memcpy(data + cur, src, len); cur += len;
		}
		inline void write(char ch){
			while (cur + 1 > this->len)
				expand();
			*(data + cur) = ch;
			cur += 1;
		}
		inline char read()
		{
			return data[cur++];
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
				if (data[i] == '0')
				{
					int curold = cur;
					cur = i + 1;
					return mylib::string(data[curold]);
				}
			}
			throw std::logic_error("no string end.");
		}
	private:
		int cur;
		char* data;
		int len;

		void expand()
		{
			int newlen = len * 2 + 1;
			char* newdata = new char[newlen];
			memcpy(newdata, data, len);
			delete data;
			data = newdata;
			len = newlen;
		}
	};

}