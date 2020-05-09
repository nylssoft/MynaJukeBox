#pragma once

#include "Album.h"
#include "MediaDatabase.h"
#include <map>
#include <stack>

class ImageCache
{
public:
	ImageCache(MediaDatabase * pMediaDatabase);

	~ImageCache(void);

	HBITMAP GetCover(const Album & album);

	HBITMAP GetThumbnail(const Album & album);

	bool ContainsCover(const Album & album);

	bool ContainsThumbnail(const Album & album);

	HBITMAP GetLoadingCover(void);

	HBITMAP GetLoadingThumbnail(void);

	bool IsLoadingBitmap(HBITMAP hbitmap);

	void LoadAsync(const Album & album);

	void DoWork();

private:
	void CreateLoadingBitmaps();

	HBITMAP m_hLoadingCover;
	HBITMAP m_hLoadingThumbnail;

	std::map<long, HBITMAP> m_coverCache;
	std::map<long, HBITMAP> m_thumbnailCache;
	CWinThread * m_pWinThread;
	std::stack<const Album *> m_loadQueue;
	MediaDatabase * m_pMediaDatabase;
	CCriticalSection m_cs;
};
