#include "stdafx.h"
#include "MediaDirectory.h"

using namespace std;

MediaDirectory::MediaDirectory(void)
: m_pkey(0)
{
}

MediaDirectory::MediaDirectory(ULONG pkey, LPCWSTR relativePath)
: m_pkey(pkey), m_relativePath(relativePath)
{
}

LPCWSTR MediaDirectory::GetRelativePath(void) const
{
	return m_relativePath.c_str();
}

ULONG MediaDirectory::GetPKey(void) const
{
	return m_pkey;
}
/*
void MediaDirectory::SetImageFileName(LPCWSTR imageFileName)
{
	m_imageFileName = imageFileName;
}

void MediaDirectory::SetThumbnailFileName(LPCWSTR thumbnailFileName)
{
	m_thumbnailFileName = thumbnailFileName;
}

const wstring & MediaDirectory::GetImageFileName(void) const
{
	return m_imageFileName;
}

const wstring & MediaDirectory::GetThumbnailFileName(void) const
{
	return m_thumbnailFileName;
}
*/