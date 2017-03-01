#include "stdafx.h"

using namespace mylib;

string::string()
{
	data = new char[1];
	data[0] = 0;
	length = 0;
}

string::string(int len)
{
	data = new char[++len];
	memset(data, 0, len);
	length = 0;
}

string::string(E::Value_Type val)
{
	data = new char[100];
	sprintf_s(data, 100, "%lf", val);
	length = inner_size();
}

string::string(E::Value_Type val, int l)
{
	char format[100];
	sprintf_s(format, "%%.%dlf", l);
	data = new char[100];
	sprintf_s(data, 100, format, val);
	length = inner_size();
}

string::string(const string& obj)
{
	int len = obj.size();
	data = new char[len + 1];
	memcpy(data, obj.data, sizeof(char) * (len + 1));
	length = obj.size();
}

string::string(const char* obj)
{
	data = const_cast<char*>(obj);
	length = inner_size();
	data = new char[length + 1];
	memcpy(data, obj, sizeof(char) * (length + 1));
}

string::string(const char * ch, int len)
{
	data = const_cast<char*>(ch);

	int j = inner_size();
	if (j < len)
		len = j;

	data = new char[len + 1];
	memcpy(data, ch, sizeof(char)*len);
	data[len] = 0;
	length = len;
}

string & string::operator=(const string & obj)
{
	delete data;

	int len = obj.size();
	data = new char[len + 1];
	memcpy(data, obj.data, sizeof(char) * (len + 1));
	length = len;
	return *this;
}

string & string::operator=(const char * obj)
{
	delete data;

	data = const_cast<char*>(obj);
	int len = inner_size();
	data = new char[len + 1];
	memcpy(data, obj, sizeof(char) * (len + 1));
	length = len;
	return *this;
}

string string::operator+(const string & obj) const
{
	string s(size() + obj.size());
	memcpy(s.data, data, size());
	memcpy(s.data + size(), obj.data, obj.size());
	s.length = size() + obj.size();
	return s;
}

E::Value_Type string::ToValueType() const
{
	char* cur = data;
	E::Value_Type result = 0.0;

	for (; *cur != '.' && *cur != 'e' && *cur!=0; result = result * 10 + (*(cur++) - '0'));

	if (*cur != 'e' && *cur != 0)
	{
		cur++;
		for (E::Value_Type d = 1; *cur != 'e' && *cur != 0; result += (*(cur++) - '0') * (d /= 10));
	}

	if (*cur == 'e')
	{
		double r2 = 0;
		for (; *cur != 0; r2 = r2 * 10 + (*(cur++) - '0'));
		result *= pow(10.0, (r2));
	}

	return result;
}

bool string::operator==(const string & obj) const
{
	if (size() != obj.size())
		return false;

	char *cur1 = data, *cur2 = obj.data;

	while (*cur1 && *cur2)
		if (*(cur1++) != *(cur2++))
			return false;

	return !(*cur1 || *cur2);
}

bool string::operator!=(const string & obj) const
{
	return !((*this) == obj);
}

bool string::operator>=(const string & obj) const
{
	char *cur1 = data, *cur2 = obj.data;

	while (*cur1 && *cur2)
		if (*(cur1) < *(cur2))
			return false;
		else if (*cur1 > *cur2)
			return true;
		else
		{
			cur1++;
			cur2++;
		}

	return !(*cur2);
}


bool string::operator<=(const string & obj) const
{
	char *cur1 = obj.data, *cur2 = data;

	while (*cur1 && *cur2)
		if (*(cur1) < *(cur2))
			return false;
		else if (*cur1>*cur2)
			return true;
		else
		{
			cur1++; 
			cur2++;
		}

	return !(*cur2);
}

bool string::operator>(const string & obj) const
{
	return !(*this <= obj);
}

bool string::operator<(const string & obj) const
{
	return !(*this >= obj);
}

bool string::operator==(const char * obj) const
{
	const char *cur1 = data, *cur2 = obj;

	while (*cur1 && *cur2)
		if (*(cur1++) != *(cur2++))
			return false;

	return !(*cur1 || *cur2);
}

int string::inner_size() const
{
	int i;
	for (i = 0; data[i]; i++);
	return i;
}

int string::size() const
{
	return length;
}

string string::CheckName() const
{
	const char* cur = data, *cur2;
	int i = 0;
	for (; *cur == ' '; cur++);
	if (*cur >= '0' && *cur <= '9')
		throw std::logic_error("Not a Name");
	for (cur2 = cur; *cur2 >= 'a' && *cur2 <= 'z' || *cur2 >= 'A' && *cur2 <= 'Z' || *cur2 == '_' || *cur2 >= '0' && *cur2<='9'; i++, cur2++);
	for (; *cur2; cur2++)
		if (*cur2 != ' ')
			throw std::logic_error("Not a Name");
	if (i == 0)
		throw std::logic_error("Not a Name");
	return string(cur, i);
}

string string::number(int n)
{
	string result(100);
	sprintf_s(result.data, 100, "%d", n);
	result.length = result.inner_size();
	return result;
}

void string::refresh()
{
	length = inner_size();
}