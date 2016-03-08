// GameNotify.cpp: implementation of the CGameNotify class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game.h"
#include "GameNotifyEx.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGameNotifyEx::CGameNotifyEx()
{
 
}

CGameNotifyEx::~CGameNotifyEx()
{

}

void CGameNotifyEx::OnNotifyReceived( int nRequestID, VOID* pDataPtr,int nSize)
{
	switch(nRequestID)
	{
	case GR_START_GAME:
		NTF_StartGame((START_GAME*)pDataPtr);
		break;
	case GR_GAME_START:
		NTF_GameStart((GAME_START_INFO*)pDataPtr);
		break;
	case GR_SENDMSG_TO_PLAYER:
 		NTF_GameMsg((GAME_MSG*)pDataPtr);
 		break;
	case GR_GAME_WIN: 
		NTF_GameWin((GAME_WIN_RESULT*)pDataPtr);
		break;
	case GR_TOCLIENT_TABLEINFO:
		NTF_GameTableInfo((GAME_TABLE_INFO*)pDataPtr);
		break;
	case GR_TOCLIENT_OFFLINE:
		NTF_PlayerOffline((LPPLAYER_OFFLINE)pDataPtr);
		break;
	case GR_CARDS_PASS:
		NTF_CardPass((CARDS_PASS*)pDataPtr);
		break;
	case GR_ZU_PAI:
		NTF_ZuPai((ZUPAI*)pDataPtr);
		break;
	case GR_CARDS_THROW:
		{
			CARDS_THROW cardthrow;
			memset(&cardthrow,0,sizeof(CARDS_THROW));
			XygInitChairCards(cardthrow.unite.nCardIDs,MAX_CARDS_PER_CHAIR);
			memcpy(&cardthrow,pDataPtr,nSize);
			NTF_CardThrow(&cardthrow);
		}
		break;
	case GR_START_COUNTDOWN:
		NTF_StartCountdown((START_COUNTDOWN*)pDataPtr);
		break;
	case GR_SYNC_START_COUNTDOWN:
		NTF_SyncStartCountdown((SYNC_COUNTDOWN*)pDataPtr);
		break;
	case GR_QZ:
		NTF_QZOK((QZ*)pDataPtr);
		break;
	case GR_YAZHU:
		NTF_YaZhuOK((YAZHU*)pDataPtr);
		break;
	case GR_ROOMLIMIT:
        NTF_RoomLimit((LPROOMLIMIT)pDataPtr);
        break;
	case TCSG_WELFAREACTIVITYID:
		NTF_WelfareActivity(*(int*)pDataPtr);
		break;
	case TCSG_ISTONGBIROOM:
		NTF_IsTongBiRoom(*(int*)pDataPtr);
		break;
	case TCSG_CHARGE_ALLURL:
		NTF_GetChargeUrl((LPCHARGEURL)pDataPtr);
		break;
	default:
		CSKGameNotify::OnNotifyReceived(nRequestID,pDataPtr,nSize);	//这里需要return，防止重复存消息
		return;
	}
}


void CGameNotifyEx::NTF_GameTableInfo(GAME_TABLE_INFO* pGameTableInfo)
{
	CGameApp* pApp=(CGameApp*)AfxGetApp();
	((CMyGame*)pApp->GetMainGame())->OPE_InitData();
	
	memcpy(pApp->GetTableInfo(),pGameTableInfo,sizeof(GAME_TABLE_INFO));

	if (pApp->IsMatch())
	{
		((CSubSystem*)pApp->GetSubSystem())->OnEnterMatchDXXW();
		((CMyGame*)pApp->GetMainGame())->OnEnterDXXW();
	}
	else
	{
		((CSubSystem*)pApp->GetSubSystem())->OnEnterGameDXXW();
		((CMyGame*)pApp->GetMainGame())->OnEnterDXXW();
	}

}

