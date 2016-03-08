// MainFrame.h: interface for the CMainFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRAME_H__C980DEE2_90FE_4279_907E_2D883004F5DB__INCLUDED_)
#define AFX_MAINFRAME_H__C980DEE2_90FE_4279_907E_2D883004F5DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CGameFrameWnd  
{
	DECLARE_DYNCREATE(CMainFrame)
public:
	CMainFrame();
	virtual ~CMainFrame();

public:
	virtual CDialogBar*       GetGameDlgBar();

protected:
	// Generated message map functions
	//{{AFX_MSG(CMainFrame)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
 	BOOL CreateDialogBar();


public:
	CDialogBar          m_wndDlgBar;

};

#endif // !defined(AFX_MAINFRAME_H__C980DEE2_90FE_4279_907E_2D883004F5DB__INCLUDED_)
