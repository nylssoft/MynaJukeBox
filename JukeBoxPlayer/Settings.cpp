#include "StdAfx.h"
#include "Settings.h"
#include <fstream>
#include <istream>
#include <sstream>

using namespace std;

Settings::Settings(void)
	: m_new(true), m_albumIdx(0), m_readOnlyMode(false)
{
	Load();
}


Settings::~Settings(void)
{
}

bool Settings::isReadOnlyMode(void) const
{
	return m_readOnlyMode;
}

bool Settings::IsNew(void) const
{
	return m_new;
}

void Settings::SetMusicDirectory(LPCWSTR musicDir)
{
	m_musicDirectory = musicDir;
}

const wstring & Settings::GetMusicDirectory(void) const
{
	return m_musicDirectory;
}

const std::wstring & Settings::GetDatabaseDirectory(void) const
{
	return m_databaseDirectory;
}

int Settings::GetCurrentAlbumIndex(void) const
{
	return m_albumIdx;
}

void Settings::SetCurrentAlbumIndex(int albumIdx)
{
	m_albumIdx = albumIdx;
}

void Settings::Load(void)
{
	wchar_t szPath[_MAX_PATH];
	::GetModuleFileName(NULL, szPath, _MAX_PATH);
	wstring basedir = szPath;
	size_t idx = basedir.rfind(L"\\");
	if (idx != wstring::npos)
	{
		basedir = basedir.substr(0, idx+1);
		wstring dirCSV = basedir + L"Directory.csv";
		m_readOnlyMode = ( GetFileAttributes( dirCSV.c_str() ) != INVALID_FILE_ATTRIBUTES );
		if (m_readOnlyMode)
		{
			m_databaseDirectory = basedir;
			m_musicDirectory = basedir;
			return;
		}
	}
	wstring localapp = GetKnownFolderLocalApp();
	localapp.append(L"\\NylsSoft");
	Settings::CreateDirectory(localapp.c_str());
	localapp.append(L"\\JukeBoxPlayer");
	Settings::CreateDirectory(localapp.c_str());
	m_databaseDirectory = localapp;
	wstring settingsfile = localapp;
	settingsfile.append(L"\\settings.dat");
	CFile f;
	if ( f.Open( settingsfile.c_str(), CFile::modeRead ) )
	{
		CArchive ar( &f, CArchive::load);
		CString str;
		ar >> str;
		ar >> m_albumIdx;
		m_musicDirectory = (LPCWSTR)str;
	}
	m_new = m_musicDirectory.empty();
	if (m_new)
	{
		m_musicDirectory = GetKnownFolderMusic();
		Save();
	}
}

void Settings::Save(void) const
{
	if (isReadOnlyMode()) return;
	wstring fname = m_databaseDirectory;
	fname.append(L"\\settings.dat");
	CFile f;
	if ( f.Open( fname.c_str(), CFile::modeCreate | CFile::modeWrite ) )
	{
		CArchive ar( &f, CArchive::store);
		CString str(m_musicDirectory.c_str());
		ar << str;
		ar << m_albumIdx;
	}
}

wstring Settings::GetKnownFolderLocalApp(void) const
{
	wstring dir;
	TCHAR szPath[MAX_PATH];
	HRESULT hr = SHGetFolderPath(NULL,  CSIDL_LOCAL_APPDATA, NULL, 0, szPath);
	if (SUCCEEDED(hr))
	{
		dir = szPath;
	}
	return dir;
}

wstring Settings::GetKnownFolderMusic(void) const
{
	wstring dir;
	TCHAR szPath[MAX_PATH];
	HRESULT hr = SHGetFolderPath(NULL,  CSIDL_MYMUSIC, NULL, 0, szPath);
	if (SUCCEEDED(hr))
	{
		dir = szPath;
	}
	return dir;
}

void Settings::CreateDirectory(LPCWSTR directory)
{
	DWORD dwFileAttributes = ::GetFileAttributes(directory);
	if (dwFileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		::CreateDirectory(directory, NULL);
	}	
}
