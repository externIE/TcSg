#include "StdAfx.h"
#include <tcSkin.h>
#include "MainFrame.h"
#include "GameChargeDlg.h"

CString  MD5String(LPTSTR lpszContent)
{	
    CString sRet;
    
    MD5_CTX mdContext;
	UwlMD5Init (&mdContext);
    UwlMD5Update (&mdContext, (unsigned char *)(LPTSTR)lpszContent, lstrlen(lpszContent));
    UwlMD5Final (&mdContext);	
    UwlConvertHexToStr(mdContext.digest, 16, sRet);
    
    return sRet;	
}


CGameChargeDlg::CGameChargeDlg(CWnd* pParent /* = NULL */)
:CDialog(CGameChargeDlg::IDD, pParent)
{
    skinppInitializeSkin();
    for (int i = 0; i < TOTAL_DENOMINATION; i++)
    {
        m_stMoneyInfo[i].enStatus = MONEY_STATUS_NORMAL;
        m_stMoneyInfo[i].nMoney = -1;
        m_stMoneyInfo[i].nExChangeTongbao = -1;
        m_stMoneyInfo[i].nExChangeSilver = -1;
        m_stMoneyInfo[i].nGivePercent = -1;
    }

    LOGFONT     logFont;
    ZeroMemory((void*)&logFont, sizeof(logFont));
    strcpy(logFont.lfFaceName, "宋体");
    logFont.lfHeight = -13;
    logFont.lfWeight = FW_NORMAL;
    logFont.lfOutPrecision  = 1;
    logFont.lfClipPrecision = 2;
    logFont.lfQuality       = 1;
    logFont.lfPitchAndFamily= 1;
    m_Font1.CreateFontIndirect(&logFont);

    logFont.lfHeight = -12;
    m_Font2.CreateFontIndirect(&logFont);

    m_bTransDeposit = TRUE;
	m_chargeStat = -1;
}

void CGameChargeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGameChargeDlg, CDialog)
ON_WM_PAINT()
ON_WM_NCHITTEST()
ON_WM_ERASEBKGND()
ON_WM_SIZE()
ON_WM_MOVE()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONDOWN()
ON_WM_CTLCOLOR()
ON_WM_SETCURSOR()
ON_COMMAND(IDC_BTN_CLOSECHARGE, OnCloseDlg)
ON_COMMAND(IDC_BTN_WEB_CHARGE, OnWebCharge)
ON_COMMAND(IDC_BTN_QRCODE_CHARGE, OnQrcodeCharge)
ON_COMMAND(IDC_BTN_QRCODE_CHARGE_WECHAT, OnQrcodeCharge_WeChat)
END_MESSAGE_MAP()

void    CGameChargeDlg::SetMoneyInfo(MONEY_INFO* pMoneyInfo, LPCHARGEURL pChargeUrl)
{
    if (NULL == pMoneyInfo)
    {
        return;
    }

    memcpy(m_stMoneyInfo, pMoneyInfo, sizeof(m_stMoneyInfo));
	memcpy(&m_chargeUrl, pChargeUrl, sizeof(m_chargeUrl));
}

void    CGameChargeDlg::OnCloseDlg()
{
	m_chargeStat = -1;
    CGameApp* pGameApp=(CGameApp*)AfxGetApp();
	if (pGameApp)
	{
		CMyGame* pGame=(CMyGame*)(pGameApp->GetMainGame());
		if (pGame)
		{
			pGame->APPLY_WaitFewTimes(10,&(SNG_MSG)CreateMsg(pGame->GetHandle(),TCSG_CALLBACKMSG_ClOSECHARGE));
		}
	}
    ShowWindow(SW_HIDE);
    skinppExitSkin();
    DestroyWindow();
}

