
// JukeBoxPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JukeBoxPlayer.h"
#include "JukeBoxPlayerDlg.h"
#include "afxdialogex.h"
#include "JukeBoxHost.h"
#include "SettingsDlg.h"
#include "Convert.h"

#include "AudioFileScanner.h"
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <fstream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CJukeBoxPlayerDlg dialog

CJukeBoxPlayerDlg::CJukeBoxPlayerDlg(IWMPPlayer * pWMPPlayer)
:	CDialogEx(CJukeBoxPlayerDlg::IDD, NULL), m_pWMPPlayer(pWMPPlayer), m_pMediaDatabase(NULL),
	m_albumIdx(0), m_restartPlayList(false), m_pSinglePlay(NULL), m_pSearcher(NULL), m_pPlayAlbum(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_APPLICATION);
	HRESULT hr = pWMPPlayer->get_controls(&m_pWMPControls);
}

void CJukeBoxPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_PLAY, m_playProgressCtrl);
}

BEGIN_MESSAGE_MAP(CJukeBoxPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_ALBUM_TRACK_LIST, &CJukeBoxPlayerDlg::OnLbnDblclkAlbumTrackList)
	ON_BN_CLICKED(IDC_ADD_PLAYLIST_BUTTON, &CJukeBoxPlayerDlg::OnBnClickedAddPlaylistButton)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CJukeBoxPlayerDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_RIGHT_BUTTON, &CJukeBoxPlayerDlg::OnBnClickedRightButton)
	ON_BN_CLICKED(IDC_LEFT_BUTTON, &CJukeBoxPlayerDlg::OnBnClickedLeftButton)
	ON_BN_CLICKED(IDC_REMOVE_PLAYLIST_BUTTON, &CJukeBoxPlayerDlg::OnBnClickedRemovePlaylistButton)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CJukeBoxPlayerDlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CJukeBoxPlayerDlg::OnBnClickedButtonSave)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CJukeBoxPlayerDlg::OnEnChangeEditSearch)
	ON_BN_CLICKED(IDC_MFCBUTTON_STOP, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonStop)
	ON_BN_CLICKED(IDC_MFCBUTTON_PAUSE, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonPause)
	ON_BN_CLICKED(IDC_MFCBUTTON_PLAY, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonPlay)
	ON_EN_SETFOCUS(IDC_EDIT_SEARCH, &CJukeBoxPlayerDlg::OnEnSetfocusEditSearch)
	ON_EN_KILLFOCUS(IDC_EDIT_SEARCH, &CJukeBoxPlayerDlg::OnEnKillfocusEditSearch)
	ON_LBN_DBLCLK(IDC_LIST_SEARCHRESULT, &CJukeBoxPlayerDlg::OnLbnDblclkListSearchresult)
	ON_LBN_SELCHANGE(IDC_LIST_SEARCHRESULT, &CJukeBoxPlayerDlg::OnLbnSelchangeListSearchresult)
	ON_BN_CLICKED(IDC_ADD_ALL_PLAYLIST_BUTTON2, &CJukeBoxPlayerDlg::OnBnClickedAddAllPlaylistButton2)
	ON_BN_CLICKED(IDC_REMOVE_ALL_PLAYLIST_BUTTON2, &CJukeBoxPlayerDlg::OnBnClickedRemoveAllPlaylistButton2)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_IMAGE_THUMB9, &CJukeBoxPlayerDlg::OnStnClickedImageThumb9)
	ON_STN_CLICKED(IDC_IMAGE_THUMB1, &CJukeBoxPlayerDlg::OnStnClickedImageThumb1)
	ON_STN_CLICKED(IDC_IMAGE_THUMB2, &CJukeBoxPlayerDlg::OnStnClickedImageThumb2)
	ON_STN_CLICKED(IDC_IMAGE_THUMB3, &CJukeBoxPlayerDlg::OnStnClickedImageThumb3)
	ON_STN_CLICKED(IDC_IMAGE_THUMB4, &CJukeBoxPlayerDlg::OnStnClickedImageThumb4)
	ON_STN_CLICKED(IDC_IMAGE_THUMB6, &CJukeBoxPlayerDlg::OnStnClickedImageThumb6)
	ON_STN_CLICKED(IDC_IMAGE_THUMB7, &CJukeBoxPlayerDlg::OnStnClickedImageThumb7)
	ON_STN_CLICKED(IDC_IMAGE_THUMB8, &CJukeBoxPlayerDlg::OnStnClickedImageThumb8)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CJukeBoxPlayerDlg::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CJukeBoxPlayerDlg::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_MFCBUTTON_CLEAR, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonClear)
	ON_BN_CLICKED(IDC_MFCBUTTON_PLAYALBUM, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonPlayalbum)
	ON_BN_CLICKED(IDC_MFCBUTTON_PREV, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonPrev)
	ON_BN_CLICKED(IDC_MFCBUTTON_NEXT, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonNext)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORWARD, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonForward)
	ON_STN_CLICKED(IDC_STATIC_REMAINING_SONGS, &CJukeBoxPlayerDlg::OnStnClickedStaticRemainingSongs)
	ON_BN_CLICKED(IDC_MFCBUTTON_PAUSECONT, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonPausecont)
	ON_BN_CLICKED(IDC_MFCBUTTON_SHUFFLE, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonShuffle)
	ON_BN_CLICKED(IDC_MFCBUTTON_OPENPLAYLIST, &CJukeBoxPlayerDlg::OnBnClickedMfcbuttonOpenplaylist)
END_MESSAGE_MAP()


// CJukeBoxPlayerDlg message handlers

