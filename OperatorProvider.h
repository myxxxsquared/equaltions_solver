#pragma once
namespace E{

	enum Operator_Type { __None = 0, _Add, _Substract, _Multiply, _Divide, _Power, _Negative, __LeftBracket, __RightBracket };


	struct OperatorInfo
	{
		Operator_Type OperatorType;
		enum { None = 0, Single_Left, Single_Right, Double } Type;
	};

	struct OperatorInfoEx
	{
		Operator_Type OperatorType;
		int Priority;
		enum { LeftToRight, RightToLeft } order;
		enum { None = 0, Single_Left, Single_Right, Double } Type;
		char Name[4];
		int NameLength;
	};

	class OperatorProvider
	{
	public:
		OperatorProvider() = delete;
		OperatorProvider(const OperatorProvider&) = delete;

		static OperatorInfo GetOperatorInfo(Operator_Type);
		static OperatorInfoEx GetOperatorInfoEx(Operator_Type);
		static Value_Type SingleCalculate(Operator_Type, Value_Type)throw(std::invalid_argument);
		static Value_Type DoubleCalculate(Operator_Type, Value_Type, Value_Type)throw(std::invalid_argument);
	};

}