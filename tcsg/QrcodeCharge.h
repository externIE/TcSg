// QrcodeCharge.h: interface for the CQrcodeCharge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QRCODECHARGE_H__C7481BD8_AD7B_4B87_8754_D22C59A00077__INCLUDED_)
#define AFX_QRCODECHARGE_H__C7481BD8_AD7B_4B87_8754_D22C59A00077__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQrcodeCharge  :public CDialog
{
// Construction
public:
	CQrcodeCharge(CWnd* pParent = NULL);   // standard constructor

	void ShowDialog();

	CAdvertCtrl   m_htmWWW;
	
public:
	// Dialog Data
	//{{AFX_DATA(CQrcodeCharge)
	enum { IDD = IDD_QRCODE_CHARGE };
	//}}AFX_DATA
    CxImage     m_ImgBottom;
    CxImage     m_ImgTop;
    CxImage     m_ImgLeft;
    CxImage     m_ImgRight;

    CFont       m_Font;
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQrcodeCharge)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CQrcodeCharge)
	virtual BOOL OnInitDialog();
    afx_msg void CloseDlg();
	afx_msg void OnPaint();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg UINT OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitData(UINT wParam,LONG lParam);
	afx_msg void OnTcyNavigate(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_QRCODECHARGE_H__C7481BD8_AD7B_4B87_8754_D22C59A00077__INCLUDED_)
