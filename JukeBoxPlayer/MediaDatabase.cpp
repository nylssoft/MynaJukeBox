#include "stdafx.h"
#include "MediaDatabase.h"
#include "Convert.h"

#include <tag.h>
#include <fileref.h>
#include <fstream>
#include <istream>
#include <sstream>

#include <queue>

using namespace std;

MediaDatabase::MediaDatabase(Settings * pSettings)
	: m_pSettings(pSettings), m_nextPKey(1)
{
	m_artists[1] = new Artist(1, L"Various");
}


MediaDatabase::~MediaDatabase(void)
{
	delete m_pSettings;
}

long MediaDatabase::GetAudioFileCount() const
{
	return m_audioFiles.size();
}

long MediaDatabase::GetAlbumCount() const
{
	return m_albums.size();
}

void MediaDatabase::Clear(void)
{
	m_albums.clear();
	m_artists.clear();
	m_audioFiles.clear();
	m_mediaDirectories.clear();
	m_nextPKey = 1;
	m_artists[1] = new Artist(1, L"Various");
}

Settings * MediaDatabase::GetSettings(void) const
{
	return m_pSettings;
}

void MediaDatabase::Save(void) const
{
	LPCWSTR SEP = L"\t";
	wofstream wostream;
	wstring basedir = m_pSettings->GetDatabaseDirectory();
	basedir.append(L"\\");
	// directories
	wstring dirCSV = basedir + L"Directory.csv";
	wostream.open(dirCSV.c_str(), ios_base::out);
	map<ULONG,MediaDirectory *>::const_iterator dirIter = m_mediaDirectories.begin();
	for (; dirIter != m_mediaDirectories.end(); ++dirIter)
	{
		MediaDirectory * pMediaDir = dirIter->second;
		wostream << pMediaDir->GetPKey() << SEP << pMediaDir->GetRelativePath() << endl; //SEP;
		// wostream << pMediaDir->GetImageFileName() << SEP << pMediaDir->GetThumbnailFileName() << endl;
	}
	wostream.close();
	// artists
	wstring artistCSV = basedir + L"Artist.csv";
	wostream.open(artistCSV.c_str(), ios_base::out);
	map<ULONG,Artist *>::const_iterator artistIter = m_artists.begin();
	for (; artistIter != m_artists.end(); ++artistIter)
	{
		Artist * pArtist = artistIter->second;
		wostream << pArtist->GetPKey() << SEP << pArtist->GetName() << endl;
	}
	wostream.close();
	// albums
	wstring albumCSV = basedir + L"Album.csv";
	wostream.open(albumCSV.c_str(), ios_base::out);
	map<ULONG,Album *>::const_iterator albumIter = m_albums.begin();
	for (; albumIter != m_albums.end(); ++albumIter)
	{
		Album * pAlbum = albumIter->second;
		wostream << pAlbum->GetPKey() << SEP << pAlbum->GetName() << SEP << pAlbum->GetAlbumArtist()->GetPKey() << SEP;
		wostream << pAlbum->GetDirectory()->GetPKey() << endl;
	}
	wostream.close();
	// audio files
	wstring audioFilesCSV = basedir + L"AudioFile.csv";
	wostream.open(audioFilesCSV.c_str(), ios_base::out);
	map<ULONG,AudioFile *>::const_iterator audioFileIter = m_audioFiles.begin();
	for (; audioFileIter != m_audioFiles.end(); ++audioFileIter)
	{
		AudioFile * pAudioFile = audioFileIter->second;
		wostream << pAudioFile->GetPKey() << SEP << pAudioFile->GetArtist()->GetPKey() << SEP << pAudioFile->GetAlbum()->GetPKey() << SEP;
		wostream << pAudioFile->GetTrackNumber() << SEP << pAudioFile->GetFileName() << SEP;		
		wostream << pAudioFile->GetDirectory()->GetPKey() << SEP << pAudioFile->GetTitle() << SEP;
		wostream << pAudioFile->GetYear() << endl;
		if (!wostream.good())
		{
			// @TODO: log error, does not handle unicode characters in title, e.g. 'tm' symbol (8482)
			// @TODO: incomplete, following audio files are missing!
			break;
		}
	}
	wostream.close();
	m_pSettings->Save();

	wstring overviewCSV = basedir + L"Overview.txt";
	wostream.open(overviewCSV.c_str(), ios_base::out);
	map<ULONG,Album *>::const_iterator albumOverviewIter = m_albums.begin();
	for (; albumOverviewIter != m_albums.end(); ++albumOverviewIter)
	{
		Album * pAlbum = albumOverviewIter->second;
		wostream << pAlbum->GetAlbumArtist()->GetName() << L" - " << pAlbum->GetName() << L" (" << pAlbum->GetYear() << L")" << endl;
	}
	wostream.close();

}

