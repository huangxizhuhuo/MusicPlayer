#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
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
