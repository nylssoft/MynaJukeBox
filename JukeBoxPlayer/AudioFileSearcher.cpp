#include "stdafx.h"
#include "AudioFileSearcher.h"
#include <atlimage.h>

#include <queue>
#include <fstream>

using namespace std;

AudioFileSearcher::AudioFileSearcher(MediaDatabase * pMediaDatabase)
	: m_pMediaDatabase(pMediaDatabase), m_pWinThread(NULL),
	m_searching(false), m_completed(false), m_pCancelEvent(NULL)
{
	m_pCancelEvent = new CEvent();
}

AudioFileSearcher::~AudioFileSearcher(void)
{
	delete m_pWinThread;
}

UINT __cdecl AudioFileSearcherThreadProc( LPVOID lpParameter )
{
	AudioFileSearcher * pSearcher = static_cast<AudioFileSearcher *>(lpParameter);
	pSearcher->DoSearch();
	::AfxEndThread( 0, FALSE ); 
	return 0L;
}

void AudioFileSearcher::SearchAsync(LPCWSTR search)
{
	if (m_pWinThread)
	{
		Cancel();
	}
	m_search = search;
	m_completed = false;
	m_searching = true;
	m_results.clear();
	if (m_search.empty())
	{
		m_completed = true;
		m_searching = false;
		return;
	}
	m_pWinThread = ::AfxBeginThread( AudioFileSearcherThreadProc, this, 0, 0, CREATE_SUSPENDED, NULL);
	m_pWinThread->m_bAutoDelete = FALSE; 
	m_pWinThread->ResumeThread();
}

void AudioFileSearcher::DoSearch()
{
	m_results.clear();
	m_results = m_pMediaDatabase->Search(m_search.c_str(), 1000, m_pCancelEvent);
	m_searching = false;
	m_completed = true;
}

void AudioFileSearcher::Cancel(void)
{
	if (m_pWinThread)
	{
		m_pCancelEvent->SetEvent();
		::WaitForSingleObject( m_pWinThread->m_hThread, INFINITE );
		delete m_pWinThread;
		m_pWinThread = NULL;
		m_pCancelEvent->ResetEvent();
	}
}

bool AudioFileSearcher::IsCompleted(void) const
{
	return m_completed;
}

bool AudioFileSearcher::IsSearching(void) const
{
	return m_searching;
}

vector<AudioFile *> AudioFileSearcher::GetResults(void)
{
	m_completed = false;
	return m_results;
}

const wstring & AudioFileSearcher::GetSearchString(void)
{
	return m_search;
}