BOOL CJukeBoxPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_pMediaDatabase = new MediaDatabase(new Settings());
	m_pSearcher = new AudioFileSearcher(m_pMediaDatabase);
	m_pImageCache = new ImageCache(m_pMediaDatabase);
	if (!m_pMediaDatabase->Load())
	{
		if (!m_pMediaDatabase->GetSettings()->isReadOnlyMode())
		{
			CSettingsDlg settingsDlg(m_pMediaDatabase);
			settingsDlg.DoModal();
		}
	}
	else
	{
		if (!m_pMediaDatabase->GetSettings()->isReadOnlyMode())
		{
			CSettingsDlg settingsDlg(m_pMediaDatabase,true);
			settingsDlg.DoModal();
		}
	}
	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			if (!m_pMediaDatabase->GetSettings()->isReadOnlyMode())
			{
				pSysMenu->AppendMenu(MF_SEPARATOR);
				pSysMenu->AppendMenu(MF_STRING, IDM_SETTINGS, L"Settings...");
			}
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	m_albums = m_pMediaDatabase->GetAlbums();	
	m_albumIdx = m_pMediaDatabase->GetSettings()->GetCurrentAlbumIndex();
	if (m_albumIdx < 0 || m_albumIdx >= static_cast<int>(m_albums.size()))
	{
		m_albumIdx = 0;
	}
	CWnd * pListBox = GetDlgItem(IDC_LIST_SEARCHRESULT);
	SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_RESETCONTENT);
	wstring maxstr;
	for (vector<Album *>::const_iterator iter = m_albums.begin(); iter != m_albums.end(); iter++)
	{
		Album * pAlbum = *iter;
		vector<AudioFile *> audiofiles = m_pMediaDatabase->GetAudioFiles(*pAlbum);
		if (!audiofiles.empty())
		{
			AudioFile * pAudioFile = audiofiles.front();
			wstring str = pAudioFile->GetAlbum()->GetAlbumArtist()->GetName() + L" - "+ pAlbum->GetName() + L" - "+pAudioFile->GetTitle();
			LRESULT idx = SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_ADDSTRING, 0, (LPARAM)str.c_str()); 
			SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_SETITEMDATA, (WPARAM)idx, (LPARAM)pAudioFile->GetPKey());
			if (str.length() > maxstr.length())
			{
				maxstr = str;
			}
		}
	}
	SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_SETHORIZONTALEXTENT, pListBox->GetDC()->GetTextExtent(maxstr.c_str()).cx, 0);
	UpdateAlbum();
	// update player
	SetTimer(IDT_CHECK_PLAYSTATE, 1000, NULL);
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	m_hBlackBrush = CreateSolidBrush(RGB(0,0,0));
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJukeBoxPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == IDM_SETTINGS)
	{
		CSettingsDlg dlgSettings(m_pMediaDatabase);
		if (dlgSettings.DoModal() == IDOK)
		{
			SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_RESETCONTENT);
			GetDlgItem(IDC_EDIT_SEARCH)->SetWindowText(L"");
			const int thumbIds[] = { IDC_IMAGE_THUMB1, IDC_IMAGE_THUMB2, IDC_IMAGE_THUMB3, IDC_IMAGE_THUMB4, IDC_IMAGE_THUMB5,
			IDC_IMAGE_THUMB6, IDC_IMAGE_THUMB7, IDC_IMAGE_THUMB8, IDC_IMAGE_THUMB9};
			for (int idx=0; idx < sizeof(thumbIds) / sizeof(int); idx++)
			{
				SendDlgItemMessage(thumbIds[idx], STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)NULL);
			}
			SendDlgItemMessage(IDC_IMAGE_PICTURE, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)NULL);
			m_albumIdx = 0;
			m_albums = m_pMediaDatabase->GetAlbums();
			m_playList.Clear();
			m_restartPlayList = false;
			m_pSinglePlay = NULL;
			// @TODO: clean up image cache
			UpdateAlbum();
		}
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CJukeBoxPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CJukeBoxPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CJukeBoxPlayerDlg::UpdateAlbum(void)
{
	UpdateThumbnails();
	int flag = SW_SHOW;
	if (static_cast<size_t>(m_albumIdx) < m_albums.size())
	{
		Album * pAlbum = m_albums[m_albumIdx];
		GetDlgItem(IDC_ARTIST_LABEL)->SetWindowText(pAlbum->GetAlbumArtist()->GetName().c_str());
		GetDlgItem(IDC_ALBUM_LABEL)->SetWindowText(pAlbum->GetName().c_str());
		GetDlgItem(IDC_STATIC_YEAR)->SetWindowText(Convert::ToString(pAlbum->GetYear()).c_str());
		wstringstream wss;
		wss << (m_albumIdx+1) << L" / " << m_albums.size();
		GetDlgItem(IDC_STATIC_ALBUM_POS)->SetWindowText(wss.str().c_str());
		UpdateAudioFiles(*pAlbum);
	}
	else
	{
		m_audioFiles.clear();
		flag = SW_HIDE;
		GetDlgItem(IDC_ARTIST_LABEL)->SetWindowText(L"");
		GetDlgItem(IDC_ALBUM_LABEL)->SetWindowText(L"");
		GetDlgItem(IDC_STATIC_YEAR)->SetWindowText(L"");
		GetDlgItem(IDC_STATIC_ALBUM_POS)->SetWindowText(L"");
		GetDlgItem(IDC_STATIC_TRACK_COUNT)->SetWindowText(L"");
		SendDlgItemMessage(IDC_ALBUM_TRACK_LIST, LB_RESETCONTENT);
		SendDlgItemMessage(IDC_IMAGE_PICTURE, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)NULL);
	}
	GetDlgItem(IDC_MFCBUTTON_PLAYALBUM)->ShowWindow(flag);
	GetDlgItem(IDC_MFCBUTTON_NEXT)->ShowWindow(flag);
	GetDlgItem(IDC_MFCBUTTON_PREV)->ShowWindow(flag);
}

