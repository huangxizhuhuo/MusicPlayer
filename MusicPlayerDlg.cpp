#include "stdafx.h"
#include "MusicPlayer.h"
#include "MusicPlayerDlg.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMusicPlayerDlg::CMusicPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMusicPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
	m_iCurVolume = 100;
}

void CMusicPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER, m_SliderCtrl);
	DDX_Control(pDX, IDC_SLIDERVOLUME, m_SliderVolume);
	DDX_Control(pDX, IDC_LISTPLAY, m_ListPlay);
	DDX_Control(pDX, IDC_IMPORT, m_ButtonAdd);
	DDX_Control(pDX, IDC_MODE, m_ButtonMode);
	DDX_Control(pDX, IDC_PLAY, m_ButtonPlay);
	DDX_Control(pDX, IDC_STOP, m_ButtonStop);
	DDX_Control(pDX, IDC_FAST, m_ButtonFast);
	DDX_Control(pDX, IDC_SLOW, m_ButtonSlow);
	DDX_Control(pDX, IDC_FULLSCREEN, m_ButtonFullScreen);
}

BEGIN_MESSAGE_MAP(CMusicPlayerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER, OnNMReleasedcaptureSlider)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERVOLUME, OnNMReleasedcaptureSlidervolume)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PLAY, OnBnClickedPlay)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
    ON_BN_CLICKED(IDC_FULLSCREEN, OnBnClickedFullscreen)
    ON_BN_CLICKED(IDC_FAST, OnBnClickedFast)
    ON_BN_CLICKED(IDC_SLOW, OnBnClickedSlow)
    ON_BN_CLICKED(IDC_MODE, OnBnClickedMode)
	ON_BN_CLICKED(IDC_IMPORT, OnBnClickedImport)
	ON_LBN_DBLCLK(IDC_LISTPLAY, OnLbnDblclkListplay)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(IDM_SINGLELOOP, OnSingleLoop)
	ON_COMMAND(IDM_ORDERPLAY, OnOrderPlay)
	ON_COMMAND(IDM_LOOPPLAY, OnLoopPlay)
	ON_COMMAND(IDM_SINGLEPLAY, OnSingleplay)
END_MESSAGE_MAP()

UINT MonitorFileChangeThread(void *pVoid);

BOOL CMusicPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 使最大化按钮不可用
	long nStyle = ::GetWindowLong( m_hWnd, GWL_STYLE ); 
	nStyle = ( nStyle & ~WS_MAXIMIZEBOX );
	SetWindowLong( m_hWnd, GWL_STYLE, nStyle );

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	SetTimer(0, 100, NULL);
	m_SliderCtrl.SetRange(0, 10000);
	m_SliderVolume.SetRange(0, 100);
	HBITMAP hBitmap;
	hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ADD));
	m_ButtonAdd.SetBitmap(hBitmap);
	hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MODE));
	m_ButtonMode.SetBitmap(hBitmap);
	hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_STOP));
	m_ButtonStop.SetBitmap(hBitmap);
	hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_FAST));
	m_ButtonFast.SetBitmap(hBitmap);
	hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SLOW));
	m_ButtonSlow.SetBitmap(hBitmap);
	hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_STOP));
	m_ButtonFullScreen.SetBitmap(hBitmap); 
	m_hBitmapPlay = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_PLAY));
	m_hBitmapPause = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_PAUSE));
	m_ButtonPlay.SetBitmap(m_hBitmapPlay);

	m_bPlay = false;
	m_iPlayMode = 2;

	m_hDirectory  = CreateFile( L"d:\\music", 
		FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );

	m_pMonitor = AfxBeginThread( MonitorFileChangeThread, (void*)this, THREAD_PRIORITY_NORMAL, 0, 0, 0 );
  
	return TRUE;
}

void CMusicPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{	
	CDialog::OnSysCommand(nID, lParam);
}

