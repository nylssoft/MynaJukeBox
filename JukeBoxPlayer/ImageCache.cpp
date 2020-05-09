#include "StdAfx.h"
#include "ImageCache.h"
#include <sstream>
#include "MediaDirectory.h"
#include "Artist.h"

using namespace std;

UINT __cdecl ImageCacheThreadProc( LPVOID lpParameter )
{
	ImageCache * pImageCache = static_cast<ImageCache *>(lpParameter);
	pImageCache->DoWork();
	::AfxEndThread( 0, FALSE ); 
	return 0L;
}

ImageCache::ImageCache(MediaDatabase * pMediaDatabase)
	: m_pMediaDatabase(pMediaDatabase), m_pWinThread(NULL)
{
	CreateLoadingBitmaps();
}


ImageCache::~ImageCache(void)
{
}

HBITMAP ImageCache::GetLoadingCover()
{
	return m_hLoadingCover;
}

HBITMAP ImageCache::GetLoadingThumbnail()
{
	return m_hLoadingThumbnail;
}

void ImageCache::CreateLoadingBitmaps()
{
	CImage coverImage;
	coverImage.Create(200,200,32);
	CDC * pDC = CDC::FromHandle(coverImage.GetDC());
	pDC->FillSolidRect(0,0,200,200,RGB(255,255,255));
	CRect crect(20,20,180,180);
	CString txt = L"Loading...";
	pDC->DrawText(txt, &crect, DT_WORDBREAK);
	pDC->LineTo(0,0);
	pDC->LineTo(0,199);
	pDC->LineTo(199,199);
	pDC->LineTo(199,0);
	pDC->LineTo(0,0);
	CImage thumbnailImage;
	thumbnailImage.Create(100,100, coverImage.GetBPP());
	HDC hdc = thumbnailImage.GetDC();
	::SetStretchBltMode(hdc, HALFTONE);
	coverImage.StretchBlt(hdc,0,0,100,100);
	thumbnailImage.ReleaseDC();
	coverImage.ReleaseDC();
	m_hLoadingCover = coverImage.Detach();
	m_hLoadingThumbnail = thumbnailImage.Detach();
}

bool ImageCache::ContainsCover(const Album & album)
{
	CSingleLock lock(&m_cs, true);
	map<long,HBITMAP>::const_iterator findIter = m_coverCache.find(album.GetPKey());
	return findIter != m_coverCache.end();
}

bool ImageCache::ContainsThumbnail(const Album & album)
{
	CSingleLock lock(&m_cs, true);
	map<long,HBITMAP>::const_iterator findIter = m_thumbnailCache.find(album.GetPKey());
	return findIter != m_thumbnailCache.end();
}

HBITMAP ImageCache::GetCover(const Album & album)
{
	CSingleLock lock(&m_cs, true);
	return m_coverCache[album.GetPKey()];
}

HBITMAP ImageCache::GetThumbnail(const Album & album)
{
	CSingleLock lock(&m_cs, true);
	return m_thumbnailCache[album.GetPKey()];
}

bool ImageCache::IsLoadingBitmap(HBITMAP hbitmap)
{
	return hbitmap == m_hLoadingCover || hbitmap == m_hLoadingThumbnail;
}

void ImageCache::LoadAsync(const Album & album)
{
	CSingleLock lock(&m_cs, true);
	m_loadQueue.push(&album);
	if (m_pWinThread == NULL)
	{
		m_pWinThread = ::AfxBeginThread( ImageCacheThreadProc, this, 0, 0, CREATE_SUSPENDED, NULL);
		m_pWinThread->m_bAutoDelete = FALSE; 
		m_pWinThread->ResumeThread();
	}
}

void ImageCache::DoWork()
{
	while (true)
	{
		while (true)
		{
			CSingleLock lock(&m_cs, true);
			if (m_loadQueue.empty())
			{
				break;
			}
			const Album * pAlbum = m_loadQueue.top();
			m_loadQueue.pop();
			map<long,HBITMAP>::const_iterator findIter = m_coverCache.find(pAlbum->GetPKey());
			if (findIter != m_coverCache.end())
			{
				continue;
			}
			lock.Unlock();

			HBITMAP hCover;
			HBITMAP hThumbnail;

			wostringstream wosfolder;
			wosfolder << m_pMediaDatabase->GetSettings()->GetMusicDirectory() << L"\\" << pAlbum->GetDirectory()->GetRelativePath() << L"\\folder.jpg";
			wstring folderFile = wosfolder.str();
			DWORD dwFileAttr = ::GetFileAttributes(folderFile.c_str());
			if (dwFileAttr != INVALID_FILE_ATTRIBUTES)
			{
				CImage image;
				image.Load(folderFile.c_str());
				CImage coverImage;
				coverImage.Create(200,200,image.GetBPP());
				HDC hdcCover = coverImage.GetDC();
				::SetStretchBltMode(hdcCover, HALFTONE);
				image.StretchBlt(hdcCover,0,0,200,200);
				coverImage.ReleaseDC();
				CImage thumbnailImage;
				thumbnailImage.Create(100,100,image.GetBPP());
				HDC hdc = thumbnailImage.GetDC();
				::SetStretchBltMode(hdc, HALFTONE);
				image.StretchBlt(hdc,0,0,100,100);
				thumbnailImage.ReleaseDC();
				hCover = coverImage.Detach();
				hThumbnail = thumbnailImage.Detach();
			}
			else
			{
				CImage coverImage;
				coverImage.Create(200,200,32);
				CDC * pDC = CDC::FromHandle(coverImage.GetDC());
				pDC->FillSolidRect(0,0,200,200,RGB(255,255,255));
				CRect crect(20,20,180,180);
				CString txt = pAlbum->GetAlbumArtist()->GetName().c_str();
				pDC->DrawText(txt, &crect, DT_WORDBREAK);
				txt = pAlbum->GetName().c_str();
				crect.MoveToY(40);
				pDC->DrawText(txt, &crect, DT_WORDBREAK);
				pDC->LineTo(0,0);
				pDC->LineTo(0,199);
				pDC->LineTo(199,199);
				pDC->LineTo(199,0);
				pDC->LineTo(0,0);
				CImage thumbnailImage;
				thumbnailImage.Create(100,100, coverImage.GetBPP());
				HDC hdc = thumbnailImage.GetDC();
				::SetStretchBltMode(hdc, HALFTONE);
				coverImage.StretchBlt(hdc,0,0,100,100);
				thumbnailImage.ReleaseDC();
				coverImage.ReleaseDC();
				hCover = coverImage.Detach();
				hThumbnail = thumbnailImage.Detach();
			}
			lock.Lock();
			m_coverCache[pAlbum->GetPKey()] = hCover;
			m_thumbnailCache[pAlbum->GetPKey()] = hThumbnail;
			lock.Unlock();
		}
		::Sleep(100);
	}
}