void CJukeBoxPlayerDlg::UpdateAudioFiles(const Album & album)
{
	HBITMAP hbitmap;
	if (m_pImageCache->ContainsCover(album))
	{
		hbitmap = m_pImageCache->GetCover(album);
	}
	else
	{
		m_pImageCache->LoadAsync(album);
		hbitmap = m_pImageCache->GetLoadingCover();
	}
	SendDlgItemMessage(IDC_IMAGE_PICTURE, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmap);
	// set audio files in list box
	SendDlgItemMessage(IDC_ALBUM_TRACK_LIST, LB_RESETCONTENT);
	m_audioFiles = m_pMediaDatabase->GetAudioFiles(album);
	vector<AudioFile *>::const_iterator iter = m_audioFiles.begin();
	for (; iter != m_audioFiles.end(); ++iter)
	{
		AudioFile * pAudioFile = *iter;
		wstring str = pAudioFile->GetTitle() + L" - " + pAudioFile->GetArtist()->GetName();
		if (pAudioFile->GetYear()>0)
		{
			str += L" - " + Convert::ToString(pAudioFile->GetYear());
		}
		int idx = SendDlgItemMessage(IDC_ALBUM_TRACK_LIST, LB_ADDSTRING, 0, (LPARAM)str.c_str());
		SendDlgItemMessage(IDC_ALBUM_TRACK_LIST, LB_SETITEMDATA, (WPARAM)idx, (LPARAM)pAudioFile->GetPKey());
	}
	wstringstream wss;
	wss << m_audioFiles.size() << L" Track" << (m_audioFiles.size()>1 ? L"s" : L"");
	GetDlgItem(IDC_STATIC_TRACK_COUNT)->SetWindowText(wss.str().c_str());
	Invalidate();
}

void CJukeBoxPlayerDlg::UpdateThumbnails(void)
{
	const int thumbIds[] = { IDC_IMAGE_THUMB1, IDC_IMAGE_THUMB2, IDC_IMAGE_THUMB3, IDC_IMAGE_THUMB4, IDC_IMAGE_THUMB5,
	IDC_IMAGE_THUMB6, IDC_IMAGE_THUMB7, IDC_IMAGE_THUMB8, IDC_IMAGE_THUMB9};
	for (int idx=0; idx <  sizeof(thumbIds) / sizeof(int) ; idx++)
	{
		SendDlgItemMessage(thumbIds[idx], STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)0L);
	}
	int startidx = 0;
	int endidx = sizeof(thumbIds) / sizeof(int);
	const int sz =  static_cast<int>(m_albums.size());
	if (sz < endidx)
	{
		startidx = endidx/2 - sz/2;
		endidx = startidx + sz;
	}
	for (int idx=startidx; idx < endidx ; idx++)
	{
		int albumIdx = m_albumIdx + idx - 4;
		if (albumIdx < 0)
		{
			albumIdx = sz + albumIdx;
		}
		else if (albumIdx >= sz)
		{
			albumIdx = albumIdx - sz;
		}
		if (albumIdx < 0 || albumIdx >= sz)
		{
			continue;
		}
		Album * pAlbum = m_albums[albumIdx];
		HBITMAP hbitmap;
		if (m_pImageCache->ContainsThumbnail(*pAlbum))
		{
			hbitmap = m_pImageCache->GetThumbnail(*pAlbum);
		}
		else
		{
			m_pImageCache->LoadAsync(*pAlbum);
			hbitmap = m_pImageCache->GetLoadingThumbnail();
		}
		SendDlgItemMessage(thumbIds[idx], STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmap);
	}
}

void CJukeBoxPlayerDlg::UpdatePlayList(void)
{
	int count = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETCOUNT);
	m_playList.Clear();
	for (int idx=0; idx<count; idx++)
	{
		ULONG pkeyAudioFile = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETITEMDATA, idx);
		AudioFile * pAudioFile = m_pMediaDatabase->GetAudioFile(pkeyAudioFile);
		m_playList.AddAudioFile(pAudioFile);
	}
}

void CJukeBoxPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	bool play = false;
	bool updatePlayList = false;
	if (nIDEvent == IDT_CHECK_PLAYSTATE)
	{
		WMPPlayState wmpPlayState;
		if (m_pWMPPlayer->get_playState(&wmpPlayState) == S_OK)
		{
			if (wmpPlayState == wmppsReady || wmpPlayState == wmppsStopped || wmpPlayState == wmppsUndefined)
			{
				play = true;
			}
		}
	}
	if (m_pSinglePlay!=NULL)
	{
		wstring currentPlay = m_pSinglePlay->GetTitle() + L" - " + m_pSinglePlay->GetArtist()->GetName();
		GetDlgItem(IDC_STATIC_NAME)->SetWindowText(currentPlay.c_str());
		MediaDirectory * pMediaDirectory = m_pSinglePlay->GetDirectory();
		std::wstring fname = m_pMediaDatabase->GetSettings()->GetMusicDirectory();
		fname.append(pMediaDirectory->GetRelativePath());
		fname.append(L"\\");
		fname.append(m_pSinglePlay->GetFileName());
		CComBSTR bstr(fname.c_str());
		m_pWMPPlayer->put_URL(bstr);
		play = false;
		m_pSinglePlay = NULL;
	}
	if (m_restartPlayList)
	{
		m_restartPlayList = false;
		play = true;
	}
	if (play)
	{
		updatePlayList = true;
		if (!m_playList.Empty())
		{
			AudioFile * pAudioFile = m_playList.RemoveFirst();
			wstringstream wss;
			wss << pAudioFile->GetTitle() << L" - " << pAudioFile->GetArtist()->GetName();
			GetDlgItem(IDC_STATIC_NAME)->SetWindowText(wss.str().c_str());
			GetDlgItem(IDC_STATIC_PLAY_YEAR)->SetWindowText(Convert::ToString(pAudioFile->GetYear()).c_str());
			MediaDirectory * pMediaDirectory = pAudioFile->GetDirectory();
			std::wstring fname = m_pMediaDatabase->GetSettings()->GetMusicDirectory();
			fname.append(pMediaDirectory->GetRelativePath());
			fname.append(L"\\");
			fname.append(pAudioFile->GetFileName());
			CComBSTR bstr(fname.c_str());
			m_pWMPPlayer->put_URL(bstr);
			m_pPlayAlbum = pAudioFile->GetAlbum();
			HBITMAP hbitmap;
			if (m_pImageCache->ContainsCover(*m_pPlayAlbum))
			{
				hbitmap = m_pImageCache->GetCover(*m_pPlayAlbum);
			}
			else
			{
				m_pImageCache->LoadAsync(*m_pPlayAlbum);
				hbitmap = m_pImageCache->GetLoadingCover();
			}
			SendDlgItemMessage(IDC_IMAGE_PLAY, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmap);
			m_playProgressCtrl.SetRange32(0, 1000);
			m_playProgressCtrl.SetPos(0);
			ShowPlayControls(false);
		}
		else
		{
			m_pPlayAlbum = NULL;
			m_pWMPControls->stop();
			ShowPlayControls(true);
		}
	}
	else if (m_pPlayAlbum!=NULL)
	{
		HBITMAP hbitmap = (HBITMAP)SendDlgItemMessage(IDC_IMAGE_PLAY, STM_GETIMAGE, IMAGE_BITMAP);
		if (m_pImageCache->IsLoadingBitmap(hbitmap) && m_pImageCache->ContainsCover(*m_pPlayAlbum))
		{
			SendDlgItemMessage(IDC_IMAGE_PLAY, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_pImageCache->GetCover(*m_pPlayAlbum));
			m_pPlayAlbum = NULL;
		}
	}
	CComBSTR currentPosition;
	m_pWMPControls->get_currentPositionString(&currentPosition);
	if (currentPosition.Length()==0)
	{
		GetDlgItem(IDC_STATIC_CURRENTPOSITION)->SetWindowText(L"00:00");
	}
	else
	{
		GetDlgItem(IDC_STATIC_CURRENTPOSITION)->SetWindowText(currentPosition);
	}
	CComPtr<IWMPMedia> pMedia;
	HRESULT hr = m_pWMPControls->get_currentItem(&pMedia);
	if (SUCCEEDED(hr) && (IWMPMedia *)pMedia != NULL)
	{
		CComBSTR duration;
		pMedia->get_durationString(&duration);
		CString cstrDuration(duration);
		if (cstrDuration==L"00:00")
		{
			cstrDuration = L"Loading...";
		}
		GetDlgItem(IDC_STATIC_DURATION)->SetWindowText(cstrDuration);
		double dDuration = 0.0;
		double dCurrent = 0.0;
		hr = m_pWMPControls->get_currentPosition(&dCurrent);
		if (SUCCEEDED(hr))
		{
			hr = pMedia->get_duration(&dDuration);
		}
		if (SUCCEEDED(hr) && dCurrent>0.0 && dDuration>0.0)
		{
			m_playProgressCtrl.SetPos((int)((dCurrent / dDuration) * 1000.0));
		}
	}
	else
	{
		GetDlgItem(IDC_STATIC_DURATION)->SetWindowText(L"00:00");
		GetDlgItem(IDC_STATIC_NAME)->SetWindowText(L"Ready");
	}
	if (updatePlayList)
	{
		SendDlgItemMessage(IDC_LIST_CURRENT_PLAY, LB_RESETCONTENT);
		vector<AudioFile *> audioFiles = m_playList.GetAudioFiles();
		for (vector<AudioFile *>::const_iterator iter = audioFiles.begin(); iter != audioFiles.end(); iter++)
		{
			AudioFile * pAudioFile = *iter;
			wstring str = pAudioFile->GetTitle();
			str.append(L" - ");
			str.append(pAudioFile->GetArtist()->GetName());
			SendDlgItemMessage(IDC_LIST_CURRENT_PLAY, LB_ADDSTRING, 0, (LPARAM)str.c_str());
		}
		ShowRemainingSongsControls();
	}

	if (m_pSearcher->IsCompleted())
	{
		CWnd * pListBox = GetDlgItem(IDC_LIST_SEARCHRESULT);
		SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_RESETCONTENT);
		vector<AudioFile *> results = m_pSearcher->GetResults();
		m_albumIdx = 0;
		m_albums.clear();
		bool useAlbumArtist = false;
		if (results.size()==0 && m_pSearcher->GetSearchString().empty())
		{
			useAlbumArtist = true;
			vector<Album *> albums = m_pMediaDatabase->GetAlbums();
			for (vector<Album *>::const_iterator iter = albums.begin(); iter != albums.end(); iter++)
			{
				vector<AudioFile *> audiofiles = m_pMediaDatabase->GetAudioFiles(**iter);
				if (!audiofiles.empty())
				{
					results.push_back(audiofiles.front());
				}
			}
		}
		wstring maxstr;
		for (vector<AudioFile *>::const_iterator iter = results.begin(); iter != results.end(); iter++)
		{
			AudioFile * pAudioFile = *iter;
			wstring str = useAlbumArtist ? pAudioFile->GetAlbum()->GetAlbumArtist()->GetName() : pAudioFile->GetArtist()->GetName();
			str += L" - "+ pAudioFile->GetAlbum()->GetName() + L" - "+pAudioFile->GetTitle();
			LRESULT idx = SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_ADDSTRING, 0, (LPARAM)str.c_str()); 
			SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_SETITEMDATA, (WPARAM)idx, (LPARAM)pAudioFile->GetPKey());
			if (str.length() > maxstr.length())
			{
				maxstr = str;
			}
			m_albums.push_back(pAudioFile->GetAlbum());
		}
		SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_SETHORIZONTALEXTENT, pListBox->GetDC()->GetTextExtent(maxstr.c_str()).cx, 0);
		UpdateAlbum();
	}
	if (!m_albums.empty())
	{
		Album * pCoverAlbum = m_albums[m_albumIdx];
		HBITMAP hbitmap = (HBITMAP)SendDlgItemMessage(IDC_IMAGE_PICTURE, STM_GETIMAGE, IMAGE_BITMAP);
		if (m_pImageCache->IsLoadingBitmap(hbitmap) && m_pImageCache->ContainsCover(*pCoverAlbum))
		{
			SendDlgItemMessage(IDC_IMAGE_PICTURE, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_pImageCache->GetCover(*pCoverAlbum));
		}
		const int thumbIds[] = {	IDC_IMAGE_THUMB1, IDC_IMAGE_THUMB2, IDC_IMAGE_THUMB3, IDC_IMAGE_THUMB4, IDC_IMAGE_THUMB5,
									IDC_IMAGE_THUMB6, IDC_IMAGE_THUMB7, IDC_IMAGE_THUMB8, IDC_IMAGE_THUMB9};
		const int sz =  static_cast<int>(m_albums.size());
		for (int idx=0; idx <  sizeof(thumbIds) / sizeof(int) ; idx++)
		{
			hbitmap = (HBITMAP)SendDlgItemMessage(thumbIds[idx], STM_GETIMAGE, IMAGE_BITMAP, (LPARAM)0L);
			if (m_pImageCache->IsLoadingBitmap(hbitmap))
			{
				int albumIdx = m_albumIdx + idx - 4;
				if (albumIdx < 0)
				{
					albumIdx = sz + albumIdx;
				}
				else if (albumIdx >= sz)
				{
					albumIdx = albumIdx - sz;
				}
				if (albumIdx < 0 || albumIdx >= sz)
				{
					continue;
				}
				Album * pThumbnailAlbum = m_albums[albumIdx];
				if (m_pImageCache->ContainsThumbnail(*pThumbnailAlbum))
				{
					SendDlgItemMessage(thumbIds[idx], STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_pImageCache->GetThumbnail(*pThumbnailAlbum));
				}
			}
		}
	}
}

