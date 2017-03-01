#include "stdafx.h"

using namespace E;


#define _NEED_BEGIN_NEW		1
#define _NEED_TAKE_IN		2
#define _NEED_TAKE_OUT		4

DifferentialProvider::DifferentialProvider()
	:Time(), VarSet(), ConSet(), equs(), set(nullptr), initaldata(), constdata(),
	alldata(), curdata(nullptr), info({ 0, 0 }), status(_NEED_BEGIN_NEW), time(0), issolving(0)
{
}

double DifferentialProvider::GetTime()
{
	if (status&_NEED_TAKE_OUT)
		TakeOut();
	return time;
}

DifferentialProvider::DifferentialProvider(const DifferentialProvider& obj)
	: Time(obj.time), VarSet(obj.VarSet), ConSet(obj.ConSet), equs(obj.equs)
	, time(obj.time), initaldata(obj.initaldata), constdata(obj.constdata)
	, info(obj.info), status(obj.status), issolving(0), alldata()
{
	if (obj.issolving)
		throw std::runtime_error("solving");

	set = new VariableSet(*obj.set);

	for (int i = 0; i < obj.alldata.size(); i++)
		alldata.push_back(new VariableData(*(obj.alldata.operator[](i))));
	curdata = alldata.top();
}

DifferentialProvider::~DifferentialProvider()
{
	if (issolving)
		StopSolve();

	if (set)
		delete set;
	while (alldata.size() > 0)
		delete alldata.pop_back();
}

void DifferentialProvider::SetTime(mylib::string str, Value_Type val)
{
	if (issolving)
		throw std::runtime_error("solving");

	if (status & _NEED_TAKE_OUT)
		TakeOut();

	Time = str;
	time = val;

	status |= _NEED_TAKE_IN;
}

void DifferentialProvider::AddVar(mylib::string str, Value_Type val)
{
	if (issolving)
		throw std::runtime_error("solving");
	if (status & _NEED_TAKE_OUT)
		TakeOut();

	str = str.CheckName();

	if (str == Time)
		throw std::logic_error("The Same Name");
	for (int i = 0; i < VarSet.size(); i++)
		if (VarSet[i] == str)
			throw std::logic_error("The Same Name");
	for (int i = 0; i < ConSet.size(); i++)
		if (ConSet[i] == str)
			throw std::logic_error("The Same Name");

	VarSet.push_back(str);
	initaldata.push_back(val);
	status |= _NEED_BEGIN_NEW;
}
int DifferentialProvider::CountVar() const
{
	return VarSet.size();
}

mylib::string DifferentialProvider::GetVarName(int i) const
{
	return VarSet.operator[](i);
}

int DifferentialProvider::GetVarIndex(mylib::string str) const
{
	for (int i = 0; i < VarSet.size(); i++)
		if (VarSet[i] == str)
			return i;
	return -1;
}
Value_Type DifferentialProvider::GetVar(int i)
{
	if (issolving)
		throw std::runtime_error("solving");
	if (status & _NEED_TAKE_OUT)
		TakeOut();

	return initaldata[i];
}
void DifferentialProvider::SetVar(int i, Value_Type val)
{
	if (issolving)
		throw std::runtime_error("solving");
	if (status & _NEED_TAKE_OUT)
		TakeOut();

	initaldata[i] = val;
	status |= _NEED_TAKE_IN;
}
void DifferentialProvider::RemoveVar(mylib::string str)
{
	if (issolving)
		throw std::runtime_error("solving");
	if (status & _NEED_TAKE_OUT)
		TakeOut();

	int i = GetVarIndex(str);
	if (i == -1)
		return;
	VarSet.remove(i);
	initaldata.remove(i);
	status |= _NEED_BEGIN_NEW;
}

void DifferentialProvider::ClearVar()
{
	if (issolving)
		throw std::runtime_error("solving");
	if (status & _NEED_TAKE_OUT)
		TakeOut();
	VarSet.clear();
	initaldata.clear();
	status |= _NEED_BEGIN_NEW;
}

void DifferentialProvider::AddCon(mylib::string str, Value_Type val)
{
	if (issolving)
		throw std::runtime_error("solving");

	str = str.CheckName();

	if (str == Time)
		throw std::logic_error("The Same Name");
	for (int i = 0; i < VarSet.size(); i++)
		if (VarSet[i] == str)
			throw std::logic_error("The Same Name");
	for (int i = 0; i < ConSet.size(); i++)
		if (ConSet[i] == str)
			throw std::logic_error("The Same Name");

	ConSet.push_back(str);
	constdata.push_back(val);
	status |= _NEED_BEGIN_NEW;
}
int DifferentialProvider::CountCon() const
{
	return ConSet.size();
}

mylib::string DifferentialProvider::GetConName(int i) const
{
	return ConSet[i];
}

