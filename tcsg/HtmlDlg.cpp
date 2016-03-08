// HtmlDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "game.h"
//#include "HtmlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHtmlDlg dialog


CHtmlDlg::CHtmlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHtmlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHtmlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHtmlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHtmlDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHtmlDlg, CDialog)
	//{{AFX_MSG_MAP(CHtmlDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHtmlDlg message handlers

BOOL CHtmlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	TCHAR szExeFile[1024];
	CGameApp* pGameApp=(CGameApp*)AfxGetApp();
	lstrcpy(szExeFile,pGameApp->GetURLWithWebSign("http://shop.uc108.com/egg/exchangeredpack.aspx",TRUE));

	RECT rc = {0,0,565,380};
	GetDlgItem(IDC_STATIC_IE)->MoveWindow(&rc);
	
	m_HTMLCTRL.CreateFromStatic(IDC_STATIC_IE,this);
	m_HTMLCTRL.Navigate(szExeFile,NULL,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHtmlDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CDialog::PostNcDestroy();
}

void CHtmlDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	DestroyWindow();
// 	CDialog::OnClose();
}

