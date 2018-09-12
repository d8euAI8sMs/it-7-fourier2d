
// fourier2dDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fourier2d.h"
#include "fourier2dDlg.h"
#include "afxdialogex.h"
#include "ImgViewer.h"

#include <array>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define RAND_RANGED(p) ((rand() / (RAND_MAX + 1.)) * (p.second - p.first) + p.first)


// CFourier2dDlg dialog



CFourier2dDlg::CFourier2dDlg(CWnd* pParent /*=NULL*/)
    : CSimulationDialog(CFourier2dDlg::IDD, pParent)
    , m_whichImage(FALSE)
    , m_resizeType(0)
    , m_noise(0)
    , m_signalParams({ SignalParams::SQUARE, {1,1}, {0.1,0.5}, {0.1,0.5}, {108,108} })
    , m_logScale(FALSE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_model.params = model::make_default_parameters();
    m_model.stage = model::stage_no;
    m_filterRadius = 0.0;
}

void CFourier2dDlg::DoDataExchange(CDataExchange* pDX)
{
    CSimulationDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IMG, m_image);
    DDX_Control(pDX, IDC_IMG2, m_fourier);
    DDX_Radio(pDX, IDC_RADIO3, m_whichImage);
    DDX_Radio(pDX, IDC_RADIO1, m_resizeType);
    DDX_Text(pDX, IDC_EDIT1, m_noise);
    DDX_Control(pDX, IDC_SLIDER1, m_filterRadiusSlider);
    DDX_Check(pDX, IDC_CHECK1, m_logScale);
    DDX_Control(pDX, IDC_QUALITY, m_quality);
}

