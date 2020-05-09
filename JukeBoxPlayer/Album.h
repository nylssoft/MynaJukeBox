#pragma once

#include "StdAfx.h"

#include <string>

#include "Artist.h"
#include "MediaDirectory.h"

class Album
{
public:
	Album(void);

	Album(ULONG pkey, LPCWSTR name, Artist * pAlbumArtist, MediaDirectory * pDirectory);

	ULONG GetPKey(void) const;

	const std::wstring & GetName(void) const;

	Artist * GetAlbumArtist(void) const;

	void SetAlbumArtist(Artist * pAlbumArtist);

	int GetYear(void) const;

	void SetYear(int albumYear);

	MediaDirectory * GetDirectory(void) const;

private:
	ULONG m_pkey;
	int m_year;
	std::wstring m_name;
	Artist * m_pAlbumArtist;
	MediaDirectory * m_pDirectory;
};
