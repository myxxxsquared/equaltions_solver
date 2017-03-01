#include "stdafx.h"

#include "Token.h"
#include "Expression.h"
#include "VariableRecord.h"
#include "OperatorProvider.h"
#include "FunctionProvider.h"

namespace E{

	Expression::Expression(const Expression & obj)
	{
		ref = obj.ref;
		length = obj.length;
		data = obj.data;

		(*ref)++;
	}

	Expression::Expression(const char * str)
	{
		struct s_part
		{
			mylib::string str;
			enum Type { __none = 0, __oper, __num, __name, __lb, __rb } type;

			s_part() {}
			s_part(mylib::string s, s_part::Type t) :str(s), type(t) {}
		};

		mylib::vector<s_part> parts;

		while (*str)
		{
			const char* start = str;
			if (str[0] == ' ' || str[0] == '\t' || str[0] == '\n' || str[0] == ',')
				str++;

			if (str[0] == '*' || str[0] == '/' || str[0] == '+' || str[0] == '-' || str[0] == '^')
			{
				parts.push_back(s_part(mylib::string(str, 1), s_part::__oper));
				str++;
			}

			if (str[0] >= '0' && str[0] <= '9' || str[0] == '.')
			{
				int i = 1;
				for (; (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == 'e'; i++);
				parts.push_back(s_part(mylib::string(str, i), s_part::__num));
				str += i;
			}

			if (str[0] >= 'A' && str[0] <= 'Z' || str[0] >= 'a' && str[0] <= 'z' || str[0] == '_')
			{
				int i = 1;
				for (; str[i] >= 'A' && str[i] <= 'Z' || str[i] >= 'a' && str[i] <= 'z' || str[i] == '_' || str[i] >= '0' && str[i] <= '9'; i++);
				parts.push_back(s_part(mylib::string(str, i), s_part::__name));
				str += i;
			}

			if (str[0] == '(')
			{
				parts.push_back(s_part(mylib::string(), s_part::__lb));
				str++;
			}

			if (str[0] == ')')
			{
				parts.push_back(s_part(mylib::string(), s_part::__rb));
				str++;
			}
			if (str == start)
				throw std::logic_error("Can't read from the string.");
		}

		mylib::vector<Token> tokens;
		int k;

		for (int i = 0; i < parts.size(); i++)
		{
			switch (parts[i].type)
			{
			case s_part::__num:
				tokens.push_back(Token(parts[i].str.ToValueType()));
				break;
			case s_part::__lb:
				tokens.push_back(Token(Token_Type::_LeftBracket));
				break;
			case s_part::__rb:
				tokens.push_back(Token(Token_Type::_RightBracket));
				break;
			case s_part::__name:
				k = FunctionProvider::GetFunctionNumber(parts[i].str);
				if (k == -1)
					tokens.push_back(Token(Token_Type::_Variable, parts[i].str));
				else
					tokens.push_back(Token(Token_Type::_FunctionMap, k));
				break;
			case s_part::__oper:
				if (i == 0)
				{
					if (parts[i].str == "+")
						break;
					else if (parts[i].str == "-")
						tokens.push_back(Token(Operator_Type::_Negative));
					else
						throw "Can't read from the string.";
				}
				else if (i == parts.size() - 1)
					throw std::logic_error("Reading Failure.");
				else
				{
					if (parts[i - 1].type == s_part::__name || parts[i - 1].type == s_part::__num || parts[i - 1].type == s_part::__rb)
					{
						if (parts[i].str == "+")
							tokens.push_back(Token(Operator_Type::_Add));
						else if (parts[i].str == "-")
							tokens.push_back(Token(Operator_Type::_Substract));
						else if (parts[i].str == "*")
							tokens.push_back(Token(Operator_Type::_Multiply));
						else if (parts[i].str == "/")
							tokens.push_back(Token(Operator_Type::_Divide));
						else if (parts[i].str == "^")
							tokens.push_back(Token(Operator_Type::_Power));
						else
							throw std::logic_error("Reading Failure.");
					}
					else
					{
						if (parts[i].str == "+")
							break;
						else if (parts[i].str == "-")
							tokens.push_back(Token(Operator_Type::_Negative));
						else
							throw std::logic_error("Reading Failure.");
					}
				}

			}
		}

		mylib::vector_var<Operator_Type> s;		//作为栈使用
		mylib::vector<Token> fin;				//最终生成
		Operator_Type optmp;
		for (int i = 0; i < tokens.size(); i++)
		{
			switch (tokens[i].GetType())
			{
			case Token_Type::_Const:
			case Token_Type::_Variable:
				fin.push_back(tokens[i]);
				while (s.size() > 0)
				{
					optmp = s.pop_back();
					if (optmp == Operator_Type::__LeftBracket)
					{
						s.push_back((optmp));
						break;
					}

					if ((int)(optmp) < 0)
					{
						optmp = (Operator_Type)(optmp & ~0x80000000);
						fin.push_back(Token(Token_Type::_FunctionMap, optmp));
						continue;
					}

					OperatorInfoEx oi2 = OperatorProvider::GetOperatorInfoEx(optmp);
					if (oi2.Type != OperatorInfoEx::Single_Left)
					{
						s.push_back(optmp);
						break;
					}
					fin.push_back(Token(optmp));
				}
				break;
			case Token_Type::_LeftBracket:
				s.push_back(Operator_Type::__LeftBracket);
				break;
			case Token_Type::_RightBracket:
#pragma warning(push)
#pragma warning (disable: 4127)
				while (true)
#pragma warning (pop)
				{
					try
					{
						optmp = s.pop_back();
						if (optmp == Operator_Type::__LeftBracket)
							break;
						fin.push_back(Token(optmp));
					}
					catch (std::logic_error)
					{
						throw std::logic_error("Reading Failure.");
					}
				}
				if (s.size() == 0)
					break;
				optmp = s.pop_back();
				if ((int)(optmp) < 0)
				{
					optmp = (Operator_Type)(optmp & ~0x80000000);
					fin.push_back(Token(Token_Type::_FunctionMap, optmp));
				}
				else
				{
					s.push_back(optmp);
				}
				break;
			case Token_Type::_FunctionMap:
				s.push_back((Operator_Type)(tokens[i].GetMapNumber() | 0x80000000));
				break;
			case Token_Type::_Operator:
				OperatorInfoEx oi = OperatorProvider::GetOperatorInfoEx(tokens[i].GetOperatorType());
				switch (oi.Type)
				{
				case OperatorInfoEx::Single_Right:
					fin.push_back(tokens[i]);
					break;
				case OperatorInfoEx::Single_Left:
					s.push_back(tokens[i].GetOperatorType());
					break;
				case OperatorInfoEx::Double:
					while (s.size() > 0)
					{
						optmp = s.pop_back();
						if (optmp == Operator_Type::__LeftBracket)
						{
							s.push_back(optmp);
							break;
						}

						OperatorInfoEx oi2 = OperatorProvider::GetOperatorInfoEx(optmp);
						if (oi2.Priority < oi.Priority)
						{
							s.push_back(optmp);
							break;
						}

						if (oi2.Priority == oi.Priority)
						{
							if (oi2.order == OperatorInfoEx::RightToLeft)
							{
								s.push_back(optmp);
								break;
							}
						}

						fin.push_back(Token(optmp));
					}
					s.push_back(tokens[i].GetOperatorType());
					break;
				}
				break;
			}
		}

		while (s.size() > 0)
		{
			fin.push_back(Token(s.pop_back()));
		}

		int count = 0;
		OperatorInfoEx oi;
		for (int i = 0; i < fin.size(); i++)
		{
			switch (fin[i].GetType())
			{
			case Token_Type::_Const:
			case Token_Type::_Variable:
				count++;
				break;
			case Token_Type::_FunctionMap:
				count -= FunctionProvider::GetFunctionArgumentNumber((FunctionProvider::Function_Type)fin[i].GetMapNumber());
				if (count < 0)
					throw std::logic_error("Reading Failure.");
				count++;
				break;
			case Token_Type::_Operator:
				oi = OperatorProvider::GetOperatorInfoEx(fin[i].GetOperatorType());
				switch (oi.Type)
				{
				case OperatorInfoEx::Double:
					if (count <= 1)
						throw std::logic_error("Reading Failure.");
					count--;
					break;
				case OperatorInfo::Single_Left:
				case OperatorInfoEx::Single_Right:
					if (count <= 0)
						throw std::logic_error("Reading Failure.");
				}
				break;
			case Token_Type::_None:
			case Token_Type::_LeftBracket:
			case Token_Type::_RightBracket:
			case Token_Type::_VariableMap:
			default:
				throw std::runtime_error("Someting is worong with the code!");
			}
		}

		if (count != 1)
		{
			throw std::logic_error("Reading Failure.");
		}

		length = fin.size();
		ref = new int(1);
		data = new Token[fin.size()];
		for (int i = 0; i < fin.size(); i++)
		{
			data[i] = fin[i];
		}
	}

	Expression::~Expression()
	{
		if (ref == 0)
			return;
		(*ref)--;
		if (*ref <= 0)
		{
			delete ref;
			delete[] data;
		}
	}

	Expression & Expression::operator=(const Expression & obj)
	{
		if (this == &obj)
			return *this;

		(*ref)--;
		if (*ref <= 0)
		{
			delete ref;
			delete[] data;
		}

		ref = obj.ref;
		length = obj.length;
		data = obj.data;

		(*ref)++;

		return *this;
	}

	Expression Expression::SingleOperator(Operator_Type o) const
	{
		Expression r;
		r.data = new Token[r.length = length + 1];
		r.ref = new int(1);
		for (int i = 0; i < length; i++)
		{
			r.data[i] = data[i];
		}
		r.data[length] = Token(o);
		return r;
	}

	Expression Expression::DoubleOperator(const Expression & obj, Operator_Type o) const
	{
		Expression r;
		r.data = new Token[r.length = length + obj.length + 1];
		r.ref = new int(1);
		for (int i = 0; i < length; i++)
		{
			r.data[i] = data[i];
		}
		for (int i = length; i < length + obj.length; i++)
		{
			r.data[i] = obj.data[i - length];
		}
		r.data[length + obj.length] = Token(o);
		return r;
	}

	Expression Expression::operator+() const
	{
		return Expression(*this);
	}

	Expression Expression::operator-() const
	{
		return SingleOperator(_Negative);
	}

	Expression Expression::operator+(const Expression & obj) const
	{
		return DoubleOperator(obj, Operator_Type::_Add);
	}

	Expression Expression::operator-(const Expression & obj) const
	{
		return DoubleOperator(obj, Operator_Type::_Substract);
	}

	Expression Expression::operator*(const Expression & obj) const
	{
		return DoubleOperator(obj, Operator_Type::_Multiply);
	}

	Expression Expression::operator/(const Expression & obj) const
	{
		return DoubleOperator(obj, Operator_Type::_Divide);
	}

	Expression Expression::operator^(const Expression & obj) const
	{
		return DoubleOperator(obj, Operator_Type::_Power);
	}


	int Expression::WriteFile(HANDLE hFile, VariableSet & v)
	{
		BOOL bResult;
		DWORD written, tmp;
		bResult = ::WriteFile(hFile, &length, sizeof(int), &written, NULL);
		DWORD buffer[3];
		if (!bResult || written != sizeof(int))
			throw std::runtime_error("Writing Failure.");

		for (int i = 0; i < length; i++)
		{
			buffer[0] = (int)(data[i].GetType());
			buffer[1] = buffer[2] = 0;
			switch (data[i].GetType())
			{
			case Token_Type::_None:
			case Token_Type::_LeftBracket:
			case Token_Type::_RightBracket:
				break;
			case Token_Type::_Const:
				*((Value_Type*)(buffer + 1)) = data[i].GetValue();
				break;
			case Token_Type::_FunctionMap:
			case Token_Type::_VariableMap:
				*((int*)(buffer + 1)) = data[i].GetMapNumber();
				break;
			case Token_Type::_Operator:
				*((int*)(buffer + 1)) = (int)data[i].GetOperatorType();
				break;
			case Token_Type::_Variable:
				if ((*((int*)(buffer + 1)) = v.GetNameID(data[i].GetName())) == -1)
				{
					if (HFILE_ERROR == SetFilePointer(hFile, -(long)written, NULL, FILE_CURRENT))
						throw std::runtime_error("Writing failure and can't go back to default position.");
					throw std::logic_error("At least one variable can't be found in VariableSet");
				}
				break;
			default:
				if (HFILE_ERROR == SetFilePointer(hFile, -(long)written, NULL, FILE_CURRENT))
					throw std::runtime_error("Writing failure and can't go back to default position.");
				throw std::logic_error("Invalid Token Type");
			}
			bResult = ::WriteFile(hFile, &buffer, 12, &tmp, NULL);
			written += tmp;
			if (!bResult || tmp != 12)
				throw std::runtime_error("Writing Failure.");
		}
		if ((long)written != CalcFileLength())
			throw std::runtime_error("runtime check failure. written size error.");
		return written;
	}

	Expression Expression::ReadFile(HANDLE hFile, VariableSet & var, int* Length)
	{
		BOOL bResult;
		Expression r;
		DWORD dwRead, dwTmp;

		bResult = ::ReadFile(hFile, &r.length, sizeof(int), &dwRead, NULL);

		if (!bResult || dwRead != sizeof(int))
			throw std::runtime_error("Reading Failure.");

		r.data = new Token[r.length];
		r.ref = new int;
		*r.ref = 1;

		DWORD buffer[3];

		for (int i = 0; i < r.length; i++)
		{
			bResult = ::ReadFile(hFile, buffer, 12, &dwTmp, NULL);
			dwRead += dwTmp;
			if (!bResult || dwTmp != 12)
				throw std::runtime_error("Reading Failure.");
			switch ((Token_Type)(buffer[0]))
			{
			case Token_Type::_None:
			case Token_Type::_LeftBracket:
			case Token_Type::_RightBracket:
				if (buffer[1] || buffer[2])
				{
					if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
						throw std::runtime_error("Reading failure and can't go back to default position.");
					throw std::logic_error("Invalid Format");
				}
				r.data[i] = Token((Token_Type)(buffer[0]));
				break;
			case Token_Type::_Const:
				r.data[i] = Token(*((Value_Type*)(buffer + 1)));
				break;
			case Token_Type::_FunctionMap:
				r.data[i] = Token(Token_Type::_FunctionMap, *((int*)(buffer + 1)));
				break;
			case Token_Type::_Operator:
				r.data[i] = Token(*((Operator_Type*)(buffer + 1)));
				break;
			case Token_Type::_VariableMap:
				r.data[i] = Token(Token_Type::_VariableMap, *((int*)(buffer + 1)));
				break;
			case Token_Type::_Variable:
				try
				{
					mylib::string s = var.GetVarName(*((int*)(buffer + 1)));
					r.data[i] = Token(Token_Type::_Variable, s);
				}
				catch (std::invalid_argument)
				{
					if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
						throw std::runtime_error("Reading failure and can't go back to default position.");
					throw std::logic_error("A variable can't be read.");
				}
				break;
			default:
				throw std::logic_error("Invalid Format");
			}
		}

		if (Length)
			*Length = sizeof(int) + 12 * r.length;
		return r;
	}

	mylib::string Expression::ToString()
	{
		struct partinfo
		{
			int p;
			mylib::string str;
		};

		mylib::vector<partinfo> s;
		partinfo p, p2;

		for (int i = 0; i < length; i++)
		{
			switch (data[i].GetType())
			{
			case Token_Type::_Const:
			case Token_Type::_Variable:
			case Token_Type::_VariableMap:
				p.p = 0x7FFFFFFF;
				p.str = data[i].ToString();
				s.push_back(p);
				break;
			case Token_Type::_FunctionMap:
				p = s.pop_back();
				p.str = FunctionProvider::GetFunctionName((FunctionProvider::Function_Type)data[i].GetMapNumber()) + mylib::string("(") + p.str + mylib::string(")");
				s.push_back(p);
				break;
			case Token_Type::_Operator:
				OperatorInfoEx oi = OperatorProvider::GetOperatorInfoEx(data[i].GetOperatorType());
				switch (oi.Type)
				{
				case OperatorInfoEx::Single_Left:
					p = s.pop_back();
					p.str = mylib::string(oi.Name) + (p.p < 0x7FFFFFFF ? mylib::string("(") + p.str + mylib::string(")") : p.str);
					s.push_back(p);
					break;
				case OperatorInfoEx::Single_Right:
					p = s.pop_back();
					p.str = (p.p < 0x7FFFFFFF ? mylib::string("(") + p.str + mylib::string(")") : p.str) + mylib::string(oi.Name);
					s.push_back(p);
					break;
				case OperatorInfoEx::Double:
					p = s.pop_back();
					p2 = s.pop_back();
					if (p.p < oi.Priority || p.p == oi.Priority && oi.order == OperatorInfoEx::LeftToRight)
						p.str = mylib::string("(") + p.str + mylib::string(")");
					if (p2.p < oi.Priority || p2.p == oi.Priority && oi.order == OperatorInfoEx::RightToLeft)
						p2.str = mylib::string("(") + p2.str + mylib::string(")");
					p.str = p2.str + mylib::string(oi.Name) + p.str;
					p.p = oi.Priority;
					s.push_back(p);
					break;
				}

			}
		}

		p = s.pop_back();

		if (s.size())
			throw std::runtime_error("runtime check failure.");

		return p.str;
	}

	Expression Number(double d)
	{
		Expression r;
		r.ref = new int(1);
		r.data = new Token[1];
		r.data[0] = Token((Value_Type)d);
		r.length = 1;
		return r;
	}

	Expression Exp(Expression expr)
	{
		Expression r;
		r.ref = new int(1);
		r.data = new Token[r.length = expr.length + 1];

		for (int i = 0; i < expr.length; i++)
			r.data[i] = expr.data[i];

		r.data[expr.length] = Token(Token_Type::_FunctionMap, FunctionProvider::Exp);
		return r;
	}

	Expression SingleFunction(Expression expr, int t)
	{
		Expression r;
		r.ref = new int(1);
		r.data = new Token[r.length = expr.length + 1];

		for (int i = 0; i < expr.length; i++)
			r.data[i] = expr.data[i];

		r.data[expr.length] = Token(Token_Type::_FunctionMap, t);
		return r;
	}

	Expression Ln(Expression expr)
	{
		Expression r;
		r.ref = new int(1);
		r.data = new Token[r.length = expr.length + 1];

		for (int i = 0; i < expr.length; i++)
			r.data[i] = expr.data[i];

		r.data[expr.length] = Token(Token_Type::_FunctionMap, FunctionProvider::Ln);
		return r;
	}

	Expression Variable(const char* ch)
	{
		Expression r;
		r.ref = new int(1);
		r.data = new Token[1];
		r.data[0] = Token(Token_Type::_Variable, ch);
		r.length = 1;
		return r;
	}
	/*
	int ExpressionSet::CalcFileLength()
	{
	int result = 16;
	for (int i = 0; i < Data.size(); i++)
	{
	result += Data[i].CalcFileLength();
	}
	return result;
	}

	int ExpressionSet::WriteFile(HANDLE hFile, VariableSet & var)
	{
	int totallen = CalcFileLength();
	int buffer[4] = { _EXPRESSION_SIGNATURE, _EXPRESSION_VERSION, totallen, var.GetVarNames().size() };
	BOOL bResult;
	DWORD dwWrite;
	bResult = ::WriteFile(hFile, buffer, 16, &dwWrite, NULL);
	if (!bResult || dwWrite != 16)
	throw std::runtime_error("Writing Failure.");

	for (int i = 0; i < Data.size(); i++)
	{
	try
	{
	dwWrite += Data[i].WriteFile(hFile, var);
	}
	catch (std::logic_error)
	{
	if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwWrite, NULL, FILE_CURRENT))
	throw std::runtime_error("Reading failure and can't go back to default position.");
	throw;
	}
	}

	if (totallen != (long)dwWrite)
	throw std::runtime_error("runtime check failure. written size error.");

	return totallen;
	}

	ExpressionSet ExpressionSet::ReadFile(HANDLE hFile, VariableSet & v, int * Length)
	{
	ExpressionSet r;

	int buffer[4];
	BOOL bResult;
	DWORD dwRead;
	bResult = ::ReadFile(hFile, buffer, 16, &dwRead, NULL);
	if (!bResult || dwRead != 16)
	throw std::runtime_error("Reading Failure.");

	if (buffer[0] != _EXPRESSION_SIGNATURE)
	{
	if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
	throw std::runtime_error("Reading failure and can't go back to default position.");
	throw std::logic_error("Invalid Signature.");
	}

	if (buffer[1] != _EXPRESSION_VERSION)
	{
	if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
	throw std::runtime_error("Reading failure and can't go back to default position.");
	throw std::logic_error("Invalid Version.");
	}

	for (int i = 0; i < buffer[3]; i++)
	{
	try
	{
	int len;
	Expression e = Expression::ReadFile(hFile, v, &len);
	dwRead += len;
	r.Data.push_back(e);
	}
	catch(std::logic_error)
	{
	if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
	throw std::runtime_error("Reading failure and can't go back to default position.");
	throw;
	}
	}

	if ((long)dwRead != buffer[2])
	{
	if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
	throw std::runtime_error("Reading failure and can't go back to default position.");
	throw std::logic_error("Invalid Format");
	}

	if (Length)
	*Length = dwRead;

	return r;
	}
	*/

}