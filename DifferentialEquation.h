#pragma once

#include "Expression.h"

namespace E{

	class VariableData;
	class VariableRecord;

	class Differential
	{
	public:
		inline Differential(mylib::string Variable) :var(Variable){}
		Differential(const Differential &) = default;
		~Differential() = default;
		Differential& operator=(const Differential&) = default;
		inline mylib::string GetVariable() { return var; }

		inline mylib::string ToString()
		{
			return mylib::string("D(") + var + mylib::string(")");
		}

	private:
		mylib::string var;
	};

	typedef Differential D;

	class DifferentialEquation
	{
	public:
		inline DifferentialEquation(const Differential& left, const Expression & right)
			:Left(left), Right(right) {}
		~DifferentialEquation() = default;
		DifferentialEquation(const DifferentialEquation &) = default;
		DifferentialEquation& operator=(const DifferentialEquation &) = default;

		mylib::string ToString()
		{
			return Left.ToString() + mylib::string("=") + Right.ToString();
		}

		Differential Left;
		Expression Right;
	};

	class NormalEquation
	{
	public:
		inline NormalEquation(const mylib::string& left, const Expression & right)
			:Left(left), Right(right) {}
		~NormalEquation() = default;
		NormalEquation(const NormalEquation &) = default;
		NormalEquation& operator=(const NormalEquation &) = default;

		mylib::string ToString()
		{
			return Left + mylib::string("=") + Right.ToString();
		}

		mylib::string Left;
		Expression Right;
	};

	inline DifferentialEquation operator==(const Differential & left, const Expression & right)
	{
		return DifferentialEquation(left, right);
	}

	inline NormalEquation operator==(const mylib::string left, const Expression& right)
	{
		return NormalEquation(left, right);
	}

	class DifferentialEquationSet
	{
	public:
		DifferentialEquationSet() = default;
		DifferentialEquationSet(const DifferentialEquationSet &) = default;
		~DifferentialEquationSet() = default;
		DifferentialEquationSet& operator=(const DifferentialEquationSet &) = default;

		inline void AddDifferential(const DifferentialEquation & e) { diff.push_back(e); }
		inline void AddNormal(const NormalEquation & e) { norm.push_back(e); }
		inline void RemoveDifferentialAt(int i) { diff.erase(i); }
		inline void RemoveNormalAt(int i) { norm.erase(i); }
		inline void Clear() { diff.clear(); norm.clear(); }
		inline DifferentialEquation& GetDifferentialEquation(int i) { return diff[i]; }
		inline NormalEquation& GetNormalEquation(int i) { return norm[i]; }
		inline int CountDifferentialEqualtion()const { return diff.size(); }
		inline int CountNormalEqualtion() const { return norm.size(); }

		int CalcFileLength();
		int WriteFile(HANDLE hFile, VariableSet &v)throw(std::runtime_error, std::logic_error);
		static DifferentialEquationSet ReadFile(HANDLE hFile, VariableSet & v, int * Length)throw(std::runtime_error, std::logic_error);

		friend class DifferentialEquationSetSolver;
	private:
		mylib::vector<DifferentialEquation> diff;
		mylib::vector<NormalEquation> norm;
	};

	struct DifferentialEquationSolverInfo
	{
		Value_Type delta;
		Value_Type setdelta;
	};

	struct SolverStatus
	{
		DifferentialEquationSolverInfo info;
		VariableData* data;
		DifferentialEquationSet * set;

		mylib::vector_var<int> * lpdiffvar;
		mylib::vector<MapExpression> * lpdiffexpr;
		mylib::vector_var<int> * lpnormvar;
		mylib::vector<MapExpression> * lpnormexpr;

		volatile int* satuscallback;

		Value_Type destnation;

		int numberofvar;
		Value_Type delta;
		VariableRecord *K, *TMP;
		int diffvars;
		int normvars;

		volatile bool stoping;
	};


	class GlobalSolver
	{
	public:
		inline GlobalSolver() { bRunning = -1; SetGlobalSolver(this); InitSolver(); }
		~GlobalSolver();

		GlobalSolver(const GlobalSolver&) = delete;
		GlobalSolver & operator=(const GlobalSolver&) = delete;

		void InitSolver();
		void StartSolve(DifferentialEquationSet &, VariableData &, DifferentialEquationSolverInfo, Value_Type, int* satuscallback = nullptr);
		void Solve(DifferentialEquationSet &, VariableData &, DifferentialEquationSolverInfo, Value_Type);
		void StopSolve();
		//0位空闲，1位正忙，-1表示未初始化
		inline int GetStatus() { return bRunning; }

		static GlobalSolver* GetGlobalSolver() { return GS; }
	private:
		static GlobalSolver *GS;
		static void SetGlobalSolver(GlobalSolver * gs) { GS = gs; }

		void MainThread();
		void MainThreadProc();
		static DWORD WINAPI ThreadProc(LPVOID);

		void DirectCalc(VariableRecord & lst, VariableRecord & nxt, mylib::stack<Value_Type> &);

		volatile int bRunning;
		SolverStatus SS;
		HANDLE hThread;
		volatile BOOL hRunningEvent;
	};

}