bool MediaDatabase::Load(void)
{
	wifstream wistream;
	wstring basedir = m_pSettings->GetDatabaseDirectory();
	basedir.append(L"\\");
	// directories
	m_mediaDirectories.clear();
	wstring dirCSV = basedir + L"Directory.csv";
	wistream.open(dirCSV.c_str());
	wchar_t buffer[4096];
	while (!wistream.eof())
	{
		wistream.getline(buffer, 4096);
		wstring parseline = buffer;
		vector<wstring> values = Convert::ToVector(parseline);
		if (values.empty())
		{
			break;
		}
		wstring relpath;
		if (values.size() > 1)
		{
			relpath = values[1];
		}
		MediaDirectory * pMediaDir = new MediaDirectory(Convert::ToULONG(values[0]), relpath.c_str());
		m_mediaDirectories[pMediaDir->GetPKey()] = pMediaDir;
		m_nextPKey = max(pMediaDir->GetPKey()+1, m_nextPKey);
	}
	wistream.close();
	// artists
	wstring artistCSV = basedir + L"Artist.csv";
	wistream.open(artistCSV.c_str());	
	while (!wistream.eof())
	{
		wistream.getline(buffer, 4096);
		wstring parseline = buffer;
		vector<wstring> values = Convert::ToVector(parseline);
		if (values.size() < 2)
		{
			break;
		}
		Artist * pArtist = new Artist(Convert::ToULONG(values[0]), values[1].c_str());
		m_artists[pArtist->GetPKey()] = pArtist;
		m_nextPKey = max(pArtist->GetPKey()+1, m_nextPKey);
	}
	wistream.close();
	// albums
	m_albums.clear();
	wstring albumCSV = basedir + L"Album.csv";
	wistream.open(albumCSV.c_str());	
	while (!wistream.eof())
	{
		wistream.getline(buffer, 4096);
		wstring parseline = buffer;
		vector<wstring> values = Convert::ToVector(parseline);
		if (values.size() < 4)
		{
			break;
		}
		Artist * pAlbumArtist = GetArtist(Convert::ToULONG(values[2])); // fkeyArtist
		MediaDirectory * pMediaDirectory = GetDirectory(Convert::ToULONG(values[3])); // fkeyDirectory
		Album * pAlbum = new Album(Convert::ToULONG(values[0]),  // pkey
					values[1].c_str(),   // name
					pAlbumArtist, 
					pMediaDirectory); 
		m_albums[pAlbum->GetPKey()] = pAlbum;
		m_nextPKey = max(pAlbum->GetPKey()+1, m_nextPKey);
	}
	wistream.close();
	// audio files
	m_audioFiles.clear();
	wstring audioFileCSV = basedir + L"AudioFile.csv";
	wistream.open(audioFileCSV.c_str());	
	while (!wistream.eof())
	{
		wistream.getline(buffer, 4096);
		wstring parseline = buffer;
		vector<wstring> values = Convert::ToVector(parseline);
		if (values.size() < 7)
		{
			break;
		}
		int year = 0;
		if (values.size() >= 8)
		{
			year = static_cast<int>(Convert::ToULONG(values[7]));
		}
		Artist * pArtist = GetArtist(Convert::ToULONG(values[1])); // fkeyArtist
		Album * pAlbum = GetAlbum(Convert::ToULONG(values[2])); // fkeyAlbum
		if (pAlbum->GetYear()==0)
		{
			pAlbum->SetYear(year);
		}
		MediaDirectory * pDirectory = GetDirectory(Convert::ToULONG(values[5])); // fkeyDirectory
		AudioFile * pAudioFile = new AudioFile(Convert::ToULONG(values[0]), // pkey
							pArtist,
							pAlbum,
							Convert::ToULONG(values[3]), // trackNumber
							year,
							values[4].c_str(),  // fileName
							pDirectory,
							values[6].c_str()); // title
		m_audioFiles[pAudioFile->GetPKey()] = pAudioFile;
		m_nextPKey = max(pAudioFile->GetPKey()+1, m_nextPKey);
	}
	return !m_mediaDirectories.empty() && !m_artists.empty() && !m_albums.empty() && !m_audioFiles.empty();
}

