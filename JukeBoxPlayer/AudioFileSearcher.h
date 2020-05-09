#pragma once

#include "StdAfx.h"
#include "MediaDatabase.h"

class AudioFileSearcher
{
public:
	AudioFileSearcher(MediaDatabase * pMediaDatabase);

	~AudioFileSearcher(void);

	void SearchAsync(LPCWSTR search);

	void DoSearch(void);

	void Cancel(void);

	bool IsSearching(void) const;

	bool IsCompleted(void) const;

	std::vector<AudioFile *> GetResults(void);

	const std::wstring & GetSearchString(void);

private:
	MediaDatabase * m_pMediaDatabase;
	CWinThread * m_pWinThread;
	volatile bool m_searching;
	volatile bool m_completed;
	CCriticalSection m_cs;
	CEvent * m_pCancelEvent;
	std::wstring m_search;
	std::vector<AudioFile *> m_results;
};

