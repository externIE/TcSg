#if !defined(AFX_HTMLDLG_H__15DB2088_EFB9_47A8_AA69_17BE45C8451F__INCLUDED_)
#define AFX_HTMLDLG_H__15DB2088_EFB9_47A8_AA69_17BE45C8451F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HtmlDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHtmlDlg dialog

class CHtmlDlg : public CDialog
{
// Construction
public:
	CHtmlDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHtmlDlg)
	enum { IDD = IDD_DIALOG_IE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CHtmlCtrl				m_HTMLCTRL;					//html¿Ø¼þ
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHtmlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLDLG_H__15DB2088_EFB9_47A8_AA69_17BE45C8451F__INCLUDED_)