void CJukeBoxPlayerDlg::ShowPlayControls(bool hide)
{
	int flag = hide ? SW_HIDE : SW_NORMAL;
	GetDlgItem(IDC_STATIC_CURRENTPOSITION)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC_DURATION)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC_REMAINING_SONGS)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC_NAME)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC_PLAY_YEAR)->ShowWindow(flag);
	GetDlgItem(IDC_MFCBUTTON_FORWARD)->ShowWindow(flag);
	GetDlgItem(IDC_IMAGE_PLAY)->ShowWindow(flag);
	GetDlgItem(IDC_LIST_CURRENT_PLAY)->ShowWindow(flag);
	m_playProgressCtrl.ShowWindow(flag);
	GetDlgItem(IDC_MFCBUTTON_PAUSECONT)->ShowWindow(flag);
	GetDlgItem(IDC_MFCBUTTON_CLEAR)->ShowWindow(flag);
	GetDlgItem(IDC_MFCBUTTON_SHUFFLE)->ShowWindow(hide && !m_albums.empty() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_MFCBUTTON_OPENPLAYLIST)->ShowWindow(hide && !m_albums.empty() ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_STATIC_LOSTCLUB)->ShowWindow(hide ? SW_SHOW : SW_HIDE);
}

void CJukeBoxPlayerDlg::ShowRemainingSongsControls()
{
	wstringstream wss;
	int cnt = m_playList.GetAudioFileCount();
	wss << cnt << L" Song" << (cnt > 1 ? L"s" : L"") << L" remaining";
	GetDlgItem(IDC_STATIC_REMAINING_SONGS)->SetWindowText(wss.str().c_str());
	int flag = cnt==0 ? SW_HIDE : SW_NORMAL;
	GetDlgItem(IDC_STATIC_REMAINING_SONGS)->ShowWindow(flag);
	GetDlgItem(IDC_LIST_CURRENT_PLAY)->ShowWindow(flag);
	GetDlgItem(IDC_MFCBUTTON_FORWARD)->ShowWindow(flag);
}

void CJukeBoxPlayerDlg::OnLbnDblclkAlbumTrackList()
{
	int count = SendDlgItemMessage(IDC_ALBUM_TRACK_LIST, LB_GETSELCOUNT);
	if (count>0)
	{
		int * selitems = new int[count];
		SendDlgItemMessage(IDC_ALBUM_TRACK_LIST, LB_GETSELITEMS, count, (LPARAM)selitems);
		// m_pSinglePlay = m_audioFiles[selitems[0]];
		AudioFile * pAudioFile = m_audioFiles[selitems[0]];
		m_playList.AddAudioFile(pAudioFile);
		delete [] selitems;
		wstring str = pAudioFile->GetTitle();
		str.append(L" - ");
		str.append(pAudioFile->GetArtist()->GetName());
		SendDlgItemMessage(IDC_LIST_CURRENT_PLAY, LB_ADDSTRING, 0, (LPARAM)str.c_str());
		ShowRemainingSongsControls();

		wofstream wostream;
		wstring basedir = m_pMediaDatabase->GetSettings()->GetDatabaseDirectory();
		basedir.append(L"\\");
		wstring playlistfilename = basedir + L"Playlist.txt";
		wostream.open(playlistfilename.c_str(), ios_base::out |  ios_base::app);
		wostream << pAudioFile->GetTitle() << L";" << pAudioFile->GetArtist()->GetName() << L";" << pAudioFile->GetAlbum()->GetName() << std::endl;
		wostream.close();
	}
}

void CJukeBoxPlayerDlg::OnBnClickedAddPlaylistButton()
{
	int count = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETCOUNT);
	for (int itemidx = 0; itemidx < count; itemidx++)
	{
		SendDlgItemMessage(IDC_PLAY_LIST, LB_SETSEL, FALSE, itemidx);
	}
	count = SendDlgItemMessage(IDC_ALBUM_TRACK_LIST, LB_GETSELCOUNT);
	int * selitems = new int[count];
	SendDlgItemMessage(IDC_ALBUM_TRACK_LIST, LB_GETSELITEMS, count, (LPARAM)selitems);
	for (int idx=0; idx<count; idx++)
	{
		AudioFile * pAudioFile = m_audioFiles[selitems[idx]];
		wstring title = pAudioFile->GetTitle();
		title.append(L" - ");
		title.append(pAudioFile->GetArtist()->GetName());
		int itemidx = SendDlgItemMessage(IDC_PLAY_LIST, LB_ADDSTRING, 0, (LPARAM)title.c_str());
		SendDlgItemMessage(IDC_PLAY_LIST, LB_SETITEMDATA, (WPARAM)itemidx, (LPARAM)pAudioFile->GetPKey());
		SendDlgItemMessage(IDC_PLAY_LIST, LB_SETSEL, TRUE, itemidx);
	}
	delete selitems;
	SendDlgItemMessage(IDC_ALBUM_TRACK_LIST, LB_SETSEL, (WPARAM)false, (LPARAM)-1);
}