void CGameNotifyEx::NTF_GameAbort(GAME_ABORT* pGameAbort)
{	
	CGameApp* pApp=(CGameApp*)AfxGetApp();
	CGameFrameWnd * pFrame=(CGameFrameWnd*)AfxGetMainWnd();

	CGameNotify::NTF_GameAbort(pGameAbort);//结束游戏并且断开与服务器连接

	pApp->m_bGameAobrt = TRUE;

	pFrame->SetForegroundWindow();
	
	CString sMsg;
	if(pGameAbort->bForce )
	{
		sMsg.Format(_T("%s 已经强行退出，游戏结束。(扣除%d两,%d分)"),
			pApp->GetPlayerNameByChairNO(pGameAbort->nChairNO),
			-pGameAbort->nDepositDfif,
			-pGameAbort->nScoreDiff);
	}
	else
	{
		sMsg.Format(_T("%s 已经离开，游戏结束。"),
			pApp->GetPlayerNameByChairNO(pGameAbort->nChairNO),pGameAbort->nChairNO);
	}	 
	((CMyGame*)theApp.GetMainGame())->GetGameInfoEx()->GetGameStartInfo()->dwStatus=0;

	pApp->ShowQuitTimerBox(sMsg,10,TIMERDLG_FLAGS_ICON|TIMERDLG_FLAGS_DANYMIC,"xyf抱歉");
}


void CGameNotifyEx::NTF_StartGame(START_GAME* pStartGame)
{ 
	CGameApp* pApp=(CGameApp*)AfxGetApp();
	CGameFrameWnd* pFrame=(CGameFrameWnd*)AfxGetMainWnd();

	((CGameInfoEx*)pApp->GetGameInfo())->NTF_StartGame(pStartGame);
	((CMyGame*)pApp->GetMainGame())->NTF_StartGame(pStartGame->nChairNO);
	if(pStartGame->nReserved[0]==1)
	{
		((CMyGame*)pApp->GetMainGame())->OPE_ShowBankerFlag(FALSE);
	}
}

// 
void CGameNotifyEx::NTF_GameStart(GAME_START_INFO * pStartData)
{
	CGameApp* pApp=(CGameApp*)AfxGetApp();
	CGameFrameWnd* pFrame=(CGameFrameWnd*)AfxGetMainWnd();
	
	//游戏开始，必须执行此函数，否则会有严重错误
	////////////////////////////////////////////
	pApp->GameRun();
	///////////////////////////////////////////

	((CGameInfoEx*)pApp->GetGameInfo())->NTF_GameStart(pStartData);
	((CMyGame*)pApp->GetMainGame())->NTF_GameStart();
}


void CGameNotifyEx::NTF_GameMsg(GAME_MSG* pGameMsg)
{
	CGameApp* pApp=(CGameApp*)AfxGetApp();
	((CMyGame*)pApp->GetMainGame())->NTF_GameMsg(pGameMsg);
}

void CGameNotifyEx::NTF_GameWin(GAME_WIN_RESULT* pGameWin)
{
	CGameApp* pApp=(CGameApp*)AfxGetApp();

	//游戏结束，必须执行此函数，否则会有严重错误
	/////////////////////////////////////////////
	pApp->GameStop();
	/////////////////////////////////////////////

	((CGameInfoEx*)pApp->GetGameInfo())->NTF_GameWin(pGameWin);
	CMyGame* pMyGame = (CMyGame*)pApp->GetMainGame();
	if (pGameWin->nReserved[0]!=0)
	{
		pMyGame->m_CEventPipe->PushEvent(pMyGame->GetHandle(),TCSG_CALLBACKMSG_GAMESTANDOFF,pGameWin,sizeof(GAME_WIN_RESULT));
	}
	else
	{
		pMyGame->m_CEventPipe->PushEvent(pMyGame->GetHandle(),TCSG_CALLBACKMSG_GAMEWIN,pGameWin,sizeof(GAME_WIN_RESULT));
	}
}


void CGameNotifyEx::NTF_PlayerOffline(LPPLAYER_OFFLINE lpOffline)
{
	CGameApp* pApp=(CGameApp*)AfxGetApp();
	((CMyGame*)pApp->GetMainGame())->NTF_PlayerOffline(lpOffline->nUserID);
}

void CGameNotifyEx::NTF_CardPass(CARDS_PASS* pCardsPass)
{
	CGameApp* pGameApp=(CGameApp*)AfxGetApp();
	((CMyGame*)pGameApp->GetMainGame())->NTF_CardsPass(pCardsPass);
}


void CGameNotifyEx::NTF_CardThrow(CARDS_THROW* pCarsThrow)
{
	CGameApp* pGameApp=(CGameApp*)AfxGetApp();
	((CMyGame*)pGameApp->GetMainGame())->NTF_CardsThrow(pCarsThrow);
}

