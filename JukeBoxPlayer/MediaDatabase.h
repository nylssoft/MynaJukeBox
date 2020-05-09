#pragma once

#include "StdAfx.h"
#include <map>
#include <vector>

#include "AudioFile.h"
#include "Artist.h"
#include "Album.h"
#include "MediaDirectory.h"
#include "Settings.h"

// Represents the database for all audio media information.
class MediaDatabase
{
public:
	// Constructs an empty database.
	MediaDatabase(Settings * pSettings);

	// Destroys the database.
	~MediaDatabase(void);

	// Returns the base directory.
	Settings * GetSettings(void) const;

	// Clears all metadata.
	void Clear(void);

	// Loads the metadata for all audio media files.
	bool Load(void);

	// Saves the metadata for all audio media files.
	void Save(void) const;

	// Returns the artist with the given key or NULL.
	Artist * GetArtist(ULONG pkeyArtist) const;

	// Returns the album with the given key or NULL.
	Album * GetAlbum(ULONG pkeyAlbum) const;

	// Returns the audio file with the given key or NULL.
	AudioFile * GetAudioFile(ULONG pkeyAudioFile) const;

	// Returns the media directory with the given key or NULL.
	MediaDirectory * GetDirectory(ULONG pkeyDirectory) const;

	// Searches for audio files that matches the given search string.
	std::vector<AudioFile *> Search(LPCWSTR search, UINT maxResults, CEvent * pCancelEvent) const;

	// Finds the audio file with the given album and title.
	AudioFile * FindTitle(LPCWSTR album, LPCWSTR title) const;

	// Returns all media directories of the database.
	std::vector<MediaDirectory *> GetDirectories(void) const;

	// Returns all albums of the database.
	std::vector<Album *> GetAlbums(void) const;

	// Returns the audio files for the specified album.
	std::vector<AudioFile *> GetAudioFiles(const Album & album) const;

	// Returns the total number of audio files.
	long GetAudioFileCount(void) const;

	// Returns the total number of albums.
	long GetAlbumCount(void) const;

	// Returns whether the specified file name represents an audio file.
	bool IsAudioFile(LPCWSTR fileName) const;

	// Inserts a new directory into the database if it does not exist.
	// Returns the existing directory if the directory already exists.
	// The parameter 'dirExists' is set to 'true' if the directory already exists; false otherwise.
	MediaDirectory * InsertDirectory(LPCWSTR directory, bool & dirExists);

	// Inserts an audio file into the database.
	// Returns NULL if the metadata cannot be extracted from the specified file name.
	AudioFile * InsertAudioFile(LPCWSTR musicDir, MediaDirectory * pDirectory, LPCWSTR fileName);

	// Inserts an album into the database.
	// Returns the inserted album if it does not exist; otherwise the existing album is returned.
	Album * InsertAlbum(LPCWSTR album, Artist * pArtist, MediaDirectory * pDirectory);

	// Inserts an artist into the database.
	// Returns the inserted artist if if it does not exist; otherwise the existing artist is returned.
	Artist * InsertArtist(LPCWSTR artist);

	// Removes a media directory and all dependend audio files from the database.
	void RemoveDirectory(MediaDirectory * pDirectory);

private:
	// Hides copy constructor.
	MediaDatabase(const MediaDatabase &);
	// Returns the next available pkey.
	ULONG GetNextPkey(void);
	// Returns whether the specified filename starts with one of the suffixes.
	bool ContainsSuffix(LPCWSTR fileName, const LPCWSTR suffixes [], int suffixCount) const;

	// maps a PKEY to an Artist
	std::map<ULONG,Artist *> m_artists;
	// maps a PKEY to an Album
	std::map<ULONG,Album *> m_albums;
	// maps a PKEY to a MediaDirectory
	std::map<ULONG,MediaDirectory *> m_mediaDirectories;
	// maps a PKEY to an AudioFile
	std::map<ULONG,AudioFile *> m_audioFiles;
	// the next unused PKEY
	ULONG m_nextPKey;
	// settings, e.g. music directory, database directory
	Settings * m_pSettings;
};
