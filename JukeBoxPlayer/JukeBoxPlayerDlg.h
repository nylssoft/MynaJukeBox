
// JukeBoxPlayerDlg.h : header file
//

#pragma once

#include <wmp.h>
#include "MediaDatabase.h"
#include "PlayList.h"
#include "Settings.h"
#include "AudioFileSearcher.h"
#include "ImageCache.h"

#include <map>
#include <vector>
#include "afxcmn.h"

// CJukeBoxPlayerDlg dialog
class CJukeBoxPlayerDlg : public CDialogEx
{
// Construction
public:
	CJukeBoxPlayerDlg(IWMPPlayer * pWMPPlayer);

// Dialog Data
	enum { IDD = IDD_JUKEBOXPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	
	void UpdateAlbum(void);

	void UpdateAudioFiles(const Album & album);

	void UpdateThumbnails(void);

	void UpdatePlayList(void);

    void ShowPlayControls(bool hide);

	void ShowRemainingSongsControls(void);

private:

	IWMPPlayer * m_pWMPPlayer;

	CComPtr<IWMPControls> m_pWMPControls;

	MediaDatabase * m_pMediaDatabase;

	int m_albumIdx;

	std::vector<Album *> m_albums;

	std::vector<AudioFile *> m_audioFiles;

	PlayList m_playList;

	bool m_restartPlayList;

	AudioFile * m_pSinglePlay;

	AudioFileSearcher * m_pSearcher;

	ImageCache * m_pImageCache;

	HBRUSH m_hBlackBrush;

	Album * m_pPlayAlbum;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLbnDblclkAlbumTrackList();
	afx_msg void OnBnClickedAddPlaylistButton();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedRightButton();
	afx_msg void OnBnClickedLeftButton();
	afx_msg void OnBnClickedRemovePlaylistButton();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnEnChangeEditSearch();
	afx_msg void OnBnClickedMfcbuttonStop();
	afx_msg void OnBnClickedMfcbuttonPause();
	afx_msg void OnBnClickedMfcbuttonPlay();
	afx_msg void OnEnSetfocusEditSearch();
	afx_msg void OnEnKillfocusEditSearch();
	afx_msg void OnLbnDblclkListSearchresult();
	afx_msg void OnLbnSelchangeListSearchresult();
	afx_msg void OnBnClickedAddAllPlaylistButton2();
	afx_msg void OnBnClickedRemoveAllPlaylistButton2();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnClickedImageThumb9();
	afx_msg void OnStnClickedImageThumb1();
	afx_msg void OnStnClickedImageThumb2();
	afx_msg void OnStnClickedImageThumb3();
	afx_msg void OnStnClickedImageThumb4();
	afx_msg void OnStnClickedImageThumb6();
	afx_msg void OnStnClickedImageThumb7();
	afx_msg void OnStnClickedImageThumb8();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedMfcbuttonClear();
	afx_msg void OnBnClickedMfcbuttonPlayalbum();
	afx_msg void OnBnClickedMfcbuttonClear3();
	afx_msg void OnBnClickedMfcbuttonClear2();
	afx_msg void OnBnClickedMfcbuttonPrev();
	afx_msg void OnBnClickedMfcbuttonNext();
	afx_msg void OnBnClickedMfcbuttonForward();
	afx_msg void OnStnClickedStaticRemainingSongs();
	CProgressCtrl m_playProgressCtrl;
	afx_msg void OnBnClickedMfcbuttonPausecont();
	afx_msg void OnBnClickedMfcbuttonShuffle();
	afx_msg void OnBnClickedMfcbuttonOpenplaylist();
};