int DifferentialProvider::GetConIndex(mylib::string str) const
{
	for (int i = 0; i < ConSet.size(); i++)
		if (ConSet[i] == str)
			return i;
	return -1;
}
Value_Type DifferentialProvider::GetCon(int i) const
{
	return constdata[i];
}
void DifferentialProvider::SetCon(int i, Value_Type val)
{
	if (issolving)
		throw std::runtime_error("solving");

	constdata[i] = val;
	status |= _NEED_BEGIN_NEW;
}
void DifferentialProvider::RemoveCon(mylib::string str)
{
	if (issolving)
		throw std::runtime_error("solving");

	int i = GetConIndex(str);
	if (i == -1)
		return;
	ConSet.remove(i);
	constdata.remove(i);

	status |= _NEED_BEGIN_NEW;
}

void DifferentialProvider::ClearCon()
{
	if (issolving)
		throw std::runtime_error("solving");
	if (status & _NEED_TAKE_OUT)
		TakeOut();
	ConSet.clear();
	constdata.clear();
	status |= _NEED_BEGIN_NEW;
}

Value_Type DifferentialProvider::GetValue(mylib::string str)
{
	if (issolving)
		throw std::runtime_error("solving");
	if (status & _NEED_TAKE_OUT)
		TakeOut();

	if (str == Time)
		return time;
	for (int i = 0; i < VarSet.size(); i++)
		if (VarSet[i] == str)
			return initaldata[i];
	for (int i = 0; i < ConSet.size(); i++)
		if (ConSet[i] == str)
			return constdata[i];
	throw std::logic_error("Can't find var");
}
void DifferentialProvider::SetValue(mylib::string str, Value_Type val)
{
	if (issolving)
		throw std::runtime_error("solving");
	if (status & _NEED_TAKE_OUT)
		TakeOut();

	if (str == Time)
	{
		time = val; return;
	}
	for (int i = 0; i < VarSet.size(); i++)
		if (VarSet[i] == str)
		{
			initaldata[i] = val; return;
		}
	for (int i = 0; i < ConSet.size(); i++)
		if (ConSet[i] == str)
		{
			constdata[i] = val; return;
		}
	throw std::logic_error("Can't find var");
}

void DifferentialProvider::AddDiff(DifferentialEquation e)
{
	equs.AddDifferential(e);
}
void DifferentialProvider::AddNorm(NormalEquation e)
{
	equs.AddNormal(e);
}
DifferentialEquation& DifferentialProvider::GetDiff(int i)
{
	return equs.GetDifferentialEquation(i);
}
NormalEquation& DifferentialProvider::GetNorm(int i)
{
	return equs.GetNormalEquation(i);
}
void DifferentialProvider::RemoveDiff(int i)
{
	equs.RemoveDifferentialAt(i);
}
void DifferentialProvider::RemoveNorm(int i)
{
	equs.RemoveNormalAt(i);
}

DifferentialEquationSolverInfo DifferentialProvider::GetSolverInfo() const
{
	return info;
}
void DifferentialProvider::SetSolverInfo(DifferentialEquationSolverInfo info)
{
	this->info = info;
}
void DifferentialProvider::StartSolveFor(Value_Type val)
{
	if (issolving)
		throw std::runtime_error("solving");
	PrepareToSolve();
	issolving = 1;
	GlobalSolver::GetGlobalSolver()->StartSolve(equs, *curdata, info, val, (int*)&issolving);
	status |= _NEED_TAKE_OUT;
}
void DifferentialProvider::SolveFor(Value_Type val)
{
	if (issolving)
		throw std::runtime_error("solving");
	PrepareToSolve();
	GlobalSolver::GetGlobalSolver()->Solve(equs, *curdata, info, val);
	status |= _NEED_TAKE_OUT;
}

void DifferentialProvider::StopSolve()
{
	if (!issolving)
		return;
	GlobalSolver::GetGlobalSolver()->StopSolve();
}

void DifferentialProvider::BeginNewData()
{
	if (issolving)
		throw std::runtime_error("solving");
	if (status & _NEED_TAKE_OUT)
		TakeOut();

	if (set)
		delete set;
	set = new VariableSet(Time, VarSet, ConSet);

	curdata = new VariableData(*set);
	alldata.push_back(curdata);
	status &= ~_NEED_BEGIN_NEW;
	status |= _NEED_TAKE_IN;
}
int DifferentialProvider::GetDataNumber()
{
	return alldata.size();
}
VariableData& DifferentialProvider::GetData(int i)
{
	return *alldata[i];
}
void DifferentialProvider::RemoveData(int i)
{
	delete alldata[i];
	alldata.remove(i);
}
void DifferentialProvider::ClearData()
{
	for (int i = 0; i < alldata.size(); i++)
		delete alldata[i];
	alldata.clear();
}

