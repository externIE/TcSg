// GameInfo.cpp: implementation of the CGameInfoEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game.h"
#include "GameInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameInfoEx::CGameInfoEx()
{
	ZeroMemory(&m_GameTableInfo,sizeof(m_GameTableInfo));
	ZeroMemory(&m_GameWinResult,sizeof(m_GameWinResult));
	InitialGameTableInfo(&m_GameTableInfo);
	m_bnAlowLook=FALSE;
}

CGameInfoEx::~CGameInfoEx()
{

}

GAME_WIN_RESULT*  CGameInfoEx::GetGameWin()
{
  return &m_GameWinResult;
}

GAME_START_INFO*  CGameInfoEx::GetGameStartInfo()
{
	return &m_GameTableInfo.gamestart;
}

GAME_TABLE_INFO*  CGameInfoEx::GetGameTableInfo()
{
	return &m_GameTableInfo;
}

int   CGameInfoEx::GetCurrentRank()
{
	return 1;//m_GameTableInfo.m_Public.nCurrentRank;
}

void  CGameInfoEx::NTF_StartGame(START_GAME* pStartGame)
{
	int chairno=pStartGame->nChairNO;

	m_GameEnterInfo.ei.dwUserStatus[chairno]|=US_GAME_STARTED;
}

void CGameInfoEx::NTF_GameStart(GAME_START_INFO * pStartData)
{
	ResetStatusWhenStart();

	GAME_START_INFO * pGameStartInfo=(GAME_START_INFO*)pStartData;
    memcpy(&m_GameTableInfo.gamestart,pStartData,sizeof(GAME_START_INFO));
	ZeroMemory(&m_GameWinResult,sizeof(m_GameWinResult));
}

void  CGameInfoEx::NTF_Zupai(ZUPAI* pZupai)
{
	SANGONG_DETAIL& detail = m_GameTableInfo.m_Player[pZupai->nChairNO].stDetail;

	for (int i=0;i<CHAIR_CARDS;i++)
	{
		int nCardID=pZupai->nCardID[i];
		detail.stCardInfo[i].nCardID = nCardID;
		detail.stCardInfo[i].nCardIndex = SK_GetCardIndexEx(nCardID);
		detail.stCardInfo[i].nShape=SK_GetCardShapeEx(nCardID);
		detail.stCardInfo[i].nValue=SK_GetCardValueEx(nCardID);
		detail.stCardInfo[i].nCardStatus=CARD_STATUS_THROWDOWN;
		detail.stCardInfo[i].nChairNO = pZupai->nChairNO;
		detail.stCardInfo[i].nPositionIndex = i; //
		detail.stCardInfo[i].nUniteCount=CHAIR_CARDS;
	}
	detail.nCardType = pZupai->nCardType;
}

void  CGameInfoEx::NTF_GameWin(GAME_WIN_RESULT* pGameWin)
{ 	
	m_GameTableInfo.gamestart.dwStatus=0;
	memcpy(&m_GameWinResult,pGameWin,sizeof(GAME_WIN_RESULT));
}

void  CGameInfoEx::ClearTableInfo()
{
	ZeroMemory(&m_GameTableInfo,sizeof(m_GameTableInfo));
	InitialGameTableInfo(&m_GameTableInfo);
}

void  CGameInfoEx::ResetStatusWhenStart()
{
	for(int i=0;i<MAX_CHAIR_COUNT;i++)
	{
		m_GameEnterInfo.ei.dwUserStatus[i]&=~US_GAME_STARTED;
	}

}

DWORD CGameInfoEx::GetStatus()
{
	return m_GameTableInfo.gamestart.dwStatus;
}

void CGameInfoEx::SetStatus(DWORD dwStatus)
{
    m_GameTableInfo.gamestart.dwStatus=dwStatus;
}

BOOL CGameInfoEx::IsGamePlaying()
{
    return IS_BIT_SET(GetStatus(),TS_PLAYING_GAME);
}

BOOL CGameInfoEx::IsForbidDesert()
{
	return m_GameTableInfo.gamestart.bForbidDesert;
}

BOOL CGameInfoEx::IsChairReady(int chairno)
{
	return IS_BIT_SET(m_GameEnterInfo.ei.dwUserStatus[chairno],US_GAME_STARTED);
}

void CGameInfoEx::ResetChairStatus(int chairno)
{
	if (chairno >= 0 && chairno < TOTAL_CHAIRS)
	{
		m_GameEnterInfo.ei.dwUserStatus[chairno] = 0;
	}
}

void CGameInfoEx::ResetChairReady()
{
	for(int i=0;i<MAX_CHAIR_COUNT;i++)
	{
		m_GameEnterInfo.ei.dwUserStatus[i] = 0;
	}
}	

void CGameInfoEx::SetUserStatus(DWORD dwUS[], int len)
{
	for (int i = 0; i < MAX_CHAIR_COUNT && i < len; i++)
	{		
		m_GameEnterInfo.ei.dwUserStatus[i] = dwUS[i];
	}
}

void CGameInfoEx::SetCurrentChairNO(int chairno)
{
	GetGameStartInfo()->nCurrentChair=chairno;
}

int CGameInfoEx::GetCurrentChairNO()
{
	return GetGameStartInfo()->nCurrentChair;
}

int  CGameInfoEx::GetBoutCount()
{
	return GetGameStartInfo()->nBoutCount;
}

int CGameInfoEx::GetBaseDeposit()
{
	return -1;
}

int CGameInfoEx::GetBaseScore()
{
    return GetGameStartInfo()->nBaseScore;
}

int  CGameInfoEx::GetBanker()
{
	return GetGameStartInfo()->nBanker;
}

int CGameInfoEx::GetAutoPasTime()
{
	return GetGameStartInfo()->nAutoGiveUp;
}

int CGameInfoEx::GetOfflineTime()
{
	return GetGameStartInfo()->nOffline;
}

int CGameInfoEx::GetThrowWait()
{
	return GetGameStartInfo()->nThrowWait;
}

void CGameInfoEx::SetObjectGains(int nGains)
{
	GetGameStartInfo()->nObjectGains = nGains;
}

int  CGameInfoEx::GetObjectGains()
{
	return GetGameStartInfo()->nObjectGains;
}

void CGameInfoEx::SetBankerChairNO(int chairno)
{
	GetGameStartInfo()->nBanker = chairno;
}
