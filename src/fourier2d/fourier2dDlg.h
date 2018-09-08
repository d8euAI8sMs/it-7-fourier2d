
// fourier2dDlg.h : header file
//

#pragma once

#include <util/common/gui/SimulationDialog.h>


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
};