int DifferentialProvider::WriteFile(const char* filename, int contain)
{
	HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Can't Open File");
	int result;
	try
	{
		result = WriteFile(hFile, contain);
	}
	catch (...)
	{
		SetEndOfFile(hFile);
		CloseHandle(hFile);
		throw;
	}
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	return result;
}
int DifferentialProvider::WriteFile(HANDLE hFile, int contain)
{
	if (issolving)
		throw std::runtime_error("solving");

	if (status & _NEED_TAKE_OUT)
		TakeOut();

	if (status & _NEED_BEGIN_NEW)
		BeginNewData();

	if (status & _NEED_TAKE_IN)
		TakeIn();

	if (set == nullptr)
		return 0;

	struct
	{
		int Signature;
		int Version;
		int TotalSize;
		int contain;
		int DataNumber;
		int pDiff;
		double info_delta;
		double info_setdeltea;
	}_struct = { _EQUPRO_SIGNATURE, _EQUPRO_VERSION, 0, contain, 0, 0, info.delta, info.setdelta };

	const DWORD defpointer = ::SetFilePointer(hFile, 0, 0, FILE_CURRENT);
	if (HFILE_ERROR == defpointer)
		throw std::runtime_error("Writing Failure.");

	if (contain & CONTAIN_DATA)
		_struct.DataNumber = alldata.size();

	DWORD written, tmp;
	BOOL bResult;
	bResult = ::WriteFile(hFile, &_struct, 40, &written, NULL);
	if (!bResult || written != 40)
		throw std::runtime_error("Writing Failure.");

	mylib::array_val<int> pData;

	if (contain & CONTAIN_DATA)
	{
		pData.ReNew(alldata.size());
		bResult = ::WriteFile(hFile, pData, 4 * alldata.size(), &tmp, NULL);
		if (!bResult || tmp != 4 * (DWORD)alldata.size())
			throw std::runtime_error("Writing Failure.");
		written += tmp;
	}

	if (contain & CONTAIN_DATA)
	{
		for (int i = 0; i < alldata.size(); i++)
		{
			pData[i] = written;
			try
			{
				written += alldata[i]->GetVariableSet().WriteFile(hFile);
				written += alldata[i]->WriteFile(hFile);
			}
			catch (std::logic_error)
			{
				if (HFILE_ERROR == SetFilePointer(hFile, -(long)written, NULL, FILE_CURRENT))
					throw std::runtime_error("Writing failure and can't go back to default position.");
				throw;
			}
		}
	}

	if (contain & CONTAIN_EQUA)
	{
		_struct.pDiff = written;
		try
		{
			written += set->WriteFile(hFile);
			written += equs.WriteFile(hFile, *set);
		}
		catch (std::logic_error)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)written, NULL, FILE_CURRENT))
				throw std::runtime_error("Writing failure and can't go back to default position.");
			throw;
		}
	}

	_struct.TotalSize = written;

	if (HFILE_ERROR == SetFilePointer(hFile, -(long)written, NULL, FILE_CURRENT))
		throw std::runtime_error("Failure to set pointer");

	bResult = ::WriteFile(hFile, &_struct, 40, &tmp, NULL);
	if (!bResult || tmp != 40)
		throw std::runtime_error("Writing Failure.");

	if (contain & CONTAIN_DATA)
	{
		bResult = ::WriteFile(hFile, pData, 4 * alldata.size(), &tmp, NULL);
		if (!bResult || tmp != 4 * (DWORD)alldata.size())
			throw std::runtime_error("Writing Failure.");
	}

	if (HFILE_ERROR == SetFilePointer(hFile, (long)(written - 24 - 4 * alldata.size()), NULL, FILE_CURRENT))
		throw std::runtime_error("Failure to set pointer");

	return written;
}

