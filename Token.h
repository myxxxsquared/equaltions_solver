#pragma once
namespace E{

	//结尾符号，常数，变量，运算符，左括号，右括号，到变量集映射，函数调用
	enum Token_Type { _None = 0, _Const, _Variable, _LeftBracket, _RightBracket, _VariableMap, _FunctionMap, _Operator };
	enum Operator_Type;

	class Token
	{
	public:
		Token();
		Token(Token_Type) throw(std::logic_error);
		Token(Value_Type);
		Token(Token_Type, const char*) throw(std::logic_error);
		Token(Token_Type, int) throw(std::logic_error);
		Token(Operator_Type);
		Token(const Token&);
		~Token();
		Token& operator=(const Token&);

		inline Value_Type GetValue() const throw(std::logic_error)
		{
			if (type == _Const)return val;
			else throw std::logic_error("Wrong Type");
		}

		inline Token_Type GetType() const throw(std::logic_error)
		{
			return type;
		}

		inline const char* GetName() const throw(std::logic_error)
		{
			if (type == Token_Type::_Variable) return data;
			else throw std::logic_error("Wrong Type");
		}

		inline Operator_Type GetOperatorType() const throw(std::logic_error)
		{
			if (type == Token_Type::_Operator) return ot;
			else throw std::logic_error("Wrong Type");
		}

		inline int GetMapNumber() const
		{
			if (type == Token_Type::_VariableMap || type == Token_Type::_FunctionMap) return mapnumber;
			else throw std::logic_error("Wrong Type");
		}

		mylib::string ToString();

	private:
		Token_Type type;
		union { char* data; int mapnumber; Operator_Type ot; Value_Type val; };
		int* ref;
	};
}