void    CGameChargeDlg::OnWebCharge()
{
	m_chargeStat = 1;
    CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
	if (pApp)
	{
		CMyGame* pGame=(CMyGame*)(pApp->GetMainGame());
		if (pGame)
		{
			pGame->APPLY_WaitFewTimes(10,&(SNG_MSG)CreateMsg(pGame->GetHandle(),TCSG_CALLBACKMSG_WEBCHARGE));
		}
	}
    
    CString str;
    //str.Format("http://pay.uc108.com/game/?cm=19");
	str.Format(m_chargeUrl.WebUrl);
    
    if (pApp && pApp->GetOfflineFlag()==OFFLINEFLAG_OFFLINE)
        pApp->OpenHtml(str);
    else
        pApp->OpenHtmlInHall(str);

    {
        CGameRequest* pGameConnect = (CGameRequest*)theApp.GetGameConnect();
        if (pGameConnect)
        {
            pGameConnect->GC_ChargeBtnCount(OTHERWAY_CHARGE_BTN);
        }
    }
}


void    CGameChargeDlg::OnQrcodeCharge()
{
	m_chargeStat = 2;
    //先准备好所有数据
	CGameApp* pGameApp=(CGameApp*)AfxGetApp();
	if (pGameApp)
	{
		CMyGame* pGame=(CMyGame*)(pGameApp->GetMainGame());
		if (pGame)
		{
			pGame->APPLY_WaitFewTimes(10,&(SNG_MSG)CreateMsg(pGame->GetHandle(),TCSG_CALLBACKMSG_QRCODECHARGE));
		}
	}
    int nGameID = GAME_ID;
    int nUserID = theApp.GetUserID();
    int nMoney = 0;
    int nExChange = 1;

    int nTotalSel = 0;
    for (int i = 0; i < TOTAL_DENOMINATION; i++)
    {
        if (MONEY_STATUS_SEL == m_stMoneyInfo[i].enStatus)
        {
            nMoney = m_stMoneyInfo[i].nMoney;
            nTotalSel++;
        }
    }

    if (nTotalSel > 1 || nMoney <= 0)
    {
        int nLifeTime = 10;
        theApp.ShowTimerMsgBox(_T("对不起，系统错误！请重试！"), NULL, NULL, nLifeTime, 
			    TIMERDLG_FLAGS_ICON|TIMERDLG_FLAGS_DANYMIC|TIMERDLG_FLAGS_ZERO_CANCEL,"xyf抱歉");
        OnCloseDlg();
        return;
    }

    if (nTotalSel == 0)
    {
        int nLifeTime = 10;
        theApp.ShowTimerMsgBox(_T("请选择您要充值的金额！"), NULL, NULL, nLifeTime, 
			    TIMERDLG_FLAGS_ICON|TIMERDLG_FLAGS_DANYMIC|TIMERDLG_FLAGS_ZERO_CANCEL,"xyf抱歉");
        return;
    }

    if (m_bTransDeposit)
    {
        nExChange = 1;
    }
    else
    {
        nExChange = 0;
    }

    long unix_time = time(NULL);
    CString strTpMjKey;//密钥
    strTpMjKey.Format("j8xpu92od6t8t1q4");
    CString strMD5;
    strMD5.Format("exchange%dgame_id%dmoney%dunix_time%lduser_id%d%s", nExChange, nGameID, nMoney, unix_time, nUserID, strTpMjKey);

    CString strMD5Dest = MD5String(strMD5.GetBuffer(512));
	strMD5.ReleaseBuffer();

    CString strBaseUrl;
	strBaseUrl.Format(m_chargeUrl.OrCodeUrl);

    CString strDstUrl;
    strDstUrl.Format("%s?game_id=%d&user_id=%d&money=%d&exchange=%d&unix_time=%ld&sign=%s", strBaseUrl, nGameID, nUserID, nMoney, nExChange, unix_time, strMD5Dest);
 
    theApp.ShowQrChargeDlg(strDstUrl);
    HideChargeDlg();

    {
        CGameRequest* pGameConnect = (CGameRequest*)theApp.GetGameConnect();
        if (pGameConnect)
        {
            pGameConnect->GC_ChargeBtnCount(QRCODE_CHARGE_BTN, nMoney);
        }
    }
}