int DifferentialProvider::ReadFile(HANDLE hFile, int contain)
{
	if (issolving)
		throw std::runtime_error("solving");

	if (set)
		delete set;
	while (alldata.size() > 0)
		delete alldata.pop_back();

	Time = mylib::string();
	VarSet = mylib::vector<mylib::string>();
	equs = E::DifferentialEquationSet();
	initaldata = mylib::vector_var < E::Value_Type >();
	constdata = mylib::vector_var < E::Value_Type >();
	curdata = nullptr;
	info = { 0, 0 };
	status = _NEED_BEGIN_NEW;

	struct
	{
		int Signature;
		int Version;
		int TotalSize;
		int contain;
		int DataNumber;
		int pDiff;
		double info_delta;
		double info_setdeltea;
	}_struct;

	BOOL bResult;
	DWORD dwRead, dwTmp;

	const DWORD defpointer = ::SetFilePointer(hFile, 0, 0, FILE_CURRENT);

	bResult = ::ReadFile(hFile, &_struct, 40, &dwRead, NULL);

	if (!bResult || dwRead != 40)
		throw std::runtime_error("Reading Failure.");

	if (_struct.Signature != _EQUPRO_SIGNATURE)
	{
		if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
			throw std::runtime_error("Reading failure and can't go back to default position.");
		throw std::logic_error("Invalid Signature.");
	}

	if (_struct.Version != _EQUPRO_VERSION)
	{
		if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
			throw std::runtime_error("Reading failure and can't go back to default position.");
		throw std::logic_error("Invalid Version.");
	}

	if (~_struct.contain & contain)
		throw std::logic_error("Invalid Contain");

	info.delta = _struct.info_delta;
	info.setdelta = _struct.info_setdeltea;

	if (contain & CONTAIN_DATA)
	{
		mylib::array_val<int> pData(_struct.DataNumber);
		bResult = ::ReadFile(hFile, pData, 4 * _struct.DataNumber, &dwTmp, NULL);
		if (!bResult || (int)dwTmp != 4 * _struct.DataNumber)
			throw std::runtime_error("Reading Failure.");

		for (int i = 0; i < _struct.DataNumber; i++)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, defpointer + pData[i], NULL, FILE_BEGIN))
				throw std::runtime_error("Fail to move pointer");

			try
			{
				alldata.push_back(new VariableData(VariableData::ReadFile(hFile, VariableSet::ReadFile(hFile, NULL), NULL)));
			}
			catch (std::logic_error)
			{
				if (HFILE_ERROR == SetFilePointer(hFile, defpointer, NULL, FILE_BEGIN))
					throw std::runtime_error("Reading failure and can't go back to default position.");
				throw;
			}
		}
		curdata = alldata[alldata.size() - 1];
	}

	if (contain & CONTAIN_EQUA)
	{
		if (HFILE_ERROR == SetFilePointer(hFile, defpointer + _struct.pDiff, NULL, FILE_BEGIN))
			throw std::runtime_error("Fail to move pointer");

		try
		{
			VariableSet s = VariableSet::ReadFile(hFile, NULL);
			Time = s.GetTimeName();
			VarSet = s.GetVarNames();
			ConSet = s.GetConNames();

			initaldata.push_back(0.0);
			for (int i = 0; i < VarSet.size(); i++)
				initaldata.push_back(0.0);
			for (int i = 0; i < ConSet.size(); i++)
				constdata.push_back(0.0);

			equs = DifferentialEquationSet::ReadFile(hFile, s, NULL);
		}
		catch (std::logic_error)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, defpointer, NULL, FILE_BEGIN))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw;
		}
	}

	if (HFILE_ERROR == SetFilePointer(hFile, defpointer + _struct.TotalSize, NULL, FILE_BEGIN))
		throw std::runtime_error("Fail to move pointer");

	status = _NEED_BEGIN_NEW;

	return _struct.TotalSize;
}

int DifferentialProvider::ReadFile(const char* filename, int contain)
{
	HANDLE hFile = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Can't Open File");
	int result;
	try
	{
		result = ReadFile(hFile, contain);
	}
	catch (...)
	{
		CloseHandle(hFile);
		throw;
	}
	CloseHandle(hFile);
	return result;
}

void DifferentialProvider::TakeOut(double t)
{
	VariableRecord&& rd = curdata->GetRecord(t);
	time = rd[0];
	for (int i = 0; i < VarSet.size(); i++)
		initaldata[i] = rd[i + 1];
	status = _NEED_BEGIN_NEW;
}

void DifferentialProvider::TakeOut()
{
	if (!(status & _NEED_TAKE_OUT))
		return;

	VariableRecord& rd = curdata->GetLast();
	time = rd[0];
	for (int i = 0; i < VarSet.size(); i++)
		initaldata[i] = rd[i + 1];
	status &= ~_NEED_TAKE_OUT;
}

void DifferentialProvider::TakeIn()
{
	if (!(status & _NEED_TAKE_IN))
		return;

	curdata->AddRecord();
	{
		VariableRecord& rd = curdata->GetLast();
		rd[0] = time;
		for (int i = 0; i < VarSet.size(); i++)
			rd[i + 1] = initaldata[i];
	}
	{
		VariableRecord& rd = curdata->GetConstRecord();
		for (int i = 0; i < ConSet.size(); i++)
			rd[i] = constdata[i];
	}
	status &= ~_NEED_TAKE_IN;
}

void DifferentialProvider::PrepareToSolve()
{
	if (issolving)
		throw std::runtime_error("solving");
	if (status & _NEED_BEGIN_NEW)
		BeginNewData();
	if (status & _NEED_TAKE_IN)
		TakeIn();
}

