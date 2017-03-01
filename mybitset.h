#pragma once

namespace mylib{
	class bitset
	{
	public:
		bitset(int length) throw(std::invalid_argument);
		bitset(const bitset&) = delete;
		~bitset();
		bitset& operator=(const bitset&) = delete;

		bool get(int) throw(std::out_of_range);
		void set(int, bool) throw(std::out_of_range);

		bool Or();
		void setalltrue();

	private:
		int len;
		int*data;
	};
}