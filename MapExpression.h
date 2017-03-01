#pragma once
#include "Expression.h"
#include "VariableData.h"

namespace E{
	class MapExpression
	{
	public:
		MapExpression(const Expression &, const VariableSet *) throw(std::logic_error);
		MapExpression(const MapExpression &) = default;
		~MapExpression() = default;
		MapExpression& operator =(MapExpression &) = default;

		Value_Type Calculate(VariableRecord *, VariableRecord *)  const throw(std::logic_error);
		Value_Type Calculate(VariableRecord *, VariableRecord *, mylib::stack<Value_Type> &)  const throw(std::logic_error);

	private:
		Expression expr;
	};
}