void CJukeBoxPlayerDlg::OnBnClickedAddAllPlaylistButton2()
{
	for (size_t idx=0; idx<m_audioFiles.size(); idx++)
	{
		AudioFile * pAudioFile = m_audioFiles[idx];
		wstring title = pAudioFile->GetTitle();
		title.append(L" - ");
		title.append(pAudioFile->GetArtist()->GetName());
		int itemidx = SendDlgItemMessage(IDC_PLAY_LIST, LB_ADDSTRING, 0, (LPARAM)title.c_str());
		SendDlgItemMessage(IDC_PLAY_LIST, LB_SETITEMDATA, (WPARAM)itemidx, (LPARAM)pAudioFile->GetPKey());
	}
}

void CJukeBoxPlayerDlg::OnBnClickedButtonPlay()
{
	UpdatePlayList();
	m_restartPlayList = true;
	SendDlgItemMessage(IDC_PLAY_LIST, LB_RESETCONTENT);
}

void CJukeBoxPlayerDlg::OnBnClickedRightButton()
{
	int lastalbumidx = m_albums.size()-1;
	if (m_albumIdx<lastalbumidx)
	{
		m_albumIdx++;
	}
	else
	{
		m_albumIdx = 0;
	}
	UpdateAlbum();
}

void CJukeBoxPlayerDlg::OnBnClickedLeftButton()
{
	if (m_albumIdx==0)
	{
		m_albumIdx = m_albums.size() - 1;
	}
	else if (m_albumIdx>0)
	{
		--m_albumIdx;
	}
	UpdateAlbum();
}

void CJukeBoxPlayerDlg::OnBnClickedRemovePlaylistButton()
{
	int firstsel = -1;
	while (true)
	{
		int count = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETSELCOUNT);
		if (count == 0) break;
		int * selitems = new int[1];
		SendDlgItemMessage(IDC_PLAY_LIST, LB_GETSELITEMS, 1, (LPARAM)selitems);
		SendDlgItemMessage(IDC_PLAY_LIST, LB_DELETESTRING, selitems[0]);
		if (firstsel<0)
		{
			firstsel = max(0, selitems[0] - 1);
		}
		delete selitems;
	}
	int cnt = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETCOUNT);
	if (firstsel >= 0 && firstsel < cnt)
	{
		SendDlgItemMessage(IDC_PLAY_LIST, LB_SETSEL, true, firstsel);
	}
}

void CJukeBoxPlayerDlg::OnBnClickedRemoveAllPlaylistButton2()
{
	SendDlgItemMessage(IDC_PLAY_LIST, LB_RESETCONTENT);
}

void CJukeBoxPlayerDlg::OnBnClickedButtonLoad()
{
	CFileDialog fOpenDlg(TRUE, L"pl", L"", OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, L"Play list (*.pl)|*.pl||");  
	fOpenDlg.m_pOFN->lpstrTitle=L"Load play list";  
	fOpenDlg.m_pOFN->lpstrInitialDir=m_pMediaDatabase->GetSettings()->GetDatabaseDirectory().c_str();
	if(fOpenDlg.DoModal()==IDOK)
	{
		PlayList playList;
		playList.Load(fOpenDlg.GetPathName(), *m_pMediaDatabase);
		vector<AudioFile *> audioFiles = playList.GetAudioFiles();
		vector<AudioFile *>::const_iterator iter = audioFiles.begin();
		SendDlgItemMessage(IDC_PLAY_LIST, LB_RESETCONTENT);
		for (; iter != audioFiles.end(); ++iter)
		{
			AudioFile * pAudioFile = *iter;
			wstring title = pAudioFile->GetTitle();
			title.append(L" - ");
			title.append(pAudioFile->GetArtist()->GetName());
			int itemidx = SendDlgItemMessage(IDC_PLAY_LIST, LB_ADDSTRING, 0, (LPARAM)title.c_str());
			SendDlgItemMessage(IDC_PLAY_LIST, LB_SETITEMDATA, (WPARAM)itemidx, (LPARAM)pAudioFile->GetPKey());
		}
	}
}

void CJukeBoxPlayerDlg::OnBnClickedButtonSave()
{
	CFileDialog fOpenDlg(false, L"pl", L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Play list (*.pl)|*.pl||");  
	fOpenDlg.m_pOFN->lpstrTitle=L"Save play list";  
	fOpenDlg.m_pOFN->lpstrInitialDir=m_pMediaDatabase->GetSettings()->GetDatabaseDirectory().c_str();
	if(fOpenDlg.DoModal()==IDOK)
	{
		int count = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETCOUNT);
		PlayList playList;
		for (int idx=0; idx<count; idx++)
		{
			ULONG pkeyAudioFile = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETITEMDATA, idx);
			AudioFile * pAudioFile = m_pMediaDatabase->GetAudioFile(pkeyAudioFile);
			playList.AddAudioFile(pAudioFile);
		}
		playList.Save(fOpenDlg.GetPathName());
	}
}

void CJukeBoxPlayerDlg::OnEnChangeEditSearch()
{
	SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_RESETCONTENT);
	int idx = SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_ADDSTRING, 0, (LPARAM)L"Searching ..."); 
	SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_SETITEMDATA, (WPARAM)idx, (LPARAM)-1);
	CString txt;
	GetDlgItemText(IDC_EDIT_SEARCH, txt);
	m_pSearcher->Cancel();
	m_pSearcher->SearchAsync((LPCWSTR)txt);
}

void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonStop()
{
	m_pWMPControls->stop();
}

void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonPause()
{
	m_pWMPControls->pause();
}

void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonPlay()
{
	m_pWMPControls->play();
}

void CJukeBoxPlayerDlg::OnEnSetfocusEditSearch()
{
}

void CJukeBoxPlayerDlg::OnEnKillfocusEditSearch()
{
}

void CJukeBoxPlayerDlg::OnLbnDblclkListSearchresult()
{
}

