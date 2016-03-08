// MainFrame.cpp: implementation of the CMainFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainFrame.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMainFrame, CGameFrameWnd)

CMainFrame::CMainFrame()
{

}

CMainFrame::~CMainFrame()
{

}

BEGIN_MESSAGE_MAP(CMainFrame, CGameFrameWnd)
//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{ 
	if (CGameFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	  	////////  m_wndDialogBar        //////////////////
	if(!CreateDialogBar())
	{
		TRACE0("Failed to create dialog bar\n");
		return -1;
	}

	return 0;
}

BOOL CMainFrame::CreateDialogBar()
{
   	if (!m_wndDlgBar.Create(this, IDD_DLGBAR,
		CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_DLGBAR))
		return FALSE;
	return TRUE;
	
} 

CDialogBar* CMainFrame::GetGameDlgBar()
{
	return &m_wndDlgBar;
}

void CMainFrame::OnClose()
{
	CGameFrameWnd::OnClose();
}
