
// fourier2dDlg.h : header file
//

#pragma once

#include <util/common/gui/SimulationDialog.h>
#include <util/common/gui/PlotControl.h>

#include "model.h"
#include "afxcmn.h"
#include "RandomSignalParams.h"


// CFourier2dDlg dialog
class CFourier2dDlg : public CSimulationDialog
{
// Construction
public:
    CFourier2dDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    enum { IDD = IDD_FOURIER2D_DIALOG };

    virtual void OnSimulation();

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    CPlotControl m_image;
    CPlotControl m_fourier;
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    int m_whichImage;
    int m_resizeType;
    model::model_data m_model;
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton4();
    afx_msg void OnBnClickedButton5();
    afx_msg void OnBnClickedRadio3();
    afx_msg void OnBnClickedRadio4();
    afx_msg void OnBnClickedRadio5();
    afx_msg void OnBnClickedRadio1();
    afx_msg void OnBnClickedRadio2();
    afx_msg void OnBnClickedApply();
    afx_msg void OnBnClickedRandom();
    afx_msg void OnBnClickedFile();
    afx_msg void OnBnClickedFilter();
    double m_noise;
    double m_filterRadius;
    CSliderCtrl m_filterRadiusSlider;
    SignalParams m_signalParams;
    afx_msg void OnBnClickedButton8();
    afx_msg void OnBnClickedButton9();
    BOOL m_logScale;
    afx_msg void OnBnClickedCheck1();
    CStatic m_quality;
    afx_msg void OnBnClickedCheck2();
    BOOL m_bButterworth;
};
