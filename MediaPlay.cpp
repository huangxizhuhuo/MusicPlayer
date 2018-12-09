#include "MediaPlay.h"

#define SAFE_RELEASE(x) if(x) { x->Release(); x = NULL; }

#define WM_GRAPH_NOTIFY (WM_APP+1)

CMediaPlayer::CMediaPlayer()
{
	m_pGraphBuilder =	NULL;
	m_pMediaControl =	NULL;
	m_pMediaEvent   =	NULL;
	m_pVideoWindow  =	NULL;
	m_pMediaPosition=	NULL;
	m_pBasicAudio   =	NULL;
	m_hWnd			=	NULL;
	m_bPlaying      =	false;
	m_bFullScreen   =	false;
}

CMediaPlayer::~CMediaPlayer()
{
	Shutdown();
}

bool CMediaPlayer::Init(HWND hWnd)
{
	CoInitialize(NULL);
	m_hWnd = hWnd;
	if(FAILED(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder,(void**)&m_pGraphBuilder)))
	{
		return false;
	}
	m_pGraphBuilder->QueryInterface(IID_IMediaControl,(void**)&m_pMediaControl);
	m_pGraphBuilder->QueryInterface(IID_IMediaEventEx,(void**)&m_pMediaEvent);
	m_pGraphBuilder->QueryInterface(IID_IVideoWindow,(void**)&m_pVideoWindow);
	m_pGraphBuilder->QueryInterface(IID_IMediaPosition,(void**)&m_pMediaPosition);
	m_pGraphBuilder->QueryInterface(IID_IBasicAudio,(void**)&m_pBasicAudio);
	m_pVideoWindow->put_MessageDrain((OAHWND)hWnd);
	return true;
}

int CMediaPlayer::GetCurrentPos()
{
	double dLength,dCurLength;
	if(m_pMediaPosition==NULL)
		return 0;
	m_pMediaPosition->get_Duration(&dLength);
	m_pMediaPosition->get_CurrentPosition(&dCurLength);
	return (int)((dCurLength/dLength)*10000);
}

int CMediaPlayer::GetCurrentTime()
{
	if(m_pMediaPosition==NULL)
		return 0;
	double dCurLength;
	m_pMediaPosition->get_CurrentPosition(&dCurLength);
	return (int)dCurLength;
}

int CMediaPlayer::GetDuration()
{
	if(m_pMediaPosition==NULL)
		return 0;
	double dCurLength;
	m_pMediaPosition->get_Duration(&dCurLength);
	return (int)dCurLength;
}

double CMediaPlayer::GetCurrentSpeed()
{
	if(m_pMediaPosition==NULL)
		return false;
	double rate;
	m_pMediaPosition->get_Rate(&rate);   
	return rate/m_dOriginalRate;

}

void CMediaPlayer::PutCurrentPos(double dPos)
{
	if(m_pMediaPosition==NULL)
		return;
	double dLength;
	m_pMediaPosition->get_Duration(&dLength);
	m_pMediaPosition->put_CurrentPosition(dLength/10000.0*dPos);
}

bool CMediaPlayer::Free()
{
	return false;
}

void CMediaPlayer::SetFullScreen()
{
	if(m_pVideoWindow==NULL)
		return;
	//m_pVideoWindow->put_FullScreenMode(0);
	m_bFullScreen = !m_bFullScreen;
	if(m_bFullScreen)
	{
		m_pVideoWindow->put_Owner(NULL);
		m_pVideoWindow->GetWindowPosition(&m_iLeft,&m_iTop,&m_iWidth,&m_iHeight);
		int iWidth=GetSystemMetrics(SM_CXSCREEN); 
        int iHeight=GetSystemMetrics(SM_CYSCREEN); 	
		m_pVideoWindow->SetWindowPosition(0,0,iWidth,iHeight);
		//m_pVideoWindow->HideCursor(0);
	}
	else
	{
		m_pVideoWindow->put_Owner((OAHWND)m_hWnd);
		m_pVideoWindow->SetWindowPosition(m_iLeft,m_iTop,m_iWidth,m_iHeight);
	}
}
bool CMediaPlayer::Shutdown()
{
	SAFE_RELEASE(m_pBasicAudio);
	SAFE_RELEASE(m_pMediaPosition);
	SAFE_RELEASE(m_pVideoWindow);
	SAFE_RELEASE(m_pMediaEvent);
	SAFE_RELEASE(m_pMediaControl);
	SAFE_RELEASE(m_pGraphBuilder);
	CoUninitialize();
	return true;
}

