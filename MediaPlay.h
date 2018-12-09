#pragma once

#include <dshow.h>

class CMediaPlayer
{
private:
	IGraphBuilder  *m_pGraphBuilder;
    IMediaControl  *m_pMediaControl;
    IMediaEventEx  *m_pMediaEvent;
	IVideoWindow   *m_pVideoWindow;
	IMediaPosition *m_pMediaPosition;
	IBasicAudio    *m_pBasicAudio;
	HWND            m_hWnd;
	bool            m_bPlaying;
	bool            m_bFullScreen;
	long            m_iLeft;
	long            m_iTop;
	long            m_iWidth;
	long            m_iHeight;
	long            m_lVolume;
	double          m_dOriginalRate;
public:
	CMediaPlayer();
	~CMediaPlayer();
	bool	Init(HWND hWnd);
	bool	Free();
	bool	Shutdown();
	bool	Render(WCHAR *wFileName);
	bool	Play();
	bool	Stop();
	bool	Pause(); 
    bool	Playing();
	bool	Esc();
	bool	Fast();
	bool	Slow();
	bool	Normal();
	int		GetCurrentPos();
	int     GetCurrentTime();
	double  GetCurrentSpeed();
	int		GetDuration();
	void	PutCurrentPos(double dPos);
	bool	PutVolume(long Percent);
	long	GetVolume();
	void	SetFullScreen();
};


