#include "stdafx.h"
#include "WelfareDialog.h"

CWelfareDialog::CWelfareDialog():CShinyNovaGui()
{
	m_plWelfareDlg = NULL;
	m_btnGetAward  = NULL;
	m_btnClose     = NULL;
	m_welfareState = -1;
	memset(&m_welfareCfg, 0, sizeof(m_welfareCfg));
}

CWelfareDialog::~CWelfareDialog()
{

}

void CWelfareDialog::OnIniData()
{

	CMyGame* pMainGame=(CMyGame*)GetObject(OBJECTNAME_MAIN_GAME);
	CString str;
	SNG_PIC pic;


	pic = Res_GetPICStruct("welfare_bk");

	if(m_plWelfareDlg == NULL)
	{
		m_plWelfareDlg = CreatePlane(pic.width, pic.height);
	}

	m_plWelfareDlg->Erasure();
	m_plWelfareDlg->TransGUI();
	m_plWelfareDlg->DrawPic(pic);
	m_plWelfareDlg->SetFloor(FLOOR_SYSTEMWARNING);
	m_plWelfareDlg->Updata();
	str.Format(_T("当您的总银子(包含保险箱)低于%d两时，可免费领取补充到%d银两。"), m_welfareCfg.nWelfareLine, m_welfareCfg.nSendLine);
	m_plWelfareDlg->SetBesideLine(30, 30);
	m_plWelfareDlg->SetTextSize(14, 500);
	m_plWelfareDlg->SetTextColor(RGB(0, 0, 0));
	m_plWelfareDlg->TextOutEx(230, 50, str);

	//pic = Res_GetPICStruct("btn_welfare_silver");
	//m_plWelfareDlg->DrawPic(pic, 65, 75, pic.width, pic.height, 0);
	str.Format(_T("还剩下%d次可领"), m_welfareCfg.nDailyLimitNum - m_welfareSurplusNum);
	m_plWelfareDlg->SetTextSize(12);
	m_plWelfareDlg->SetTextColor(RGB(0, 0, 0));
	m_plWelfareDlg->TextOutEx(155, 135, str);
	
	if(m_welfareState == 0)
	{
		str.Format(_T("%d两"), m_getMonney);
	}
	else if(m_welfareState == -1)
	{
		str.Format(_T("活动无效"));
	}
	else if(m_welfareState == -2)
	{
		str.Format(_T("未满足上述条件"));
	}
	else if(m_welfareState == -3)
	{
		str.Format(_T("游戏进行中,不能领取"));
	}
	else
	{
		str.Format(_T("已达每日领取上限"));
	}

	m_plWelfareDlg->SetTextSize(18);
	//m_plWelfareDlg->SetTextColor(RGB(253,216,3));
	if(m_welfareState == 0)
	{
		m_plWelfareDlg->SetTextColor(RGB(0x05, 0x79, 0x10));
	}
	else
	{
		m_plWelfareDlg->SetTextColor(RGB(0xb4, 0x34, 0x0b));
	}
	
	m_plWelfareDlg->TextOutEx(155, 102, str);
		
	if(m_btnClose == NULL)
	{
		PGL_MSG_CLASS(m_btnClose,CXyStandButton);
		m_btnClose->CreateGUI(Res_GetPICStruct("btn_welfare_close"), &(SNG_MSG)CreateMsg(pMainGame->GetHandle(), TCSG_CALLBACKMSG_BTN_CLOSE_WELFARE));
	}
	
	m_btnClose->MoveTo(m_plWelfareDlg->GetLeft()+291, m_plWelfareDlg->GetTop()+18);
	m_btnClose->SetFloor(m_plWelfareDlg->GetSurfaceZ());
	m_btnClose->SetButtonSound(BUTTON_STATUS_TOUCH, "SndTouchButton");
	m_btnClose->SetButtonSound(BUTTON_STATUS_LEFT_DOWN, "SndHitButton");
	m_plWelfareDlg->AddChild(m_btnClose, OBJECT_LOCK_ALL);
	
	if(m_btnGetAward == NULL)
	{
		PGL_MSG_CLASS(m_btnGetAward, CXyStandButton);
		m_btnGetAward->CreateGUI(Res_GetPICStruct("btn_welfare_getaward"), &(SNG_MSG)CreateMsg(pMainGame->GetHandle(), TCSG_CALLBACKMSG_BTN_GET_WElFAREAWARED));
	}
	
	m_btnGetAward->MoveTo(m_plWelfareDlg->GetX(), m_plWelfareDlg->GetTop()+176);
	m_btnGetAward->SetFloor(m_plWelfareDlg->GetSurfaceZ());
	m_btnGetAward->SetButtonSound(BUTTON_STATUS_TOUCH, "SndTouchButton");
	m_btnGetAward->SetButtonSound(BUTTON_STATUS_LEFT_DOWN, "SndHitButton");
	m_plWelfareDlg->AddChild(m_btnGetAward, OBJECT_LOCK_ALL);
}

void CWelfareDialog::OnInitSize()
{
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	CMyGame* pGame=(CMyGame*)(theApp.GetMainGame());

	RECT rect=pGame->GetGameRect();

	int x=rect.left;
	int y=rect.top;
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;

	if(m_plWelfareDlg != NULL)
	{
		m_plWelfareDlg->MoveTo(x+w-155,220);
		PGL_HIDE_OBJECT(m_plWelfareDlg);
		m_plWelfareDlg->Updata();
	}

	return;
}

