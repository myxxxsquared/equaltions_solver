#pragma once

namespace E{

	class FunctionProvider
	{
	public:

		enum Function_Type{ None = 0, Exp, Ln, Log, Sin, Cos, Rand };

		FunctionProvider() = delete;
		FunctionProvider(const FunctionProvider &) = delete;

		static int GetFunctionNumber(const mylib::string &);
		static mylib::string GetFunctionName(Function_Type);
		static int GetFunctionArgumentNumber(Function_Type);

		static void Calculate(mylib::stack<Value_Type> &, Function_Type)throw(std::invalid_argument);
	};
}