// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JukeBoxPlayer.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"
#include "Settings.h"
#include "MediaDatabase.h"
#include "AudioFileScanner.h"

using namespace std;

// CSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialogEx)

CSettingsDlg::CSettingsDlg(MediaDatabase * pMediaDatabase, bool autoScan /*=FALSE*/, CWnd* pParent /*=NULL*/)
: CDialogEx(CSettingsDlg::IDD, pParent), m_pMediaDatabase(pMediaDatabase), m_autoScan(autoScan)
{
	m_pScanner = new AudioFileScanner(pMediaDatabase);
}

CSettingsDlg::~CSettingsDlg()
{
	delete m_pScanner;
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_MUSICDIR, m_browsectrl);
	DDX_Control(pDX, IDC_PROGRESS_SCAN, m_scanprogress);
	DDX_Control(pDX, IDC_CHECK_FULLSCAN, m_fullscan);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSettingsDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CSettingsDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE_MUSICDIR, &CSettingsDlg::OnEnChangeMfceditbrowseMusicdir)
END_MESSAGE_MAP()


BOOL CSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_browsectrl.SetWindowText(m_pMediaDatabase->GetSettings()->GetMusicDirectory().c_str());
	CString str;
	str.Format(L"Album: %ld Songs: %ld", m_pMediaDatabase->GetAlbumCount(), m_pMediaDatabase->GetAudioFileCount());
	GetDlgItem(IDC_STATIC_SCAN_STATISTIC)->SetWindowText((LPCWSTR)str);
	m_fullscan.SetCheck(m_pMediaDatabase->GetAudioFileCount()==0);
	m_fullscan.EnableWindow(m_fullscan.GetCheck()==0);
	SetTimer(IDT_CHECK_PLAYSTATE, 100, NULL);
	if (m_autoScan)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCEDITBROWSE_MUSICDIR)->EnableWindow(FALSE);
		m_pScanner->ScanAsync(false);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// CSettingsDlg message handlers


void CSettingsDlg::OnBnClickedButtonScan()
{
	// TODO: Add your control notification handler code here
}


void CSettingsDlg::OnBnClickedOk()
{
	CString txt;
	m_browsectrl.GetWindowText(txt);
	wstring newdir = (LPCWSTR)txt;
	bool fullscan = m_fullscan.GetCheck() != 0;
	if (newdir != m_pMediaDatabase->GetSettings()->GetMusicDirectory())
	{
		m_oldMusicDir = m_pMediaDatabase->GetSettings()->GetMusicDirectory();
		m_pMediaDatabase->GetSettings()->SetMusicDirectory(newdir.c_str());
	}
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_MFCEDITBROWSE_MUSICDIR)->EnableWindow(FALSE);
	m_pScanner->ScanAsync(fullscan);
	// TODO: Add your control notification handler code here	
	// CDialogEx::OnOK();
}


void CSettingsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == IDT_CHECK_PLAYSTATE)
	{
		if (m_pScanner->IsCompleted())
		{
			m_pMediaDatabase->Save();
			CDialogEx::OnOK();
			return;
		}
		if (m_pScanner->IsScanning())
		{
			long current, total, albumCount, fileCount;
			wstring dir;
			m_pScanner->GetState(current, total, dir, albumCount, fileCount);
			m_scanprogress.SetRange32(0, total);
			m_scanprogress.SetPos(current);
			GetDlgItem(IDC_STATIC_CURRENT_SCANDIR)->SetWindowText(dir.c_str());
			CString str;
			str.Format(L"Album: %ld Songs: %ld",albumCount, fileCount);
			GetDlgItem(IDC_STATIC_SCAN_STATISTIC)->SetWindowText((LPCWSTR)str);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CSettingsDlg::OnBnClickedCancel()
{
	if (m_pScanner->IsScanning())
	{
		m_pScanner->Cancel();
		if (!m_oldMusicDir.empty())
		{
			m_pMediaDatabase->GetSettings()->SetMusicDirectory(m_oldMusicDir.c_str());
		}
		m_pMediaDatabase->Load();
	}
	CDialogEx::OnCancel();
}


void CSettingsDlg::OnEnChangeMfceditbrowseMusicdir()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString txt;
	m_browsectrl.GetWindowText(txt);
	wstring newdir = (LPCWSTR)txt;
	if (newdir != m_pMediaDatabase->GetSettings()->GetMusicDirectory())
	{
		m_fullscan.SetCheck(true);
		m_fullscan.EnableWindow(false);
	}
	else
	{
		m_fullscan.SetCheck(false);
		m_fullscan.EnableWindow(true);
	}
}