void CWelfareDialog::OnGetWelfareConfig(int nActivityID)
{
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	if (pGameApp != NULL)
	{
		CString sRet;
		GET_WELFARECONFIG_OK getWfcOK={0};
		BOOL bRet = pGameApp->GetGameConnect()->GC_GetWelfareConfig(sRet, nActivityID, &getWfcOK);
				
		if(bRet == TRUE && getWfcOK.nActivityID > 0)
		{
			memcpy(&m_welfareCfg, &getWfcOK, sizeof(m_welfareCfg));
			CheckGetFareWare();			
		}
	}
}

void CWelfareDialog::CheckGetFareWare()
{
    //CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	int nChairNo = theApp.GetChairNO();

	if (m_welfareCfg.nStatus == 2) //活动关闭
	{
		m_realWelfStat = -1;
		m_welfareState = -1;
		return;
	}
	
	if (m_welfareCfg.nActivityID < 0) //无效活动
	{
		m_realWelfStat = -1;
		m_welfareState = -1;
		return;
	}
	
	CString sRet;
	SAFE_DEPOSIT_EX SafeDeposit;
	memset(&SafeDeposit, 0, sizeof(SAFE_DEPOSIT_EX));
	GAMEPLAYER player = theApp.m_GamePlayer[nChairNo];
	
	//身上携带的银子高于低保线
	if(player.nDeposit >= m_welfareCfg.nWelfareLine)
	{
		m_realWelfStat = -2;
		m_welfareState = -2;
		return;
	}
	
	theApp.GetGameConnect()->GC_LookSafeDeposit(sRet,&SafeDeposit);
	//保险箱+携带银两高于低保线
	if(SafeDeposit.nDeposit + player.nDeposit >= m_welfareCfg.nWelfareLine) 
	{
		m_realWelfStat = -2;
		m_welfareState = -2;
		return;
	}
	
	//符合领取低保条件
	m_getMonney = m_welfareCfg.nSendLine - (SafeDeposit.nDeposit + player.nDeposit);
	m_realWelfStat = 0;
	m_welfareState = 0;
    return;
}

void CWelfareDialog::CreateWelfDialog()
{
	OnIniData();
	OnInitSize();	
}

void CWelfareDialog::ShowWelfareDlg()
{
	PGL_SHOW_OBJECT(m_plWelfareDlg);
}

void CWelfareDialog::HideWelfareDlg()
{
	PGL_HIDE_OBJECT(m_plWelfareDlg);
}

void CWelfareDialog::OnApplyWelfare(int nSaveTo, int *pGetAward)
{
	if (theApp != NULL)
	{
		CString sRet;
		int nActivityID = m_welfareCfg.nActivityID;
		int nSendSilver = 0;
		BOOL bRet = theApp.GetGameConnect()->GC_ApplyWelfare(sRet, nActivityID, nSaveTo, nSendSilver);
		*pGetAward = nSendSilver;
	}	
	HideWelfareDlg();
	return;
}

int CWelfareDialog::GetWelfareLine()
{
	int welfLine = m_welfareCfg.nWelfareLine;
	return welfLine;
}

void CWelfareDialog::SetBtnEnable(bool bIsTrue)
{
	if(m_btnGetAward != NULL)
	{
		if(bIsTrue)
		{
			m_btnGetAward->Enable(true);
		}
		else
		{
			m_btnGetAward->Enable(false);
		}
	}
}

int CWelfareDialog::RecordOrReadWelfareNum(bool bIsWrite)
{
	char recordKey[32] = {0};
	
	int nChairNo = theApp.GetChairNO();
	GAMEPLAYER player = theApp.m_GamePlayer[nChairNo];
	
	struct tm *tm;  
	time_t timep;  
	time(&timep);  
	tm = localtime(&timep);
	
	sprintf(recordKey, "%d/%02d/%02d/%d", (int) tm->tm_year + 1900,
		(int) tm->tm_mon + 1, (int) tm->tm_mday, player.nUserID);
	
	TCHAR tcExePath[MAX_PATH] = {0};
	::GetModuleFileName(NULL, tcExePath, MAX_PATH);
	TCHAR *pFind = _tcsrchr(tcExePath, '\\'); 
	if(pFind == NULL)
		return 0;
	
	*pFind = '\0';
	CString szIniPath = tcExePath;
	szIniPath += "\\"; 
	szIniPath += WELFARE_TXT_NAME;
	if(bIsWrite)
	{
		m_welfareSurplusNum++;
		if(m_welfareSurplusNum >= m_welfareCfg.nDailyLimitNum)
		{
			m_welfareSurplusNum = m_welfareCfg.nDailyLimitNum;
		}
		
		TCHAR welfareSurplusNum[4]= {0};
		sprintf(welfareSurplusNum, "%d", m_welfareSurplusNum);
		
		::WritePrivateProfileString(TEXT("welfareAccout"), TEXT(recordKey), TEXT(welfareSurplusNum), szIniPath);
	}
	else
	{
		TCHAR szKeyValue[MAX_PATH] = {0}; 
		//::GetPrivateProfileString(TEXT("welfareAccout"), TEXT(recordKey), NULL, szKeyValue, MAX_PATH, szIniPath);
		//m_welfareSurplusNum = ::GetPrivateProfileInt(TEXT("welfareAccout"), TEXT(recordKey), m_welfareCfg.nDailyLimitNum, szIniPath);
		m_welfareSurplusNum = ::GetPrivateProfileInt(TEXT("welfareAccout"), TEXT(recordKey), 0, szIniPath);
	}

	return m_welfareSurplusNum;
}

