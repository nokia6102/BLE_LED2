
// GenMatrixFont.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CGenMatrixFontApp:
// �аѾ\��@�����O�� GenMatrixFont.cpp
//

class CGenMatrixFontApp : public CWinApp
{
public:
	CGenMatrixFontApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CGenMatrixFontApp theApp;