BEGIN_MESSAGE_MAP(CFourier2dDlg, CSimulationDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_BUTTON3, &CFourier2dDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON4, &CFourier2dDlg::OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BUTTON5, &CFourier2dDlg::OnBnClickedButton5)
    ON_BN_CLICKED(IDC_RADIO3, &CFourier2dDlg::OnBnClickedRadio3)
    ON_BN_CLICKED(IDC_RADIO4, &CFourier2dDlg::OnBnClickedRadio4)
    ON_BN_CLICKED(IDC_RADIO5, &CFourier2dDlg::OnBnClickedRadio5)
    ON_BN_CLICKED(IDC_RADIO1, &CFourier2dDlg::OnBnClickedRadio1)
    ON_BN_CLICKED(IDC_RADIO2, &CFourier2dDlg::OnBnClickedRadio2)
    ON_BN_CLICKED(IDC_BUTTON7, &CFourier2dDlg::OnBnClickedApply)
    ON_BN_CLICKED(IDC_BUTTON1, &CFourier2dDlg::OnBnClickedRandom)
    ON_BN_CLICKED(IDC_BUTTON2, &CFourier2dDlg::OnBnClickedFile)
    ON_BN_CLICKED(IDC_BUTTON6, &CFourier2dDlg::OnBnClickedFilter)
    ON_BN_CLICKED(IDC_BUTTON8, &CFourier2dDlg::OnBnClickedButton8)
    ON_BN_CLICKED(IDC_BUTTON9, &CFourier2dDlg::OnBnClickedButton9)
    ON_BN_CLICKED(IDC_CHECK1, &CFourier2dDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CFourier2dDlg message handlers

BOOL CFourier2dDlg::OnInitDialog()
{
    CSimulationDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here

    m_filterRadiusSlider.SetRange(0, 1000, TRUE);
    m_filterRadiusSlider.SetPos(500);
    m_filterRadius = 0.5;

    auto c1 = [this] (CDC & dc, const plot::viewport & vp)
    {
        if (m_model.stage == model::stage_no) return;
        CDC memDC; memDC.CreateCompatibleDC(&dc);
        CBitmap * bmp;
        switch (m_whichImage)
        {
        case 0: bmp = &m_model.csource; break;
        case 1: bmp = &m_model.cnoised; break;
        case 2: bmp = &m_model.cfiltered; break;
        default: return;
        }
        memDC.SelectObject(bmp);
        auto wh = m_model.csource.GetBitmapDimension();
        dc.SetStretchBltMode(HALFTONE);
        dc.StretchBlt(vp.screen.xmin, vp.screen.ymin,
                      vp.screen.width(), vp.screen.height(),
                      &memDC, 0, 0, wh.cx, wh.cy, SRCCOPY);
    };
    auto c2 = [this] (CDC & dc, const plot::viewport & vp)
    {
        if (m_model.stage != model::stage_fourier &&
            m_model.stage != model::stage_final) return;
        CDC memDC; memDC.CreateCompatibleDC(&dc);
        memDC.SelectObject(&m_model.cfourier);
        auto wh = m_model.cfourier.GetBitmapDimension();
        dc.StretchBlt(vp.screen.xmin, vp.screen.ymin,
                      vp.screen.width(), vp.screen.height(),
                      &memDC, 0, 0, wh.cx, wh.cy, SRCCOPY);
        double r = m_filterRadius;
        size_t r0 = r * vp.screen.width() / 2;
        auto pen = plot::palette::pen(0x0000ff, 3);
        dc.SelectObject(pen.get());
        CBrush brNull;
	    LOGBRUSH lb;
	    lb.lbStyle = BS_NULL;
	    brNull.CreateBrushIndirect(&lb);
	    dc.SelectObject(&brNull);
        dc.Ellipse(vp.screen.xmin + vp.screen.width() / 2 - r0,
                   vp.screen.ymin + vp.screen.height() / 2 - r0,
                   vp.screen.xmin + vp.screen.width() / 2 + r0,
                   vp.screen.ymin + vp.screen.height() / 2 + r0);
    };

    m_image.symmetric = true;
    m_image.plot_layer.with(model::make_root_drawable({
        plot::custom_drawable::create(c1)
    }));
    m_fourier.symmetric = true;
    m_fourier.plot_layer.with(model::make_root_drawable({
        plot::custom_drawable::create(c2)
    }));

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFourier2dDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CSimulationDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFourier2dDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CFourier2dDlg::OnSimulation()
{
    CSimulationDialog::OnSimulation();
}



void CFourier2dDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    int pos = m_filterRadiusSlider.GetPos();
    m_filterRadius = pos / 1000.0;
    m_fourier.RedrawWindow();

    CSimulationDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CFourier2dDlg::OnBnClickedButton3()
{
    if (m_model.stage == model::stage_no) return;
    CImgViewer v(this, &m_model.csource);
    v.DoModal();
}


void CFourier2dDlg::OnBnClickedButton4()
{
    if (m_model.stage == model::stage_source || 
        m_model.stage == model::stage_no) return;
    CImgViewer v(this, &m_model.cnoised);
    v.DoModal();
}


void CFourier2dDlg::OnBnClickedButton5()
{
    if (m_model.stage != model::stage_final) return;
    CImgViewer v(this, &m_model.cfiltered);
    v.DoModal();
}


void CFourier2dDlg::OnBnClickedRadio3()
{
    m_whichImage = 0;
    m_image.RedrawWindow();
}


void CFourier2dDlg::OnBnClickedRadio4()
{
    m_whichImage = 1;
    m_image.RedrawWindow();
}


void CFourier2dDlg::OnBnClickedRadio5()
{
    m_whichImage = 2;
    m_image.RedrawWindow();
}


void CFourier2dDlg::OnBnClickedRadio1()
{
    OnBnClickedApply();
}


void CFourier2dDlg::OnBnClickedRadio2()
{
    OnBnClickedApply();
}


void CFourier2dDlg::OnBnClickedApply()
{
    UpdateData(TRUE);

    if (m_model.stage == model::stage_no) return;

    m_model.noised = m_model.source;

    double m = 0, e = 0, en = 0;
    for (size_t i = 0; i < m_model.source.h; ++i)
    for (size_t j = 0; j < m_model.source.w; ++j)
        m += m_model.source.data[i][j].re;
    m /= m_model.source.h * m_model.source.w;
    for (size_t i = 0; i < m_model.source.h; ++i)
    for (size_t j = 0; j < m_model.source.w; ++j)
    {
        double n = rand() / (RAND_MAX + 1.) - 0.5;
        m_model.noised.data[i][j].re = n;
        double d = m_model.source.data[i][j].re - m;
        e += d * d;
        en += n * n;
    }

    double a = std::sqrt(m_noise / 100. * e / en);

    for (size_t i = 0; i < m_model.source.h; ++i)
    for (size_t j = 0; j < m_model.source.w; ++j)
    {
        m_model.noised.data[i][j].re =
            std::abs(m_model.source.data[i][j].re + a * m_model.noised.data[i][j].re);
    }

    m_model.noised.to_cbitmap(m_model.cnoised, 1, false);
    m_model.stage = model::stage_source;
    m_image.RedrawWindow();

    auto bmp = m_model.noised;
    size_t wh = 1 << ((size_t) std::ceil(std::log2(std::fmax(bmp.w, bmp.h))));

    if (m_resizeType == 0)
        bmp.resize(wh, wh);
    else bmp.reshape(wh, wh);

    m_model.fourier.fourier_of(bmp);

    auto tmp = m_model.fourier;

    tmp.rearrange();
    if (m_logScale) tmp.log(1);

    double d = 0;
    for (size_t i = 0; i < tmp.h; ++i)
    for (size_t j = 0; j < tmp.w; ++j)
    {
        if (i == 0 && j == 0 || i + 1 == tmp.h && j + 1 == tmp.w ||
            i + 1 == tmp.h && j == 0 || i == 0 && j + 1 == tmp.w) continue;
        if (!m_logScale) d += math::sqnorm(tmp.data[i][j]);
        else d = std::fmax(d, tmp.data[i][j].re);
    }
    if (!m_logScale) d /= tmp.h * tmp.w;
    tmp.to_cbitmap(m_model.cfourier, std::sqrt(d), !m_logScale);
    m_model.stage = model::stage_fourier;
    m_fourier.RedrawWindow();

    OnBnClickedFilter();
}


void CFourier2dDlg::OnBnClickedRandom()
{
    CRandomSignalParams dlg(this);
    dlg.m_params = m_signalParams;
    if (dlg.DoModal() != IDOK) return;
        
    m_signalParams = dlg.m_params;

    size_t w = m_signalParams.dimensions.first;
    size_t h = m_signalParams.dimensions.second;
    m_model.source.data.clear();
    m_model.source.reshape(h, w);

    size_t r1 = (size_t) RAND_RANGED(m_signalParams.impulseCount);
    std::vector < std::array < double, 4 > > ps(r1);
    for (size_t i = 0; i < r1; ++i) ps[i] = {{ /*0*/ RAND_RANGED(m_signalParams.impulseWidth),
                                               /*1*/ RAND_RANGED(m_signalParams.impulseHeight),
                                               /*2*/ RAND_RANGED(std::make_pair(-0.5, 0.5)),
                                               /*3*/ RAND_RANGED(std::make_pair(-0.5, 0.5)) }};
    for (size_t i = 0; i < h; ++i)
    for (size_t j = 0; j < w; ++j)
    {
        double x = (double) i / h - 0.5, y = (double) j / w - 0.5;
        for each (auto & p in ps)
        {
            switch (m_signalParams.impulseType)
            {
            case SignalParams::SQUARE:
                if ((std::get<2>(p) - std::get<0>(p) / 2 <= x && x <= std::get<2>(p) + std::get<0>(p) / 2) &&
                    (std::get<3>(p) - std::get<1>(p) / 2 <= y && y <= std::get<3>(p) + std::get<1>(p) / 2))
                    m_model.source.data[i][j] = 1;
                break;
            case SignalParams::CIRCLE:
                if (((x - std::get<2>(p)) * (x - std::get<2>(p)) / std::get<0>(p) / std::get<0>(p) * 4 +
                     (y - std::get<3>(p)) * (y - std::get<3>(p)) / std::get<1>(p) / std::get<1>(p) * 4) <= 1)
                    m_model.source.data[i][j] = 1;
                break;
            case SignalParams::GAUSS:
            default:
                    m_model.source.data[i][j].re += std::exp(- (x - std::get<2>(p)) * (x - std::get<2>(p)) / std::get<0>(p) / std::get<0>(p) * 9 -
                                                               (y - std::get<3>(p)) * (y - std::get<3>(p)) / std::get<1>(p) / std::get<1>(p) * 9);
                break;
            }
        }
    }
    m_model.source.to_cbitmap(m_model.csource, 1, false);
    m_model.stage = model::stage_source;
    m_image.RedrawWindow();

    OnBnClickedApply();
}


void CFourier2dDlg::OnBnClickedFile()
{
    CFileDialog f(TRUE, TEXT("bmp"));
    if (f.DoModal() != IDOK) return;
    CImage img;
    img.Load(f.GetPathName());
    CBitmap bmp;
    bmp.Attach(img.Detach());

    m_model.source.from_cbitmap(bmp);

    m_model.source.to_cbitmap(m_model.csource, 1, false);
    m_model.stage = model::stage_source;
    m_image.RedrawWindow();

    OnBnClickedApply();
}


void CFourier2dDlg::OnBnClickedFilter()
{
    if (m_model.stage != model::stage_fourier &&
        m_model.stage != model::stage_final) return;

    size_t fr = m_filterRadius * m_model.fourier.h / 2;

    auto tmp = m_model.fourier;
    for (size_t i = 0; i < m_model.fourier.h; ++i)
    for (size_t j = 0; j < m_model.fourier.w; ++j)
    {
        double d = i * i + j * j;
        if (d < fr * fr) continue;
        d = (m_model.fourier.h - i) * (m_model.fourier.h - i) + j * j;
        if (d < fr * fr) continue;
        d = i * i + (m_model.fourier.w - j) * (m_model.fourier.w - j);
        if (d < fr * fr) continue;
        d = (m_model.fourier.h - i) * (m_model.fourier.h - i) +
            (m_model.fourier.w - j) * (m_model.fourier.w - j);
        if (d < fr * fr) continue;
        tmp.data[i][j] = 0;
    }

    m_model.filtered.fourier_of(tmp, true);
    if (m_resizeType == 0)
        m_model.filtered.resize(m_model.source.h, m_model.source.w);
    else m_model.filtered.reshape(m_model.source.h, m_model.source.w);
    m_model.filtered.to_cbitmap(m_model.cfiltered, 1, false);

    m_model.stage = model::stage_final;
    
    m_image.RedrawWindow();

    double enow = 0, ewas = 0;
    double now = 0, was = 0;

    for (size_t i = 0; i < m_model.filtered.h; ++i)
    for (size_t j = 0; j < m_model.filtered.w; ++j)
    {
        ewas += m_model.source.data[i][j].re * m_model.source.data[i][j].re;
        enow += m_model.filtered.data[i][j].re * m_model.filtered.data[i][j].re;
    }

    for (size_t i = 0; i < m_model.filtered.h; ++i)
    for (size_t j = 0; j < m_model.filtered.w; ++j)
    {
        now += math::sqnorm(m_model.filtered.data[i][j].re / enow * ewas - m_model.source.data[i][j].re);
        was += math::sqnorm(m_model.noised.data[i][j].re - m_model.source.data[i][j].re);
    }

    now /= m_model.filtered.h * m_model.filtered.w;
    was /= m_model.filtered.h * m_model.filtered.w;

    CString buf; buf.Format(TEXT("%lf, was %lf"), std::sqrt(now), std::sqrt(was));
    m_quality.SetWindowText(buf);
}


void CFourier2dDlg::OnBnClickedButton8()
{
    if (m_model.stage == model::stage_no) return;

    auto bmp = m_model.noised;
    size_t wh = 1 << ((size_t) std::ceil(std::log2(std::fmax(bmp.w, bmp.h))));

    if (m_resizeType == 0)
        bmp.resize(wh, wh);
    else bmp.reshape(wh, wh);

    CBitmap bmp0;
    bmp.to_cbitmap(bmp0, 1, false);

    CImgViewer v(this, &bmp0);
    v.DoModal();
}


void CFourier2dDlg::OnBnClickedButton9()
{
    if (m_model.stage != model::stage_fourier &&
        m_model.stage != model::stage_final) return;
    CImgViewer v(this, &m_model.cfourier);
    v.DoModal();
}


void CFourier2dDlg::OnBnClickedCheck1()
{
    OnBnClickedApply();
}
