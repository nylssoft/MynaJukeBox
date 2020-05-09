#pragma once

#include "StdAfx.h"
#include <string>

class Artist
{
public:
	Artist(void);

	Artist(ULONG pkey, LPCWSTR name);

	ULONG GetPKey(void) const;

	const std::wstring & GetName(void) const;

private:
	ULONG m_pkey;
	std::wstring m_name;
};
