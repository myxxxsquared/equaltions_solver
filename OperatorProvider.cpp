#include "stdafx.h"

#include "OperatorProvider.h"

namespace E{
	//enum Operator_Type{ __None = 0, _Add, _Substract, _Multiply, _Divide, _Power, _Negative };

	const OperatorInfo OI[] = {
		{ Operator_Type::__None, /*0, OperatorInfo::LeftToRight,*/ OperatorInfo::None },
		{ Operator_Type::_Add, /*1, OperatorInfo::LeftToRight,*/ OperatorInfo::Double },
		{ Operator_Type::_Substract, /*1, OperatorInfo::LeftToRight,*/ OperatorInfo::Double },
		{ Operator_Type::_Multiply, /*2, OperatorInfo::LeftToRight,*/ OperatorInfo::Double },
		{ Operator_Type::_Divide, /*2, OperatorInfo::LeftToRight,*/ OperatorInfo::Double },
		{ Operator_Type::_Power, /*3, OperatorInfo::LeftToRight,*/ OperatorInfo::Double },
		{ Operator_Type::_Negative, /*INT_MAX, OperatorInfo::LeftToRight,*/ OperatorInfo::Single_Left }
	};

	const OperatorInfoEx OIEx[] =
	{
		{ Operator_Type::__None, 0, OperatorInfoEx::LeftToRight, OperatorInfoEx::None, "", 0 },
		{ Operator_Type::_Add, 1, OperatorInfoEx::LeftToRight, OperatorInfoEx::Double, "+", 1 },
		{ Operator_Type::_Substract, 1, OperatorInfoEx::LeftToRight, OperatorInfoEx::Double, "-", 1 },
		{ Operator_Type::_Multiply, 2, OperatorInfoEx::LeftToRight, OperatorInfoEx::Double, "*", 1 },
		{ Operator_Type::_Divide, 2, OperatorInfoEx::LeftToRight, OperatorInfoEx::Double, "/", 1 },
		{ Operator_Type::_Power, 3, OperatorInfoEx::LeftToRight, OperatorInfoEx::Double, "^", 1 },
		{ Operator_Type::_Negative, INT_MAX, OperatorInfoEx::LeftToRight, OperatorInfoEx::Single_Left, "-", 1 }
	};

	OperatorInfo OperatorProvider::GetOperatorInfo(Operator_Type ot)
	{
		return OI[(int)ot];
	}

	OperatorInfoEx OperatorProvider::GetOperatorInfoEx(Operator_Type ot)
	{
		return OIEx[(int)ot];
	}

	Value_Type OperatorProvider::SingleCalculate(Operator_Type ot, Value_Type val)
	{
		switch (ot)
		{
		case Operator_Type::_Negative:
			return -val;
		default:
			throw std::invalid_argument("Invalid Operator_Type");
		}
	}

	Value_Type OperatorProvider::DoubleCalculate(Operator_Type ot, Value_Type left, Value_Type right)
	{
		switch (ot)
		{
		case Operator_Type::_Add:
			return left + right;
		case Operator_Type::_Substract:
			return left - right;
		case Operator_Type::_Multiply:
			return left*right;
		case Operator_Type::_Divide:
			return left / right;
		case Operator_Type::_Power:
			return pow(left, right);
		default:
			throw std::invalid_argument("Invalid Operator_Type");
		}
	}

}