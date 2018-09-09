#pragma once

#include <util/common/gui/PlotControl.h>

// CImgViewer dialog

class CImgViewer : public CDialogEx
{
	DECLARE_DYNAMIC(CImgViewer)

public:
	CImgViewer(CWnd* pParent, CBitmap * bmp);   // standard constructor
	virtual ~CImgViewer();
    CBitmap * m_bmp;

// Dialog Data
	enum { IDD = IDD_IMGVIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    CPlotControl m_img;
    virtual BOOL OnInitDialog();
};
