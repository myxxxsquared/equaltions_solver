#pragma once

#include "FunctionProvider.h"

namespace E{

	class Token;
	enum Operator_Type;
	class VariableSet;

	class Expression
	{
	public:
		Expression(const Expression &);

		Expression(const char*) throw(std::logic_error);

		~Expression();
		Expression & operator=(const Expression &);

		Expression operator+() const;
		Expression operator-() const;
		Expression operator+(const Expression &) const;
		Expression operator-(const Expression &) const;
		Expression operator*(const Expression &) const;
		Expression operator/(const Expression &) const;
		Expression operator^(const Expression &) const;

		inline int CalcFileLength() { return sizeof(int) + length * 12; }
		int WriteFile(HANDLE hFile, VariableSet & v) throw(std::runtime_error, std::logic_error);
		static Expression ReadFile(HANDLE hFile, VariableSet & v, int * Length)throw(std::runtime_error, std::logic_error);

		mylib::string ToString();

		friend Expression Number(double);
		friend Expression Exp(Expression expr);
		friend Expression SingleFunction(Expression expr, int funtype);
		friend Expression Ln(Expression expr);
		friend Expression Variable(const char*);

		friend class MapExpression;

	private:
		inline Expression()
		{
			ref = 0;
		}
		Expression SingleOperator(Operator_Type) const;
		Expression DoubleOperator(const Expression &, Operator_Type) const;

		int length;
		Token* data;
		int* ref;
	};

	/*
	class ExpressionSet
	{
	public:
	mylib::vector<Expression> Data;
	ExpressionSet() = default;
	ExpressionSet(mylib::vector<Expression> & data)
	:Data(data) {}

	int CalcFileLength();
	int WriteFile(HANDLE hFile, VariableSet &v)throw(std::runtime_error, std::logic_error);
	static ExpressionSet ReadFile(HANDLE hFile, VariableSet & v, int * Length)throw(std::runtime_error, std::logic_error);
	};
	*/

	Expression Number(double);
	Expression Variable(const char*);
	Expression Exp(Expression expr);
	Expression SingleFunction(Expression expr, int);


}