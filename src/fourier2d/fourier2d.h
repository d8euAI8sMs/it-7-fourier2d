
// fourier2d.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"        // main symbols


// CFourier2dApp:
// See fourier2d.cpp for the implementation of this class
//

class CFourier2dApp : public CWinApp
{
public:
    CFourier2dApp();

// Overrides
public:
    virtual BOOL InitInstance();

// Implementation

    DECLARE_MESSAGE_MAP()
};

extern CFourier2dApp theApp;