#include "stdafx.h"
#include "PlayList.h"
#include "Convert.h"
#include "MediaDatabase.h"

#include <fstream>
#include <istream>
#include <sstream>

using namespace std;

PlayList::PlayList()
{
}

PlayList::~PlayList()
{
}

vector<AudioFile *> PlayList::GetAudioFiles(void) const
{
	return m_audioFiles;
}

bool PlayList::Empty(void) const
{
	return m_audioFiles.empty();
}

AudioFile * PlayList::RemoveFirst(void)
{
	AudioFile * pRet = m_audioFiles.front();
	m_audioFiles.erase(m_audioFiles.begin());
	return pRet;
}

void PlayList::Clear(void)
{
	m_audioFiles.clear();
}

void PlayList::AddAudioFile(AudioFile * pAudioFile)
{
	m_audioFiles.push_back(pAudioFile);
}

AudioFile * PlayList::GetAudioFile(int audioFileIdx) const
{
	return m_audioFiles[audioFileIdx];
}

int PlayList::GetAudioFileCount(void) const
{
	return m_audioFiles.size();
}

void PlayList::Save(LPCWSTR szFileName) const
{
	LPCWSTR SEP = L"\t";
	wofstream wostream;
	wostream.open(szFileName, ios_base::out);
	vector<AudioFile *>::const_iterator fileIter = m_audioFiles.begin();
	for (; fileIter != m_audioFiles.end(); ++fileIter)
	{
		AudioFile * pAudioFile = *fileIter;
		wostream << pAudioFile->GetAlbum()->GetName() << SEP << pAudioFile->GetTitle() << endl;
	}
	wostream.close();
}

void PlayList::Load(LPCWSTR szFileName, const MediaDatabase & mediaDatabase)
{
	wifstream wistream;
	m_audioFiles.clear();
	wistream.open(szFileName);
	wchar_t buffer[4096];
	while (!wistream.eof())
	{
		wistream.getline(buffer, 4096);
		wstring parseline = buffer;
		vector<wstring> values = Convert::ToVector(parseline);
		if (values.size() < 2)
		{
			break;
		}
		AudioFile * pAudioFile = mediaDatabase.FindTitle(values[0].c_str(),values[1].c_str());
		if (pAudioFile != NULL)
		{
			m_audioFiles.push_back(pAudioFile);
		}
	}
	wistream.close();
}
