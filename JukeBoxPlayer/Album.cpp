#include "stdafx.h"
#include "Album.h"

using namespace std;

Album::Album(void) : m_year(0)
{
}

Album::Album(ULONG pkey, LPCWSTR name, Artist * pAlbumArtist, MediaDirectory * pDirectory)
: m_pkey(pkey), m_name(name), m_pAlbumArtist(pAlbumArtist), m_pDirectory(pDirectory), m_year(0)
{
}

ULONG Album::GetPKey(void) const
{
	return m_pkey;
}

const wstring & Album::GetName(void) const
{
	return m_name;
}

Artist * Album::GetAlbumArtist(void) const
{
	return m_pAlbumArtist;
}

void Album::SetAlbumArtist(Artist * pArtist)
{
	m_pAlbumArtist = pArtist;
}

int Album::GetYear(void) const
{
	return m_year;
}

void Album::SetYear(int year)
{
	m_year = year;
}

MediaDirectory * Album::GetDirectory(void) const
{
	return m_pDirectory;
}
