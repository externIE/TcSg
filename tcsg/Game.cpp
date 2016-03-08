// Game.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Game.h"

//#include "vld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////////

//二维码充值start
//小额房间
int g_nMoney_Small[TOTAL_DENOMINATION] = {1, 5, 10, 20, 50, 100, 200, 500};
int g_nSilver_Small[TOTAL_DENOMINATION] = {6000, 37500, 100000, 220000, 555000, 1150000, 2300000, 5800000};

//大额房间
int g_nMoney_Large[TOTAL_DENOMINATION] = {10, 20, 50, 100, 200, 300, 500, 1000};
int g_nSilver_Large[TOTAL_DENOMINATION] = {100000, 220000, 555000, 1150000, 2300000, 3450000, 5800000, 11700000};
//二维码充值end


/////////////////////////////////////////////////////////////////////////////
// CGameApp
using namespace ShinyNova;

BEGIN_MESSAGE_MAP(CGameApp, CSKGameWinApp)
	//{{AFX_MSG_MAP(CGameApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameApp construction

extern int   MsgBoxEx(HWND   hwnd,   TCHAR   *szText,   TCHAR   *szCaption,   UINT   uType)     ;

CGameApp::CGameApp():CSKGameWinApp(APP_NAME,GAME_PORT,VER_MAJOR,VER_MINOR)
{
   	memset(&m_GameSet,0,sizeof(m_GameSet));
	m_bGameAobrt = FALSE;
	m_bUseBigUI	= FALSE;

	//二维码充值start
    m_bLargeRoom = FALSE;
    m_pDlgQrcode = NULL;
	m_chargeStat = -1;
    //二维码充值end

	memset(&m_ChargeUrl, 0, sizeof(m_ChargeUrl));
}

CGameApp::~CGameApp()
{

}
/////////////////////////////////////////////////////////////////////////////
// The one and only CGameApp object
CGameTable m_MyGameTable;
CGameApp theApp;
HANDLE g_hMutex = NULL; 
HHOOK   hMsgBoxHook = NULL;   
BOOL bOpen= FALSE;
/////////////////////////////////////////////////////////////////////////////

int  CGameApp::GetTableChairCount()
{
	return TOTAL_CHAIRS;
}

CXyGameFrame* CGameApp::CreateLogicFrame()
{
	return new CMyGameFrame;
}

