#include "stdafx.h"
#include "MusicPlayer.h"
#include "MusicPlayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMusicPlayerApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


CMusicPlayerApp::CMusicPlayerApp()
{
}

CMusicPlayerApp theApp;

BOOL CMusicPlayerApp::InitInstance()
{
	InitCommonControls();
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	SetRegistryKey(_T("CUMTSCY-MUSICPLAYER"));

	CMusicPlayerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}

BOOL CMusicPlayerApp::OnIdle(LONG lCount)
{
	return CWinApp::OnIdle(lCount);
}
