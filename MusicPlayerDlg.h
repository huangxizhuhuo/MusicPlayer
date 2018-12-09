#pragma once

#include "MediaPlay.h" 
#include <vector>
#include <map>

class CMusicPlayerDlg : public CDialog
{
private:
	
	CWinThread* m_pMonitor;
	CMediaPlayer m_MediaFile;
	CString m_szCurPath;
	LPWSTR	m_strFilename;
	CMenu	m_Menu;
	HBITMAP m_hBitmapPlay;
	HBITMAP m_hBitmapPause;
	bool	m_bPlay;
	int     m_iCurVolume;
	int		m_iPlayMode;
	int		m_iCurSel;

public:

	CMusicPlayerDlg(CWnd* pParent = NULL);
	enum { IDD = IDD_MUSICPLAYER_DIALOG };
	
protected:

	HICON m_hIcon;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);
	void FreeCurrentPlaying();
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

public:

	void AddMediaFile(const CString& szFile);
	void DelMediaFile(const CString& szFile);


public:

	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNMReleasedcaptureSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSlidervolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedImport();
	afx_msg void OnLbnDblclkListplay();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedFullscreen();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnSingleLoop();
	afx_msg void OnOrderPlay();
	afx_msg void OnLoopPlay();
	afx_msg void OnSingleplay();
	afx_msg void OnBnClickedFast();
	afx_msg void OnBnClickedSlow();
	afx_msg void OnBnClickedMode();
	
	CButton		m_ButtonAdd;
	CButton		m_ButtonMode;
	CButton		m_ButtonPlay;
	CButton		m_ButtonStop;
	CButton		m_ButtonFast;
	CButton		m_ButtonSlow;
	CButton		m_ButtonFullScreen;
	CListBox    m_ListPlay;
	CSliderCtrl m_SliderVolume;
	CSliderCtrl m_SliderCtrl;
	HANDLE		m_hDirectory;
	std::vector<CString> m_MediaFiles; 
};
