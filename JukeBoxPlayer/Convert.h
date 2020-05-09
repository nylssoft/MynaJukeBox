#pragma once

#include "StdAfx.h"

#include <string>
#include <vector>

class Convert
{
public:

	static ULONG ToULONG(const std::wstring & str);

	static std::vector<std::wstring> ToVector(const std::wstring & strVector);

	static std::wstring ToString(long val);
};
