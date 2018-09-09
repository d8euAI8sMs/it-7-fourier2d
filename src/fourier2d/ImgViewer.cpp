// ImgViewer.cpp : implementation file
//

#include "stdafx.h"
#include "fourier2d.h"
#include "ImgViewer.h"
#include "afxdialogex.h"
#include "model.h"

#include <util/common/plot/plot.h>


// CImgViewer dialog

IMPLEMENT_DYNAMIC(CImgViewer, CDialogEx)

CImgViewer::CImgViewer(CWnd* pParent, CBitmap * bmp)
	: CDialogEx(CImgViewer::IDD, pParent)
    , m_bmp(bmp)
{
}

CImgViewer::~CImgViewer()
{
}

void CImgViewer::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PICTURE, m_img);
}


BEGIN_MESSAGE_MAP(CImgViewer, CDialogEx)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CImgViewer message handlers


void CImgViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
    OnOK();
    CDialogEx::OnLButtonDown(nFlags, point);
}


BOOL CImgViewer::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    auto c1 = [this] (CDC & dc, const plot::viewport & vp)
    {
        if (m_bmp == nullptr) return;
        CDC memDC; memDC.CreateCompatibleDC(&dc);
        memDC.SelectObject(m_bmp);
        auto wh = m_bmp->GetBitmapDimension();
        dc.BitBlt(vp.screen.xmin, vp.screen.ymin, wh.cx, wh.cy,
                      &memDC, 0, 0, SRCCOPY);
    };

    m_img.plot_layer.with(model::make_root_drawable({
        plot::custom_drawable::create(c1)
    }));

    auto wh = m_bmp->GetBitmapDimension();
    CRect sz;
    m_img.GetClientRect(&sz);

    int dx = wh.cx - sz.Width(), dy = wh.cy - sz.Height();
    m_img.SetWindowPos(nullptr, 0, 0, sz.Width() + dx, sz.Height() + dy, SWP_NOMOVE | SWP_NOZORDER);
    GetWindowRect(&sz);
    SetWindowPos(nullptr, sz.left, sz.top, sz.Width() + dx, sz.Height() + dy, SWP_NOMOVE | SWP_NOZORDER);

    m_img.RedrawWindow();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