bool MediaDatabase::IsAudioFile(LPCWSTR fileName) const
{
	const LPCWSTR audioSuffixes[] = { L"mp3", L"m4a", L"wma" };
	return ContainsSuffix(fileName, audioSuffixes, sizeof(audioSuffixes)/sizeof(LPCWSTR));
}

bool MediaDatabase::ContainsSuffix(LPCWSTR fileName, const LPCWSTR suffixes [], int suffixCount) const
{
	wstring fn = fileName;
	int stridx = fn.find_last_of(L".");
	if (stridx<=0)
	{
		return false;
	}
	wstring suffix = fn.substr(stridx+1);
	for (int idx=0; idx < suffixCount; idx++)
	{
		if (_wcsicmp(suffix.c_str(), suffixes[idx])==0)
		{
			return true;
		}
	}
	return false;
}

AudioFile * MediaDatabase::InsertAudioFile(LPCWSTR musicDir, MediaDirectory * pDirectory, LPCWSTR fileName)
{
	try
	{
		wstring absname = musicDir;
		absname.append(pDirectory->GetRelativePath());
		absname.append(L"\\");
		absname.append(fileName);
		TagLib::FileName fn(absname.c_str());
		TagLib::FileRef fileRef = TagLib::FileRef(fn);
		TagLib::Tag * pTag = fileRef.tag();
		wstring artist = pTag->artist().toWString();
		int year = pTag->year();
		size_t idx = artist.find(L"/");
		if (idx != wstring::npos)
		{
			artist = artist.substr(0,idx);
		}
		wstring album = pTag->album().toWString();	
		wstring title = pTag->title().toWString();
		if (album.empty() || artist.empty() || title.empty())
		{
			return NULL;
		}
		Artist * pArtist = InsertArtist(artist.c_str());
		Album * pAlbum = InsertAlbum(album.c_str(), pArtist, pDirectory);
		if (pAlbum->GetYear()==0)
		{
			pAlbum->SetYear(year);
		}
		ULONG pkey = GetNextPkey();
		AudioFile * pAudioFile = new AudioFile(pkey, pArtist, pAlbum, pTag->track(), year, fileName, pDirectory, title.c_str());
		m_audioFiles[pkey] = pAudioFile;
		return pAudioFile;
	}
	catch (...)
	{
		//@TODO: error logging
		return NULL;
	}
}

Artist * MediaDatabase::GetArtist(ULONG pkeyArtist) const
{
	map<ULONG,Artist *>::const_iterator iter = m_artists.find(pkeyArtist);
	if (iter == m_artists.end()) 
	{
		return NULL;
	}
	return iter->second;
}

Album * MediaDatabase::GetAlbum(ULONG pkeyAlbum) const
{
	map<ULONG,Album *>::const_iterator iter = m_albums.find(pkeyAlbum);
	if (iter == m_albums.end()) 
	{
		return NULL;
	}
	return iter->second;
}

MediaDirectory * MediaDatabase::GetDirectory(ULONG pkey) const
{
	map<ULONG,MediaDirectory *>::const_iterator iter = m_mediaDirectories.find(pkey);
	if (iter == m_mediaDirectories.end())
	{
		return NULL;
	}
	return iter->second;
}

AudioFile * MediaDatabase::GetAudioFile(ULONG pkey) const
{
	map<ULONG,AudioFile *>::const_iterator iter = m_audioFiles.find(pkey);
	if (iter == m_audioFiles.end())
	{
		return NULL;
	}
	return iter->second;
}

MediaDirectory * MediaDatabase::InsertDirectory(LPCWSTR directory, bool & dirExists)
{
	map<ULONG,MediaDirectory *>::const_iterator iter = m_mediaDirectories.begin();
	for (; iter != m_mediaDirectories.end(); ++iter)
	{
		if (_wcsicmp(iter->second->GetRelativePath(), directory)==0)
		{
			dirExists = true;
			return iter->second;
		}
	}
	ULONG pkey = GetNextPkey();
	MediaDirectory * pMediaDir = new MediaDirectory(pkey, directory);
	m_mediaDirectories[pkey] = pMediaDir;
	dirExists = false;
	return pMediaDir;
}

