#pragma once

#include <utility>

struct SignalParams
{
    enum ImpulseType : int { SQUARE = 0, CIRCLE, GAUSS };
    template < typename T >
    using pair_t = std::pair < T, T > ;
    int impulseType;
    pair_t < size_t > impulseCount;
    pair_t < double > impulseWidth;
    pair_t < double > impulseHeight;
    pair_t < size_t > dimensions;
};

// CRandomSignalParams dialog

class CRandomSignalParams : public CDialogEx
{
	DECLARE_DYNAMIC(CRandomSignalParams)

public:
	CRandomSignalParams(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRandomSignalParams();

// Dialog Data
	enum { IDD = IDD_RANDOMSIGNALPARAMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    SignalParams m_params;
};
