#include "stdafx.h"
#include "AudioFileScanner.h"
#include <atlimage.h>

#include <queue>
#include <fstream>

using namespace std;

AudioFileScanner::AudioFileScanner(MediaDatabase * pMediaDatabase)
	: m_pMediaDatabase(pMediaDatabase), m_pWinThread(NULL), m_quit(false),
	m_scanning(false), m_completed(false), m_current(0), m_total(0),
	m_fileCount(0), m_albumCount(0)
{	 
}

AudioFileScanner::~AudioFileScanner(void)
{
	delete m_pWinThread;
}

UINT __cdecl ThreadProc( LPVOID lpParameter )
{
	AudioFileScanner * pScanner = static_cast<AudioFileScanner *>(lpParameter);
	pScanner->DoScan();
	::AfxEndThread( 0, FALSE ); 
	return 0L;
}

void AudioFileScanner::ScanAsync(bool fullScan)
{
	m_quit = false;
	m_completed = false;
	m_currentDirectory = _T("");
	m_current = 0;
	m_total = 0;
	m_fileCount = 0;
	m_albumCount = 0;
	m_scanning = true;
	if (fullScan)
	{
		m_pMediaDatabase->Clear();
	}
	m_pWinThread = ::AfxBeginThread( ThreadProc, this, 0, 0, CREATE_SUSPENDED, NULL);
	m_pWinThread->m_bAutoDelete = FALSE; 
	m_pWinThread->ResumeThread();
}

void AudioFileScanner::DoScan()
{
	wofstream logstream;
	wstring logfilename = m_pMediaDatabase->GetSettings()->GetDatabaseDirectory();
	logfilename.append(L"\\scan.log");
	logstream.open(logfilename.c_str(),  ios_base::out);
	wstring basedir = m_pMediaDatabase->GetSettings()->GetMusicDirectory();
	vector<wstring> directories;
	vector<ULONG> dirPKeys;
	FindDirectories(basedir.c_str(), directories);
	// start progress
	CSingleLock singleLock(&m_cs, true);
	m_current = 0;
	m_total = directories.size();
	singleLock.Unlock();
	// iterate over all found directories
	for (vector<wstring>::const_iterator iter = directories.begin(); iter != directories.end() && !m_quit; iter++)
	{
		bool dirExists = false;
		wstring mediadir = iter->substr(basedir.length());
		// update progress
		singleLock.Lock();
		m_currentDirectory = mediadir;
		m_current++;
		m_albumCount = m_pMediaDatabase->GetAlbumCount();
		m_fileCount = m_pMediaDatabase->GetAudioFileCount();
		singleLock.Unlock();
		// scan directory
		MediaDirectory * pMediaDirectory = m_pMediaDatabase->InsertDirectory(mediadir.c_str(), dirExists);
		dirPKeys.push_back(pMediaDirectory->GetPKey());
		if (dirExists)
		{
			continue;
		}
		CFileFind finder;
		CString strWildcard(iter->c_str());
		strWildcard += _T("\\*.*");
		BOOL bWorking = finder.FindFile(strWildcard);
		while (bWorking && !m_quit)
		{
			bWorking = finder.FindNextFile();
			if (finder.IsDots() || finder.IsDirectory())
			{
				continue;
			}
			wstring fileName = (LPCWSTR)finder.GetFilePath();
			if (m_pMediaDatabase->IsAudioFile(fileName.c_str()))
			{
				AudioFile * pAudioFile = m_pMediaDatabase->InsertAudioFile(basedir.c_str(), pMediaDirectory, (LPCWSTR)finder.GetFileName());
				if (pAudioFile == NULL)
				{
					logstream << L"No album, artist or title for audio file '" << mediadir.c_str() << L"\\" << fileName.c_str() << L"' found! Ignored." << endl;
				}
			}
		}
		finder.Close();
	}
	// cleanup unused directories
	vector<MediaDirectory *> mediaDirs = m_pMediaDatabase->GetDirectories();
	for (vector<MediaDirectory *>::const_iterator iter = mediaDirs.begin(); iter != mediaDirs.end(); iter++)
	{
		MediaDirectory * pDir = *iter;
		vector<ULONG>::const_iterator findIter = find(dirPKeys.begin(), dirPKeys.end(), pDir->GetPKey());
		if (findIter == dirPKeys.end())
		{
			m_pMediaDatabase->RemoveDirectory(pDir);
		}
	}
	m_completed = !m_quit;
	m_scanning = false;
}

void AudioFileScanner::FindDirectories(LPCWSTR startDir, vector<wstring> & directories)
{
	CFileFind finder;
	CString strWildcard(startDir);
	strWildcard += _T("\\*.*");
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
		{
			CString str = finder.GetFilePath();
			directories.push_back((LPCWSTR)str);
			FindDirectories((LPCWSTR)str, directories);
		}
	}
	finder.Close();
}

void AudioFileScanner::Cancel(void)
{
	if (m_pWinThread)
	{
		m_quit = true;
		::WaitForSingleObject( m_pWinThread->m_hThread, INFINITE ); 
	}
}

bool AudioFileScanner::IsCompleted(void)
{
	return m_completed;
}

bool AudioFileScanner::IsScanning(void)
{
	return m_scanning;
}

void AudioFileScanner::GetState(long & current, long & total, wstring & directory, long & albumCount, long & fileCount)
{
	CSingleLock lock(&m_cs, true);
	current = m_current;
	total = m_total;
	albumCount = m_albumCount;
	fileCount = m_fileCount;
	directory = m_currentDirectory;
}