Artist * MediaDatabase::InsertArtist(LPCWSTR artist)
{
	map<ULONG,Artist *>::const_iterator iter = m_artists.begin();
	for (; iter != m_artists.end(); ++iter)
	{
		if (_wcsicmp(iter->second->GetName().c_str(), artist)==0)
		{
			return iter->second;
		}
	}
	ULONG pkey = GetNextPkey();
	Artist * pArtist = new Artist(pkey,artist);
	m_artists[pkey] = pArtist;
	return pArtist;
}

Album * MediaDatabase::InsertAlbum(LPCWSTR album, Artist * pArtist, MediaDirectory * pDirectory)
{
	map<ULONG,Album *>::const_iterator iter = m_albums.begin();
	for (; iter != m_albums.end(); ++iter)
	{
		if (_wcsicmp(iter->second->GetName().c_str(), album)==0 &&
			pDirectory->GetPKey()==iter->second->GetDirectory()->GetPKey())
		{
			Album * pAlbum = iter->second;
			if (pAlbum->GetAlbumArtist()->GetPKey() != pArtist->GetPKey())
			{
				pAlbum->SetAlbumArtist(m_artists[1]); // Various
			}
			return pAlbum;
		}
	}
	ULONG pkey = GetNextPkey();
	Album * pAlbum = new Album(pkey,album,pArtist,pDirectory);
	m_albums[pkey] = pAlbum;
	return pAlbum;
}

ULONG MediaDatabase::GetNextPkey()
{
	return ++m_nextPKey;
}

vector<MediaDirectory *> MediaDatabase::GetDirectories(void) const
{
	vector<MediaDirectory *> ret;
	map<ULONG, MediaDirectory *>::const_iterator iter = m_mediaDirectories.begin();
	for (; iter != m_mediaDirectories.end(); ++iter)
	{
		ret.push_back(iter->second);
	}
	return ret;
}

bool sortalbums(Album * pA1, Album * pA2)
{
	int c = _wcsicmp(pA1->GetAlbumArtist()->GetName().c_str(),  pA2->GetAlbumArtist()->GetName().c_str());
	if (c==0)
	{
		return _wcsicmp(pA1->GetName().c_str(), pA2->GetName().c_str()) < 0;
	}
	return c < 0;
}

vector<Album *> MediaDatabase::GetAlbums(void) const
{
	vector<Album *> ret;
	map<ULONG,Album *>::const_iterator iter = m_albums.begin();
	for (; iter != m_albums.end(); ++iter)
	{		
		ret.push_back(iter->second);
	}
	sort(ret.begin(), ret.end(), sortalbums);
	return ret;
}

vector<AudioFile *> MediaDatabase::GetAudioFiles(const Album & album) const
{
	vector<AudioFile *> ret;
	map<ULONG,AudioFile *>::const_iterator iter = m_audioFiles.begin();
	for (; iter != m_audioFiles.end(); ++iter)
	{
		AudioFile * pAudioFile = iter->second;
		if (pAudioFile->GetAlbum()->GetPKey() == album.GetPKey())
		{
			ret.push_back(pAudioFile);
		}
	}
	return ret;
}

typedef struct struct_Keyword
{
	wstring keyword;
	double weight;
} Keyword;

typedef struct struct_ScoredAudioFile
{
	AudioFile * pAudioFile;
	double score;
	bool operator < (const struct struct_ScoredAudioFile & o)
	{
		return o.score < score;
	}
} ScoredAudioFile;

void AddWeightedKeywords(vector<Keyword> & keywords, LPCWSTR word, double weight)
{
	// @TODO:
	// find all words, get word propability p := #occurance / #total
	// => weight = weight / (p * total) ?
	CString n = word;
	n = n.MakeLower();
	int iStart = 0;
	int cnt = 0;
	for (int iStart = 0; iStart >= 0;)
	{
		CString token = n.Tokenize(L" ,", iStart);
		if (token.GetLength()>0)
		{
			Keyword keyword;
			keyword.keyword = (LPCWSTR)token;
			weight = weight * (1.0 / (double)(cnt+1));
			keyword.weight = weight;
			keywords.push_back(keyword);
			cnt++;
		}
	}
}

double Score(vector<Keyword> & keywords, LPCWSTR search)
{
	CString searchStr(search);
	int cnt=0;
	bool found = false;
	double weight = 0.0;
	for (int iStart = 0; iStart >=0; )
	{
		CString s = searchStr.Tokenize(L" ,", iStart);
		if (s.GetLength()==0) continue;
		cnt++;
		found = false;
		for (vector<Keyword>::const_iterator iter = keywords.begin(); iter != keywords.end(); iter++)
		{
			const Keyword & keyword = *iter;
			LPCWSTR wstr = keyword.keyword.c_str();
			if (_wcsnicmp(wstr, (LPCWSTR)s, s.GetLength())==0)
			{
				found = true;
				weight += keyword.weight;
				break;
			}
		}
		if (!found)
		{
			return -1.0;
		}
	}
	if (found)
	{
		return weight;
	}
	return -1.0;
}

