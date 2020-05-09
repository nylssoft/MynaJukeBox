#include "stdafx.h"
#include "Artist.h"

using namespace std;

Artist::Artist(void)
{
}

Artist::Artist(ULONG pkey, LPCWSTR name)
: m_pkey(pkey), m_name(name)
{
}

ULONG Artist::GetPKey(void) const
{
	return m_pkey;
}

const wstring & Artist::GetName(void) const
{
	return m_name;
}