void    CGameChargeDlg::OnQrcodeCharge_WeChat()
{
	m_chargeStat = 2;
    //先准备好所有数据
	CGameApp* pGameApp=(CGameApp*)AfxGetApp();
	if (pGameApp)
	{
		CMyGame* pGame=(CMyGame*)(pGameApp->GetMainGame());
		if (pGame)
		{
			pGame->APPLY_WaitFewTimes(10,&(SNG_MSG)CreateMsg(pGame->GetHandle(),TCSG_CALLBACKMSG_QRCODECHARGE));
		}
	}
    int nGameID = GAME_ID;
    int nUserID = theApp.GetUserID();
    int nMoney = 0;
    int nExChange = 1;
	
    int nTotalSel = 0;
    for (int i = 0; i < TOTAL_DENOMINATION; i++)
    {
        if (MONEY_STATUS_SEL == m_stMoneyInfo[i].enStatus)
        {
            nMoney = m_stMoneyInfo[i].nMoney;
            nTotalSel++;
        }
    }
	
    if (nTotalSel > 1 || nMoney <= 0)
    {
        int nLifeTime = 10;
        theApp.ShowTimerMsgBox(_T("对不起，系统错误！请重试！"), NULL, NULL, nLifeTime, 
			TIMERDLG_FLAGS_ICON|TIMERDLG_FLAGS_DANYMIC|TIMERDLG_FLAGS_ZERO_CANCEL,"xyf抱歉");
        OnCloseDlg();
        return;
    }
	
    if (nTotalSel == 0)
    {
        int nLifeTime = 10;
        theApp.ShowTimerMsgBox(_T("请选择您要充值的金额！"), NULL, NULL, nLifeTime, 
			TIMERDLG_FLAGS_ICON|TIMERDLG_FLAGS_DANYMIC|TIMERDLG_FLAGS_ZERO_CANCEL,"xyf抱歉");
        return;
    }
	
    if (m_bTransDeposit)
    {
        nExChange = 1;
    }
    else
    {
        nExChange = 0;
    }
	
    long unix_time = time(NULL);
    CString strTpMjKey;//密钥
    strTpMjKey.Format("j8xpu92od6t8t1q4");
    CString strMD5;
    strMD5.Format("exchange%dgame_id%dmoney%dunix_time%lduser_id%d%s", nExChange, nGameID, nMoney, unix_time, nUserID, strTpMjKey);
	
    CString strMD5Dest = MD5String(strMD5.GetBuffer(512));
	strMD5.ReleaseBuffer();
	
    CString strBaseUrl;
	strBaseUrl.Format(m_chargeUrl.OrCodeUrl_WeChat);
	
    CString strDstUrl;
    strDstUrl.Format("%s?game_id=%d&user_id=%d&money=%d&exchange=%d&unix_time=%ld&sign=%s", strBaseUrl, nGameID, nUserID, nMoney, nExChange, unix_time, strMD5Dest);
	
    theApp.ShowQrChargeDlg(strDstUrl);
    HideChargeDlg();
	
    {
        CGameRequest* pGameConnect = (CGameRequest*)theApp.GetGameConnect();
        if (pGameConnect)
        {
            pGameConnect->GC_ChargeBtnCount(QRCODE_CHARGE_BTN_WECHAT, nMoney);
        }
    }
}

BOOL    CGameChargeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    CString str;
    str.Format(_T("%sskins\\skin1\\skin.ini"), theApp.GetGameFolder());
    skinppLoadSkin(str.GetBuffer(0), TRUE);
    
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    RECT rc;

    //pMainFrame->GetWindowRect(&rc);
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	CMyGame* pGame=(CMyGame*)(theApp.GetMainGame());
	rc = pGame->GetGameRect();
    MoveWindow((rc.right - rc.left)/2-200, (rc.bottom - rc.top)/2 - 170, 400, 340);

    str.Format(_T("%sskins\\skin1\\charge_frame.bmp"), theApp.GetGameFolder());
    m_imgBK.Load(str.GetBuffer(0), CXIMAGE_FORMAT_BMP);

    for (int i = 0; i < TOTAL_DENOMINATION; i++)
    {
        str.Format(_T("%sskins\\skin1\\money_normal_%d.bmp"), theApp.GetGameFolder(), m_stMoneyInfo[i].nMoney);
        m_imgMoneyNoraml[i].Load(str.GetBuffer(0), CXIMAGE_FORMAT_BMP);

        str.Format(_T("%sskins\\skin1\\money_touch_%d.bmp"), theApp.GetGameFolder(), m_stMoneyInfo[i].nMoney);
        m_imgMoneyTouch[i].Load(str.GetBuffer(0), CXIMAGE_FORMAT_BMP);

        str.Format(_T("%sskins\\skin1\\money_selet_%d.bmp"), theApp.GetGameFolder(), m_stMoneyInfo[i].nMoney);
        m_imgMoneySel[i].Load(str.GetBuffer(0), CXIMAGE_FORMAT_BMP);
    }

    return TRUE;
}