BOOL CGameApp::InitInstance()
{
	InitAppSet();                   //初始化程序设置
    ReadGameSet();					//获取游戏初始化设置
	CheckReplayMode();
#ifdef _OFFLINE_
	SetOfflineFlag(OFFLINEFLAG_OFFLINE);
#endif
	if (GetOfflineFlag()==OFFLINEFLAG_OFFLINE)
	{
		m_GameInit.nUserID=0;
		m_GameInit.nAreaID=1;
		m_GameInit.nGameID=53;
		m_GameInit.nRoomID=0;
		m_GameInit.nTableNO=0;
		m_GameInit.nChairNO=0;
		m_GameInit.hRoomWnd=0;
		m_GameInit.bLookOn =FALSE;
		m_GameInit.bReEnter=FALSE;
		lstrcpy(m_GameInit.szGameIP,_T("127.0.0.1"));
		lstrcpy(m_GameInit.szGameName,_T("斗牛"));
	}

	if(!CGameWinApp::InitInstance())//进行SDI初始化，创建通讯线程，连接游戏服务器
		return FALSE;    

     if(!CreateGameFrame())          //创建游戏界面框架
		return FALSE;  

	if (GetOfflineFlag()==OFFLINEFLAG_OFFLINE)
	{
		m_MatchTable.nChairCount=TOTAL_CHAIRS;
 		m_GameTable.nPlayerCount=TOTAL_CHAIRS;
		lstrcpy(m_GameRoom.szWWW,_T("www.uc108.com"));
 		lstrcpy(m_GameRoom.szWWW2,_T("www2.youxi518.com"));
		lstrcpy(m_GameRoom.szRoomName,_T("古越斋"));
		lstrcpy(m_GameRoom.szAreaName,_T("绍兴电信1站"));
		for(int i=0;i<TOTAL_CHAIRS;i++)
		{
			sprintf(m_GamePlayer[i].szUsername,_T("长崎雨_%d今夜"),i);
			sprintf(m_GamePlayer[i].szNickName,_T("wsk_%d"),i);
			m_GamePlayer[i].nChairNO=i;
			m_GamePlayer[i].nTableNO=0;
			m_GamePlayer[i].nUserID =10+i;
			m_GamePlayer[i].nScore=rand()%999999;

			m_GamePlayer[i].nWin=rand()%1000;
			m_GamePlayer[i].nLoss=rand()%1000;
			m_GamePlayer[i].nStandOff=rand()%1000;
			m_GamePlayer[i].nBout=m_GamePlayer[i].nWin+m_GamePlayer[i].nLoss+m_GamePlayer[i].nStandOff;
			m_GamePlayer[i].nBreakOff=rand()%1000;
			m_GamePlayer[i].nNetSpeed=100+rand()%1000;
			m_GamePlayer[i].nExperience=rand()%1000000;
			
			//////////////////////
			sprintf(m_Visitor[i].szUsername ,_T("超级注射器%i"),i);
			sprintf(m_Visitor[i].szNickName,_T("美工"));
			m_Visitor[i].nChairNO=i;
			m_Visitor[i].nUserID =20+i;
			m_Visitor[i].bLookOn =1;
		}
		
		sprintf(m_GamePlayer[0].szNickName,_T("玩家"));
		sprintf(m_GamePlayer[1].szNickName,_T("策划"));
		sprintf(m_GamePlayer[2].szNickName,_T("策划"));
		sprintf(m_GamePlayer[3].szNickName,_T("程序"));
		
		sprintf(m_GamePlayer[0].szUsername,_T("玩家大人需要八字"));
		sprintf(m_GamePlayer[1].szUsername,_T("MMMMMMMMMMMMMMMM"));
		sprintf(m_GamePlayer[2].szUsername,_T("黑木大人是猪mmmm"));
		sprintf(m_GamePlayer[3].szUsername,_T("黑木大人是猪mmmm"));

		sprintf(m_GamePlayer[0].szLevelName,"一段");
		sprintf(m_GamePlayer[1].szLevelName,"判官");
		sprintf(m_GamePlayer[2].szLevelName,"美国总统大人");
		sprintf(m_GamePlayer[3].szLevelName,"超级大富翁");
		
		m_GamePlayer[0].nPortrait=0;
		m_GamePlayer[1].nPortrait =10;
		m_GamePlayer[2].nPortrait =9;
		m_GamePlayer[3].nPortrait =10;

		srand(GetTickCount());
		m_GamePlayer[0].nClothingID =rand()%116;
		m_GamePlayer[1].nClothingID =rand()%116;
		m_GamePlayer[2].nClothingID =70;
		m_GamePlayer[3].nClothingID =rand()%116;

		
		m_GamePlayer[0].nDeposit=99999999;
		m_Visitor[0].nPortrait =10;
		m_GamePlayer[0].nPortrait=10;
		m_GamePlayer[1].nPortrait =9;

	
		int result[MAX_CHAIR_COUNT];
		memset(result,0,sizeof(result));
		for(int k=0;k<100;k++)
		{
			for(int i=0;i<TOTAL_CHAIRS;i++)
			{
				result[i]=1000000+(k+1)*100+i;
			}
			SaveResultDiff(result);
		}

		UpdataResultList();

		AfxGetMainWnd()->PostMessage(WM_INIT_DATA,0,0);
	}
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CGameApp message handlers
DWORD CGameApp::GetUserConfig()
{
	return 0;
}

CGameInfo* CGameApp::GetGameInfo()
{
  return &m_GameInfoEx;
}

VOID*  CGameApp::GetTableInfo()
{
 	return (VOID*)&m_GameInfoEx.m_GameTableInfo;
}

int CGameApp::GetTableInfoSize()
{
	return sizeof(m_GameInfoEx.m_GameTableInfo);	
}

VOID*  CGameApp::GetEnterInfo()
{
	return (VOID*)&m_GameInfoEx.m_GameEnterInfo;
}

int CGameApp::GetEnterInfoSize()
{
	return sizeof(m_GameInfoEx.m_GameEnterInfo);
}

void CGameApp::ReadGameSet()
{
	CString sConfigFile=GetGameConfigFile();
	m_GameSet.nSoundEffect =GetPrivateProfileInt ( _T("GAMESET"), _T("SoundEffect"), 0, sConfigFile) ; 
	m_GameSet.nMusic =GetPrivateProfileInt ( _T("GAMESET"), _T("Music"), 0, sConfigFile) ; 
	m_GameSet.nRejectKanPaiAsk=GetPrivateProfileInt ( _T("GAMESET"), _T("RejectKanPaiAsk"), 0, sConfigFile) ;
	m_GameSet.nRejectPassSound=GetPrivateProfileInt ( _T("GAMESET"), _T("RejectPassSound"), 0, sConfigFile) ;
	m_GameSet.nLikeReMaind =GetPrivateProfileInt ( _T("GAMESET"), _T("LikeReMaind"), 0, sConfigFile) ; 
	m_GameSet.nUnLikeTip =GetPrivateProfileInt ( _T("GAMESET"), _T("UnLikeTip"), 0, sConfigFile) ; 
}

void CGameApp::FreshGameSet()
{

}

void CGameApp::SaveGameSet()
{	
	CString sConfigFile=GetGameConfigFile();
	
	TCHAR szText[256];
	memset(szText,0,sizeof(szText));
	
	sprintf(szText,_T("%d"),m_GameSet.nSoundEffect);
	WritePrivateProfileString ( _T("GAMESET"),_T("SoundEffect"),szText,  sConfigFile) ; 
	
	sprintf(szText,_T("%d"),m_GameSet.nMusic);
	WritePrivateProfileString ( _T("GAMESET"),_T("Music"),szText,  sConfigFile); 

	sprintf(szText,_T("%d"),m_GameSet.nRejectKanPaiAsk) ;
	WritePrivateProfileString ( _T("GAMESET"),_T("RejectKanPaiAsk"),szText,sConfigFile) ; 

	sprintf(szText,_T("%d"),m_GameSet.nRejectPassSound) ;
	WritePrivateProfileString ( _T("GAMESET"),_T("RejectPassSound"),szText,sConfigFile) ; 

	sprintf(szText,_T("%d"),m_GameSet.nLikeReMaind);
	WritePrivateProfileString ( _T("GAMESET"),_T("LikeReMaind"),szText,  sConfigFile) ; 

	sprintf(szText,_T("%d"),m_GameSet.nUnLikeTip);
	WritePrivateProfileString ( _T("GAMESET"),_T("UnLikeTip"),szText,  sConfigFile) ; 
}

void CGameApp::FreshScores()
{

}

CGameConnect* CGameApp::GetGameConnect()
{
	return &m_GameRequest;
}

// CGameSkin* CGameApp::GetGameSkin()
// {
// 	return &m_GameSkinEx;
// }

CGameNotify* CGameApp::GetGameNotify()
{
	return &m_GameNotifyEx;
}

	
int CGameApp::ExitInstance() 
{
// 	CString fontpath;
// 	fontpath.Format("skins\\%s.tff",GetArtFontType());
// 	CString str=GetGamePath(fontpath);
// 	::RemoveFontResource(str);
//	SendMessage(HWND_BROADCAST,WM_FONTCHANGE,0,0);
  	SaveGameSet();
  	return CGameWinApp::ExitInstance();
}
 

LRESULT   CALLBACK   CBTProc(int   nCode,   
							 WPARAM   wParam,   
							 LPARAM   lParam   
							 )   
{   
	if(nCode   ==   HCBT_ACTIVATE)   
	{   
		if (bOpen==FALSE)
		{
			HWND   hWnd   =   (HWND)wParam;   
			CRect   rect;
			GetWindowRect(hWnd,   &rect);
			CGameFrameWnd* pFrame=(CGameFrameWnd*)AfxGetMainWnd();
			CRect rcTable;
			pFrame->GetWindowRect(&rcTable);
			MoveWindow(hWnd,  rcTable.Width()-271,   36,   rect.Width(),   rect.Height(),   FALSE); 
			bOpen = TRUE;
		}		
	}   
	return   CallNextHookEx(hMsgBoxHook,   nCode,   wParam,   lParam);
}   


int   MsgBoxEx(HWND   hwnd,   TCHAR   *szText,   TCHAR   *szCaption,   UINT   uType)     
{     
	int   retval;     
	bOpen = FALSE;
	//   Install   a   window   hook,   so   we   can   intercept   the   message-box     
	//   creation,   and   customize   it     
	hMsgBoxHook   =   SetWindowsHookEx(     
		WH_CBT,     
		CBTProc,     
		NULL,     
		GetCurrentThreadId()       //   Only   install   for   THIS   thread!!!     
		);     
	//   Display   a   standard   message   box     
	retval   =   MessageBox(hwnd,   szText,   szCaption,   uType);     
	//   remove   the   window   hook     
	UnhookWindowsHookEx(hMsgBoxHook);     
	return   retval;     
}     

void CGameApp::EraseBkgnd()
{
 //  	CMainFrame* fram=(CMainFrame*)AfxGetMainWnd();
//	fram->m_pChatView->OnErase();
}


BOOL CGameApp::IsRejectKanPaiAsk()
{
	return m_GameSet.nRejectKanPaiAsk;
}

BOOL CGameApp::IsRejectPassSound()
{
	return m_GameSet.nRejectPassSound;
}

BOOL	CGameApp::ParseCommond(LPCTSTR str)
{
	//调试用，外部版不做处理
	if(GetOfflineFlag() != OFFLINEFLAG_OFFLINE)
		return FALSE;


	if (!strcmp(str,"/QuitGame"))
	{
		::PostMessage(AfxGetMainWnd()->m_hWnd,WM_QUIT,0,0);
		return TRUE;
	}
	else if (!strcmp(str,"/cardmaster"))
	{
		m_GameRoom.dwGameOptions|=GO_USE_CARDMASTER;
		CMyGame* pGame=(CMyGame*)GetMainGame();
		pGame->OPE_ShowAssistantButton(TRUE);
	}
	else if (!strcmp(str,"/chat"))
	{
		CString str;
		str.Format("<c=%d><size=16>===[本局结束]===\r\n",RGB(171,23,23));
		ShowMsgInChatView(str);

		str.Format("\r\n<c=%d><size=16>淘汰赛第%s轮\r\n",RGB_RED,"三");
		ShowMsgInChatView(str);

		str.Format("\r\n<c=%d><size=16><第%d桌,第%d局>\r\n",RGB_BLUE,2,3);
	    ShowMsgInChatView(str);


		str.Format("<c=%d><size=16>===[本局结束]===\r\n",RGB(171,23,23));
		ShowMsgInChatView(str);
		for(int i=0;i<4;i++)
		{

			CString str;
			
			int nScore=10000-rand()%20000;
			if (i==GetChairNO())
			{
				str.Format("<c=%d>[%s]",RGB(171,23,23),GetUserNameByChairNO(i));
				ShowMsgInChatView(str);
				
				for(int s=0;s<=35-str.GetLength();s++)
					ShowMsgInChatView(" ");

				if (nScore>=0)
					str.Format("<c=%d>+%d分\r\n",RGB(171,23,23),nScore);
				else
					str.Format("<c=%d>%d分\r\n",RGB(171,23,23),nScore);

				ShowMsgInChatView(str);
			}
			else
			{
				str.Format("<c=%d>[%s]",RGB(64,64,64),GetUserNameByChairNO(i));
				ShowMsgInChatView(str);
				
				for(int s=0;s<=35-str.GetLength();s++)
					ShowMsgInChatView(" ");

			
				if (nScore>=0)
					str.Format("<c=%d> %d分\r\n",RGB(64,64,64),nScore);
				else
					str.Format("<c=%d>%d分\r\n",RGB(64,64,64),nScore);

				ShowMsgInChatView(str);
			}
		}
		ShowMsgInChatView("\r\n");
		
		CHAT_TO_TABLE ctt;
		memset(&ctt,0,sizeof(CHAT_TO_TABLE));
		int chairno=rand()%3+1;
		ctt.nUserID=GetPlayerIDByChairNO(chairno);

		strcpy(ctt.szChatMsg,"打好牌才是关键，什么都是浮云");

		GetSubSystem()->OnMsg(CreateMsg(0,GAME_MSG_NOTIFY_TABLECHAT,(UINT)&ctt,0));

		return TRUE;
	}
	else if (!strcmp(str,"/pgl"))
	{
		CMyGame* pGame=(CMyGame*)GetMainGame();
		
		CARDS_INFO ci;
		memset(&ci,0,sizeof(CARDS_INFO));
		XygInitChairCards(ci.nCardIDs,MAX_CARDS_PER_CHAIR);
		ci.nChairNO=0;
		ci.nCardsCount=27;
		for(int i=0;i<ci.nCardsCount;i++)
			ci.nCardIDs[i]=45;
		
		pGame->NTF_ChairCard(&ci);
		return TRUE;
	}
	else if (!strcmp(str,"/showgamewin"))
	{
		CMyGame* pGame=(CMyGame*)GetMainGame();
		GAME_WIN_RESULT* pGameWin=((CGameInfoEx*)pGame->GetGameInfo())->GetGameWin();
		for(int i=0;i<TOTAL_CHAIRS;i++)
		{
			pGameWin->gamewin.nScoreDiffs[i]=rand()%1000-500;
			pGameWin->gamewin.nDepositDiffs[i]=rand()%1000-500;
		}
		
		pGameWin->gamewin.dwWinFlags=GW_NORMAL;
		pGame->NTF_GameWin();
		return TRUE;
	}
	else if (!strcmp(str,"/test"))
	{
		CMyGame* pGame=(CMyGame*)GetMainGame();
		
		ASK_CARD_VIEW acv;
		memset(&acv,0,sizeof(acv));
		acv.nChairNO=3;
		acv.nUserID=13;
		pGame->NTF_AskCardView(&acv);
		return TRUE;
	}
	else if (!strcmp(str,"/DealCard"))
	{
		CMyGame* pGame=(CMyGame*)GetMainGame();
		pGame->OPE_InitData();
		pGame->OPE_DealCard();
		return TRUE;
	}
	else if (!strcmp(str,"/MoveClock"))
	{
		CMyGame* pGame=(CMyGame*)GetMainGame();
		pGame->GUI_MoveClock(rand()%4,30);
		return TRUE;
	}
	else if (!strcmp(str,"/ShowSystem"))
	{CMyGame* pGame=(CMyGame*)GetMainGame();
	pGame->ShowSystemNote(TRUE);
	return TRUE;
	}
	else if (!strcmp(str,"/ReMindPass"))
	{CMyGame* pGame=(CMyGame*)GetMainGame();
	pGame->MOVIE_CanNotThrow();
	return TRUE;
	}
	else if (!strcmp(str,"/AutoPlay"))
	{
		CMyGame* pGame=(CMyGame*)GetMainGame();
		pGame->m_bnAutoPlay=!pGame->m_bnAutoPlay;
		return TRUE;
	}
	else if (!strcmp(str,"/FreshCard"))
	{
		CMyGame* pGame=(CMyGame*)GetMainGame();
		for(int i=0;i<TOTAL_CARDS;i++)
			pGame->m_Card[i]->Fresh();
		return TRUE;
	}
	else if (!strcmp(str,"/GameWin"))
	{
        CMyGame* pGame=(CMyGame*)GetMainGame();
		pGame->REQ_SendMsgToServer(SYSMSG_GAME_WIN,0,NULL);
		return TRUE;
	}
	else if (!strcmp(str,"/FreshRankList"))
	{
        CMyGame* pGame=(CMyGame*)GetMainGame();
		FreshRankList();
		return TRUE;
	}


	return FALSE;
}

void CGameApp::CreateArrageTable()//组建新的桌子
{
	CMyGame* pGame=(CMyGame*)GetMainGame();
	pGame->CreateMatchTable();
}

void CGameApp::DisBandTable()
{
	CMyGame* pGame=(CMyGame*)GetMainGame();

}
int	CGameApp::GetThrowWait()
{
	return ((CGameInfoEx*)GetGameInfo())->GetThrowWait();
}

int	CGameApp::GetCurrentRank()
{
	return ((CGameInfoEx*)GetGameInfo())->GetCurrentRank();
}

int	CGameApp::GetChairCardsCount()
{
	return CHAIR_CARDS;
}

BOOL CGameApp::IsSilence()
{
	return theApp.m_GameSet.nSoundEffect;
}

BOOL CGameApp::IsLikeRemaind()
{
	return theApp.m_GameSet.nLikeReMaind;
}

BOOL CGameApp::IsUnlikeTip()
{
	return theApp.m_GameSet.nUnLikeTip;
}

BOOL    CGameApp::IsAllowCardMaster()
{
	return IS_BIT_SET(GetGameOptions(),GO_USE_CARDMASTER);
//	return TRUE;
}

BOOL CGameApp::IsSaveReplay()
{
//	return TRUE;
	return FALSE;
}

BOOL CGameApp::CreateGameFrame()
{
	SetRegistryKey(_T(GetGameName()));
	
	int nWindowsWidth=GetSystemMetrics(SM_CXSCREEN);
	int nWindowsHeight=GetSystemMetrics(SM_CYSCREEN);//;
	int nTitleHeight=GetSystemMetrics(SM_CYCAPTION);
	nWindowsHeight-=nTitleHeight;
	
	RECT rc={0,0,nWindowsWidth,nWindowsHeight-nTitleHeight};
	
	m_pGameFrameWnd=(CMainFrame*)(RUNTIME_CLASS(CMainFrame))->CreateObject();
	m_pMainWnd=m_pGameFrameWnd;
	
	m_pGameFrameWnd->CreateWnd(GetGameName(),WS_MINIMIZE|WS_MAXIMIZE|WS_SYSMENU|WS_CLIPCHILDREN,0,rc,NULL);
	m_pGameFrameWnd->CreateCore(CreateLogicFrame());
	m_pGameFrameWnd->ShowWindow(SW_SHOW);
	m_pGameFrameWnd->SetForegroundWindow();
	m_pGameFrameWnd->UpdateWindow();
	
	////设置窗口透明//////////////////////////////////////////////////
	/*
	SetWindowLong(m_pGameFrameWnd->GetSafeHwnd(),GWL_EXSTYLE, 
	GetWindowLong(m_pGameFrameWnd->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);

	HINSTANCE   hInst   =   LoadLibrary( "User32.DLL");   
	if(hInst)   
	{                         
		typedef   BOOL   (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD);                     
		MYFUNC   fun   =   NULL; 
		//取得SetLayeredWindowAttributes函数指针           
		fun=(MYFUNC)GetProcAddress(hInst,"SetLayeredWindowAttributes"); 
		if(fun)fun(m_pGameFrameWnd->GetSafeHwnd(),0,0,1);   
        
		FreeLibrary(hInst);   
	}*/
	///////////////////////////////////////////////////////////////////

	/////设置左上角图标///////////////////////////////////////////
	HICON   hicon = LoadIcon(IDR_MAINFRAME);
	AfxGetMainWnd()   -> SendMessage(WM_SETICON,TRUE,(LPARAM)hicon);
	/////////////////////////////////////////////////////////////

	//解决客户端在windows7下无法前置窗口的问题。
	///////////////////////////////////////////////////////////////////////////////
	::SetWindowPos(m_pGameFrameWnd->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	::SetWindowPos(m_pGameFrameWnd->m_hWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	///////////////////////////////////////////////////////////////////////////////

	NotifyRoomWhenStart(m_pGameFrameWnd->m_hWnd);
	return TRUE;
}

void CGameApp::SetUseBigUI(BOOL bUse)
{
	m_bUseBigUI = bUse;
}

BOOL CGameApp::IsUseBigUI()
{
	return m_bUseBigUI;
}

BOOL CGameApp::IsTakeDepositInGame()
{
//	if (!IsLookOn()&& IsPlayDeposit()&& IS_BIT_SET(GetRoomConfigs() ,RC_TAKEDEPOSITINGAME))
//		return TRUE;
//	else
//		return FALSE;
	if (!IsLookOn())
	{
		return TRUE;
	}
	return FALSE;
}
	
BOOL CGameApp::IsAllowAskExit()
{
	return FALSE;
}

void CGameApp::AchToUI()
{
	CMyGame* pGame=(CMyGame*)GetMainGame();
	if (pGame) 
		pGame->OnMsg(CreateMsg(0,TCSG_GAMEMSG_ACHDATATOUI));
}
 
void CGameApp::ClearChairStatus()
{
	m_GameInfoEx.ResetChairReady();
}

void CGameApp::SetUserStatus(DWORD dwUserStatus[], int nLen)
{
	m_GameInfoEx.SetUserStatus(dwUserStatus,nLen);
}

void   CGameApp::ResetDataByNewPos(USER_POSITION* pUserPos)
{
	InitialGameTableInfo(&((CGameInfoEx*)GetGameInfo())->m_GameTableInfo);
	int nCount=pUserPos->nPlayerCount;
	
	m_GameInit.nChairNO = pUserPos->nChairNO;
	m_GameInit.nTableNO=pUserPos->nTableNO;
	m_GameTable.nTableNO=pUserPos->nTableNO;
	m_GameTable.nPlayerCount=nCount;
	
	memset(&m_GamePlayer,0,sizeof(m_GamePlayer));
	
	SOLO_PLAYER * psp=(SOLO_PLAYER*) ((PBYTE)pUserPos +sizeof(USER_POSITION));
	
	for(int i=0;i<nCount;i++)
	{
		if (psp)
		{
			SetGamePlayerBySoloPlayer(&m_GamePlayer[psp->nChairNO],psp);
		}
		
		psp++;
	}
	
	//最后是gametableinfo
	memcpy(&((CGameInfoEx*)GetGameInfo())->m_GameEnterInfo.ei.dwUserStatus,pUserPos->dwUserStatus,sizeof(DWORD)*TOTAL_CHAIRS);
	if(IS_BIT_SET(pUserPos->dwTableStatus,TS_PLAYING_GAME))
	{
		memcpy(&((CGameInfoEx*)GetGameInfo())->m_GameTableInfo,psp,sizeof(GAME_TABLE_INFO));
	}
}

//二维码充值start
void    CGameApp::ShowGameCharge()
{
    MONEY_INFO stMoneyInfo[TOTAL_DENOMINATION];
    ZeroMemory(stMoneyInfo, sizeof(stMoneyInfo));
    
    if (!m_bLargeRoom)
    {//如果是小额房间
        for (int i = 0; i < TOTAL_DENOMINATION; i++)
        {
            if (2 == i)
            {
                stMoneyInfo[i].enStatus = MONEY_STATUS_SEL;
            }
            else
            {
                stMoneyInfo[i].enStatus = MONEY_STATUS_NORMAL;
            }
            
            stMoneyInfo[i].nMoney = g_nMoney_Small[i];
            stMoneyInfo[i].nExChangeTongbao = 0;
            stMoneyInfo[i].nExChangeSilver = g_nSilver_Small[i];
            stMoneyInfo[i].nGivePercent = 0;
        }
    }
    else
    {
        for (int i = 0; i < TOTAL_DENOMINATION; i++)
        {
            if (2 == i)
            {
                stMoneyInfo[i].enStatus = MONEY_STATUS_SEL;
            }
            else
            {
                stMoneyInfo[i].enStatus = MONEY_STATUS_NORMAL;
            }
            stMoneyInfo[i].nMoney = g_nMoney_Large[i];
            stMoneyInfo[i].nExChangeTongbao = 0;
            stMoneyInfo[i].nExChangeSilver = g_nSilver_Large[i];
            stMoneyInfo[i].nGivePercent = 0;
        }
    }
    
    
    m_GameCharge.SetMoneyInfo(stMoneyInfo, &m_ChargeUrl);
    
    if (m_GameCharge.GetSafeHwnd() == NULL)
    {
        m_GameCharge.Create(CGameChargeDlg::IDD, AfxGetMainWnd());
    }
    
    m_GameCharge.ShowChargeDlg();
	if(m_chargeStat == -1)
	{
		m_GameCharge.SetChargeStat(0);
	}
	
}

void    CGameApp::ShowQrChargeDlg(CString& strDstUrl)
{
    if (m_pDlgQrcode)
    {
        if (NULL != m_pDlgQrcode->GetSafeHwnd())
        {
            m_pDlgQrcode->ShowWindow(SW_HIDE);
            m_pDlgQrcode->DestroyWindow();
        }
        delete m_pDlgQrcode;
        m_pDlgQrcode = NULL;
    }
    
    m_pDlgQrcode = new CQrcodeCharge;
    m_pDlgQrcode->Create(CQrcodeCharge::IDD,NULL);
    m_pDlgQrcode->ShowDialog();
    
    TCHAR *  pszFile=new TCHAR[strDstUrl.GetLength()+2];
    lstrcpy(pszFile,strDstUrl);
    m_pDlgQrcode->PostMessage(WM_INIT_DATA,(UINT)pszFile,0);
    return;
}
//二维码充值end

//获取充值界面的URL
void    CGameApp::GetChargeUrl(LPCHARGEURL pChargeUrl)
{
     if(pChargeUrl->WebUrl != NULL)
	 {
		 lstrcpy(m_ChargeUrl.WebUrl, pChargeUrl->WebUrl);
	 }
	 if(pChargeUrl->OrCodeUrl != NULL)
	 {
		 lstrcpy(m_ChargeUrl.OrCodeUrl, pChargeUrl->OrCodeUrl);
	 }
	 if(pChargeUrl->OrCodeUrl_WeChat != NULL)
	 {
		 lstrcpy(m_ChargeUrl.OrCodeUrl_WeChat, pChargeUrl->OrCodeUrl_WeChat);
	 }
	 if(pChargeUrl->TongBaoUrl != NULL)
	 {
		 lstrcpy(m_ChargeUrl.TongBaoUrl, pChargeUrl->TongBaoUrl);
	 }
	 if(pChargeUrl->LearnMoreUrl != NULL)
	 {
		 lstrcpy(m_ChargeUrl.LearnMoreUrl, pChargeUrl->LearnMoreUrl);
	 }
}

