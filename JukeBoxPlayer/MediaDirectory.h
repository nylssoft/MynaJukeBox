#pragma once

#include "StdAfx.h"

#include <string>


class MediaDirectory
{
public:
	MediaDirectory(void);

	MediaDirectory(ULONG pkey, LPCWSTR relativePath);

	LPCWSTR GetRelativePath(void) const;

	ULONG GetPKey(void) const;

private:
	ULONG m_pkey;
	std::wstring m_relativePath;
};
