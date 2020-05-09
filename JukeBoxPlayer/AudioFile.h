#pragma once

#include "StdAfx.h"
#include <string>

#include "Artist.h"
#include "Album.h"
#include "MediaDirectory.h"

class AudioFile
{
public:
	AudioFile();

	AudioFile(ULONG pkey, Artist * pArtist, Album * pAlbum, UINT trackNumber, int year, LPCWSTR fileName, MediaDirectory * pMediaDirectory, LPCWSTR title);

	ULONG GetPKey(void) const;

	MediaDirectory * GetDirectory(void) const;

	const std::wstring & GetFileName(void) const;

	Artist * GetArtist(void) const;

	Album * GetAlbum(void) const;

	const std::wstring & GetTitle(void) const;

	UINT GetTrackNumber(void) const;

	int GetYear(void) const;

private:
	ULONG m_pkey;
	Artist * m_pArtist;
	Album * m_pAlbum;
	MediaDirectory * m_pDirectory;
	UINT m_trackNumber;
	int m_year;
	std::wstring m_fileName;
	std::wstring m_title;
};
