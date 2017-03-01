#pragma once
namespace E{
	class VariableSet
	{
	public:
		inline VariableSet(mylib::string t, mylib::vector<mylib::string> varnames, mylib::vector<mylib::string> connames)
			:VarNames(varnames), time(t), ConNames(connames){}

		inline VariableSet(const VariableSet & obj)
			: VarNames(obj.VarNames), time(obj.time), ConNames(obj.ConNames){}

		~VariableSet() = default;
		VariableSet& operator =(const VariableSet & obj) = default;

		inline const mylib::string & GetTimeName() const{ return time; }
		inline const mylib::vector<mylib::string> & GetVarNames() const { return VarNames; }
		inline const mylib::vector<mylib::string> & GetConNames() const { return ConNames; }

		mylib::string GetVarName(int index) const;

		inline int GetTotalVar() const { return VarNames.size() + 1; }
		inline int GetTotalCon() const { return ConNames.size(); }
		int GetNameID(mylib::string) const;

		inline int CalcFileLength() const;
		int WriteFile(HANDLE hFile)const throw(std::runtime_error, std::logic_error);
		static VariableSet ReadFile(HANDLE hFile, int * Length)throw(std::runtime_error, std::logic_error);

		friend class GlobalSolver;

	private:
		mylib::string time;
		mylib::vector<mylib::string> VarNames;
		mylib::vector<mylib::string> ConNames;
	};

	class VariableRecord
	{
	public:
		VariableRecord(const VariableSet & v, bool IsCon = false);
		VariableRecord(int length);
		VariableRecord(const VariableRecord & obj);
		VariableRecord(VariableRecord && obj);
		~VariableRecord();
		VariableRecord & operator =(const VariableRecord &);
		VariableRecord & operator =(VariableRecord &&);
		Value_Type* GetData() { return data; }
		int GetLength() const { return length; }
		inline Value_Type& operator[](int index)throw(std::invalid_argument)
		{
			if (index >= 0 && index < length)
				return data[index];
			else
				throw std::invalid_argument("Invalid index");
		}

		void CopyFrom(const VariableRecord &) throw(std::logic_error);

		inline bool operator<(const VariableRecord & r) const { return data[0] < r.data[0]; }
		inline bool operator<=(const VariableRecord & r) const { return data[0] <= r.data[0]; }
		inline bool operator>(const VariableRecord & r) const  { return data[0] > r.data[0]; }
		inline bool operator>=(const VariableRecord & r) const  { return data[0] >= r.data[0]; }
		inline bool operator==(const VariableRecord & r) const  { return data[0] == r.data[0]; }
		inline bool operator!=(const VariableRecord & r) const  { return data[0] != r.data[0]; }

		inline bool operator<(Value_Type r) const { return data[0] < r; }
		inline bool operator<=(Value_Type r) const { return data[0] <= r; }
		inline bool operator>(Value_Type r) const { return data[0] > r; }
		inline bool operator>=(Value_Type r) const { return data[0] >= r; }
		inline bool operator==(Value_Type r) const { return data[0] == r; }
		inline bool operator!=(Value_Type r) const { return data[0] != r; }

		friend class GlobalSolver;

	private:
		VariableRecord() = default;
		void InnerInit(int);
		int length;
		Value_Type * data;
	};
}