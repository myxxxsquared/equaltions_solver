#include "stdafx.h"

using namespace mylib;

bitset::bitset(int length)
{
	if (length <= 0)
		throw std::invalid_argument("length can't below zero.");

	int ilen = length / 32;
	int ileft = length % 32;

	len = length;

	if (ileft > 0)
		ilen++;

	data = new int[len];
	memset(data, 0, 4 * ilen);
}

bitset::~bitset()
{
	delete[] data;
}

bool bitset::get(int index)
{
	if (index < 0 || index >= len)
		throw std::out_of_range("index is out of range.");

	int ilen = index / 32;
	int ileft = index % 32;

	return data[ilen] && (1 << ileft);
}

void bitset::set(int index, bool val)
{
	if (index < 0 || index >= len)
		throw std::out_of_range("index is out of range.");

	int ilen = index / 32;
	int ileft = index % 32;

	data[ilen] = val ? data[ilen] | (1 << ileft) : data[ilen] & ~(1 << ileft);
}

bool bitset::Or()
{
	bool result = false;
	int ilen = len / 32;
	int ileft = len % 32;

	for (int i = 0; i < ilen; i++)
		result = result || data[i];

	if (ileft > 0)
	{
		int tmp = 0;
		for (int i = 0; i < ileft; i++)
			tmp = (tmp << 1) | 1;
		result = result || (data[ilen] & tmp);
	}
	return result;
}

void bitset::setalltrue()
{
	int ilen = len / 32;
	int ileft = len % 32;

	if (ileft > 0)
		ilen++;

	memset(data, -1, ilen * 4);
}