bool CMediaPlayer::Play()
{
	m_bPlaying=true;   
	if(m_pMediaControl==NULL || m_pMediaPosition==NULL)
		return false;
	if(FAILED(m_pMediaControl->Run()))
		return false;
	//PutVolume(0);
	/*if( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
	{
		m_pMediaEvent->SetNotifyWindow((OAHWND)m_hWnd,WM_GRAPH_NOTIFY,NULL);
	}*/
	m_pMediaPosition->get_Rate(&m_dOriginalRate);   
	return true;
}

bool CMediaPlayer::Pause()
{
	m_bPlaying=false;   
	if(m_pMediaControl==NULL)
		return false;
	if(FAILED(m_pMediaControl->Pause()))
		return false;;
	return true;
}

bool CMediaPlayer::Stop()
{
	m_bPlaying=false;  
	if(m_pMediaControl==NULL || m_pMediaPosition==NULL)
		return false;
	if(FAILED(m_pMediaControl->Stop()))
		return false;
	m_pMediaPosition->put_CurrentPosition(0);
	return true;
}

bool CMediaPlayer::Fast()
{
	if(m_pMediaPosition==NULL)
		return false;
	double rate;
	m_pMediaPosition->get_Rate(&rate);   
	m_pMediaPosition->put_Rate(rate*1.1f);	
	return true;
}

bool CMediaPlayer::Slow()
{
	if(m_pMediaPosition==NULL)
		return false;
	double rate;
	m_pMediaPosition->get_Rate(&rate);    
	m_pMediaPosition->put_Rate(rate*0.9f);
	return true;
}

bool CMediaPlayer::Normal()
{
	if(m_pMediaPosition==NULL)
		return false;
	m_pMediaPosition->put_Rate(1.0f);
	return true;
}

bool CMediaPlayer::Esc()
{
	if(m_pMediaControl==NULL)
		return false;
	m_pMediaControl->Stop();
	double dLength;
	m_pMediaPosition->get_Duration(&dLength);
	m_pMediaPosition->put_CurrentPosition(dLength);
	m_pMediaControl->Run();
	::Sleep(2);
	return true;
}

bool CMediaPlayer::Render(WCHAR *wFileName)
{
	if(m_pGraphBuilder == NULL || m_pMediaControl == NULL)// Error checking
        return false;
	//m_pMediaEvent->SetNotifyWindow((OAHWND)m_hWnd,WM_GRAPH_NOTIFY,NULL);//register a window to process event notifications
	m_pMediaControl->Stop();//Stop the current song 
	//WCHAR wFileName[MAX_PATH];
	//mbstowcs(wFileName,FileName,MAX_PATH);//字节串→字符串
	//MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,FileName,strlen(FileName),wFileName,0);
	if(FAILED(m_pGraphBuilder->RenderFile(wFileName,NULL)))
		return false;
	m_pVideoWindow->put_Owner((OAHWND)m_hWnd);//在游戏窗口内播放视频
	m_pVideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPSIBLINGS);
	RECT rWindow;
	GetClientRect(m_hWnd,&rWindow);
	m_pVideoWindow->SetWindowPosition(0,0,rWindow.right,rWindow.bottom);
	m_pVideoWindow->put_Visible(OATRUE);
	return true;
}

bool CMediaPlayer::PutVolume(long Percent)
{
	if(m_pBasicAudio==NULL)
		return false;
	long lVolume;
    if(!Percent)
        lVolume = -10000;
    else 
        lVolume = -20 * (100 - (Percent % 101));
    if(FAILED(m_pBasicAudio->put_Volume(lVolume)))
        return false;
    m_lVolume = Percent % 101;
    return true;
}

long CMediaPlayer::GetVolume()
{
	if(m_pBasicAudio==NULL)
		return 0;
	long lVolume;//x mod y = x - y (x div y)
	m_pBasicAudio->get_Volume(&lVolume);//100%101=100-101*(100/101)
	return m_lVolume=100+lVolume/20;	
}

bool CMediaPlayer::Playing()
{ 
    if(m_pMediaEvent == NULL)// Error checking
        return false;
    long Event, Param1, Param2;// Get event and handle it
    m_pMediaEvent->GetEvent(&Event, &Param1, &Param2, 1);
    if(Event == EC_COMPLETE) 
    {
        m_pMediaEvent->FreeEventParams(Event, Param1, Param2);
        return true;
    }
    m_pMediaEvent->FreeEventParams(Event, Param1, Param2);
    return false;
}
