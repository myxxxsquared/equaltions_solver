#include "stdafx.h"
#include "FunctionProvider.h"

namespace E{
	//enum Function_Type { None = 0, Exp, Ln };

	struct FunctionInfo
	{
		int FunctionNumber;
		mylib::string Name;
		int ArgumentNumber;
	};

	FunctionInfo FI[] = {
		{ 1, mylib::string("exp"), 1 },
		{ 2, mylib::string("ln"), 1 },
		{ 3, mylib::string("log"), 2 },
		{ 4, mylib::string("sin"), 1 },
		{ 5, mylib::string("cos"), 1 },
		{ 6, mylib::string("rand", 0) },
		{ -1 }
	};

	int FunctionProvider::GetFunctionNumber(const mylib::string & str)
	{
		FunctionInfo * f;
		for (f = FI; (*f).FunctionNumber != -1; f++)
			if ((*f).Name == str)
				return (*f).FunctionNumber;
		return -1;
	}

	mylib::string FunctionProvider::GetFunctionName(Function_Type t)
	{
		FunctionInfo *f;
		for (f = FI; (*f).FunctionNumber != -1; f++)
			if ((*f).FunctionNumber == t)
				return (*f).Name;
		throw std::invalid_argument("Invalid Function_Type");
	}

	int FunctionProvider::GetFunctionArgumentNumber(Function_Type t)
	{
		FunctionInfo *f;
		for (f = FI; (*f).FunctionNumber != -1; f++)
			if ((*f).FunctionNumber == t)
				return (*f).ArgumentNumber;
		throw std::invalid_argument("Invalid Function_Type");
	}

	void FunctionProvider::Calculate(mylib::stack<Value_Type>& s, Function_Type f)
	{
		Value_Type tmp1, tmp2;
		switch (f)
		{
		case FunctionProvider::None:
			break;
		case FunctionProvider::Exp:
			tmp1 = s.pop();
			tmp1 = exp(tmp1);
			s.push(tmp1);
			break;
		case FunctionProvider::Sin:
			tmp1 = s.pop();
			tmp1 = sin(tmp1);
			s.push(tmp1);
			break;
		case FunctionProvider::Cos:
			tmp1 = s.pop();
			tmp1 = cos(tmp1);
			s.push(tmp1);
			break;
		case FunctionProvider::Ln:
			tmp1 = s.pop();
			tmp1 = log(tmp1);
			s.push(tmp1);
			break;
		case FunctionProvider::Log:
			tmp1 = s.pop();
			tmp2 = s.pop();
			tmp1 = log(tmp1) / log(tmp2);
			s.push(tmp1);
			break;
		case FunctionProvider::Rand:
			s.push(rand() / 32768.0);
		default:
			throw std::invalid_argument("Invalid Function_Type");
		}
	}
}