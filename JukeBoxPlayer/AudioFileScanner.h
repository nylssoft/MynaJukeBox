#pragma once

#include "StdAfx.h"
#include "MediaDatabase.h"

class AudioFileScanner
{
public:
	AudioFileScanner(MediaDatabase * pMediaDatabase);

	~AudioFileScanner(void);

	void ScanAsync(bool fullScan);

	void DoScan(void);

	void Cancel(void);

	bool IsScanning(void);

	bool IsCompleted(void);

	void GetState(long & current, long & total, std::wstring & directory, long & albumCount, long & fileCount);

private:
	void FindDirectories(LPCWSTR startDir, std::vector<std::wstring> & directories);

	MediaDatabase * m_pMediaDatabase;
	CWinThread * m_pWinThread;
	volatile bool m_scanning;
	volatile bool m_quit;
	volatile bool m_completed;
	CCriticalSection m_cs;
	long m_total;
	long m_current;
	long m_albumCount;
	long m_fileCount;
	std::wstring m_currentDirectory;
};

