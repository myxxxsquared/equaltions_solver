#include "stdafx.h"

#include "Token.h"
#include "FunctionProvider.h"
#include "OperatorProvider.h"
#include "MapExpression.h"

namespace E{

	MapExpression::MapExpression(const Expression & e, const VariableSet * v)
	{
		expr.length = e.length;
		expr.data = new Token[expr.length];
		expr.ref = new int(1);

		for (int i = 0; i < expr.length; i++)
			if (e.data[i].GetType() == Token_Type::_Variable)
			{
				if (!v)
					throw std::logic_error("A variable can't be found in VariableSet");

				int j = v->GetNameID(mylib::string(e.data[i].GetName()));
				if (j == -1)
				{
					delete[] expr.data;
					throw std::logic_error("A variable can't be found in VariableSet");
				}
				if (j > v->GetVarNames().size())
					j = v->GetVarNames().size() - j;
				expr.data[i] = Token(Token_Type::_VariableMap, j);
			}
			else
			{
				expr.data[i] = e.data[i];
			}
	}

	Value_Type MapExpression::Calculate(VariableRecord * r, VariableRecord * con) const
	{
		mylib::stack<Value_Type> s;
		Value_Type tmp1, tmp2;
		OperatorInfo OI;
		for (int i = 0; i < expr.length; i++)
		{
			switch (expr.data[i].GetType())
			{
			case Token_Type::_Const:
				s.push(expr.data[i].GetValue());
				break;
			case Token_Type::_VariableMap:
				if (!r)
					throw std::logic_error("A variable can't be found in VariableSet");
				if (expr.data[i].GetMapNumber() < 0)
					s.push(con->GetData()[-1 - expr.data[i].GetMapNumber()]);
				else
					s.push(r->GetData()[expr.data[i].GetMapNumber()]);
				break;
			case Token_Type::_Operator:
				OI = OperatorProvider::GetOperatorInfo(expr.data[i].GetOperatorType());
				if (OI.Type == OperatorInfo::Single_Left || OI.Type == OperatorInfo::Single_Right)
				{
					tmp1 = s.pop();
					tmp1 = OperatorProvider::SingleCalculate(OI.OperatorType, tmp1);
					s.push(tmp1);
				}
				else if (OI.Type == OperatorInfo::Double)
				{
					tmp1 = s.pop();
					tmp2 = s.pop();

					tmp1 = OperatorProvider::DoubleCalculate(OI.OperatorType, tmp2, tmp1);

					s.push(tmp1);
				}
				else
				{
					throw std::logic_error("Invalid Operator_Type");
				}
				break;
			case Token_Type::_FunctionMap:
				FunctionProvider::Calculate(s, (FunctionProvider::Function_Type)expr.data[i].GetMapNumber());
				break;
			default:
				throw std::logic_error("Invalid Token_Type");
			}
		}
		tmp1 = s.pop();
		if (!s.empty())
			throw std::logic_error("Invalid Expression");

		return tmp1;
	}

	Value_Type MapExpression::Calculate(VariableRecord * r, VariableRecord * con, mylib::stack<Value_Type>& s) const throw(std::logic_error)
	{
		s.clear();
		Value_Type tmp1, tmp2;
		OperatorInfo OI;
		for (int i = 0; i < expr.length; i++)
		{
			switch (expr.data[i].GetType())
			{
			case Token_Type::_Const:
				s.push(expr.data[i].GetValue());
				break;
			case Token_Type::_VariableMap:
				if (expr.data[i].GetMapNumber() < 0)
					s.push(con->GetData()[-1 - expr.data[i].GetMapNumber()]);
				else
					s.push(r->GetData()[expr.data[i].GetMapNumber()]);
				break;
			case Token_Type::_Operator:
				OI = OperatorProvider::GetOperatorInfo(expr.data[i].GetOperatorType());
				if (OI.Type == OperatorInfo::Single_Left || OI.Type == OperatorInfo::Single_Right)
				{
					tmp1 = s.pop();
					tmp1 = OperatorProvider::SingleCalculate(OI.OperatorType, tmp1);
					s.push(tmp1);
				}
				else if (OI.Type == OperatorInfo::Double)
				{
					tmp1 = s.pop();
					tmp2 = s.pop();

					tmp1 = OperatorProvider::DoubleCalculate(OI.OperatorType, tmp2, tmp1);

					s.push(tmp1);
				}
				else
				{
					throw std::logic_error("Invalid Operator_Type");
				}
				break;
			case Token_Type::_FunctionMap:
				FunctionProvider::Calculate(s, (FunctionProvider::Function_Type)expr.data[i].GetMapNumber());
				break;
			default:
				throw std::logic_error("Invalid Token_Type");
			}
		}
		tmp1 = s.pop();
		if (!s.empty())
			throw std::logic_error("Invalid Expression");

		s.clear();

		return tmp1;
	}

}