void CJukeBoxPlayerDlg::OnLbnSelchangeListSearchresult()
{
		int count = SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_GETSELCOUNT);
		if (count == 0) return;
		int selidx = SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_GETCURSEL);
		long audioFilePkey = SendDlgItemMessage(IDC_LIST_SEARCHRESULT, LB_GETITEMDATA, selidx);
		if (audioFilePkey<0) return;
		AudioFile * pAudioFile = m_pMediaDatabase->GetAudioFile(audioFilePkey);
		int idx = 0;
		for (vector<Album *>::const_iterator iter = m_albums.begin(); iter != m_albums.end(); iter++, idx++)
		{
			Album * pAlbum = *iter;
			if (pAlbum->GetPKey() == pAudioFile->GetAlbum()->GetPKey())
			{
				m_albumIdx = idx;
				break;
			}
		}
		UpdateAlbum();
		idx = 0;
		for (vector<AudioFile *>::const_iterator iter = m_audioFiles.begin(); iter != m_audioFiles.end(); iter++, idx++)
		{
			AudioFile * pAlbumAudioFile = *iter;
			if (pAlbumAudioFile->GetPKey() == pAudioFile->GetPKey())
			{
				SendDlgItemMessage(IDC_ALBUM_TRACK_LIST, LB_SETSEL, TRUE, idx);
				break;
			}
		}
}

BOOL CJukeBoxPlayerDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return CDialogEx::OnEraseBkgnd(pDC);
}

HBRUSH CJukeBoxPlayerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor==CTLCOLOR_DLG)/*pWnd->GetDlgCtrlID()==IDD_JUKEBOXPLAYER_DIALOG*/
	{
		return m_hBlackBrush;
	}
	if (nCtlColor==CTLCOLOR_LISTBOX)//pWnd->GetDlgCtrlID() == IDC_STATIC_ALBUM)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SetBkMode(TRANSPARENT);
		return m_hBlackBrush;
	}
	if (nCtlColor==CTLCOLOR_STATIC)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkMode(TRANSPARENT);
		return m_hBlackBrush;
	}
	return hbr;
}

void CJukeBoxPlayerDlg::OnStnClickedImageThumb1()
{
	m_albumIdx = (m_albumIdx < 4) ? m_albums.size() - (4 - m_albumIdx) : m_albumIdx - 4;
	UpdateAlbum();
}

void CJukeBoxPlayerDlg::OnStnClickedImageThumb2()
{
	m_albumIdx = (m_albumIdx < 3) ? m_albums.size() - (3 - m_albumIdx) : m_albumIdx - 3;
	UpdateAlbum();
}

void CJukeBoxPlayerDlg::OnStnClickedImageThumb3()
{
	m_albumIdx = (m_albumIdx < 2) ? m_albums.size() - (2 - m_albumIdx) : m_albumIdx - 2;
	UpdateAlbum();
}

void CJukeBoxPlayerDlg::OnStnClickedImageThumb4()
{
	m_albumIdx = (m_albumIdx < 1) ? m_albums.size() - (1 - m_albumIdx) : m_albumIdx - 1;
	UpdateAlbum();
}

void CJukeBoxPlayerDlg::OnStnClickedImageThumb6()
{
	m_albumIdx = (m_albumIdx + 1) % m_albums.size();
	UpdateAlbum();
}

void CJukeBoxPlayerDlg::OnStnClickedImageThumb7()
{
	m_albumIdx = (m_albumIdx + 2) % m_albums.size();
	UpdateAlbum();
}

void CJukeBoxPlayerDlg::OnStnClickedImageThumb8()
{
	m_albumIdx = (m_albumIdx + 3) % m_albums.size();
	UpdateAlbum();
}

void CJukeBoxPlayerDlg::OnStnClickedImageThumb9()
{
	m_albumIdx = (m_albumIdx + 4) % m_albums.size();
	UpdateAlbum();
}

void CJukeBoxPlayerDlg::OnBnClickedButtonUp()
{
	int count = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETSELCOUNT);
	if (count != 1)
	{
		return;
	}
	int * selitems = new int[1];
	SendDlgItemMessage(IDC_PLAY_LIST, LB_GETSELITEMS, 1, (LPARAM)selitems);
	int selidx = selitems[0];
	delete selitems;
	if (selidx < 1)
	{
		return;
	}
	count = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETCOUNT);
	vector<ULONG> audioFiles;
	for (int idx=0; idx<count; idx++)
	{
		audioFiles.push_back(SendDlgItemMessage(IDC_PLAY_LIST, LB_GETITEMDATA, idx));
	}
	ULONG tmp = audioFiles[selidx-1];
	audioFiles[selidx-1] = audioFiles[selidx];
	audioFiles[selidx] = tmp;
	SendDlgItemMessage(IDC_PLAY_LIST, LB_RESETCONTENT);
	for (size_t idx=0; idx<audioFiles.size(); idx++)
	{
		AudioFile * pAudioFile = m_pMediaDatabase->GetAudioFile(audioFiles[idx]);
		wstring title = pAudioFile->GetTitle();
		title.append(L" - ");
		title.append(pAudioFile->GetArtist()->GetName());
		int itemidx = SendDlgItemMessage(IDC_PLAY_LIST, LB_ADDSTRING, 0, (LPARAM)title.c_str());
		SendDlgItemMessage(IDC_PLAY_LIST, LB_SETITEMDATA, (WPARAM)itemidx, (LPARAM)pAudioFile->GetPKey());
	}
	SendDlgItemMessage(IDC_PLAY_LIST, LB_SETSEL, TRUE, selidx-1);
}