void CGameNotifyEx::NTF_UserPosition(LPUSER_POSITION pUserPos)
{
	if (!theApp.IsLookOn())
	{
		theApp.SetUserStatus(pUserPos->dwUserStatus, TOTAL_CHAIRS);
	}

	CGameApp* pGameApp=(CGameApp*)AfxGetApp();
	((CMyGame*)pGameApp->GetMainGame())->m_nNewTableCountDown=pUserPos->nCountdown;
	CSKGameNotify::NTF_UserPosition(pUserPos);
}

void CGameNotifyEx::NTF_ZuPai(ZUPAI* pZuPai)
{
#ifdef _AUTOPLAY_
	UwlLogFile("[Begin]CGameNotifyEx::NTF_ZuPai--Time:%u,nUserID:%d,nRoomID:%d,nTableNO:%d,nChairNO:%d",
		GetTickCount(), pZuPai->nUserID, pZuPai->nRoomID, pZuPai->nTableNO, pZuPai->nChairNO);
#endif

	((CGameInfoEx*)theApp.GetGameInfo())->NTF_Zupai(pZuPai);

	CGameApp* pGameApp=(CGameApp*)AfxGetApp();
	if (pGameApp)
	{
		CMyGame* pGame=(CMyGame*)(pGameApp->GetMainGame());
		if (pGame)
		{
			pGame->m_CEventPipe->PushEvent(pGame->GetHandle(),TCSG_CALLBACKMSG_ZUPAI,pZuPai,sizeof(ZUPAI));
			//pGame->NTF_ZuPai(pZuPai);
		}
	}

#ifdef _AUTOPLAY_
UwlLogFile("[End]CGameNotifyEx::NTF_ZuPai--Time:%u,nUserID:%d,nRoomID:%d,nTableNO:%d,nChairNO:%d",
		GetTickCount(), pZuPai->nUserID, pZuPai->nRoomID, pZuPai->nTableNO, pZuPai->nChairNO);
#endif
}

void CGameNotifyEx::NTF_GameStartSolo(LPVOID pStartInfo)
{
	NTF_GameStart((GAME_START_INFO*)pStartInfo);
}

void CGameNotifyEx::NTF_UserDepositEvent(LPUSER_DEPOSITEVENT pUser_Deposit)
{
	CSKGameNotify::NTF_UserDepositEvent(pUser_Deposit);
	GAMEPLAYER* player=&theApp.m_GamePlayer[pUser_Deposit->nChairNO];
	player->nDeposit = pUser_Deposit->nDeposit;
	((CMyGame*)(theApp.GetMainGame()))->m_ptrPlayer[((CMyGame*)(theApp.GetMainGame()))->RUL_GetDrawIndexByChairNO(pUser_Deposit->nChairNO)]->DrawPlayerBoard();
}


void CGameNotifyEx::NTF_StartCountdown(START_COUNTDOWN* pCountdown)
{
	CGameApp* pApp=(CGameApp*)AfxGetApp();
	((CMyGame*)pApp->GetMainGame())->NTF_StartCountdown(pCountdown);
}

void CGameNotifyEx::NTF_SyncStartCountdown(SYNC_COUNTDOWN* pSyncCountdown)
{
	CGameApp* pApp=(CGameApp*)AfxGetApp();
	((CMyGame*)pApp->GetMainGame())->NTF_SyncStartCountdown(pSyncCountdown);
}

void CGameNotifyEx::NTF_PlayerEnter(SOLO_PLAYER * pSoloPlayer)
{
	CSKGameNotify::NTF_PlayerEnter(pSoloPlayer);
}

