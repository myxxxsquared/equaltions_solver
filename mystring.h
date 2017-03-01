#pragma once

namespace mylib{

	class string
	{
	public:
		string();
		string(int);
		string(E::Value_Type);
		string(E::Value_Type, int);
		string(const string&);
		inline string(string&& str){ data = str.data; length = str.length; str.data = nullptr; }
		string(const char*);
		string(const char*, int);
		inline ~string() { if (data)delete[] data; }
		string & operator=(const string &);
		static string number(int n);
		inline string & operator=(string && str)
		{
			if (&str == this) return *this;
			if (data)
				delete[] data;
			data = str.data;
			str.data = nullptr;
			length = str.length;
			return *this;
		}
		string & operator=(const char*);

		inline char& operator[](int index) throw(std::invalid_argument)
		{
			if (index >= size() || index < 0)
				throw std::invalid_argument("Invalid index");
			return data[index];
		}

		inline operator char*()
		{
			return data;
		}

		inline operator const char*() const
		{
			return data;
		}

		string operator + (const string &) const;

		E::Value_Type ToValueType() const;

		bool operator==(const string&) const;
		bool operator!=(const string&) const;
		bool operator>(const string&) const;
		bool operator<(const string&) const;
		bool operator>=(const string&) const;
		bool operator<=(const string&) const;

		bool operator==(const char *) const;
		int size() const;

		string CheckName() const;

		void refresh();
	private:
		int inner_size() const;
		char* data;
		int length;
	};

}