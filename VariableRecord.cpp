#include "stdafx.h"
#include "VariableRecord.h"

namespace E{

	VariableRecord::VariableRecord(const VariableSet & v, bool IsCon)
	{
		length = IsCon ? v.GetTotalCon() : v.GetTotalVar();
		data = new Value_Type[length];
	}

	VariableRecord::VariableRecord(int length)
	{
		this->length = length;
		data = new Value_Type[length];
	}


	VariableRecord::VariableRecord(const VariableRecord & obj)
	{
		length = obj.length;
		data = new Value_Type[length];
		memcpy(data, obj.data, sizeof(Value_Type)* length);
	}

	VariableRecord::VariableRecord(VariableRecord && obj)
	{
		length = obj.length;
		data = obj.data;
		obj.data = nullptr;
	}

	VariableRecord::~VariableRecord()
	{
		if (!this->data)
			return;

		delete[] data;
		data = nullptr;
	}

	VariableRecord & VariableRecord::operator=(const VariableRecord & obj)
	{
		if (this == &obj)
			return *this;

		if (this->data)
			delete[] this->data;
		length = obj.length;
		data = new Value_Type[length];
		memcpy(data, obj.data, sizeof(Value_Type)* length);
		return *this;
	}

	VariableRecord & VariableRecord::operator=(VariableRecord && obj)
	{
		if (this == &obj)
			return *this;

		if (this->data)
			delete[] this->data;

		length = obj.length;
		data = obj.data;
		obj.data = nullptr;

		return *this;
	}

	void VariableRecord::CopyFrom(const VariableRecord & obj)
	{
		if (this->length != obj.length)
			throw std::logic_error("Invalid Copy");

		memcpy(this->data, obj.data, sizeof(Value_Type)*length);
	}

	void VariableRecord::InnerInit(int len)
	{
		this->length = len;
		data = new Value_Type[len];
	}

	int VariableSet::GetNameID(mylib::string s) const
	{
		if (s == time)
			return 0;

		for (int i = 0; i < VarNames.size(); i++)
			if (s == VarNames[i])
				return i + 1;

		for (int i = 0; i < ConNames.size(); i++)
			if (s == ConNames[i])
				return i + VarNames.size() + 1;

		return -1;
	}

	mylib::string VariableSet::GetVarName(int index)const
	{
		if (index == 0)
			return time;
		if (index < 0)
			throw std::invalid_argument("Invalid Index.");
		if (index > this->VarNames.size())
		{
			if (index > this->ConNames.size() + this->VarNames.size())
				throw std::invalid_argument("Invalid Index.");
			return ConNames[index - this->VarNames.size() - 1];
		}
		return VarNames[index - 1];
	}

	int VariableSet::CalcFileLength() const
	{
		int result = 24;
		for (int i = 0; i < VarNames.size(); i++)
			result += VarNames[i].size();
		for (int i = 0; i < ConNames.size(); i++)
			result += ConNames[i].size();
		result += (VarNames.size() + ConNames.size()) * 4 + time.size();
		return result;
	}

