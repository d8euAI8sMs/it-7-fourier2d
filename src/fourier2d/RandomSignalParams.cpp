// RandomSignalParams.cpp : implementation file
//

#include "stdafx.h"
#include "fourier2d.h"
#include "RandomSignalParams.h"
#include "afxdialogex.h"


// CRandomSignalParams dialog

IMPLEMENT_DYNAMIC(CRandomSignalParams, CDialogEx)

CRandomSignalParams::CRandomSignalParams(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRandomSignalParams::IDD, pParent)
    , m_params({ 0 })
{
}

CRandomSignalParams::~CRandomSignalParams()
{
}

void CRandomSignalParams::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_params.impulseCount.first);
    DDX_Text(pDX, IDC_EDIT2, m_params.impulseCount.second);
    DDX_Text(pDX, IDC_EDIT3, m_params.impulseWidth.first);
    DDX_Text(pDX, IDC_EDIT4, m_params.impulseWidth.second);
    DDX_Text(pDX, IDC_EDIT5, m_params.impulseHeight.first);
    DDX_Text(pDX, IDC_EDIT6, m_params.impulseHeight.second);
    DDX_Text(pDX, IDC_EDIT7, m_params.dimensions.first);
    DDX_Text(pDX, IDC_EDIT8, m_params.dimensions.second);
    DDX_Radio(pDX, IDC_RADIO1, m_params.impulseType);
}


BEGIN_MESSAGE_MAP(CRandomSignalParams, CDialogEx)
END_MESSAGE_MAP()


// CRandomSignalParams message handlers
