#pragma once

#include <string>

// Represents application settings.
class Settings
{
public:
	bool isReadOnlyMode(void) const;

	// Constructs the settings.
	Settings(void);

	// Destroys the settings.
	~Settings(void);

	// Returns whether the settings are new (first application start).
	bool IsNew(void) const;

	// Sets the music directory.
	void SetMusicDirectory(LPCWSTR musicDir);

	// Returns the music directory.
	const std::wstring & GetMusicDirectory(void) const;

	// Returns the database directory.
	const std::wstring & GetDatabaseDirectory(void) const;

	// Returns the current album index.
	int GetCurrentAlbumIndex(void) const;

	// Sets the current album index
	void SetCurrentAlbumIndex(int albumIdx);

	// Saves the settings.
	void Save(void) const;

private:
	// Loads the settings from the settings file
	void Load(void);
	// Creates the directory if it does not exist.
	static void CreateDirectory(LPCWSTR directory);

	// Returns the local application directory (known folder).
	std::wstring GetKnownFolderLocalApp(void) const;
	// Returns the windows music directory (known folder).
	std::wstring GetKnownFolderMusic(void) const;

	// current album index
	int m_albumIdx;
	// music directory
	std::wstring m_musicDirectory;
	// database directory
	std::wstring m_databaseDirectory;
	// flag whether the settings are new
	bool m_new;
	// flag whether readonly mode is activated (fixed music directory)
	bool m_readOnlyMode;
};

