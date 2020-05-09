#pragma once

#include "StdAfx.h"
#include <vector>
#include "MediaDatabase.h"
#include "AudioFile.h"

// Represents an audio file play list.
class PlayList
{
public:
	// Constructs an empty play list.
	PlayList();

	// Destroys the play list.
	~PlayList();

	// Returns the audio files of the play list.
	std::vector<AudioFile *> GetAudioFiles(void) const;

	// Returns whether the play list is empty.
	bool Empty(void) const;

	// Removes the first audio file of the play list.
	AudioFile * RemoveFirst(void);

	// Clears the play list:
	void Clear(void);

	// Adds an audio file to the play list.
	void AddAudioFile(AudioFile * pAudioFile);

	// Returns the audio file for the given index.
	AudioFile * GetAudioFile(int audioFileIdx) const;

	// Returns the number of audio files in the play list.
	int GetAudioFileCount(void) const;

	// Loads the play list from a file.
	void Load(LPCWSTR szFileName, const MediaDatabase & mediaDatabase);

	// Saves the play list to a file.
	void Save(LPCWSTR szFileName) const;

private:
	// audio files
	std::vector<AudioFile *> m_audioFiles;
};
