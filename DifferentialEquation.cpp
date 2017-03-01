#include "stdafx.h"

#include "DifferentialEquation.h"
#include "MapExpression.h"

namespace E{

	class DifferentialEquationSetSolver
	{
	public:
		DifferentialEquationSetSolver(DifferentialEquationSet, VariableData *, DifferentialEquationSolverInfo) throw (std::logic_error);
		DifferentialEquationSetSolver(DifferentialEquationSetSolver&) = default;
		~DifferentialEquationSetSolver() = default;
		DifferentialEquationSetSolver& operator=(DifferentialEquationSetSolver &) = default;
		friend class GlobalSolver;
	private:
		DifferentialEquationSolverInfo info;

		mylib::vector_var<int> diffvar;
		mylib::vector<MapExpression> diffexpr;

		mylib::vector_var<int> normvar;
		mylib::vector<MapExpression> normexpr;

		VariableData* data;
	};

	int DifferentialEquationSet::CalcFileLength()
	{
		int result = 20;
		for (int i = 0; i < diff.size(); i++)
			result += diff[i].Right.CalcFileLength();

		for (int i = 0; i < norm.size(); i++)
			result += norm[i].Right.CalcFileLength();

		result += sizeof(int) * (diff.size() + norm.size());

		return result;
	}

	int DifferentialEquationSet::WriteFile(HANDLE hFile, VariableSet & v)
	{
		struct EqualtionSet_File
		{
			int Signature;
			int Version;
			int TotalSize;
			int diffNumber;
			int normNumber;
		} _struct = { _EQUALTIONS_SIGNATURE, _EQUALTIONS_VERSION, CalcFileLength(), diff.size(), norm.size() };

		BOOL bResult;
		DWORD dwWritten;
		DWORD dwTmp;
		int id;

		bResult = ::WriteFile(hFile, &_struct, 20, &dwWritten, NULL);
		if (!bResult || dwWritten != 20)
			throw std::runtime_error("Writing Failure.");

		for (int i = 0; i < _struct.diffNumber; i++)
		{
			if ((id = v.GetNameID(diff[i].Left.GetVariable())) == -1)
			{
				if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwWritten, NULL, FILE_CURRENT))
					throw std::runtime_error("Writing failure and can't go back to default position.");
				throw std::logic_error("A variable can't be found in VariableSet.");
			}
			bResult = ::WriteFile(hFile, &id, sizeof(int), &dwTmp, NULL);
			if (!bResult || dwTmp != sizeof(int))
				throw std::runtime_error("Writing Failure.");
			dwWritten += dwTmp;