void    CGameChargeDlg::OnPaint()
{
    CPaintDC dc(this);

    RECT rc;
    GetClientRect(&rc);
    m_imgBK.Draw(dc.m_hDC, rc);

    DrawMoneyRect(dc);

    DrawChargeText(dc);
}

void    CGameChargeDlg::GetMoneyRect(int nIndex, RECT& rc)
{//nIndex 0-7
    if (nIndex >= TOTAL_DENOMINATION)
    {
        return;
    }

    int nIndexX = nIndex % (TOTAL_DENOMINATION/2);
    int nIndexY = nIndex / (TOTAL_DENOMINATION/2);

    rc.left     = 60 + 71*(nIndexX);
    rc.top      = 90 + 32*(nIndexY);
    rc.right    = rc.left + MONEY_RECT_WIDTH;
    rc.bottom   = rc.top + MONEY_RECT_HEIGHT;

    return;
}


void    CGameChargeDlg::DrawMoneyRect(CPaintDC& dc)
{
    for (int i = 0; i < TOTAL_DENOMINATION; i++)
    {
        RECT rcMoneyInfo;
        ZeroMemory(&rcMoneyInfo, sizeof(rcMoneyInfo));

        GetMoneyRect(i, rcMoneyInfo);

        if (m_stMoneyInfo[i].nMoney > 0)
        {
            if (MONEY_STATUS_NORMAL == m_stMoneyInfo[i].enStatus)
            {
                m_imgMoneyNoraml[i].Draw(dc.m_hDC, rcMoneyInfo);
            }
            else if (MONEY_STATUS_TOUCH == m_stMoneyInfo[i].enStatus)
            {
                m_imgMoneyTouch[i].Draw(dc.m_hDC, rcMoneyInfo);
            }
            else if (MONEY_STATUS_SEL == m_stMoneyInfo[i].enStatus)
            {
                m_imgMoneySel[i].Draw(dc.m_hDC, rcMoneyInfo);
            }
        }
    }

    return;
}


void    CGameChargeDlg::DrawChargeText(CPaintDC& dc)
{
    COLORREF OldColor = dc.SetTextColor(RGB(0, 0, 0));
    CFont*   pOldFont = dc.SelectObject(&m_Font1);
    dc.SetBkMode(TRANSPARENT);
    int nPosX = 45;
    int nPosY = 210;
    CString strText;
    strText.Format("充值");
    dc.TextOut(10, 6, strText);


    strText.Format("兑换获得银子：");
    dc.TextOut(nPosX, nPosY, strText);
    CSize csExt3 = dc.GetTextExtent(strText);

    int nSilver = 0;

    for (int i = 0; i < TOTAL_DENOMINATION; i++)
    {
        if (m_stMoneyInfo[i].enStatus == MONEY_STATUS_SEL)
        {
            nSilver = m_stMoneyInfo[i].nExChangeSilver;
            break;
        }
    }


    if (0 != nSilver)
    {
        //兑换获得银子
        if (m_bTransDeposit)
        {
            dc.SetTextColor(RGB(255, 100, 0));
        }
        else
        {
            dc.SetTextColor(RGB(125, 125, 125));
            nSilver = 0;
        }
        dc.SelectObject(m_Font1);
        strText.Format("%d两", nSilver);
        dc.TextOut(nPosX+csExt3.cx+2, nPosY, strText);

        CSize csExt7 = dc.GetTextExtent(strText);
        strText.Format("（注：银子将充值至保险箱）");
        dc.SetTextColor(RGB(125, 125, 125));
        dc.SelectObject(m_Font2);
        dc.TextOut(nPosX+csExt3.cx+2+csExt7.cx, nPosY, strText);
    }
    else
    {

    }

    dc.SelectObject(pOldFont);
    dc.SetTextColor(OldColor);
    return;
}