void CMusicPlayerDlg::OnPaint() 
{
	if(IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CMusicPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMusicPlayerDlg::FreeCurrentPlaying()
{		
	m_MediaFile.Stop();	   
	m_MediaFile.Shutdown();	 
}

void CMusicPlayerDlg::OnBnClickedPlay()
{
	m_bPlay = !m_bPlay;
	if(!m_bPlay)
	{   
	    m_MediaFile.Play();
		m_ButtonPlay.SetBitmap(m_hBitmapPause);
		SetDlgItemText(IDC_STATE, L"播放");		
	}
	else
	{	
		m_MediaFile.Pause();
		m_ButtonPlay.SetBitmap(m_hBitmapPlay);
		SetDlgItemText(IDC_STATE, L"暂停");
	}
}

void CMusicPlayerDlg::OnBnClickedStop()
{
	m_bPlay = false;
	m_MediaFile.Stop();	
	m_ButtonPlay.SetBitmap(m_hBitmapPlay);
	SetDlgItemText(IDC_STATE,L"准备就绪");
}

void CMusicPlayerDlg::OnTimer(UINT nIDEvent)
{
	if( nIDEvent == 0 )
	{
		if( GetFocus()->GetSafeHwnd() != m_SliderCtrl.GetSafeHwnd() )
		{
			m_SliderCtrl.SetPos(m_MediaFile.GetCurrentPos());
		}
		if( GetFocus()->GetSafeHwnd() != m_SliderVolume.GetSafeHwnd() )
		{
			m_SliderVolume.SetPos(m_MediaFile.GetVolume());
		}
		CString strTime;
		int iMinutes, iSeconds, iMinutesTotal, iSecondsTotal;
		iMinutes	  = m_MediaFile.GetCurrentTime()/60;
		iSeconds	  = m_MediaFile.GetCurrentTime()%60;	
		iMinutesTotal = m_MediaFile.GetDuration()/60;
		iSecondsTotal = m_MediaFile.GetDuration()%60;
		strTime.Format(L"%02d : %02d / %02d : %02d", iMinutes, iSeconds, iMinutesTotal, iSecondsTotal);
		SetDlgItemText(IDC_TIME, strTime);
		if(m_MediaFile.Playing())
		{
			switch(m_iPlayMode)
			{
			case 1:
				m_ListPlay.SetCurSel(m_iCurSel);
				OnLbnDblclkListplay();
				break;
			case 2:	
				if(m_iCurSel != m_MediaFiles.size()-1)
				{
					m_ListPlay.SetCurSel(m_iCurSel+1);
					OnLbnDblclkListplay();
				}
				break;
			case 3:	
				if(m_iCurSel == m_MediaFiles.size()-1)
				{
					m_ListPlay.SetCurSel(0);
				}
				else
				{
					m_ListPlay.SetCurSel(m_iCurSel+1);
				}
				OnLbnDblclkListplay();
				break;
			default:
				break;
			}
		}
	}
}

void CMusicPlayerDlg::OnNMReleasedcaptureSlider(NMHDR *pNMHDR, LRESULT *pResult)
{		
	m_MediaFile.PutCurrentPos((double)m_SliderCtrl.GetPos());
	::SetFocus( m_hWnd );
}

void CMusicPlayerDlg::OnNMReleasedcaptureSlidervolume(NMHDR *pNMHDR, LRESULT *pResult)
{	
	m_MediaFile.PutVolume(m_SliderVolume.GetPos());	
	m_iCurVolume = m_SliderVolume.GetPos();
	::SetFocus( m_hWnd );
}

int CALLBACK BrowseCallBackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)   
{   
    if(uMsg == BFFM_INITIALIZED )
    {   
        ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);   
    }   
    return 0;  
}

void CMusicPlayerDlg::OnBnClickedImport()
{	
	BROWSEINFO		bi;    
    TCHAR			szDisplayName[MAX_PATH];    
    LPITEMIDLIST	pidl = NULL;    
    LPMALLOC		pMalloc = NULL;        
    ZeroMemory(&bi, sizeof(bi));  
	bi.pidlRoot  = NULL;
    bi.hwndOwner = m_hWnd;    
	bi.pszDisplayName = szDisplayName;    
    bi.lpszTitle = TEXT("");
	bi.lParam = (LPARAM)m_szCurPath.GetBuffer();
    bi.lpfn = BrowseCallBackProc;
    bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_USENEWUI;
    pidl = SHBrowseForFolder(&bi);    
    if(pidl != NULL)    
	{    
		SHGetPathFromIDList(pidl, szDisplayName); 
		CFileFind fileFind;
		CString filePath,fileName;	
		CString szDir = CString(szDisplayName);
		if(szDir.Right(1) != "\\")	
		{
			szDir += "\\";
		}
		m_szCurPath = szDir;
		szDir += "*.*";     
		BOOL res = fileFind.FindFile(szDir);
		while(res)
		{ 
			res = fileFind.FindNextFile();
			if(fileFind.IsDirectory() && !fileFind.IsDots())
			{
				continue;     
			}
			else if(!fileFind.IsDirectory() && !fileFind.IsDots()) 
			{   
				filePath = fileFind.GetFilePath(); 
				if( filePath.Right(3).CompareNoCase(L"mp3") &&
					filePath.Right(3).CompareNoCase(L"wma") &&
					filePath.Right(3).CompareNoCase(L"wmv") &&
					filePath.Right(3).CompareNoCase(L"avi") &&
					filePath.Right(3).CompareNoCase(L"mpg") &&
					filePath.Right(3).CompareNoCase(L"mid") &&
					filePath.Right(3).CompareNoCase(L"wav") )  
				{
					continue;  
				}
				fileName = fileFind.GetFileName();
				m_ListPlay.AddString(fileName);  
				m_MediaFiles.push_back( filePath );
			} 
		}
		fileFind.Close();   
	}
}

