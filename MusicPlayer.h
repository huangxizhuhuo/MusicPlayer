#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"

class CMusicPlayerApp : public CWinApp
{
public:
	CMusicPlayerApp();

	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CMusicPlayerApp theApp;