void    CGameChargeDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    CWnd* pHwnd = GetDlgItem(IDC_BTN_CLOSECHARGE);
    if (pHwnd)
    {
        pHwnd->MoveWindow(364, 0, 34, 23);
    }

    pHwnd = GetDlgItem(IDC_BTN_QRCODE_CHARGE);
    if (pHwnd)
    {
        pHwnd->MoveWindow(14, 256, 121, 34);
    }
    
	pHwnd = GetDlgItem(IDC_BTN_QRCODE_CHARGE_WECHAT);
    if (pHwnd)
    {
        pHwnd->MoveWindow(140, 256, 121, 34);
    }

    pHwnd = GetDlgItem(IDC_BTN_WEB_CHARGE);
    if (pHwnd)
    {
        pHwnd->MoveWindow(266, 256, 121, 34);
    }
}

UINT    CGameChargeDlg::OnNcHitTest(CPoint point)
{
    CRect rc;
    GetClientRect(&rc);
    rc.bottom = 18;
    ClientToScreen(&rc);
    
    return rc.PtInRect(point) ? HTCAPTION:CDialog::OnNcHitTest(point);
}

BOOL    CGameChargeDlg::OnEraseBkgnd(CDC* pDC)
{
    return CDialog::OnEraseBkgnd(pDC);
}

void    CGameChargeDlg::ShowChargeDlg()
{
    ShowWindow(SW_SHOW);
}

void    CGameChargeDlg::HideChargeDlg()
{
    ShowWindow(SW_HIDE);
}

void    CGameChargeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    int nIndex = -1;
    for (int i = 0; i < TOTAL_DENOMINATION; i++)
    {
        RECT rc;
        GetMoneyRect(i, rc);
        if (rc.left <= point.x && rc.right >= point.x && rc.top <= point.y && rc.bottom >= point.y)
        {
            nIndex = i;
            break;
        }
    }

    if (-1 != nIndex)
    {
        for (i = 0; i < TOTAL_DENOMINATION; i++)
        {
            RECT rc;
            GetMoneyRect(i, rc);
            if (i == nIndex)
            {
                m_stMoneyInfo[i].enStatus = MONEY_STATUS_SEL;
            }
            else
            {
                m_stMoneyInfo[i].enStatus = MONEY_STATUS_NORMAL;
            }

            Invalidate();
        }
    }
}

void    CGameChargeDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    for (int i = 0; i < TOTAL_DENOMINATION; i++)
    {
        RECT rc;
        GetMoneyRect(i, rc);
        if (rc.left <= point.x && rc.right >= point.x && rc.top <= point.y && rc.bottom >= point.y)
        {
            if (MONEY_STATUS_NORMAL == m_stMoneyInfo[i].enStatus)
            {
                m_stMoneyInfo[i].enStatus = MONEY_STATUS_TOUCH;
                InvalidateRect(&rc);
            }
        }
        else
        {
            if (MONEY_STATUS_TOUCH == m_stMoneyInfo[i].enStatus)
            {
                m_stMoneyInfo[i].enStatus = MONEY_STATUS_NORMAL;
                InvalidateRect(&rc);
            }
        }
    }
}

HBRUSH  CGameChargeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor); 
    
    // TODO: 在此更改 DC 的任何属性
    // TODO: 如果默认的不是所需画笔，则返回另一个画笔 
    return hbr; 
}

BOOL    CGameChargeDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	
    return CDialog::OnSetCursor(pWnd, nHitTest, message);
}
