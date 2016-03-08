// QrcodeCharge.cpp: implementation of the CQrcodeCharge class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game.h"
#include "MainFrame.h"
#include "QrcodeCharge.h"
#include <tcSkin.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQrcodeCharge::CQrcodeCharge(CWnd* pParent /*=NULL*/)
: CDialog(CQrcodeCharge::IDD, pParent)
{
    skinppExitSkin();
    skinppInitializeSkin();

    LOGFONT     logFont;
    ZeroMemory((void*)&logFont, sizeof(logFont));
    strcpy(logFont.lfFaceName, "宋体");
    logFont.lfHeight = -13;
    logFont.lfWeight = FW_NORMAL;
    logFont.lfOutPrecision  = 1;
    logFont.lfClipPrecision = 2;
    logFont.lfQuality       = 1;
    logFont.lfPitchAndFamily= 1;
    m_Font.CreateFontIndirect(&logFont);
}


void CQrcodeCharge::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameSetDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQrcodeCharge, CDialog)
//{{AFX_MSG_MAP(CQrcodeCharge)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_MESSAGE(WM_INIT_DATA,OnInitData)
	ON_COMMAND(IDC_CLOSE_QRCODECHARGE,CloseDlg)
	ON_MESSAGE(WM_TCY_NAVIGATE,OnTcyNavigate)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameSetDlg message handlers
void CQrcodeCharge::OnTcyNavigate(WPARAM wParam,LPARAM lParam)
{
	CAHREF * ahref=(CAHREF*)wParam;
	if(ahref->IsApp_MemberClose())
	{
		//PostMessage(WM_CLOSE, 0, 0);
		DestroyWindow();
	}

}
BOOL CQrcodeCharge::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	RECT rc;
    //pMainFrame->GetWindowRect(&rc);
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	CMyGame* pGame=(CMyGame*)(theApp.GetMainGame());
	rc = pGame->GetGameRect();
    MoveWindow((rc.right - rc.left)/2-200, (rc.bottom - rc.top)/2 - 170, 400, 340);
	
	CString str;
    str.Format(_T("%sskins\\skin1\\skin.ini"), theApp.GetGameFolder());
    skinppLoadSkin(str.GetBuffer(0), TRUE);

	str.Format(_T("%sskins\\skin1\\QRcharge_Bottom.bmp"),theApp.GetGameFolder());
	m_ImgBottom.Load(str.GetBuffer(0),CXIMAGE_FORMAT_BMP);

	str.Format(_T("%sskins\\skin1\\QRcharge_Top.bmp"),theApp.GetGameFolder());
	m_ImgTop.Load(str.GetBuffer(0),CXIMAGE_FORMAT_BMP);

	str.Format(_T("%sskins\\skin1\\QRcharge_Left.bmp"),theApp.GetGameFolder());
	m_ImgLeft.Load(str.GetBuffer(0),CXIMAGE_FORMAT_BMP);

	str.Format(_T("%sskins\\skin1\\QRcharge_Right.bmp"),theApp.GetGameFolder());
	m_ImgRight.Load(str.GetBuffer(0),CXIMAGE_FORMAT_BMP);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CQrcodeCharge::OnPaint() 
{
	CPaintDC dc(this); // device context for painting	
	// TODO: Add your message handler code here

    RECT rcTop = {5,0,395,24};
    m_ImgTop.Draw(dc.m_hDC,rcTop);
    
    RECT rcLeft = {0,0,5,340};
    m_ImgLeft.Draw(dc.m_hDC,rcLeft);
    
    RECT rcRight = {395,0,400,340};
    m_ImgRight.Draw(dc.m_hDC,rcRight);
    
    RECT rcBottom = {5,335,395,340};
	m_ImgBottom.Draw(dc.m_hDC,rcBottom);

    COLORREF OldColor = dc.SetTextColor(RGB(0, 0, 0));
    CFont*   pOldFont = dc.SelectObject(&m_Font);
    dc.SetBkMode(TRANSPARENT);

    CString str;
    str.Format("请耐心等待生成二维码...");
    dc.TextOut(10, 6, str);

    dc.SelectObject(pOldFont);
    dc.SetTextColor(OldColor);
}

UINT CQrcodeCharge::OnEraseBkgnd(CDC* pDC)
{
	return	CDialog::OnEraseBkgnd(pDC);
}

void CQrcodeCharge::ShowDialog()
{
	ShowWindow(SW_SHOW);
	
	RECT rcUpdate;
	GetClientRect(&rcUpdate);
	RedrawWindow(&rcUpdate);
}

UINT CQrcodeCharge::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
    rc.bottom = 24;
	ClientToScreen(&rc);
	
	return rc.PtInRect(point) ? HTCAPTION:CDialog::OnNcHitTest(point);

}

void CQrcodeCharge::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);	
	if (m_htmWWW)
	{
		m_htmWWW.MoveWindow(5,24,390,311);
		m_htmWWW.ShowWindow(SW_SHOW);
	}

    CWnd* pHwnd = GetDlgItem(IDC_CLOSE_QRCODECHARGE);
    if (pHwnd)
    {
        pHwnd->MoveWindow(364, 0, 34, 23);
    }

}

void CQrcodeCharge::CloseDlg()
{
    ShowWindow(SW_HIDE);
    DestroyWindow();
}

void CQrcodeCharge::OnInitData(UINT wParam,LONG lParam)
{
	m_htmWWW.CreateFromStatic(IDC_WWW4, this);  
	m_htmWWW.SetCheckDomain(TRUE);
	m_htmWWW.SetDomainFile(xyGetDomainFileName());
	m_htmWWW.SetNotifyParent(TRUE);

	CString sURL;
	sURL=(LPCTSTR)wParam;
	TCHAR * p =(TCHAR*) wParam;
	SAFE_DELETE_ARRAY(p);
    m_htmWWW.Navigate(sURL,NULL,NULL);
	//theApp.ShowMsgInChatView(sURL);
	

	m_htmWWW.MoveWindow(5,24,390,311);
	m_htmWWW.ShowWindow(SW_SHOW);
}