			try
			{
				dwWritten += diff[i].Right.WriteFile(hFile, v);
			}
			catch (std::logic_error())
			{
				if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwWritten, NULL, FILE_CURRENT))
					throw std::runtime_error("Writing failure and can't go back to default position.");
				throw;
			}
		}

		for (int i = 0; i < _struct.normNumber; i++)
		{
			if ((id = v.GetNameID(norm[i].Left)) == -1)
			{
				if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwWritten, NULL, FILE_CURRENT))
					throw std::runtime_error("Writing failure and can't go back to default position.");
				throw std::logic_error("A variable can't be found in VariableSet.");
			}
			bResult = ::WriteFile(hFile, &id, sizeof(int), &dwTmp, NULL);
			if (!bResult || dwTmp != sizeof(int))
				throw std::runtime_error("Writing Failure.");
			dwWritten += dwTmp;

			try
			{
				dwWritten += norm[i].Right.WriteFile(hFile, v);
			}
			catch (std::logic_error())
			{
				if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwWritten, NULL, FILE_CURRENT))
					throw std::runtime_error("Writing failure and can't go back to default position.");
				throw;
			}
		}

		if (_struct.TotalSize != (long)dwWritten)
			throw std::runtime_error("runtime check failure. written size error.");

		return dwWritten;
	}

	DifferentialEquationSet DifferentialEquationSet::ReadFile(HANDLE hFile, VariableSet & v, int * Length) throw(std::runtime_error, std::logic_error)
	{
		struct EqualtionSet_File
		{
			int Signature;
			int Version;
			int TotalSize;
			int diffNumber;
			int normNumber;
		} _struct;

		BOOL bResult;
		DWORD dwRead, dwTmp;
		DifferentialEquationSet result;

		bResult = ::ReadFile(hFile, &_struct, 20, &dwRead, NULL);

		if (!bResult || dwRead != 20)
			throw std::runtime_error("Reading failure.");

		if (_struct.Signature != _EQUALTIONS_SIGNATURE)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Signature.");
		}

		if (_struct.Version != _EQUALTIONS_VERSION)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Version.");
		}

		int readvar;
		mylib::string s;

		for (int i = 0; i < _struct.diffNumber; i++)
		{
			bResult = ::ReadFile(hFile, &readvar, sizeof(int), &dwTmp, NULL);
			if (!bResult || dwTmp != sizeof(int))
				throw std::runtime_error("Reading Failure.");
			dwRead += dwTmp;
			try
			{
				s = v.GetVarNames()[readvar - 1];
			}
			catch (std::invalid_argument)
			{
				if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
					throw std::runtime_error("Reading failure and can't go back to default position.");
				throw std::logic_error("A variable can't be read.");
			}

			try
			{
				Expression expr = Expression::ReadFile(hFile, v, (int*)&dwTmp);
				result.AddDifferential(Differential(s) == expr);
				dwRead += dwTmp;
			}
			catch (std::logic_error)
			{
				if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
					throw std::runtime_error("Reading failure and can't go back to default position.");
				throw;
			}
		}

		for (int i = 0; i < _struct.normNumber; i++)
		{
			bResult = ::ReadFile(hFile, &readvar, sizeof(int), &dwTmp, NULL);
			if (!bResult || dwTmp != sizeof(int))
				throw std::runtime_error("Reading Failure.");
			dwRead += dwTmp;
			try
			{
				s = v.GetVarNames()[readvar - 1];
			}
			catch (std::invalid_argument)
			{
				if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
					throw std::runtime_error("Reading failure and can't go back to default position.");
				throw std::logic_error("A variable can't be read.");
			}

			try
			{
				Expression expr = Expression::ReadFile(hFile, v, (int*)&dwTmp);
				result.AddNormal(s == expr);
				dwRead += dwTmp;
			}
			catch (std::logic_error)
			{
				if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
					throw std::runtime_error("Reading failure and can't go back to default position.");
				throw;
			}
		}

		if ((long)dwRead != _struct.TotalSize)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Format");
		}

		if (Length)
			*Length = _struct.TotalSize;

		return result;
	}


	DifferentialEquationSetSolver::DifferentialEquationSetSolver(DifferentialEquationSet set, VariableData * var, DifferentialEquationSolverInfo info)
	{
		this->info = info;
		this->data = var;

		mylib::bitset b(var->GetVariableSet().GetTotalVar());
		b.setalltrue();

		b.set(0, false);

		const VariableSet& pv = data->GetVariableSet();

		for (int i = 0; i < set.diff.size(); i++)
		{
			int id = pv.GetNameID(set.diff[i].Left.GetVariable());
			if (id > pv.GetVarNames().size())
				throw std::logic_error("Const can't be the left variable");
			if (id == -1)
				throw std::logic_error("At least one variable can't be found in VariableSet");

			if (!b.get(id))
				throw std::logic_error("Two equaltions have the same variable");

			b.set(id, false);

			diffvar.push_back(id);
			diffexpr.push_back(MapExpression(set.diff[i].Right, &var->GetVariableSet()));
		}

		for (int i = 0; i < set.norm.size(); i++)
		{
			int id = pv.GetNameID(set.norm[i].Left);
			if (id > pv.GetVarNames().size())
				throw std::logic_error("Const can't be the left variable");
			if (id == -1)
				throw std::logic_error("At least one variable can't be found in VariableSet");

			if (!b.get(id))
				throw std::logic_error("Two equaltions have the same variable");

			b.set(id, false);

			normvar.push_back(id);
			normexpr.push_back(MapExpression(set.norm[i].Right, &var->GetVariableSet()));
		}

		if (b.Or())
			throw std::logic_error("The set is not complete.");
	}

	GlobalSolver::~GlobalSolver()
	{
		StopSolve();
		TerminateThread(hThread, 0);
	}

	void GlobalSolver::InitSolver()
	{
		if (bRunning != -1)
			throw std::runtime_error("Solver has been inited.");

		hRunningEvent = FALSE;
		bRunning = 0;
		hThread = CreateThread(NULL, NULL, GlobalSolver::ThreadProc, this, NULL, NULL);

		if (hThread == INVALID_HANDLE_VALUE)
			throw std::runtime_error("Creating Threads Failure.");
	}

	void GlobalSolver::StartSolve(DifferentialEquationSet & set, VariableData & var, DifferentialEquationSolverInfo info, Value_Type dst, int* satuscallback)
	{
		if (info.delta > info.setdelta)
			throw std::logic_error("delta can't be lower than setdelta");

		if (bRunning == 1)
			throw std::logic_error("Solver is busy.");
		if (bRunning == -1)
			throw std::logic_error("Solver is not inited.");
		SS.satuscallback = satuscallback;
		bRunning = 1;

		SS.info = info;
		SS.data = &var;
		SS.set = &set;
		SS.destnation = dst;

		SS.stoping = false;
		hRunningEvent = TRUE;
	}

	void GlobalSolver::StopSolve()
	{
		SS.stoping = true;
		while (hRunningEvent);
		bRunning = 0;
		SS.stoping = false;
	}

	void GlobalSolver::Solve(DifferentialEquationSet & set, VariableData & var, DifferentialEquationSolverInfo info, Value_Type dst)
	{
		if (info.delta > info.setdelta)
			throw std::logic_error("delta can't be lower than setdelta");

		if (bRunning == 1)
			throw std::logic_error("Solver is busy.");
		if (bRunning == -1)
			throw std::logic_error("Solver is not inited.");

		bRunning = 1;

		SS.info = info;
		SS.data = &var;
		SS.set = &set;
		SS.destnation = dst;

		MainThreadProc();
	}

	void GlobalSolver::MainThread()
	{
#pragma warning(push)
#pragma warning (disable: 4127)
		while (true)
#pragma warning (pop)
		{
			if (hRunningEvent)
			{
				try
				{
					MainThreadProc();
				}
				catch (std::runtime_error e)
				{
					if (!(mylib::string(e.what()) == "Stoping Solving."))
						throw;
				}

				hRunningEvent = FALSE;
				if (SS.satuscallback)
					*SS.satuscallback = 0;
				SS.satuscallback = nullptr;
			}
			else
				Sleep(10);
		}
	}

	void GlobalSolver::MainThreadProc()
	{
		DifferentialEquationSetSolver solver(*SS.set, SS.data, SS.info);

		SS.data = solver.data;
		SS.lpdiffvar = &solver.diffvar;
		SS.lpdiffexpr = &solver.diffexpr;
		SS.lpnormvar = &solver.normvar;
		SS.lpnormexpr = &solver.normexpr;
		SS.info = solver.info;

		mylib::stack<Value_Type> stack;

		SS.numberofvar = SS.data->GetVariableSet().GetTotalVar();
		SS.delta = SS.info.delta;

		SS.K = new VariableRecord[4];
		for (int i = 0; i < 4; i++)
			SS.K[i].InnerInit(SS.numberofvar);

		SS.TMP = new VariableRecord[3];
		for (int i = 0; i < 3; i++)
			SS.TMP[i].InnerInit(SS.numberofvar);

		SS.diffvars = SS.lpdiffvar->size();
		SS.normvars = SS.lpnormvar->size();

		VariableRecord r1(SS.data->GetLast()), r2;
		r2.InnerInit(SS.numberofvar);

		int number_of_in = (int)(SS.info.setdelta / SS.delta) / 2;
		if (number_of_in == 0)
		{
			goto runonce;
		}

		int number_of_out = (int)((SS.destnation - SS.data->GetLast()[0]) / SS.delta) / (number_of_in * 2);
		for (int i1 = 0; i1 < number_of_out; i1++)
		{
			for (int i2 = 0; i2 < number_of_in; i2++)
			{
				DirectCalc(r1, r2, stack);
				DirectCalc(r2, r1, stack);
			}
			SS.data->AddRecord();
			SS.data->GetLast().CopyFrom(r1);
		}

		number_of_in = (int)((SS.destnation - SS.data->GetLast()[0]) / SS.delta) / 2;
		for (int i2 = 0; i2 < number_of_in; i2++)
		{
			DirectCalc(r1, r2, stack);
			DirectCalc(r2, r1, stack);
		}

		if (SS.delta < SS.destnation - r1[0])
		{
			DirectCalc(r1, r2, stack);
			SS.delta = SS.destnation - r2[0];
			DirectCalc(r2, r1, stack);
		}
		else
		{
			SS.delta = SS.destnation - r1[0];
			DirectCalc(r1, r2, stack);
			r2.CopyFrom(r1);
		}

		SS.data->AddRecord();
		SS.data->GetLast().CopyFrom(r1);
		bRunning = 0;
		return;
	runonce:
		int run_once_times = (int)((SS.destnation - SS.data->GetLast()[0]) / SS.delta) / 2;
		for (int i = 0; i < run_once_times; i++)
		{
			DirectCalc(r1, r2, stack);
			SS.data->AddRecord();
			SS.data->GetLast().CopyFrom(r2);

			DirectCalc(r2, r1, stack);
			SS.data->AddRecord();
			SS.data->GetLast().CopyFrom(r1);
		}

		if (SS.delta < SS.destnation - r1[0])
		{
			DirectCalc(r1, r2, stack);
			SS.data->AddRecord();
			SS.data->GetLast().CopyFrom(r2);

			SS.delta = SS.destnation - r2[0];
			DirectCalc(r2, r1, stack);
			SS.data->AddRecord();
			SS.data->GetLast().CopyFrom(r1);
		}
		else
		{
			SS.delta = SS.destnation - r1[0];
			DirectCalc(r1, r2, stack);
			SS.data->AddRecord();
			SS.data->GetLast().CopyFrom(r2);
		}
		bRunning = 0;
		return;
	}

	void GlobalSolver::DirectCalc(VariableRecord & lst, VariableRecord & nxt, mylib::stack<Value_Type> & stack)
	{
		VariableRecord* CON = &SS.data->GetConstRecord();

		if (SS.stoping)
			throw std::runtime_error("Stoping Solving.");

		int id;
		//计算K1和TMP1
		for (int i2 = 0; i2 < SS.diffvars; i2++)
		{
			id = SS.lpdiffvar->operator[](i2);
			SS.TMP[0][id] = lst[id] + 0.5 * SS.delta *
				(SS.K[0][id] = SS.lpdiffexpr->operator[](i2).Calculate(&lst, CON, stack));
		}
		SS.TMP[0][0] = lst[0] + SS.delta * 0.5;
		for (int i2 = 0; i2 < SS.normvars; i2++)
			SS.TMP[0][SS.lpnormvar->operator[](i2)] = SS.lpnormexpr->operator[](i2).Calculate(&SS.TMP[0], CON, stack);


		//计算K2和TMP2
		for (int i2 = 0; i2 < SS.diffvars; i2++)
		{
			id = SS.lpdiffvar->operator[](i2);
			SS.TMP[1][id] = lst[id] + 0.5 * SS.delta *
				(SS.K[1][id] = SS.lpdiffexpr->operator[](i2).Calculate(&SS.TMP[0], CON, stack));
		}
		SS.TMP[1][0] = lst[0] + SS.delta * 0.5;
		for (int i2 = 0; i2 < SS.normvars; i2++)
			SS.TMP[1][SS.lpnormvar->operator[](i2)] = SS.lpnormexpr->operator[](i2).Calculate(&SS.TMP[1], CON, stack);

		//计算K3和TMP3
		for (int i2 = 0; i2 < SS.diffvars; i2++)
		{
			id = SS.lpdiffvar->operator[](i2);
			SS.TMP[2][id] = lst[id] + SS.delta *
				(SS.K[2][id] = SS.lpdiffexpr->operator[](i2).Calculate(&SS.TMP[1], CON, stack));
		}
		SS.TMP[2][0] = lst[0] + SS.delta;
		for (int i2 = 0; i2 < SS.normvars; i2++)
			SS.TMP[2][SS.lpnormvar->operator[](i2)] = SS.lpnormexpr->operator[](i2).Calculate(&SS.TMP[2], CON, stack);

		//计算K4
		for (int i2 = 0; i2 < SS.diffvars; i2++)
		{
			id = SS.lpdiffvar->operator[](i2);
			SS.K[3][id] = SS.lpdiffexpr->operator[](i2).Calculate(&SS.TMP[2], CON, stack);
			nxt[id] = lst[id] + SS.delta / 6 * (SS.K[0][id] + 2 * (SS.K[1][id] + SS.K[2][id]) + SS.K[3][id]);
		}

		nxt[0] = lst[0] + SS.delta;
		for (int i2 = 0; i2 < SS.normvars; i2++)
			nxt[SS.lpnormvar->operator[](i2)] = SS.lpnormexpr->operator[](i2).Calculate(&nxt, CON, stack);
	}

	DWORD GlobalSolver::ThreadProc(LPVOID lp)
	{
		GlobalSolver* lpthis = (GlobalSolver*)lp;
		lpthis->MainThread();
		return 0;
	}

	GlobalSolver *GlobalSolver::GS;

	GlobalSolver gs;

}

