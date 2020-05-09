#include "stdafx.h"
#include "Convert.h"

#include <sstream>

using namespace std;


ULONG Convert::ToULONG(const wstring & str)
{
	wistringstream wis(str);
	ULONG ulong;
	wis >> ulong;
	return ulong;
}

vector<wstring> Convert::ToVector(const wstring & strVector)
{
	vector<wstring> ret;
	if (!strVector.empty())
	{
		size_t idx = 0;
		do
		{
			size_t nextidx = strVector.find_first_of(L"\t",idx);
			ret.push_back(strVector.substr(idx, nextidx - idx));
			if (nextidx == wstring::npos || nextidx == strVector.size() - 1)
			{
				break;
			}
			idx = nextidx+1;
		} while (true);
	}
	return ret;
}

wstring Convert::ToString(long val)
{
	wstringstream s;
	s << val;
	return s.str();
}
