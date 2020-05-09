#pragma once
#include "afxeditbrowsectrl.h"
#include "afxcmn.h"

#include <string>
#include "afxwin.h"

class MediaDatabase;
class AudioFileScanner;

// CSettingsDlg dialog

class CSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(MediaDatabase * pMediaDatabase, bool autoScan = FALSE, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDlg();

// Dialog Data
	enum { IDD = IDD_SETTINGS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	MediaDatabase * m_pMediaDatabase;
	AudioFileScanner * m_pScanner;
	std::wstring m_oldMusicDir;
	bool m_autoScan;

public:
	afx_msg void OnBnClickedButtonScan();
	afx_msg void OnBnClickedOk();
	CMFCEditBrowseCtrl m_browsectrl;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_scanprogress;
	afx_msg void OnBnClickedCancel();
	CButton m_fullscan;
	afx_msg void OnEnChangeMfceditbrowseMusicdir();
};