void CJukeBoxPlayerDlg::OnBnClickedButtonDown()
{
	int count = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETSELCOUNT);
	if (count != 1)
	{
		return;
	}
	int * selitems = new int[1];
	SendDlgItemMessage(IDC_PLAY_LIST, LB_GETSELITEMS, 1, (LPARAM)selitems);
	int selidx = selitems[0];
	delete selitems;
	count = SendDlgItemMessage(IDC_PLAY_LIST, LB_GETCOUNT);
	if (selidx == count-1)
	{
		return;
	}
	vector<ULONG> audioFiles;
	for (int idx=0; idx<count; idx++)
	{
		audioFiles.push_back(SendDlgItemMessage(IDC_PLAY_LIST, LB_GETITEMDATA, idx));
	}
	ULONG tmp = audioFiles[selidx+1];
	audioFiles[selidx+1] = audioFiles[selidx];
	audioFiles[selidx] = tmp;
	SendDlgItemMessage(IDC_PLAY_LIST, LB_RESETCONTENT);
	for (size_t idx=0; idx<audioFiles.size(); idx++)
	{
		AudioFile * pAudioFile = m_pMediaDatabase->GetAudioFile(audioFiles[idx]);
		wstring title = pAudioFile->GetTitle();
		title.append(L" - ");
		title.append(pAudioFile->GetArtist()->GetName());
		int itemidx = SendDlgItemMessage(IDC_PLAY_LIST, LB_ADDSTRING, 0, (LPARAM)title.c_str());
		SendDlgItemMessage(IDC_PLAY_LIST, LB_SETITEMDATA, (WPARAM)itemidx, (LPARAM)pAudioFile->GetPKey());
	}
	SendDlgItemMessage(IDC_PLAY_LIST, LB_SETSEL, TRUE, selidx+1);
}

void CJukeBoxPlayerDlg::OnCancel()
{
	m_pMediaDatabase->GetSettings()->SetCurrentAlbumIndex(m_albumIdx);
	m_pMediaDatabase->GetSettings()->Save();
	CDialogEx::OnCancel();
}

void CJukeBoxPlayerDlg::OnOK()
{
	OnLbnDblclkAlbumTrackList();
}

void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonClear()
{
	m_playList.Clear();
	m_pWMPControls->stop();
}


void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonPlayalbum()
{
	m_playList.Clear();
	for (size_t idx=0; idx<m_audioFiles.size(); idx++)
	{
		AudioFile * pAudioFile = m_audioFiles[idx];
		m_playList.AddAudioFile(pAudioFile);
	}
	m_pWMPControls->stop();
}

void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonPrev()
{
	OnBnClickedLeftButton();
}


void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonNext()
{
	OnBnClickedRightButton();
}


void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonForward()
{
	m_pWMPControls->stop();	
}


void CJukeBoxPlayerDlg::OnStnClickedStaticRemainingSongs()
{
	// TODO: Add your control notification handler code here
}


void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonPausecont()
{
	WMPPlayState wmpPlayState;
	if (m_pWMPPlayer->get_playState(&wmpPlayState) == S_OK)
	{
		if (wmpPlayState == wmppsPaused)
		{
			m_pWMPControls->play();
		}
		else
		{
			m_pWMPControls->pause();
		}
	}
}


void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonShuffle()
{
	srand(static_cast<unsigned int>(time(NULL)));
	m_playList.Clear();
	vector<long> audiopkeys;
	for (vector<Album *>::const_iterator albumiter = m_albums.begin(); albumiter != m_albums.end(); albumiter++)
	{
		vector<AudioFile *> audioFiles = m_pMediaDatabase->GetAudioFiles(**albumiter);
		for (vector<AudioFile *>::const_iterator audioiter = audioFiles.begin(); audioiter!=audioFiles.end(); audioiter++)
		{
			audiopkeys.push_back((*audioiter)->GetPKey());
		}
	}
	size_t n = min(100, audiopkeys.size());
	for (size_t i=0;i<n;i++)
	{
		int audioidx = rand() % audiopkeys.size();
		long pkey = audiopkeys[audioidx];
		AudioFile * pAudioFile = m_pMediaDatabase->GetAudioFile(pkey);
		m_playList.AddAudioFile(pAudioFile);
		vector<long>::const_iterator finditer = find(audiopkeys.begin(),audiopkeys.end(),pkey);
		audiopkeys.erase(finditer);
	}
	/*
	size_t albumcnt = m_albums.size();
	vector<long> pkeys;
	for (size_t i=0;i<100;i++)
	{
		int albumidx = rand() % albumcnt;
		Album * pAlbum = m_albums[albumidx];
		vector<AudioFile *> audiofiles = m_pMediaDatabase->GetAudioFiles(*pAlbum);
		if (!audiofiles.empty())
		{
			int fileidx = rand() % audiofiles.size();
			AudioFile * pAudioFile = audiofiles[fileidx];
			if (find(pkeys.begin(),pkeys.end(),pAudioFile->GetPKey())==pkeys.end())
			{
				pkeys.push_back(pAudioFile->GetPKey());
				m_playList.AddAudioFile(pAudioFile);
			}
		}
	}
	*/
	m_pWMPControls->stop();
}


void CJukeBoxPlayerDlg::OnBnClickedMfcbuttonOpenplaylist()
{
	wifstream wistream;
	wstring basedir = m_pMediaDatabase->GetSettings()->GetDatabaseDirectory();
	basedir.append(L"\\");
	wstring playlistfilename = basedir + L"Playlist.txt";

	ShellExecute(NULL, NULL, playlistfilename.c_str(), NULL, NULL, SW_SHOWNORMAL);

	wistream.open(playlistfilename.c_str(), ios_base::in);
	while (!wistream.eof())
	{
		wchar_t buf[1024];
		wistream.getline(buf, 1024);
		wstring line = buf;
		AudioFile * pAudioFile = NULL;
		if (!line.empty())
		{
			size_t pos1 = line.find(L";");
			if (pos1 != wstring::npos)
			{
				size_t pos2 = line.find(L";", pos1+1);
				if (pos2 != wstring::npos)
				{
					wstring title = line.substr(0,pos1);
					wstring artist = line.substr(pos1+1, pos2-pos1-1);
					wstring album = line.substr(pos2+1);
					pAudioFile = m_pMediaDatabase->FindTitle(album.c_str(), title.c_str());
				}
			}
		}
		if (pAudioFile != NULL)
		{
			m_playList.AddAudioFile(pAudioFile);
		}
	}
	wistream.close();
	m_pWMPControls->stop();
}