void CMusicPlayerDlg::OnLbnDblclkListplay()
{
	m_iCurSel = m_ListPlay.GetCurSel();				
	m_strFilename = m_MediaFiles[m_iCurSel].GetBuffer(0);			
	FreeCurrentPlaying();		
	if( m_MediaFile.Init((HWND)::GetDlgItem(m_hWnd, IDC_SHOW)) )
	{
		if( m_MediaFile.Render(m_strFilename) )
		{
			m_MediaFile.Play();
			m_MediaFile.PutVolume(m_iCurVolume);
		}
		else
		{
			m_ListPlay.SetCurSel(m_iCurSel+1);
			OnLbnDblclkListplay();
		}
	}
	else
	{
		m_ListPlay.SetCurSel(m_iCurSel+1);
		OnLbnDblclkListplay();
	}
		
	m_bPlay = true;
	m_ButtonPlay.SetBitmap(m_hBitmapPause);
	SetDlgItemText(IDC_STATE,L"播放");
	SetWindowText(m_strFilename);	

	::SetFocus( GetSafeHwnd() );
}

void CMusicPlayerDlg::OnDestroy()
{
	//::WaitForSingleObject( m_pMonitor->m_hThread, 100 );
	//DWORD dwExitCode;
	//::GetExitCodeThread( m_pMonitor->m_hThread, &dwExitCode );
	//if( dwExitCode == STILL_ACTIVE )
	//{
	//	m_pMonitor->PostThreadMessage( WM_QUIT, NULL, NULL ); 
	//	//TerminateThread( m_pMonitor->m_hThread, 0 );
	//	//CloseHandle(m_hDirectory);
	//}
	CDialog::OnDestroy();
	m_MediaFile.Shutdown();
	CloseHandle( m_hDirectory );
	
}

void CMusicPlayerDlg::OnBnClickedFullscreen()
{
	m_MediaFile.SetFullScreen();
}

void CMusicPlayerDlg::OnBnClickedFast()
{
	if(!m_MediaFile.Fast())
	{
		return;
	}
	CString strState;
	strState.Format(L"播放 x%.1f", m_MediaFile.GetCurrentSpeed());
    SetDlgItemText(IDC_STATE, strState);
}

void CMusicPlayerDlg::OnBnClickedSlow()
{
	if(!m_MediaFile.Slow())
	{
		return;
	}
	CString strState;
	strState.Format(L"播放 x%.1f", m_MediaFile.GetCurrentSpeed());
	SetDlgItemText(IDC_STATE, strState);
}

void CMusicPlayerDlg::OnBnClickedMode()
{
	m_Menu.DestroyMenu();
    POINT point;
	::GetCursorPos(&point);
    VERIFY( m_Menu.LoadMenu(IDR_MENUMODE) );
    CMenu* pPopup = m_Menu.GetSubMenu(0);
    CWnd* pWndPopupOwner = this;
    while( pWndPopupOwner->GetStyle() & WS_CHILD )
	{
	    pWndPopupOwner = pWndPopupOwner->GetParent();
	}
    pPopup->TrackPopupMenu( TPM_LEFTALIGN|TPM_LEFTBUTTON, point.x, point.y, pWndPopupOwner ); 
}

void CMusicPlayerDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{ 
	CDialog::OnInitMenuPopup( pPopupMenu, nIndex, bSysMenu );
	if( !bSysMenu && pPopupMenu != NULL )                                                                                                                     
	{                                                                                                                                                                 
		CCmdUI cmdUI;   
		cmdUI.m_pOther   =	NULL;   
		cmdUI.m_pMenu    =	pPopupMenu;   
		cmdUI.m_pSubMenu =	NULL;   
		UINT  count      =	pPopupMenu->GetMenuItemCount();   
		cmdUI.m_nIndexMax=	count;   
		for( UINT i = 0; i < count; i++ )   
		{   
			UINT nID = pPopupMenu->GetMenuItemID(i);   
			if( nID == -1 || nID == 0 )  
			{
				continue;   
			}
			cmdUI.m_nID    =   nID;   
			cmdUI.m_nIndex =   i;   
			cmdUI.DoUpdate( this, FALSE );
			if( i == m_iPlayMode )
			{
				cmdUI.SetCheck(TRUE);
			}
		}
	}                                                                                                                                                         
}

