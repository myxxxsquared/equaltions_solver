#pragma once

#include "VariableRecord.h"

namespace E{
	class VariableData
	{
	public:
		VariableData(const VariableSet &, int DefaultLength = 10000);
		VariableData(const VariableData&);
		VariableData(VariableData&&);
		~VariableData();
		VariableData& operator=(const VariableData&);
		VariableData& operator=(VariableData&&);

		void AddRecord();
		int SearchPrevious(Value_Type);
		VariableRecord GetRecord(Value_Type) throw (std::logic_error);

		inline VariableRecord& GetLast()
		{
			return *vardata[size() - 1];
		}

		inline VariableRecord& operator[](int index) throw(std::invalid_argument)
		{
			if (index < 0 || index >= length)
				throw std::invalid_argument("Invalid index");
			return *vardata[index];
		}

		inline VariableRecord& GetConstRecord(){ return *condata; }
		inline int size(){ return length; }

		inline const VariableSet& GetVariableSet() const { return varset; }

		int CalcFileLength() { return 28 + sizeof(Value_Type) * (varset.GetTotalVar() * length + varset.GetConNames().size()); }
		int WriteFile(HANDLE hFile)throw(std::runtime_error, std::logic_error);
		static VariableData ReadFile(HANDLE hFile, const VariableSet & v, int * Length)throw(std::runtime_error, std::logic_error);

	private:

		void expand();

		int deflen;
		int length;
		int setlength;
		VariableSet varset;
		VariableRecord * condata;
		VariableRecord ** vardata;
	};

}