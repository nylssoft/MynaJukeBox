#include "stdafx.h"
#include "AudioFile.h"

using namespace std;

AudioFile::AudioFile()
: m_pkey(0), m_pArtist(NULL), m_pAlbum(NULL), m_pDirectory(NULL), m_trackNumber(0), m_year(0)
{}

AudioFile::AudioFile(ULONG pkey, Artist * pArtist, Album * pAlbum, UINT trackNumber, int year, LPCWSTR fileName, MediaDirectory * pDirectory, LPCWSTR title)
: m_pkey(pkey), m_pArtist(pArtist), m_pAlbum(pAlbum), m_trackNumber(trackNumber), m_year(year), m_fileName(fileName), m_pDirectory(pDirectory), m_title(title)
{
}

ULONG AudioFile::GetPKey(void) const
{
	return m_pkey;
}

MediaDirectory * AudioFile::GetDirectory(void) const
{
	return m_pDirectory;
}

const wstring & AudioFile::GetFileName(void) const
{
	return m_fileName;
}

Artist * AudioFile::GetArtist(void) const
{
	return m_pArtist;
}

Album * AudioFile::GetAlbum(void) const
{
	return m_pAlbum;
}

const wstring & AudioFile::GetTitle(void) const
{
	return m_title;
}

UINT AudioFile::GetTrackNumber(void) const
{
	return m_trackNumber;
}

int AudioFile::GetYear(void) const
{
	return m_year;
}