	int VariableSet::WriteFile(HANDLE hFile)const throw(std::runtime_error, std::logic_error)
	{
		struct VariableSetHeader
		{
			int Signature;
			int Version;
			int TotalSize;
			int VarLength;
			int ConLength;
		} _struct = { _VARSET_SIGNATURE, _VARSET_VERSION, CalcFileLength(), VarNames.size(), ConNames.size() };

		DWORD dwWrite, dwTmp;
		BOOL bResult;
		int strlen;

		bResult = ::WriteFile(hFile, &_struct, 20, &dwWrite, NULL);
		if (!bResult || dwWrite != 20)
			throw std::runtime_error("Writing Failure.");

		//写入time
		strlen = time.size();
		bResult = ::WriteFile(hFile, &strlen, 4, &dwTmp, NULL);
		if (!bResult || dwTmp != 4)
			throw "Writing Failure";
		dwWrite += dwTmp;
		bResult = ::WriteFile(hFile, (const char*)time, strlen, &dwTmp, NULL);
		if (!bResult || (long)dwTmp != strlen)
			throw "Writing Failure";
		dwWrite += dwTmp;

		//写入变量
		for (int i = 0; i < _struct.VarLength; i++)
		{
			strlen = VarNames[i].size();
			bResult = ::WriteFile(hFile, &strlen, 4, &dwTmp, NULL);
			if (!bResult || dwTmp != 4)
				throw "Writing Failure";
			dwWrite += dwTmp;
			bResult = ::WriteFile(hFile, (const char*)VarNames[i], strlen, &dwTmp, NULL);
			if (!bResult || (long)dwTmp != strlen)
				throw "Writing Failure";
			dwWrite += dwTmp;
		}

		//写入常数
		for (int i = 0; i < _struct.ConLength; i++)
		{
			strlen = ConNames[i].size();
			bResult = ::WriteFile(hFile, &strlen, 4, &dwTmp, NULL);
			if (!bResult || dwTmp != 4)
				throw "Writing Failure";
			dwWrite += dwTmp;
			bResult = ::WriteFile(hFile, (const char*)ConNames[i], strlen, &dwTmp, NULL);
			if (!bResult || (long)dwTmp != strlen)
				throw "Writing Failure";
			dwWrite += dwTmp;
		}

		if (_struct.TotalSize != (long)dwWrite)
			throw std::runtime_error("runtime check failure. written size error.");

		return dwWrite;
	}

	VariableSet VariableSet::ReadFile(HANDLE hFile, int * Length) throw(std::runtime_error, std::logic_error)
	{
		struct VariableSetHeader
		{
			int Signature;
			int Version;
			int TotalSize;
			int VarLength;
			int ConLength;
		} _struct;

		DWORD dwRead, dwTmp;
		BOOL bResult;

		bResult = ::ReadFile(hFile, &_struct, 20, &dwRead, NULL);
		if (!bResult || dwRead != 20)
			throw std::runtime_error("Reading Failure.");

		if (_struct.Signature != _VARSET_SIGNATURE)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Signature.");
		}

		if (_struct.Version != _VARSET_VERSION)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Version.");
		}

		mylib::vector<mylib::string> lst, conlst;
		int strlen;


		bResult = ::ReadFile(hFile, &strlen, sizeof(int), &dwTmp, NULL);
		if (!bResult || dwTmp != sizeof(int))
			throw std::runtime_error("Reading Failure.");
		dwRead += dwTmp;
		mylib::string time(strlen);
		bResult = ::ReadFile(hFile, (char*)time, strlen, &dwTmp, NULL);
		time.refresh();
		if (!bResult || (long)dwTmp != strlen)
			throw std::runtime_error("Reading Failure.");
		dwRead += dwTmp;

		for (int i = 0; i < _struct.VarLength; i++)
		{
			bResult = ::ReadFile(hFile, &strlen, sizeof(int), &dwTmp, NULL);
			if (!bResult || dwTmp != sizeof(int))
				throw std::runtime_error("Reading Failure.");
			dwRead += dwTmp;
			mylib::string s(strlen);
			bResult = ::ReadFile(hFile, (char*)s, strlen, &dwTmp, NULL); s.refresh();
			if (!bResult || (long)dwTmp != strlen)
				throw std::runtime_error("Reading Failure.");
			dwRead += dwTmp;
			lst.push_back(s);
		}

		for (int i = 0; i < _struct.ConLength; i++)
		{
			bResult = ::ReadFile(hFile, &strlen, sizeof(int), &dwTmp, NULL);
			if (!bResult || dwTmp != sizeof(int))
				throw std::runtime_error("Reading Failure.");
			dwRead += dwTmp;
			mylib::string s(strlen);
			bResult = ::ReadFile(hFile, (char*)s, strlen, &dwTmp, NULL); s.refresh();
			if (!bResult || (long)dwTmp != strlen)
				throw std::runtime_error("Reading Failure.");
			dwRead += dwTmp;
			conlst.push_back(s);
		}

		if ((long)dwRead != _struct.TotalSize)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Format");
		}

		if (Length)
			*Length = dwRead;

		return VariableSet(time, lst, conlst);
	}

}