void CGameNotifyEx::NTF_PlayerAbort(GAME_ABORT * pGameAbort)
{	
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();

	//////////////////////////////////////////////////////////////////////////
	if(pGameApp->IsGameRunning())
	{
		if (IS_BIT_SET(pGameApp->GetRoomConfigs(), RC_VARIABLE_CHAIR))
		{//可变桌椅
			if (((CGameInfoEx*)theApp.GetGameInfo())->GetGameStartInfo()->bInGame[pGameAbort->nChairNO])
			{//在游戏中
				return;
			}
		}
		else
		{//桌椅数不可变
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//modify by pengsy at 20130821，增加userid，chairno，tableno验证
	GAMEPLAYER * pGamePlayer=pGameApp->GetGamePlayerByUserID(pGameAbort->nUserID);//
	if (pGamePlayer)
	{
		if(pGamePlayer->nTableNO!=pGameAbort->nTableNO )
			return;
		if(pGamePlayer->nChairNO!=pGameAbort->nChairNO )
			return;

		if(pGamePlayer->bLookOn)
			return;
	
		int chairno=pGameAbort->nChairNO;
		CString abortPlayerName = pGameApp->GetPlayerNameByChairNO(chairno);
		
		if (pGameAbort->nChairNO == pGameApp->GetChairNO())
		{
			if (pGameApp->IsLookOn())
			{ //旁观的对象已经退出
				CString sMsg;
				sMsg.Format(_T("%s 已经离开，旁观结束。"),abortPlayerName);
				int nLifeTime=10;
				
				pGameApp->ShowQuitTimerBox(sMsg,nLifeTime,TIMERDLG_FLAGS_BTNCANCEL);
			}
			else if (pGameAbort->nAbortFlag)	//pengsy
			{
				int nLifeTime=5;
				pGameApp->ShowQuitTimerBox(_T("其他玩家同意您退出游戏！"),nLifeTime,TIMERDLG_FLAGS_BTNCANCEL);
	//			pGameApp->GetGameFrame()->OnLeaveGame();
				return;
			}
		}
		
		CGameApp* pApp=(CGameApp*)AfxGetApp();
		((CMyGame*)pApp->GetMainGame())->NTF_PlayerAbort(pGameAbort);

		if (pGameApp->IsPlayerEnter(chairno))
		{
			pGameApp->GetGameFrame()->NTF_PlayerAbort(&pGameApp->m_GamePlayer[chairno]);
		}
		
		memset(&pGameApp->m_GamePlayer[chairno],0,sizeof(GAMEPLAYER));
		
		pGameApp->m_GameTable.nPlayerCount--;

		if (pGameApp->m_GameTable.nPlayerCount<0)
		{
			pGameApp->m_GameTable.nPlayerCount = 0;
		}

		memset(pGamePlayer,0,sizeof(GAMEPLAYER));
	}
}

void CGameNotifyEx::NTF_QZOK(QZ *pInfo)
{
	CGameApp* pApp=(CGameApp*)AfxGetApp();

	((CMyGame*)pApp->GetMainGame())->NTF_QZOK(pInfo);
}

void CGameNotifyEx::NTF_YaZhuOK(YAZHU *pInfo)
{
	CGameApp* pApp=(CGameApp*)AfxGetApp();

	((CMyGame*)pApp->GetMainGame())->NTF_YaZhuOK(pInfo);
}

//二维码充值start
void CGameNotifyEx::NTF_RoomLimit(LPROOMLIMIT pRoomLimit)
{
    if (NULL == pRoomLimit)
    {
        return;
    }
	
    CGameApp* pGameApp = (CGameApp*)AfxGetApp();
    if (pGameApp)
    {
        pGameApp->SetRoomLimit(pRoomLimit->bLargeRoom);
    }
}
//二维码充值end

//获取低保id
void CGameNotifyEx::NTF_WelfareActivity(int nActivityID)
{	
    CGameApp* pApp = (CGameApp*)AfxGetApp();
    if (pApp)
    {
        ((CMyGame*)pApp->GetMainGame())->OnGetWelfareConfig(nActivityID);
    }
}

//获取充值的相关url
void CGameNotifyEx::NTF_GetChargeUrl(LPCHARGEURL pChargeUrl)
{
	theApp.GetChargeUrl(pChargeUrl);
}

void CGameNotifyEx::NTF_IsTongBiRoom(int nIsTongBiRoom)
{	
    CGameApp* pApp = (CGameApp*)AfxGetApp();
    if (pApp)
    {
        ((CMyGame*)pApp->GetMainGame())->m_bTongBi=(nIsTongBiRoom==0)?FALSE:TRUE;
		if (((CMyGame*)pApp->GetMainGame())->m_bTongBi)
		{
			((CMyGame*)pApp->GetMainGame())->OPE_ShowBankerFlag(FALSE);
		}
    }
}