vector<AudioFile *> MediaDatabase::Search(LPCWSTR search, UINT maxResults, CEvent * pCancelEvent) const
{
	vector<AudioFile *> results;
	map<ULONG,AudioFile *>::const_iterator iter = m_audioFiles.begin();
	vector<ScoredAudioFile> scoredAudioFiles;

	for (; iter != m_audioFiles.end(); ++iter)
	{
		if (::WaitForSingleObject(pCancelEvent->m_hObject, 0)==WAIT_OBJECT_0)
		{
			return results;
		}
		// get title, album and artist
		AudioFile * pAudioFile = iter->second;
		Artist * pArtist = pAudioFile->GetArtist();
		Album  * pAlbum = pAudioFile->GetAlbum();
		wstring title = pAudioFile->GetTitle();
		wstring year = Convert::ToString(pAudioFile->GetYear());
		// extract keywords
		vector<Keyword> keywords;
		AddWeightedKeywords(keywords, title.c_str(), 0.1);
		AddWeightedKeywords(keywords, year.c_str(), 0.1);
		AddWeightedKeywords(keywords, pAlbum->GetName().c_str(), 0.2);
		AddWeightedKeywords(keywords, pArtist->GetName().c_str(), 0.6);
		double score = Score(keywords, search);
		if (score > 0.0)
		{
			ScoredAudioFile scoredAudioFile;
			scoredAudioFile.pAudioFile = pAudioFile;
			scoredAudioFile.score = score;
			scoredAudioFiles.push_back(scoredAudioFile);
		}
	}
	// sort
	sort(scoredAudioFiles.begin(),scoredAudioFiles.end());
	size_t cnt=0;
	for (vector<ScoredAudioFile>::const_iterator iter = scoredAudioFiles.begin(); iter != scoredAudioFiles.end() && cnt < maxResults; iter++)
	{
		AudioFile * pAudioFile = iter->pAudioFile;
		bool exists = false;
		for (vector<AudioFile *>::const_iterator resIter = results.begin(); resIter != results.end(); resIter++)
		{
			if ((*resIter)->GetAlbum()->GetPKey() == pAudioFile->GetAlbum()->GetPKey())
			{
				exists = true;
				break;
			}
		}
		if (!exists)
		{
			results.push_back(pAudioFile);
			cnt++;
		}
	}
	return results;
}

AudioFile * MediaDatabase::FindTitle(LPCWSTR albumName, LPCWSTR title) const
{
	for (map<ULONG,AudioFile *>::const_iterator iter = m_audioFiles.begin(); iter != m_audioFiles.end(); iter++)
	{
		AudioFile * pAudioFile = iter->second;
		if (pAudioFile->GetTitle() == title)
		{
			if (pAudioFile->GetAlbum()->GetName() == albumName)
			{
				return pAudioFile;
			}
		}
	}
	return NULL;
}

void MediaDatabase::RemoveDirectory(MediaDirectory * pRemoveDir)
{
	map<ULONG, MediaDirectory *>::iterator dirIter = m_mediaDirectories.find(pRemoveDir->GetPKey());
	if (dirIter != m_mediaDirectories.end())
	{
		MediaDirectory * pDirectory = dirIter->second;
		for (map<ULONG, Album *>::iterator albumIter = m_albums.begin(); albumIter != m_albums.end();)
		{
			Album * pAlbum = albumIter->second;
			if (pAlbum->GetDirectory()->GetPKey() == pDirectory->GetPKey())
			{
				for (map<ULONG, AudioFile *>::iterator fileIter = m_audioFiles.begin(); fileIter != m_audioFiles.end();)
				{
					AudioFile * pAudioFile = fileIter->second;
					if (pAudioFile->GetAlbum()->GetPKey() == pAlbum->GetPKey())
					{
						m_audioFiles.erase(fileIter++);
						delete pAudioFile;
					}
					else
					{
						++fileIter;
					}
				}
				m_albums.erase(albumIter++);
				delete pAlbum;
			}
			else
			{
				++albumIter;
			}
		}
		m_mediaDirectories.erase(dirIter);
		delete pDirectory;
	}
}
