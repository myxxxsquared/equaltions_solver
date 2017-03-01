#include "stdafx.h"

#include "VariableData.h"

namespace E{

	VariableData::VariableData(const VariableSet & set, int deflen)
		:varset(set)
	{
		this->deflen = this->setlength = deflen;
		this->length = 0;
		this->vardata = new VariableRecord*[deflen];
		this->condata = new VariableRecord(set, true);
	}

	VariableData::VariableData(const VariableData & obj)
		:varset(obj.varset)
	{
		this->deflen = obj.deflen;
		this->length = obj.length;
		this->setlength = obj.setlength;

		this->vardata = new VariableRecord*[setlength];

		for (int i = 0; i < length; i++)
			this->vardata[i] = new VariableRecord(*obj.vardata[i]);

		this->condata = new VariableRecord(*obj.condata);
	}

	VariableData::VariableData(VariableData && obj)
		:varset(obj.varset)
	{
		deflen = obj.deflen;
		length = obj.length;
		setlength = obj.setlength;

		vardata = obj.vardata;
		obj.vardata = nullptr;
		condata = obj.condata;
		obj.condata = nullptr;
	}

	VariableData::~VariableData()
	{
		if (this->vardata)
		{
			for (int i = 0; i < length; i++)
				delete this->vardata[i];

			delete[] this->vardata;

			this->vardata = nullptr;
			this->length = 0;
		}
		if (this->condata)
		{
			delete condata;
		}
	}

	VariableData & VariableData::operator=(const VariableData & obj)
	{
		if (this == &obj)
			return *this;

		if (this->vardata)
		{
			for (int i = 0; i < length; i++)
				delete this->vardata[i];

			delete[] this->vardata;
		}
		if (this->condata)
		{
			delete condata;
		}

		this->deflen = obj.deflen;
		this->length = obj.length;
		this->setlength = obj.setlength;

		this->vardata = new VariableRecord*[setlength];

		for (int i = 0; i < length; i++)
			this->vardata[i] = new VariableRecord(*obj.vardata[i]);

		this->condata = new VariableRecord(*obj.condata);
		this->varset = obj.varset;

		return *this;
	}

	VariableData & VariableData::operator=(VariableData && obj)
	{
		if (this == &obj)
			return *this;

		if (this->vardata)
		{
			for (int i = 0; i < length; i++)
				delete this->vardata[i];

			delete[] this->vardata;
		}
		if (this->condata)
		{
			delete condata;
		}
		deflen = obj.deflen;
		length = obj.length;
		setlength = obj.setlength;

		vardata = obj.vardata;
		obj.vardata = nullptr;
		condata = obj.condata;
		obj.condata = nullptr;

		return *this;
	}

	void VariableData::AddRecord()
	{
		if (length == setlength)
			expand();

		vardata[length++] = new VariableRecord(varset);
	}

	int VariableData::SearchPrevious(Value_Type val)
	{
		int first = 0, last = length - 1, mid;

		if (*(vardata[0]) > val)
			return -1;
		if (*(vardata[last]) < val)
			return last;

		while (last - first > 1)
		{
			mid = (first + last) / 2;
			if (*(vardata[mid]) == val)
				return mid;
			if (*(vardata[mid]) > val)
				last = mid;
			else
				first = mid;
		}

		if (first == last)
			return first;
		if (*(vardata[last]) == val)
			return last;
		return first;
	}

	VariableRecord VariableData::GetRecord(Value_Type val)
	{
		int pre = SearchPrevious(val);

		if (pre == -1 || pre == length)
			throw std::logic_error("Can't find record");

		if (*(vardata[pre]) == val)
			return *(vardata[pre]);

		int nxt = pre + 1;

		VariableRecord r(varset);

		VariableRecord& p = *(vardata[pre]);
		VariableRecord& n = *(vardata[nxt]);
		Value_Type prequan = (n[0] - val) / (n[0] + p[0]);
		Value_Type nxtquan = (val - p[0]) / (n[0] + p[0]);


		int len = varset.GetVarNames().size();

		for (int i = 0; i < len; i++)
			r[i] = prequan * p[0] + nxtquan * n[0];

		return r;
	}

