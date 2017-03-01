#include "stdafx.h"
#include "FunctionProvider.h"
#include "OperatorProvider.h"
#include "Token.h"
namespace E{

	Token::Token()
	{
		memset(this, 0, sizeof(Token));
	}

	Token::Token(Token_Type t)
	{
		if (t != Token_Type::_None && t != Token_Type::_LeftBracket && t != Token_Type::_RightBracket)
			throw std::logic_error("Wrong Type");
		type = (t), data = (nullptr), ref = nullptr;
	}

	Token::Token(Value_Type v)
		:type(Token_Type::_Const), ref(nullptr), val(v) {}

	Token::Token(Token_Type t, const char* ch)
	{
		if (t == Token_Type::_Variable)
		{
			type = t;
			data = new char[strlen(ch) + 1];
			memcpy(data, ch, strlen(ch) + 1);
			ref = new int(1);
		}
		else
			throw std::logic_error("Wrong Type");
	}

	Token::Token(Token_Type t, int mapnumber)
	{
		if (t == Token_Type::_VariableMap || t == Token_Type::_FunctionMap)
			this->mapnumber = mapnumber, type = t, ref = nullptr;
		else
			throw std::logic_error("Wrong Type");
	}

	Token::Token(Operator_Type o)
		:type(Token_Type::_Operator), ref(nullptr), ot(o) {}

	Token::Token(const Token & obj)
	{
		memcpy(this, &obj, sizeof(Token));

		if (obj.ref != nullptr)
		{
			(*ref)++;
		}
	}

	Token::~Token()
	{
		if (ref != nullptr)
		{
			(*ref)--;
			if (*ref <= 0)
			{
				delete ref;
				delete[] data;
				ref = nullptr;
				data = nullptr;
			}
		}
	}

	Token & Token::operator=(const Token & obj)
	{
		if (this == &obj)
			return *this;

		if (ref != nullptr)
		{
			(*ref)--;
			if (*ref <= 0)
			{
				delete ref;
				delete[] data;
				ref = nullptr;
				data = nullptr;
			}
		}

		memcpy(this, &obj, sizeof(Token));

		if (obj.ref != nullptr)
		{
			(*ref)++;
		}
		return *this;
	}

	mylib::string Token::ToString()
	{
		switch (type)
		{
		case Token_Type::_None:
			return mylib::string();
		case Token_Type::_Const:
			return mylib::string(this->val);
		case Token_Type::_FunctionMap:
			return FunctionProvider::GetFunctionName((FunctionProvider::Function_Type)this->mapnumber);
		case Token_Type::_LeftBracket:
			return mylib::string("(");
		case Token_Type::_RightBracket:
			return mylib::string(")");
		case Token_Type::_Variable:
			return mylib::string(data);
		case Token_Type::_Operator:
			return mylib::string(OperatorProvider::GetOperatorInfoEx(this->ot).Name);
		case Token_Type::_VariableMap:
			return mylib::string("VariableMap_") + mylib::string((Value_Type)mapnumber);
		}
		throw std::logic_error("Invalid Token_Type");
	}

}