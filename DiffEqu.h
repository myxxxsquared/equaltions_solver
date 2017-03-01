#pragma once

#include "Variable_Basic.h"
#include "mylib.h"
#include "VariableData.h"
#include "DifferentialEquation.h"

#define CONTAIN_ALL					0xffffffff
#define CONTAIN_DATA				0x00000001
#define CONTAIN_EQUA				0x00000002

namespace E{

	class DifferentialProvider
	{
	public:
		DifferentialProvider();
		~DifferentialProvider();

		DifferentialProvider(const DifferentialProvider&);
		DifferentialProvider& operator=(const DifferentialProvider&) = delete;
		DifferentialProvider(DifferentialProvider&&) = delete;
		DifferentialProvider& operator=(DifferentialProvider&&) = delete;

		void SetTime(mylib::string, Value_Type val = 0);
		mylib::string GetTimeName(){ return Time; }
		double GetTime();

		void AddVar(mylib::string, Value_Type val = 0);
		int CountVar() const;
		mylib::string GetVarName(int) const;
		int GetVarIndex(mylib::string) const;
		Value_Type GetVar(int);
		void SetVar(int, Value_Type);
		void RemoveVar(mylib::string);
		void ClearVar();

		void AddCon(mylib::string, Value_Type val = 0);
		int CountCon() const;
		mylib::string GetConName(int) const;
		int GetConIndex(mylib::string) const;
		Value_Type GetCon(int) const;
		void SetCon(int, Value_Type);
		void RemoveCon(mylib::string);
		void ClearCon();

		Value_Type GetValue(mylib::string);
		void SetValue(mylib::string, Value_Type);

		void AddDiff(DifferentialEquation);
		void AddNorm(NormalEquation);
		DifferentialEquation& GetDiff(int);
		NormalEquation& GetNorm(int);
		void RemoveDiff(int);
		void RemoveNorm(int);
		int CountDiff()const{ return equs.CountDifferentialEqualtion(); }
		int CountNorm()const{ return equs.CountNormalEqualtion(); }

		void ClearEqus(){ equs.Clear(); }

		DifferentialEquationSolverInfo GetSolverInfo() const;
		void SetSolverInfo(DifferentialEquationSolverInfo);

		void StartSolveFor(Value_Type);
		void SolveFor(Value_Type);

		void StopSolve();

		void BeginNewData();
		int GetDataNumber();
		VariableData& GetData(int);
		inline VariableData* GetLastData(){ return curdata; }
		void RemoveData(int);
		void ClearData();
		inline int IsSovling(){ return issolving; }

		int WriteFile(const char* filename, int contain = CONTAIN_ALL);
		int WriteFile(HANDLE hFile, int contain = CONTAIN_ALL);
		int ReadFile(const char* filename, int contain = CONTAIN_ALL);
		int ReadFile(HANDLE hFile, int contain = CONTAIN_ALL);

		void TakeOut(double t);
	private:
		void TakeOut();
		void TakeIn();
		void PrepareToSolve();

		mylib::string Time;
		mylib::vector<mylib::string> VarSet;
		mylib::vector<mylib::string> ConSet;
		DifferentialEquationSet equs;

		VariableSet * set;
		Value_Type time;
		mylib::vector_var<Value_Type> initaldata;
		mylib::vector_var<Value_Type> constdata;
		mylib::vector_var<VariableData*> alldata;
		VariableData * curdata;

		DifferentialEquationSolverInfo info;

		int status;
		volatile int issolving;
	};

}