	int VariableData::WriteFile(HANDLE hFile) throw(std::runtime_error, std::logic_error)
	{
		struct
		{
			int Signature;
			int Version;
			int TotalSize;
			int ValueTypeSize;
			int RecordSize;
			int RecordNumber;
			int ConstRecordSize;
		}_struct =
		{ _VARDATA_SIGNATURE, _VARDATA_VERSION, CalcFileLength(),
		sizeof(Value_Type), varset.GetTotalVar(), length, varset.GetConNames().size() };

		DWORD dwWrite, dwTmp;
		BOOL bResult;

		bResult = ::WriteFile(hFile, &_struct, 28, &dwWrite, NULL);
		if (!bResult || dwWrite != 28)
			throw std::runtime_error("Writing Error.");

		int recordlength = _struct.ValueTypeSize * _struct.RecordSize;

		for (int i = 0; i < length; i++)
		{
			void* buf = (*vardata[i]).GetData();
			bResult = ::WriteFile(hFile, buf, recordlength, &dwTmp, NULL);
			if (!bResult || (long)dwTmp != recordlength)
				throw std::runtime_error("Writing Error.");
			dwWrite += dwTmp;
		}

		recordlength = _struct.ValueTypeSize*_struct.ConstRecordSize;
		void* buf = (*condata).GetData();
		bResult = ::WriteFile(hFile, buf, recordlength, &dwTmp, NULL);
		if (!bResult || (long)dwTmp != recordlength)
			throw std::runtime_error("Writing Error.");
		dwWrite += dwTmp;

		if (_struct.TotalSize != (long)dwWrite)
			throw std::runtime_error("runtime check failure. written size error.");

		return dwWrite;
	}

	VariableData VariableData::ReadFile(HANDLE hFile, const VariableSet & v, int * Length) throw(std::runtime_error, std::logic_error)
	{
		struct
		{
			int Signature;
			int Version;
			int TotalSize;
			int ValueTypeSize;
			int RecordSize;
			int RecordNumber;
			int ConstRecordSize;
		}_struct;

		DWORD dwRead, dwTmp;
		BOOL bResult;

		bResult = ::ReadFile(hFile, &_struct, 28, &dwRead, NULL);
		if (!bResult || dwRead != 28)
			throw std::runtime_error("Reading Failure.");

		if (_struct.Signature != _VARDATA_SIGNATURE)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Signature.");
		}

		if (_struct.Version != _VARDATA_VERSION)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Version.");
		}

		if (_struct.ValueTypeSize != sizeof(Value_Type))
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Type.");
		}

		if (_struct.RecordSize != v.GetTotalVar())
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Size.");
		}

		if (_struct.ConstRecordSize != v.GetTotalCon())
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Size.");
		}

		VariableData result(v);

		int recordlength = _struct.ValueTypeSize * _struct.RecordSize;
		for (int i = 0; i < _struct.RecordNumber; i++)
		{
			result.AddRecord();
			bResult = ::ReadFile(hFile, result[i].GetData(), recordlength, &dwTmp, NULL);
			if (!bResult || (long)dwTmp != recordlength)
				throw std::runtime_error("Reading Error.");
			dwRead += dwTmp;
		}

		recordlength = _struct.ValueTypeSize*_struct.ConstRecordSize;
		bResult = ::ReadFile(hFile, result.condata->GetData(), recordlength, &dwTmp, NULL);
		if (!bResult || (long)dwTmp != recordlength)
			throw std::runtime_error("Reading Error.");
		dwRead += dwTmp;

		if ((long)dwRead != _struct.TotalSize)
		{
			if (HFILE_ERROR == SetFilePointer(hFile, -(long)dwRead, NULL, FILE_CURRENT))
				throw std::runtime_error("Reading failure and can't go back to default position.");
			throw std::logic_error("Invalid Format");
		}

		if (Length)
			*Length = dwRead;

		return result;
	}

	void VariableData::expand()
	{
		VariableRecord** newmem = new VariableRecord*[setlength + deflen];
		memcpy(newmem, vardata, sizeof(VariableRecord*) * length);
		setlength += deflen;
		VariableRecord** oldmem = vardata;
		vardata = newmem;
		delete[] oldmem;
	}

}