void CMusicPlayerDlg::OnSingleplay()
{
	m_iPlayMode = 0;
}

void CMusicPlayerDlg::OnSingleLoop()
{
	m_iPlayMode = 1;
}

void CMusicPlayerDlg::OnOrderPlay()
{
	m_iPlayMode = 2;
}

void CMusicPlayerDlg::OnLoopPlay()
{
	m_iPlayMode = 3;
}

void CMusicPlayerDlg::AddMediaFile(const CString& szFile)
{
	m_MediaFiles.push_back( szFile );
}

void CMusicPlayerDlg::DelMediaFile(const CString& szFile)
{
	std::vector<CString>::iterator iter = find(m_MediaFiles.begin(), m_MediaFiles.end(), szFile);
	if(iter != m_MediaFiles.end()) 
	{
		m_MediaFiles.erase(iter);
	}
}

UINT MonitorFileChangeThread(void *pVoid)
{
    CMusicPlayerDlg* pDlg = (CMusicPlayerDlg*)pVoid;
	// 字符数组要充分大
	char buf[(sizeof(FILE_NOTIFY_INFORMATION)+MAX_PATH)*100];// sizeof(FILE_NOTIFY_INFORMATION) = 16
	FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION*)buf;
	DWORD dwBytesReturned;        

    while(true)
    {
		// 不监视子文件夹
		if( ::ReadDirectoryChangesW( pDlg->m_hDirectory, pNotify, sizeof(buf), false,
			FILE_NOTIFY_CHANGE_FILE_NAME| FILE_NOTIFY_CHANGE_DIR_NAME|
			FILE_NOTIFY_CHANGE_CREATION| FILE_NOTIFY_CHANGE_SECURITY,
			&dwBytesReturned, NULL, NULL ) )
		{
			FILE_NOTIFY_INFORMATION* pNotifyNext = pNotify;
			char szTmp[MAX_PATH];
			char szStr[MAX_PATH];

			while( pNotifyNext != NULL )
			{	
				memset( szTmp, 0, sizeof(szTmp) );
				WideCharToMultiByte( CP_ACP, 0, pNotifyNext->FileName, pNotifyNext->FileNameLength/2, szTmp, MAX_PATH, NULL, NULL );
				strcpy_s( szStr, szTmp );

				switch(pNotifyNext->Action)
				{
					case FILE_ACTION_ADDED:
						{
							CString szFileName = CString(szStr);
							if( szFileName.Find('/') == -1 && szFileName.Find('\\') == -1 )
							{
								CString szInfo = CString("新增文件") + szFileName;
								MessageBox( pDlg->m_hWnd, szInfo, L"监控线程", MB_ICONINFORMATION );
								pDlg->m_ListPlay.AddString(szFileName);
								pDlg->AddMediaFile( CString("D:\\music\\") + CString(szStr) );// 区分大小写
							}
						}
						break;
					case FILE_ACTION_REMOVED:
						{
							CString szFileName = CString(szStr);
							if( szFileName.Find('/') == -1 && szFileName.Find('\\') == -1 )
							{
								int iIndex = pDlg->m_ListPlay.FindString( 0, szFileName );
								if( iIndex != -1 )
								{
									CString szInfo = CString("移除文件") + szFileName;
									MessageBox( pDlg->m_hWnd, szInfo, L"监控线程", MB_ICONINFORMATION );
									pDlg->m_ListPlay.DeleteString(iIndex);
									pDlg->DelMediaFile( CString("D:\\music\\") + CString(szStr) );
								}
							}
						}
						break;
					case FILE_ACTION_MODIFIED:
						break;
					case FILE_ACTION_RENAMED_OLD_NAME:
						break;
					case FILE_ACTION_RENAMED_NEW_NAME:
						break;
					default:
						break;
				}
				if( pNotifyNext->NextEntryOffset == 0)
				{
					break;
				}
				pNotifyNext = (PFILE_NOTIFY_INFORMATION)( (char*)pNotifyNext + pNotifyNext->NextEntryOffset );
			}
		}		
    }
    return 0;
}

BOOL CMusicPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
	//if( pMsg->message == WM_APP + 1 )
	//{
	//	//if( pMsg->message == WM_KEYDOWN )
	//	{
	//		if( pMsg->wParam == VK_TAB )
	//		{
	//			MessageBox(L"sss",0,0);
	//		}
	//	}
	//}
	return CDialog::PreTranslateMessage(pMsg);
}