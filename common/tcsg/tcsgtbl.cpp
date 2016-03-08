#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "StdAfx.h"
#include <algorithm>
#include <functional>
#include <vector>
using namespace std;

//计算牌的索引值
int TCSG_CalcCardValue(int nCardID)
{//0-12
	if (nCardID<0||nCardID>=TOTAL_CARDS)
		return -1;
	else
		return nCardID%LAYOUT_MOD;
}
//计算点数
int TCSG_CalcCardPoint(int nCardID)
{
	if (nCardID<0||nCardID>=TOTAL_CARDS)
	{
		return -1;
	}
	else
	{
		if (nCardID==52 || nCardID==53)
		{//大小王为0点
			return 0;
		}
		else if (nCardID%LAYOUT_MOD<9) 
		{//id(0-8)相当于(2-10点)
			return nCardID%LAYOUT_MOD + 2;
		}
		else if (nCardID%LAYOUT_MOD<=11) 
		{//J,Q,K全为0点
			return 10;
		}
		else if (nCardID%LAYOUT_MOD==12)
		{//A为1点
			return 1;
		}

		assert(false);
		return -1;
	}
}

CGameTable::CGameTable(int roomid, int tableno, int score_mult, 
			   int totalchairs, DWORD gameflags, DWORD gameflags2, int max_asks,
			   int totalcards, 
			   int totalpacks, int chaircards, int bottomcards,
			   int layoutnum, int layoutmod, int layoutnumex, 
			   int abtpairs[],
			   int throwwait, int maxautothrow, int entrustwait,
			   int max_auction, int min_auction, int def_auction,
			   FP_SK_GetXXX fpSKGetCardIndex,
			   FP_SK_GetXXX fpSKGetCardShape,
			   FP_SK_GetXXX fpSKGetCardValue,
			   FP_SK_GetXXX fpSKGetCardScore,
			   FP_SK_GetXXXEx fpSKGetCardPRI,
			   FP_SK_GetXXXEx fpSKGetIndexPRI)
			   :CSkTable(roomid,tableno,score_mult,totalchairs,gameflags,gameflags2,max_asks,
			   totalcards,
			   totalpacks,chaircards,bottomcards,
			   layoutnum,layoutmod,layoutnumex,abtpairs,
			   throwwait,maxautothrow, entrustwait,
			   max_auction,min_auction,def_auction,
			   fpSKGetCardIndex,fpSKGetCardShape,fpSKGetCardValue,fpSKGetCardScore,fpSKGetCardPRI,fpSKGetIndexPRI)
{
	// 固定信息，跟局数无关
	m_GameTalbeInfo=NULL;
	ResetMembers();

	TCHAR szRoomID[16];
	memset(szRoomID, 0, sizeof(szRoomID));
	_stprintf(szRoomID, _T("%ld"), roomid);
	m_bResetBanker=TRUE;
	m_nTempBanker=0;
	
	int nTongBiRoom=GetPrivateProfileInt(_T("TongBiRoom"),szRoomID,0,GetINIFileName());
	if(nTongBiRoom==1)
		m_bTongBiRoom=TRUE;
	else
		m_bTongBiRoom=FALSE;

	m_dwStatusOnStart = 0;
}

CGameTable::~CGameTable()
{
 	ResetMembers();
	
	if (m_GameTalbeInfo)
		delete m_GameTalbeInfo;
}

void InitialGameTableInfo(GAME_TABLE_INFO* table)
{
	if (!table) 
		return;
	ZeroMemory(table, sizeof(GAME_TABLE_INFO));
	table->m_Public.nWaitChair=-1;
	table->m_Public.nCurrentRank=1;//定死打1
	XygInitChairCards(table->gamestart.nHandID,CHAIR_CARDS);

	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		table->m_Player[i].nThrowTime = THROW_WAIT;
	}
}

//扩大了级牌的权值
int  My_GetIndexPRI(int nCardID/*nCardIndex*/,int nRank, DWORD gameflags)
{
	if (nCardID==14)
	{
		return 41;
	}
	else if (nCardID==15)
	{
		return 42;
	}
	else if (nCardID==nRank)
	{
		return 40;
	}
	else
	{
		return nCardID;
	}
}

GAME_TABLE_INFO* CGameTable::GetGameTableInfo()
{
	if (!m_GameTalbeInfo)
		ConstructGameData();

	return m_GameTalbeInfo;
}

GAME_PUBLIC_INFO* CGameTable::GetPublicInfo()
{
	if (!m_GameTalbeInfo)
		ConstructGameData();

	return &m_GameTalbeInfo->m_Public;
}

GAME_START_INFO* CGameTable::GetStartInfo()
{
	if (!m_GameTalbeInfo)
		ConstructGameData();

	return &m_GameTalbeInfo->gamestart;
}

GAME_PLAYER_INFO* CGameTable::GetPlayerInfo(int chairno)
{
	if (!m_GameTalbeInfo)
		ConstructGameData();

	return &m_GameTalbeInfo->m_Player[chairno];
}

void CGameTable::ResetMembers(BOOL bResetAll)
{
	CSkTable::ResetMembers(bResetAll);
	m_dwActionDealCard = 0;
	m_dwActionYaZhu=0;
	m_nObjectGains = 0;
	InitialGameTableInfo(m_GameTalbeInfo);
	memset(m_nAutoPlayCount,0,sizeof(m_nAutoPlayCount));
	memset(m_nBombHadDeal,-1,sizeof(m_nBombHadDeal));
	memset(m_nBottomCatch,-1,sizeof(m_nBottomCatch));
	m_nOperateTime = THROW_WAIT;
	m_nYaZhuTime = YAZHU_WAIT;
	m_nQZTime = QZ_WAIT;
	memset(m_nWinGains,0,sizeof(m_nWinGains));
	memset(m_nTongBiWinGains,0,sizeof(m_nTongBiWinGains));
	memset(m_nPlayerBaseDeposit,0,sizeof(m_nPlayerBaseDeposit));
	memset(m_nTongBiBaseDeposit,0,sizeof(m_nTongBiBaseDeposit));

	if(bResetAll){
		// 动态信息，跟上局相关
		if (m_GameTalbeInfo)
		{	
			for(int i=0;i<TOTAL_CHAIRS;i++)
				m_GameTalbeInfo->m_Player[i].nWaitTime=THROW_WAIT;
		}

		memset(m_nResultDiff,0,sizeof(m_nResultDiff));
		memset(m_nTotalResult,0,sizeof(m_nTotalResult));
	}

}

void CGameTable::ResetTable()
{
	CTable::ResetTable();

	memset(m_nResultDiff,0,sizeof(m_nResultDiff));
	memset(m_nTotalResult,0,sizeof(m_nTotalResult));
	m_nOperateTime = THROW_WAIT;
	m_nYaZhuTime = YAZHU_WAIT;
	m_nQZTime = QZ_WAIT;
}

int CGameTable::GetGameTableInfoSize()
{
	return sizeof(GAME_TABLE_INFO);
}

void CGameTable::FillupGameTableInfo(void* pData, int nLen, int chairno, BOOL lookon)
{
	ZeroMemory(pData, nLen);
	FillupStartData(pData,nLen);

	//////////////////////////////////////////////////////////////////////////
	{
		GAME_START_INFO* pStart = (GAME_START_INFO*)pData;
		pStart->nInHandCount = CHAIR_CARDS;
	}
	//////////////////////////////////////////////////////////////////////////

	//断线前的耗时
	if (m_dwActionStart>0)
	{
		GAME_START_INFO* pStart = (GAME_START_INFO*)pData;

		if (IS_BIT_SET(m_dwStatus, TS_WAITING_QZ))
		{
			pStart->nReserved[0] = (GetTickCount()-m_dwActionStart)/1000; 
		}
		else if (IS_BIT_SET(m_dwStatus, TS_WAITING_YAZHU))
		{
			pStart->nReserved[0] = (GetTickCount()-m_dwActionYaZhu)/1000; 
		}
		else if (IS_BIT_SET(m_dwStatus, TS_WAITING_ZUPAI))
		{
			pStart->nReserved[0] = (GetTickCount()-m_dwActionDealCard)/1000; 
		}		
		
		if (pStart->nReserved[0] < 0)
			pStart->nReserved[0] = 0;
		else if (pStart->nReserved[0] > CalcOpeTime()-5) 
			pStart->nReserved[0] = CalcOpeTime()-5;					   //至少留5秒 操作时间
	}

	//跳过GAME_START_INFO,拷贝公共信息
	GAME_TABLE_INFO* pGameInfo=(GAME_TABLE_INFO*) pData;
	//拷贝公共信息
	GAME_TABLE_INFO* pGameData=GetGameTableInfo();

	//拷贝玩家私密信息
	for(int k=0;k<TOTAL_CHAIRS;k++)
	{
		memcpy(&pGameInfo->m_Player[k],&(pGameData->m_Player[k]),sizeof(GAME_PLAYER_INFO));
		if (!pGameData->m_Public.bFinishZuPai[k]) 
		{
			if (lookon || k!=chairno)
			{
				for (int j=0;j<CHAIR_CARDS;j++)
				{
					memset(&(pGameInfo->m_Player[k].stDetail.stCardInfo[j]), 0, sizeof(pGameInfo->m_Player[k].stDetail.stCardInfo[j]));
					pGameInfo->m_Player[k].stDetail.stCardInfo[j].nCardID = -1;	//牌	
					pGameInfo->m_Player[k].stDetail.nCardPoint[j] = 0;			//每张牌点数
				}
				pGameInfo->m_Player[k].stDetail.nMaxCardIndex = -1;				//最大的那张牌的索引（stCardInfo[nMaxCardIndex]）
				pGameInfo->m_Player[k].stDetail.nCardType = CARD_TYPE_POINT0;		//牌型
			}
		}
		GetPlayerInfo(k)->nInHandCount=XygCardRemains(m_nCardsLayIn[k],SK_LAYOUT_NUM);
	}

//	if (!IS_BIT_SET(m_dwStatus,TS_WAITING_AUCTION))
//	{
//		memcpy(pGameInfo->gamestart.nBottomIDs, m_nBottomIDs, sizeof(pGameInfo->gamestart.nBottomIDs));
//	}

	if (!lookon)
	{
		pGameInfo->m_Player[chairno].nAskExitCount = m_nAskExit[chairno];
	}

	memcpy(&pGameInfo->m_Public,&(pGameData->m_Public),sizeof(GAME_PUBLIC_INFO));
	BOOL bHaveCard = HaveCards(chairno);
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		if (pGameInfo->m_Public.GameCard[i].nCardStatus==CARD_STATUS_INHAND)
		{
			if (lookon || pGameInfo->m_Public.GameCard[i].nChairNO!=chairno)
			{
				pGameInfo->m_Public.GameCard[i].nCardID=-1;
			}

			int chairno=pGameInfo->m_Public.GameCard[i].nChairNO;
			pGameInfo->m_Public.GameCard[i].nUniteCount=GetPlayerInfo(chairno)->nInHandCount;
		}
		if (pGameInfo->m_Public.GameCard[i].nCardStatus==CARD_STATUS_WAITDEAL
			&& IS_BIT_SET(m_dwStatus,TS_WAITING_AUCTION))
		{
			pGameInfo->m_Public.GameCard[i].nCardID=-1;
		}
	}


	//玩家状态
	memcpy(pGameInfo->m_Public.dwUserStatus,m_dwUserStatus,sizeof(pGameInfo->m_Public.dwUserStatus));
	memcpy(pGameInfo->m_Public.nResultDiff,m_nResultDiff,sizeof(m_nResultDiff));
	memcpy(pGameInfo->m_Public.nTotalResult,m_nTotalResult,sizeof(m_nTotalResult));
}

int CGameTable::GetEnterGameInfoSize()
{
	return sizeof(GAME_ENTER_INFO);
}

void CGameTable::FillupEnterGameInfo(void* pData, int nLen, int chairno, BOOL lookon)
{
	CTable::FillupEnterGameInfo(pData,nLen,chairno,lookon);

	GAME_ENTER_INFO* pEnterGame=(GAME_ENTER_INFO*)pData;

	memcpy(pEnterGame->nResultDiff,m_nResultDiff,sizeof(m_nResultDiff));
	memcpy(pEnterGame->nTotalResult,m_nTotalResult,sizeof(m_nTotalResult));
}


int CGameTable::GetGameStartSize()
{
	return sizeof(GAME_START_INFO);
}

void CGameTable::FillupGameStart(void* pData, int nLen, int chairno, BOOL lookon)
{
	ZeroMemory(pData, nLen);
	FillupStartData(pData,nLen);

	GAME_START_INFO* pStartData=(GAME_START_INFO*)pData;
	int sss = sizeof(GAME_START_INFO);
	pStartData->nInHandCount=CHAIR_CARDS;
	if (!lookon)
	{//填充玩家的手牌
		XygInitChairCards(pStartData->nHandID,CHAIR_CARDS);
		GetInHandCard(chairno,pStartData->nHandID);
	}
}

int CGameTable::GetGameWinSize()
{
    return sizeof(GAME_WIN_RESULT);
}

int CGameTable::FillupGameWin(void* pData, int nLen, int chairno)
{
	GAME_WIN_RESULT* pGameWin=(GAME_WIN_RESULT*)pData;
	ZeroMemory(pGameWin,sizeof(GAME_WIN_RESULT));

	for(int i=0;i<m_nTotalChairs;i++)
	{
		XygInitChairCards(pGameWin->nCardID[i],CHAIR_CARDS);
		pGameWin->nCardCount[i]=GetInHandCard(i,pGameWin->nCardID[i]);
	}

	//底牌
	memcpy(pGameWin->nBottomIDs, m_nBottomIDs, sizeof(pGameWin->nBottomIDs));
	
	{
		if(m_bTongBiRoom)
		{
			for(int i=0;i<TOTAL_CHAIRS;i++)
			{ 
				if (!m_bInGame[i]) 
					continue;
				for(int j=0;j<TOTAL_CHAIRS;j++)
				{
					if (j==i || !m_bInGame[j]) 
						continue;
					pGameWin->nGainsWin[i]+=m_nTongBiWinGains[i][j]*m_nTongBiBaseDeposit[i][j];
				}
			}
		}
		else
		{
			for(int i=0;i<TOTAL_CHAIRS;i++)
			{ 
				if (i==m_nBanker || !m_bInGame[i]) 
					continue;
				pGameWin->nGainsWin[i]=m_nWinGains[i]*m_nPlayerBaseDeposit[i];
				pGameWin->nGainsWin[m_nBanker]-=pGameWin->nGainsWin[i];
			}
		}
	}

	int nResult = CTable::FillupGameWin(pData, nLen, chairno);

	m_nTempBanker=m_nBanker;
	if(GetPlayerInfo(m_nBanker)->stDetail.nCardType==CARD_TYPE_POINT0)
		m_bResetBanker=TRUE;
	
	return nResult;
}

void CGameTable::FillupStartData(void* pData, int nLen)
{
	GAME_START_INFO* pStartData=(GAME_START_INFO*)pData;
	memcpy(pStartData->szSerialNO, m_szSerialNO, sizeof(m_szSerialNO));
	pStartData->nBoutCount=m_nBoutCount;
	pStartData->nBaseScore=m_nBaseScore;
	pStartData->bNeedDeposit=m_bNeedDeposit;
	pStartData->bForbidDesert=m_bForbidDesert;  //禁止强退,提醒凯子发布的时候打开
	pStartData->nBanker=m_nBanker;
	pStartData->nCurrentChair=GetCurrentChair();
	pStartData->dwStatus=m_dwStatus;
	pStartData->nThrowWait=m_nOperateTime;
	pStartData->nAutoGiveUp=SK_AUTO_END_GAME;
	pStartData->nOffline=SK_AUTO_CHECK_OFFLINE;
	
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		pStartData->nBaseDeposit[i]=m_nPlayerBaseDeposit[i];
	}

	pStartData->nObjectGains=m_nObjectGains;

	memcpy(pStartData->bInGame, m_bInGame, sizeof(m_bInGame));
	pStartData->nQZWait = m_nQZTime;
	pStartData->nYaZhuWait = m_nYaZhuTime;		// 押注等待时长
	pStartData->nZuPaiWait = m_nOperateTime;	// 组牌等待时长
	memcpy(pStartData->nTongBiBaseDeposit, m_nTongBiBaseDeposit, sizeof(m_nTongBiBaseDeposit));
}

int CGameTable::CalcBankerChairBefore()
{
	int result = 0;

	int nRand = XygGetRandomBetween(m_nTotalChairs);
	int cnt=0;
	for (int i=0;i<nRand+1;i++)
	{
		for (int j=0;j<TOTAL_CHAIRS;j++)
		{
			if (m_bInGame[j])
			{
				cnt++;
				if (cnt == nRand+1)
				{
					result = j;
					break;
				}
			}
		}
	}

	return result;
}


int CGameTable::CalcBankerChairAfter(void* pData, int nLen)
{
    return m_nBanker;
}

BOOL CGameTable::CalcWinPoints(void* pData, int nLen, int chairno, int nWinPoints[])
{
	GAME_WIN_RESULT* pGameWin = (GAME_WIN_RESULT*)pData;
	memset(nWinPoints, 0, sizeof(int)*TOTAL_CHAIRS);

	if(m_bTongBiRoom)
	{
		for(int i=0;i<m_nTotalChairs;i++)
		{
			if (!m_bInGame[i]) 
				continue;
			for(int j=0;j<m_nTotalChairs;j++)
			{
				if (j==i || !m_bInGame[j]) 
					continue;
				nWinPoints[i]+=m_nTongBiWinGains[i][j];
			}
		}
	}
	else
	{
		for(int i=0;i<m_nTotalChairs;i++)
		{
			if (!m_bInGame[i]) 
				continue;
			nWinPoints[i]=m_nWinGains[i];
		}
	}

	
	/////////////////////////////////////////////////////
	int total=0;
	for(int i = 0; i < m_nTotalChairs; i++)
		total+=nWinPoints[i];
	
	if (total!=0)
	{
		UwlLogFile("错误的记分错误，记分总和不为0!");
		for(int i = 0; i < m_nTotalChairs; i++)
			nWinPoints[i]=0;
	}
	////////////////////////////////////////////////////
 
	return TRUE;
}

int CGameTable::CalcResultDiffs(void* pData, int nLen, int nScoreDiffs[], int nDepositDiffs[])
{
	LPGAME_WIN pGameWin = (LPGAME_WIN)pData;
	for(int i = 0; i < m_nTotalChairs; i++)
	{
		nScoreDiffs[i] = 0;
	}

	if(m_nBaseDeposit)
	{// 玩银子，计算银子结果
		if(m_bTongBiRoom)
		{
			for(int i = 0; i < m_nTotalChairs; i++)
			{		
				if (!m_bInGame[i]) 
					continue;
				for(int j = 0; j < m_nTotalChairs; j++)
				{		
					if (j==i || !m_bInGame[j]) 
						continue;
					nDepositDiffs[i] += m_nTongBiBaseDeposit[i][j] * m_nTongBiWinGains[i][j];
				}
			}
		}
		else
		{
			for(int i = 0; i < m_nTotalChairs; i++)
			{		
				if (i==m_nBanker) 
					continue;
				nDepositDiffs[i] = m_nPlayerBaseDeposit[i] * pGameWin->nWinPoints[i];
				nDepositDiffs[m_nBanker]-=nDepositDiffs[i];
			}
		}
		for(int i = 0; i < m_nTotalChairs; i++)
		{
			// 计算积分结果
			if (nDepositDiffs[i]>0)
			{
				nScoreDiffs[i] += 4;
			}
			else if (nDepositDiffs[i]<0)
			{
				nScoreDiffs[i] -= 1;
			}
		}
	}

	return 0;
}

void CGameTable::ConstructGameData()
{
	if (!m_GameTalbeInfo)
	{
		m_GameTalbeInfo=new GAME_TABLE_INFO;
		InitialGameTableInfo(m_GameTalbeInfo);
	}
}

int  CGameTable::GetRandomBombNum(int seed)
{
	int nBombNum = 0;
	
	srand( (unsigned)time( NULL ) + seed * 1000 );
	
	int nRand = rand()%1001;
	if (nRand >= 0 && nRand <350)
		nBombNum = 0;
	else if(nRand >= 350 && nRand < 995)
		nBombNum = 1;
	else if (nRand >= 995&& nRand < 998)
	{
		nBombNum = 2;
	}
	else if(nRand >= 998 && nRand < 1000)
	{
		nBombNum = XygGetRandomBetweenEx(3, 4);
	}
	else if(nRand == 1000)
	{
		nBombNum = XygGetRandomBetweenEx(5, 7);
	}
	
	return nBombNum;
}

BOOL CGameTable::MakeDealCards(int nMakeChance)
{
	if (nMakeChance==MAKEBOMB_CHANCE)
		return FALSE;

	CPlayer* ptrP = m_ptrPlayers[0];
	if (!ptrP)
		return FALSE;

	int seed = ptrP->m_lTokenID * 10 + ptrP->m_hSocket;
	srand( (unsigned)time( NULL ) + seed * 1000 );
	
	BOOL bMake = FALSE;
	int nRand = rand()%101;
	if (nRand > 0 && nRand <=nMakeChance)
		bMake = TRUE;
	
	if(!bMake)
		return FALSE;
	
	int BombNum = GetRandomBombNum(seed);
	if (BombNum == 0)
		return FALSE;
	
	int ChairValue[TOTAL_CHAIRS];
	XygInitChairCards(ChairValue, TOTAL_CHAIRS);
	
	for (int i = 0; i < TOTAL_CHAIRS; i++)
	{
		ChairValue[i] = i;
	}
	
	random_shuffle(ChairValue, ChairValue+TOTAL_CHAIRS);
	
	int	BombValue[LAYOUT_NUM];
	XygInitChairCards(BombValue, LAYOUT_NUM);
	for (i = 1; i < 14; i++)	//从2到A
	{
		BombValue[i] = i;
	}
	random_shuffle(BombValue+1, BombValue+14);
	
	memset(m_nBombHadDeal,-1,sizeof(m_nBombHadDeal));
	int nCount = 1;
	int Bomb0 = XygGetRandomBetween(BombNum);
	Bomb0 = (Bomb0 > 3) ? 3 : Bomb0;
	MakeCardsLayIn(ChairValue[0], Bomb0, nCount, BombValue);
	int Bomb1 = XygGetRandomBetween(BombNum-Bomb0);
	Bomb1 = (Bomb1 > 3) ? 3 : Bomb1;
	MakeCardsLayIn(ChairValue[1], Bomb1, nCount, BombValue);
	int Bomb2 = BombNum - Bomb0 - Bomb1;
	Bomb2 = (Bomb2 > 3) ? 3 : Bomb2;
	MakeCardsLayIn(ChairValue[2], Bomb2, nCount, BombValue);
	
	return TRUE;
}

void CGameTable::MakeCardsLayIn(int nChair, int nBombNum, int& nCurIndex, int nBombValue[])
{
	for (int i = 0; i < nBombNum; i++)
	{
		int nValue = nBombValue[nCurIndex];
		m_nBombHadDeal[nCurIndex-1].chairno = nChair;
		m_nBombHadDeal[nCurIndex-1].nValueIndex = nValue;
		m_nCardsLayIn[nChair][nValue] = 4;
		nCurIndex++;
		UwlLogFile(_T("chair:%d, %d"),nChair,nValue+1);
	}
}


void CGameTable::DealCardNormal()
{	
	DealCardNormal_Begin();
	DealCardNormal_End();
}

void CGameTable::DealCardNormal_Begin()
{
	//抓牌，并计算牌信息
	int nCardID = -1;
	for(int j=0;j<CHAIR_CARDS;j++)
	{
		for(int i=0;i<TOTAL_CHAIRS;i++)
		{
			if (m_bInGame[i])
			{
				SANGONG_DETAIL* pDetail=&(GetPlayerInfo(i)->stDetail);
				if (NULL==pDetail) continue;
				
				nCardID = CatchOneCard(i);
				GetPublicInfo()->GameCard[GetPublicInfo()->nCurrentCatch-1].nUniteCount = CHAIR_CARDS;
				GetPublicInfo()->GameCard[GetPublicInfo()->nCurrentCatch-1].nPositionIndex=j;
				pDetail->stCardInfo[j] = GetPublicInfo()->GameCard[GetPublicInfo()->nCurrentCatch-1];
				pDetail->nCardPoint[j] = TCSG_CalcCardPoint(pDetail->stCardInfo[j].nCardID);
				pDetail->nMaxCardIndex=CalcMaxCard(pDetail->stCardInfo, CHAIR_CARDS);
				pDetail->nCardType=CalcCardType(i, pDetail);
			}
		}
	}
}

void CGameTable::DealCardNormal_End()
{	
	//胜负关系
	if(m_bTongBiRoom)
	{
		for (int i=0;i<TOTAL_CHAIRS;i++)
		{
			if (!m_bInGame[i]) 
				continue;
			for(int j=0;j<TOTAL_CHAIRS;j++)
			{
				if (j==i || !m_bInGame[j]) 
					continue;
				m_nTongBiWinGains[i][j]=GetWinGains(GetPlayerInfo(i)->stDetail, GetPlayerInfo(j)->stDetail);
				m_nTongBiBaseDeposit[i][j]=CalcPlayerBaseDeposit(i,j);
			}
		}
	}
	else
	{
		for (int i=0;i<TOTAL_CHAIRS;i++)
		{
			if (m_nBanker == i || !m_bInGame[i]) 
				continue;
			BOOL bValidateBet=ValidateBetDbl(GetPublicInfo()->nYaZhuDbl[i]);
			if (bValidateBet)
			{
				int nWinDbl=GetWinGains(GetPlayerInfo(i)->stDetail, GetPlayerInfo(m_nBanker)->stDetail);
				m_nWinGains[i]=GetPublicInfo()->nYaZhuDbl[i]*nWinDbl;
				m_nWinGains[m_nBanker]-=m_nWinGains[i];
			}
		}
	}
}

int CGameTable::CalcCardType(int nChairNO, PSANGONG_DETAIL pDetail)
{
	if (pDetail->nCardPoint[0]==3
		&&pDetail->nCardPoint[1]==3
		&&pDetail->nCardPoint[2]==3)
	{
		return CARD_TYPE_BAOJIU;
	}
	else
	{
		int nLay[LAYOUT_NUM];
		memset(nLay,0,sizeof(nLay));
		for(int i = 0; i<CHAIR_CARDS; i++)
		{
			nLay[pDetail->stCardInfo[i].nCardIndex]++;
		}
		for (int j=0; j<LAYOUT_NUM; j++)
		{//3个一样是炸弹
			if (nLay[j]==3)
			{
				return CARD_TYPE_ZHADAN;
			}
		}

		if (nLay[10]+nLay[11]+nLay[12]+nLay[14]+nLay[15]==3)
		{//J,Q,K,大小王和为3个
			return CARD_TYPE_SANGONG;
		}
		else
		{
			int nTotalPoint=0;
			for(int i=0;i<CHAIR_CARDS;i++)
				nTotalPoint+=pDetail->nCardPoint[i];
			int nPoint=nTotalPoint%10;
			return nPoint;
		}
	}
	return CARD_TYPE_POINT0;
}

int CGameTable::IsMakedCard(int nCardID)
{
	int nIndex = SK_GetCardIndexEx(nCardID,0);
	if (nIndex<0)
		return -1;

	for (int i=0;i<LAYOUT_NUM;i++)
	{
		if (m_nBombHadDeal[i].chairno<0
			|| m_nBombHadDeal[i].nValueIndex<0)
			continue;
		
		if (m_nBombHadDeal[i].nValueIndex == nIndex)
			return m_nBombHadDeal[i].chairno;
	}

	return -1;
}

void CGameTable::StartDeal()
{
	CString strIniFile = GetINIFileName();
	TCHAR szRoomID[16];
	memset(szRoomID, 0, sizeof(szRoomID));
	_stprintf(szRoomID, _T("%ld"), m_nRoomID);
	
	{//读取游戏操作时间
		m_nOperateTime = GetPrivateProfileInt(_T("OperateTime"), szRoomID, ZUPAI_WAIT, strIniFile);		
	}
	
	//玩家是否参与本局游戏
	GAME_START_INFO* pStartInfo=GetStartInfo();
	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		if (m_ptrPlayers[i] && !m_ptrPlayers[i]->m_bIdlePlayer)
		{
			m_bInGame[i] = TRUE;
		}
		else
		{
			m_bInGame[i] = FALSE;
		} 
	}

	if(m_bTongBiRoom)
	{
		m_dwStatusOnStart=TS_PLAYING_GAME|TS_WAITING_ZUPAI;
		//通比场直接发牌
		DealCardAfterYaZhu();
	}
	else
	{
		//在没有玩家离开时m_nTempBanker就是m_nBanker 
		//当有玩家离开时m_nBanker会被置为0，此时m_nTempBanker还是记着上一局的m_nBanker
		if (TRUE == m_bResetBanker || FALSE == m_bInGame[m_nTempBanker])
		{
			m_bResetBanker=FALSE;
			m_dwStatusOnStart=TS_PLAYING_GAME|TS_WAITING_QZ;
		}
		else
		{
			m_nBanker=m_nTempBanker;
			m_dwStatusOnStart=TS_PLAYING_GAME|TS_WAITING_YAZHU;
			for(i=0;i<TOTAL_CHAIRS;i++)
			{
				if (!m_bInGame[i]) 
					continue;
				if (i == m_nBanker) 
					continue;
				m_nPlayerBaseDeposit[i]=CalcPlayerBaseDeposit(i,m_nBanker);
			}
		}
	}
}


BOOL CGameTable::CheckCards()
{
	int	nCardsLay[MAX_CARDS_LAYOUT_NUM];	// 
	ZeroMemory(nCardsLay, sizeof(nCardsLay));
	
	GAME_PUBLIC_INFO* pPublicInfo=GetPublicInfo();
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		nCardsLay[pPublicInfo->GameCard[i].nCardID%54]++;
	}
	
	for (i=0;i<54;i++)
	{
		if(nCardsLay[i] != m_nTotalPacks){
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CGameTable::ReadCardsFromFile()
{
	TCHAR szKey[32];
	TCHAR szCards[TOTAL_CHAIRS][256];
	///////////////////////////////////////////////
	GAME_PUBLIC_INFO* pPublicInfo=GetPublicInfo();
	int nInGameCount=0;
	for(int i=0; i<TOTAL_CHAIRS; i++)
	{
		if(m_bInGame[i])
			nInGameCount++;
	}

	int nTempCount=-1;
	for(i=0; i<TOTAL_CHAIRS; i++)
	{
		if(!m_bInGame[i])
			continue;
		nTempCount++;

		sprintf(szKey,_T("Chair%d"),i);
		GetPrivateProfileString ( _T("StartDeal"),szKey, _T(""),szCards[i], 256, GetINIFileName()) ; 
		
		TCHAR *fields[128];
		TCHAR *p1, *p2;
		p1=szCards[i];
		int nCount=RetrieveFields ( p1, fields, 60, &p2 ) ;
		for(int x=0; x<nCount; x++)
		{
			int nIndex=x*nInGameCount+nTempCount;
			int nCardID=atoi(fields[x]);
			pPublicInfo->GameCard[nIndex].nCardID=nCardID;
			pPublicInfo->GameCard[nIndex].nCardIndex=SK_GetCardIndexEx(nCardID,0);
			pPublicInfo->GameCard[nIndex].nShape=SK_GetCardShapeEx(nCardID,0);
			pPublicInfo->GameCard[nIndex].nValue=SK_GetCardValueEx(nCardID,0);
			pPublicInfo->GameCard[nIndex].nCardStatus=CARD_STATUS_WAITDEAL;
			pPublicInfo->GameCard[nIndex].nChairNO=-1;
			pPublicInfo->GameCard[nIndex].nPositionIndex=nIndex;
			pPublicInfo->GameCard[nIndex].nUniteCount=0;
		}
	}
	return TRUE;
}

int CGameTable::RetrieveFields ( TCHAR *buf, TCHAR **fields, int maxfields, TCHAR**buf2 )
{
	if ( buf == NULL )
		return 0;
	
	TCHAR *p;
	p = buf;
	int count = 0;
	
	try{
		while ( 1 ) {
			fields [ count ++ ] = p;
			while ( *p != '|' && *p != '\0' ) p++;
			if ( *p == '\0' || count >= maxfields ) 
				break;
			*p = '\0';
			p++;
		}
	}catch(...)
	{
		buf2=NULL;
		return 0;
	}
	
	if ( *p == '\0' ) 
		*buf2 = NULL;
	else
		*buf2 = p+1;
	*p = '\0';
	
	return count;
}

CString CGameTable::GetINIFileName()
{
	CString sRet;
	TCHAR szFilePath[MAX_PATH];
	GetModuleFileName(NULL,szFilePath,MAX_PATH);
	
	*strrchr(szFilePath,'\\')=0;	
	strcat(szFilePath, "\\");
	strcat(szFilePath,PRODUCT_NAME);
	strcat(szFilePath,".ini");
	sRet.Format(_T("%s"),szFilePath);
	return sRet;
}


int  CGameTable::CatchOneCard(int chairno)
{
	if (GetPublicInfo()->nCurrentCatch>=m_nTotalCards)
		return -1;//没牌了
	
	GetPublicInfo()->GameCard[GetPublicInfo()->nCurrentCatch].nCardStatus=CARD_STATUS_INHAND;
	GetPublicInfo()->GameCard[GetPublicInfo()->nCurrentCatch].nChairNO=chairno;

	int nCardID=GetPublicInfo()->GameCard[GetPublicInfo()->nCurrentCatch].nCardID;
	int nIndex=SK_GetCardIndexEx(nCardID, 0);
	m_nCardsLayIn[chairno][nIndex]++;
    GetPublicInfo()->nCurrentCatch++;
	
	return nCardID;
}

BOOL CGameTable::GiveCard(int chairno,int destchair,int nCardID)
{
	CARDINFO* card=GetCard(nCardID);
	if (!card) return FALSE;
	if (card->nChairNO!=chairno)return FALSE;
	card->nChairNO=destchair;
	m_nCardsLayIn[chairno][card->nCardIndex]--;
	m_nCardsLayIn[destchair][card->nCardIndex]++;
	return TRUE;
}

BOOL  CGameTable::ValidateThrow(CARDS_THROW* pCardsThrow)
{
	if (!IS_BIT_SET(m_dwStatus,TS_PLAYING_GAME))
		return FALSE;
	if (!IS_BIT_SET(m_dwStatus,TS_WAITING_THROW))
		return FALSE;
	if (pCardsThrow->nChairNO!=GetCurrentChair())
		return FALSE;
	if (!IsCardInHand(pCardsThrow->nChairNO,pCardsThrow->unite.nCardIDs,pCardsThrow->unite.nCardCount))
        return FALSE;
	if (pCardsThrow->unite.nCardCount!=GetCardCount(pCardsThrow->unite.nCardIDs,MAX_CARDS_PER_CHAIR))
		return FALSE;
	
	
	int nThrowHand[MAX_CARDS_PER_CHAIR];
	XygInitChairCards(nThrowHand,MAX_CARDS_PER_CHAIR);
    memcpy(nThrowHand,pCardsThrow->unite.nCardIDs,sizeof(int)*MAX_CARDS_PER_CHAIR);
	CARD_UNITE unit_details;
	ZeroMemory(&unit_details,sizeof(CARD_UNITE));
	
	if (!GetUniteDetails(pCardsThrow->nChairNO,nThrowHand,MAX_CARDS_PER_CHAIR,unit_details,pCardsThrow->unite.dwCardType))
		return FALSE;
	
	if (GetPublicInfo()->nWaitChair!=-1
		&&!CompareCards(GetPublicInfo()->WaitCardUnite,pCardsThrow->unite))//不是第一手牌，必须比前一首牌大
		return FALSE;
	
	//这5种牌型，牌数必须一致
	if (GetPublicInfo()->nWaitChair!=-1
		&& GetPublicInfo()->WaitCardUnite.dwCardType == pCardsThrow->unite.dwCardType)
	{
		if ((pCardsThrow->unite.dwCardType == CARD_UNITE_TYPE_ABT_SINGLE
			|| pCardsThrow->unite.dwCardType == CARD_UNITE_TYPE_ABT_COUPLE
			|| pCardsThrow->unite.dwCardType == CARD_UNITE_TYPE_ABT_THREE
			|| pCardsThrow->unite.dwCardType == CARD_UNITE_TYPE_ABT_THREE_1
			|| pCardsThrow->unite.dwCardType == CARD_UNITE_TYPE_ABT_THREE_COUPLE)
			&& GetPublicInfo()->WaitCardUnite.nCardCount!=pCardsThrow->unite.nCardCount)
			return FALSE;
	}

	return TRUE;
}

int  CGameTable::ThrowCards(CARDS_THROW* pCardsThrow)
{
	GetPlayerInfo(pCardsThrow->nChairNO)->nThrowCount++;

	GetPublicInfo()->nWaitChair=pCardsThrow->nChairNO;
	GetPublicInfo()->WaitCardUnite=pCardsThrow->unite;
	
	for(int i=0;i<pCardsThrow->unite.nCardCount;i++)
	{
		if (pCardsThrow->unite.nCardIDs[i]==-1) break;
		
		CARDINFO* card=GetCard(pCardsThrow->unite.nCardIDs[i]);
		if (!card) continue;
		card->nChairNO=pCardsThrow->nChairNO;
		card->nCardStatus=CARD_STATUS_THROWDOWN;
		card->nPositionIndex=i;
		card->nUniteCount=pCardsThrow->unite.nCardCount;
		int index=SK_GetCardIndexEx(pCardsThrow->unite.nCardIDs[i], 0);
		m_nCardsLayIn[pCardsThrow->nChairNO][index]--;
	}
	
	CalcBombInThrow(pCardsThrow);
	CalcChairThrowTime(pCardsThrow->nChairNO);
	
	if (IsOffline(pCardsThrow->nChairNO)
		||IsAutoPlay(pCardsThrow->nChairNO))
	{
		GetPlayerInfo(pCardsThrow->nChairNO)->nAutoThrowCount++;
	}
	
	SetStatus(TS_PLAYING_GAME|TS_WAITING_THROW);
	return 0;
}

CARDINFO*  CGameTable::GetCard(int nCardID)
{
	GAME_PUBLIC_INFO* pPublicInfo=GetPublicInfo();
	for(int i=0;i<m_nTotalCards;i++)
	{
		if (nCardID==pPublicInfo->GameCard[i].nCardID)
			return &pPublicInfo->GameCard[i];
	}
	return NULL;
}


BOOL   CGameTable::SetCardStatus(int nCardID,int chairno,int nStatus)
{
	GAME_PUBLIC_INFO* pPublicInfo=GetPublicInfo();
	for(int i=0;i<m_nTotalCards;i++)
	{
		if (pPublicInfo->GameCard[i].nCardID==nCardID)
		{
			pPublicInfo->GameCard[i].nCardStatus=nStatus;
			pPublicInfo->GameCard[i].nChairNO=chairno;
			return TRUE;
		}
	}
	
	return FALSE;
}


BOOL  CGameTable::CalcWinOnThrow(CARDS_THROW* pCardsThrow)
{
	int nMyChir=GetCurrentChair();
	int nFriend=GetNextChair(GetNextChair(GetCurrentChair()));

	if (!HaveCards(nMyChir))
	{   
		m_dwWinFlags=GW_NORMAL;
		pCardsThrow->Next_Chair=pCardsThrow->nChairNO;
		return TRUE;
	}
	else
	{
		int chairno=GetNextChair(GetCurrentChair());
        SetCurrentChair(chairno,GetPlayerInfo(chairno)->nWaitTime);
		PutThrowCardsToCost(GetCurrentChair());

		while(!HaveCards(GetCurrentChair()))
		{
			int chairno=GetNextChair(GetCurrentChair());
			SetCurrentChair(chairno,GetPlayerInfo(chairno)->nWaitTime);
			PutThrowCardsToCost(GetCurrentChair());
		}

		pCardsThrow->Next_Chair=GetCurrentChair();
		return FALSE;
	}
}

BOOL  CGameTable::CalcWinOnPass(CARDS_PASS* pCardsPass)
{
	while(1)
	{
		int chairno=GetNextChair(GetCurrentChair());
        SetCurrentChair(chairno,GetPlayerInfo(chairno)->nWaitTime);
		//SetCurrentChair(GetNextChair(GetCurrentChair()));
		PutThrowCardsToCost(GetCurrentChair());
		if (GetCurrentChair()==GetPublicInfo()->nWaitChair)//转回当前位置了
		{
			pCardsPass->nWinChair=GetPublicInfo()->nWaitChair;

			GetPublicInfo()->nWaitChair=-1;
			memset(&GetPublicInfo()->WaitCardUnite,0,sizeof(UNITE_TYPE));
			PutAllCardsToCost();//把所有打出的牌都放回废牌区
			pCardsPass->bNextFirst=TRUE;
			if (HaveCards(GetCurrentChair()))//我还有牌，那么我继续出
			{
				pCardsPass->Next_Chair=GetCurrentChair();
			}
			else
 			{	
				//没牌，应该已经结束了	//reserved
			}
			return TRUE;
		}
		else if (HaveCards(GetCurrentChair()))
		{
			pCardsPass->Next_Chair=GetCurrentChair();
			pCardsPass->bNextFirst=FALSE;
			return TRUE;
		}
	}

	return TRUE;
}

void   CGameTable::PutThrowCardsToCost(int chairno)
{

    GAME_PUBLIC_INFO* pPublicInfo=GetPublicInfo();
	for(int i=0;i<m_nTotalCards;i++)
	{
		if (pPublicInfo->GameCard[i].nCardStatus==CARD_STATUS_THROWDOWN
			&&pPublicInfo->GameCard[i].nChairNO==chairno)
		{
			pPublicInfo->GameCard[i].nCardStatus=CARD_STATUS_COST;
		}
	}
}

void   CGameTable::PutAllCardsToCost()
{
	GAME_PUBLIC_INFO* pPublicInfo=GetPublicInfo();
	for(int i=0;i<m_nTotalCards;i++)
	{
		if (pPublicInfo->GameCard[i].nCardStatus==CARD_STATUS_THROWDOWN)
		{
			pPublicInfo->GameCard[i].nCardStatus=CARD_STATUS_COST;
		}
	}
}

int  CGameTable::GetTributeCard(int chairno)
{
	int m=0;
	int temp=-1;
	GAME_PUBLIC_INFO* pPublicInfo=GetPublicInfo();
	for(int i=0;i<m_nTotalCards;i++)
	{
		if (pPublicInfo->GameCard[i].nChairNO==chairno
			&&pPublicInfo->GameCard[i].nCardStatus==CARD_STATUS_INHAND
			&&!IsJoker(pPublicInfo->GameCard[i].nCardID)//不能进贡逢人配
			&&SK_GetCardPRIEx(pPublicInfo->GameCard[i].nCardID,GetCurrentRank(),0)>m)
		{
			m=SK_GetCardPRIEx(pPublicInfo->GameCard[i].nCardID,GetCurrentRank(),0);
			temp=pPublicInfo->GameCard[i].nCardID;
		}
	}
	
	return temp;
}

int  CGameTable::GetInHandCard(int chairno,int nCardIDs[])
{
	int count=0;
	GAME_PUBLIC_INFO* pPublicInfo=GetPublicInfo();
	for(int i=0;i<m_nTotalCards;i++)
	{
		if (pPublicInfo->GameCard[i].nChairNO==chairno
			&&pPublicInfo->GameCard[i].nCardStatus==CARD_STATUS_INHAND
			&&count<m_nChairCards)
		{
			nCardIDs[count++]=pPublicInfo->GameCard[i].nCardID;
		}
	}
	return count;
}

void CGameTable::SetCurrentRank(int nRank)
{
	GetPublicInfo()->nCurrentRank=nRank;
}

int  CGameTable::GetCurrentRank()
{
	return GetPublicInfo()->nCurrentRank;
}

int  CGameTable::CalcChairThrowTime(int chairno)
{
	int nTotalCastTime=GetPlayerInfo(chairno)->nTotalThrowCost;
	nTotalCastTime+=(GetTickCount()-m_dwStatusBegin)/1000;

	GetPlayerInfo(chairno)->nThrowTime=m_nOperateTime;
	GetPlayerInfo(chairno)->nTotalThrowCost=nTotalCastTime;
	
	return m_nOperateTime;
}

void    CGameTable::CalcBombInThrow(CARDS_THROW* pCardsThrow)
{
	//记录炸弹奖励
	if (pCardsThrow->unite.dwCardType==CARD_UNITE_TYPE_BOMB
		|| pCardsThrow->unite.dwCardType==CARD_UNITE_TYPE_2KING)
	{
		GetPublicInfo()->nBombFan+=1;
		GetPlayerInfo(pCardsThrow->nChairNO)->nBombCount++;
	}
}


BOOL  CGameTable::IsCardInHand(int nChairNO,int nCardIDs[],int nCount)
{
	for(int i=0;i<nCount;i++)
	{
		if (nCardIDs[i]!=-1)
		{
			CARDINFO* card=GetCard(nCardIDs[i]);
			if (!card||card->nChairNO!=nChairNO)
				return FALSE;
		}
	}
	
	return TRUE;
}

void  CGameTable::OnPass(CARDS_PASS* pCardsPass)
{
	SetStatus(TS_PLAYING_GAME|TS_WAITING_THROW);
}

BOOL  CGameTable::ValidatePass(CARDS_PASS* pCardsPass)
{
	if (!IS_BIT_SET(m_dwStatus,TS_PLAYING_GAME))
		return FALSE;
	if (!IS_BIT_SET(m_dwStatus,TS_WAITING_THROW))
		return FALSE;
	if (!ValidateChair(pCardsPass->nChairNO)
		|| !ValidateChair(GetCurrentChair()))
		return FALSE;
	if (pCardsPass->nChairNO!=GetCurrentChair())
		return FALSE;
	
	return TRUE;
}


void CGameTable::FillupNextBoutInfo(void* pData, int nLen, int chairno)
{
	CSkTable::FillupNextBoutInfo(pData,nLen,chairno);
	LPGAME_WIN_RESULT pGameWin = (LPGAME_WIN_RESULT)pData;
	pGameWin->nNextBaseScore = GetBaseScore();

	//Add on 20121011
	//读房间设置
	//如果是比赛房间固定基础银基础分，使用固定值
	CString strIniFile = GetINIFileName();
	{
		TCHAR szRoomID[16];
		memset(szRoomID, 0, sizeof(szRoomID));
		_stprintf(szRoomID, _T("%ld"), m_nRoomID);

		int base_score = GetPrivateProfileInt(
				_T("basescore"),		// section name
				szRoomID,		// key name
				0,	// default int
				strIniFile		// initialization file name
				);
			
		if (base_score>0)
			pGameWin->nNextBaseScore = base_score;
	}
	//Add end

	//无人叫庄，此局无效
	if (pGameWin->nReserved[0]!=0)
	{
		pGameWin->nReserved[1] = chairno;
	}
}	

BOOL CGameTable::GetCardType_Bomb(int nCardIDs[],int nCardLen,int nCardLay[],int nLayLen,int nJokerCount,UNITE_TYPE& type,int nUseCount)
{
	if (nCardLen<=0 || nLayLen<=0)
		return FALSE;
	
	int nValue=0;
	if (type.dwCardType==CARD_UNITE_TYPE_BOMB)
	{
		nValue=type.nMainValue;
	}
	
	int nCardCount=0;
	int nCardIndex=-1;
	if (nUseCount)
	{
		nCardCount=nUseCount;
		nCardIndex=GetSameCountEx(nCardLay,nLayLen,nUseCount,nJokerCount,nValue);
		if (nCardIndex==-1)
			return FALSE;
	}
	else
	{
		for(int i=4;i<=4*m_nTotalPacks;i++)
		{
			nCardIndex=GetSameCountEx(nCardLay,nLayLen,i,nJokerCount,nValue);
			if (nCardIndex!=-1)
			{
				nCardCount=i;
				break;
			}
		}
		
		if (nCardIndex==-1)
			return FALSE;
	}
	// 			
	type.dwCardType=CARD_UNITE_TYPE_BOMB;
	type.dwComPareType=COMPARE_UNITE_TYPE_BOMB_EX;
	type.nMainValue=SK_GetIndexPRIEx(nCardIndex,GetCurrentRank(), 0);
	for(int k=0;k<nCardCount-4;k++)
	{
		type.nMainValue+=10000;
	}
	
	type.nCardCount=nCardCount;
	XygInitChairCards(type.nCardIDs,MAX_CARDS_PER_CHAIR);
	
	int temp[MAX_CARDS_PER_CHAIR];
	memset(temp,0,sizeof(temp));
	memcpy(temp,nCardIDs,sizeof(int)*nCardLen);
	PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,nCardIndex,nCardCount);
	return TRUE;
}


BOOL  CGameTable::GetUniteDetails(int chairno, int nCardIDs[],int nCardsLen,CARD_UNITE& unit_detail,DWORD dwFlags)
{
	int count=GetCardCount(nCardIDs,nCardsLen);
	//首先清理CARD_UNITE
	
	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_SINGLE))
		CalcCardType_Single(nCardIDs,nCardsLen,count,&unit_detail);
	
	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_COUPLE))
		CalcCardType_Couple(nCardIDs,nCardsLen,count,&unit_detail);
	
	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_THREE))
		CalcCardType_Three(nCardIDs,nCardsLen,count,&unit_detail);

	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_THREE_1))
		CaclCardType_Three_1(nCardIDs,nCardsLen,count,&unit_detail);
	
	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_THREE_COUPLE))
		CaclCardType_Three_Couple(nCardIDs,nCardsLen,count,&unit_detail);
	
	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_ABT_SINGLE))
		CaclCardType_ABT_Single(nCardIDs,nCardsLen,count,&unit_detail);
	
	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_ABT_COUPLE))
		CaclCardType_ABT_Couple(nCardIDs,nCardsLen,count,&unit_detail);
	
	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_ABT_THREE))
		CaclCardType_ABT_Three(nCardIDs,nCardsLen,count,&unit_detail);

	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_ABT_THREE_1))
		CaclCardType_ABT_Three_1(nCardIDs,nCardsLen,count,&unit_detail);
	
	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_ABT_THREE_COUPLE))
		CaclCardType_ABT_Three_Couple(nCardIDs,nCardsLen,count,&unit_detail);
	
	//4带2张单
	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_FOUR_2))
		CalcCardType_Four_2(nCardIDs,nCardsLen,count,&unit_detail);

	//4带2张对子
	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_FOUR_2_COUPLE))
		CalcCardType_Four_2_Couple(nCardIDs,nCardsLen,count,&unit_detail);

	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_BOMB))
		CalcCardType_Bomb(nCardIDs,nCardsLen,count,&unit_detail);
	

	if (IS_BIT_SET(dwFlags,CARD_UNITE_TYPE_2KING))
		CalcCardType_BOMB_2King(nCardIDs,nCardsLen,count,&unit_detail);
	
	
	if (unit_detail.nTypeCount>0)
		return TRUE;
	else
		return FALSE;
}

DWORD CGameTable::SetStatusOnStart()
{
	return AddStatus(m_dwStatusOnStart);
}

int CGameTable::SetCurrentChairOnStart()
{
	SetCurrentChair(m_nBanker,m_nOperateTime);
	
	return GetCurrentChair();
}

BOOL CGameTable::CaclCardType_Three_1(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail)
{
	if (nCardLen<=0)
		return FALSE;
	
	//3带1
	if (CardDetail->nTypeCount>=MAX_FIT_TYPE) return FALSE;
	if (nCardCount!=4) return FALSE;                  
	int nCardLay[SK_LAYOUT_NUM];
	memset(nCardLay,0,sizeof(nCardLay));
	int nJokerCount=0;
	PreDealCards(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount);
	
	int& index=CardDetail->nTypeCount;   
	//	if (!GetCardType_Three_Couple(nCardID,nCardLay,nJokerCount,CardDetail->uniteType[index]))
	//		return FALSE;
	
	BOOL bnFind=FALSE;
	while(GetCardType_Three_1(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount,CardDetail->uniteType[index]))
	{
		//迭代自身最大值
		bnFind=TRUE;
	}
	if (!bnFind) return FALSE;
	
	index++;
	return TRUE;
}

BOOL CGameTable::GetCardType_Three_1(int nCardIDs[],int nCardLen,int nCardLay[],int nLayLen,int nJokerCount,UNITE_TYPE& type)
{
	if (nCardLen<=0 || nLayLen<=0)
		return FALSE;
	
	int nMainIndex=-1;
	int nSecondIndex=-1;
	if (!GetDoubleCount(nCardLay,nLayLen,3,1,nJokerCount,type.nMainValue,nMainIndex,nSecondIndex))
		return FALSE;
	
	type.dwCardType=CARD_UNITE_TYPE_THREE_1;
	type.dwComPareType=COMPARE_UNITE_TYPE_THREE_1;
	type.nMainValue=SK_GetIndexPRIEx(nMainIndex,GetCurrentRank(), 0)*10000+SK_GetIndexPRIEx(nSecondIndex,GetCurrentRank(), 0);
	type.nCardCount=4;
	
	XygInitChairCards(type.nCardIDs,MAX_CARDS_PER_CHAIR);
	
	int temp[MAX_CARDS_PER_CHAIR];
	memset(temp,0,sizeof(temp));
	memcpy(temp,nCardIDs,sizeof(int)*nCardLen);
	PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,nMainIndex,3);
	PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,nSecondIndex,1);
	return TRUE;
}


BOOL  CGameTable::CaclCardType_ABT_Single(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail)
{
	if (nCardLen<=0)
		return FALSE;

	if (CardDetail->nTypeCount>=MAX_FIT_TYPE) return FALSE;
	if (nCardCount<5) return FALSE;          

	int nCardLay[SK_LAYOUT_NUM];
	memset(nCardLay,0,sizeof(nCardLay));
	int nJokerCount=0;
	PreDealCards(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount);
	int& index=CardDetail->nTypeCount;

	memset(&CardDetail->uniteType[index],0,sizeof(UNITE_TYPE));

	BOOL bnFind=FALSE;
	while(GetCardType_ABT_Single(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount,CardDetail->uniteType[index]))
	{
		//迭代自身最大值
		bnFind=TRUE;
	}
	if (!bnFind) return FALSE;
	if (CardDetail->uniteType[index].nCardCount!=nCardCount)	//此时牌数需要吻合，才能是此类型
		return FALSE;
// 	if (!GetCardType_ABT_Single(nCardID,nCardLay,nJokerCount,CardDetail->uniteType[index],5))
// 		return FALSE;

	index++;
	return TRUE;
}

BOOL  CGameTable::GetCardType_ABT_Single(int nCardIDs[],int nCardLen,int nCardLay[],int nLayLen,int nJokerCount,UNITE_TYPE& type,int nMaxCount)
{
	if (nCardLen<=0 || nLayLen<=0)
		return FALSE;

	if (nMaxCount<5)
		return FALSE;

	int nStartIndex=-1;
	int nValue=0;
	int nShunCount = 5;		//至少5张是顺子
	BOOL  bFixed = FALSE;
	if (type.dwCardType==CARD_UNITE_TYPE_ABT_SINGLE)
	{
		if (type.nCardCount<5)
			return FALSE;
		nShunCount = type.nCardCount;
		nValue=type.nMainValue;
		bFixed = TRUE;	//找固定长度
	}

	int nMinValue=-1;

	int nLength = 12;
	if (bFixed)	//固定长度	//不固定，则尽可能延续到A
		nLength = nShunCount;	
	for(int i=2;i<15-nShunCount;i++)
	{
		int abt_count=0;
		for(int j=0;j<nLength;j++)
		{
			if (i+j>13 || nCardLay[i+j]==0)
				break;
			abt_count++;
		}
		if ((bFixed&&abt_count+nJokerCount==nShunCount
			|| !bFixed&&abt_count+nJokerCount>=5)
			&& SK_GetIndexPRIEx(i,-1, 0)+100>nValue)
		{
			if (nMinValue==-1||SK_GetIndexPRIEx(i,GetCurrentRank(), 0)+100<nMinValue)
			{
				nStartIndex=i;
				nMinValue=SK_GetIndexPRIEx(i,GetCurrentRank(), 0)+100;
				nMaxCount=abt_count+nJokerCount;
			}
		}
	}

	if (nStartIndex==-1) return FALSE;

	type.dwCardType=CARD_UNITE_TYPE_ABT_SINGLE;
	type.dwComPareType= COMPARE_UNITE_TYPE_ABT_SINGLE; 
	type.nMainValue=nMinValue;//顺子中级牌还原
	type.nCardCount=nMaxCount;


	XygInitChairCards(type.nCardIDs,MAX_CARDS_PER_CHAIR);

	int temp[MAX_CARDS_PER_CHAIR];
	memset(temp,0,sizeof(temp));
	memcpy(temp,nCardIDs,sizeof(int)*nCardLen);
	
	{
		for(i=nStartIndex;i<nStartIndex+nMaxCount;i++)
		{
			PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,i,1);
		}
	}
	

	return TRUE;
}


BOOL  CGameTable::CaclCardType_ABT_Couple(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail)
{
	if (nCardLen<=0)
		return FALSE;

	if (CardDetail->nTypeCount>=MAX_FIT_TYPE) return FALSE;
	if (nCardCount<6) return FALSE;           //两队起连
	if (nCardCount%2!=0) return FALSE;

	int nCardLay[SK_LAYOUT_NUM];
	memset(nCardLay,0,sizeof(nCardLay));
	int nJokerCount=0;
	PreDealCards(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount);
	int& index=CardDetail->nTypeCount;

	
	memset(&CardDetail->uniteType[index],0,sizeof(UNITE_TYPE));
	if (!GetCardType_ABT_Couple(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount,CardDetail->uniteType[index],nCardCount/2))
		return FALSE;
    index++;
	return TRUE;
}

BOOL CGameTable::GetCardType_ABT_Couple(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type,int nMaxPair)
{
	if (nCardLen<=0 || nLayLen<=0)
		return FALSE;

	if (nMaxPair<3)
		return FALSE;

	int nStartIndex=-1;
	int nValue=0;
	if (type.dwCardType==CARD_UNITE_TYPE_ABT_COUPLE)
	{
		if (type.nCardCount!=nMaxPair*2)//张数不同不能比较
			return FALSE;

		nValue=type.nMainValue;
	}

	int nMinValue=-1;
	int nBaseValue=1000*nMaxPair;
	for(int i=2;i<=14-nMaxPair;i++)//3到
	{
		int Joker_Need=0;
		for(int j=0;j<nMaxPair;j++)//最多3连对
		{
			if (nCardLay[i+j]<2)
				Joker_Need+=2-nCardLay[i+j];
		}

		if (Joker_Need<=nJokerCount
			&&SK_GetIndexPRIEx(i,GetCurrentRank(), 0)+nBaseValue>nValue)
		{
			if (nMinValue==-1
				||SK_GetIndexPRIEx(i,GetCurrentRank(), 0)+nBaseValue<nMinValue)
			{
				nStartIndex=i;
				nMinValue=SK_GetIndexPRIEx(i,GetCurrentRank(), 0)+nBaseValue;
			}
		}
	}

	if (nStartIndex==-1) return FALSE;
	type.dwCardType=CARD_UNITE_TYPE_ABT_COUPLE;
	type.dwComPareType= COMPARE_UNITE_TYPE_ABT_COUPLE; 
	type.nMainValue=nMinValue;//顺子中级牌还原
	type.nCardCount=nMaxPair*2;
	
	XygInitChairCards(type.nCardIDs,MAX_CARDS_PER_CHAIR);

	int temp[MAX_CARDS_PER_CHAIR];
	memset(temp,0,sizeof(temp));
	memcpy(temp,nCardIDs,sizeof(int)*nCardLen);


    for(i=0;i<nMaxPair;i++)
		PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,nStartIndex+i,2);


	return TRUE;

}

BOOL    CGameTable::CaclCardType_ABT_Three_Couple(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail)
{
	if (nCardLen<=0)
		return FALSE;

	if (CardDetail->nTypeCount>=MAX_FIT_TYPE) return FALSE;
	if (nCardCount<10) return FALSE;           
	if (nCardCount%5!=0) return FALSE;
	
	int nCardLay[SK_LAYOUT_NUM];
	memset(nCardLay,0,sizeof(nCardLay));
	int nJokerCount=0;
	PreDealCards(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount);
	int& index=CardDetail->nTypeCount;
	
	memset(&CardDetail->uniteType[index],0,sizeof(UNITE_TYPE));
	if (!GetCardType_ABT_Three_Couple(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount,CardDetail->uniteType[index],nCardCount/5))
		return FALSE;

	index++;
	return TRUE;
}

BOOL   CGameTable::GetCardType_ABT_Three_Couple(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type,int nMaxPair)
{
	if (nCardLen<=0 || nLayLen<=0)
		return FALSE;

	if (nMaxPair<2)
		return FALSE;
	
	int nStartIndex=-1;
	int nValue=0;
	if (type.dwCardType==CARD_UNITE_TYPE_ABT_THREE_COUPLE)
	{
		if (type.nCardCount!=nMaxPair*5)//张数不同不能比较
			return FALSE;
		
		nValue=type.nMainValue;
	}

	//注意不支持财神
	int nTempLay[SK_LAYOUT_NUM];
	int nMinValue=-1;
	int nBaseValue=10000*nMaxPair;
	int nCoupleLay[SK_LAYOUT_NUM];
	int nCoupleLayTmp[SK_LAYOUT_NUM];
	int nCoupleLayTmpTmp[SK_LAYOUT_NUM];
	memset(nCoupleLay,0,sizeof(nCoupleLay));

	for(int i=2;i<=14-nMaxPair;i++)//3到K
	{
		memcpy(nTempLay,nCardLay,sizeof(nTempLay));
		int Joker_Need=0;
		for(int j=0;j<nMaxPair;j++)//最多nMaxPair连对
		{
			if (nCardLay[i+j]<3)
			{
				Joker_Need+=3-nCardLay[i+j];
				nTempLay[i+j]=0;
			}
			else
			{
				nTempLay[i+j]-=3;
			}
		}

		if (Joker_Need>nJokerCount) continue;
		
		int nCoupleCount = 0;
		memset(nCoupleLayTmp,0,sizeof(nCoupleLayTmp));
		for (int m=1;m<14;m++)
		{
			if (nTempLay[m]>=2)
			{
				nTempLay[m]-=2;
				nCoupleCount++;
				nCoupleLayTmp[m]++;
			}
		}

		/***********************************************************************
		* 当一共有nCoupleCount个对子，需要从中遍历nMaxPair个对子的所有组合
		* 利用位运算，遍历0～2的nCoupleCount次方中的数，如果该数的二进制有
		* nMaxPair个1，表示一种遍历情况
		************************************************************************/
		if (nCoupleCount>=nMaxPair)
		{
			//遍历所有对子组合,在若干对子中，取出nMaxPair个对子
			int nTotal = 1<<nCoupleCount;
			for (int j=3;j<nTotal;j++)
			{
				//有nMaxPair个对子
				if (GetBit1Count(j)==nMaxPair)
				{
					memset(nCoupleLayTmpTmp,0,sizeof(nCoupleLayTmpTmp));
					for (int k=0;k<nCoupleCount;k++)
					{
						//Lay中第k+1个数被随机组成nMaxPair个对子中的一对
						if ((j&(1<<k))!=0)
						{
							int nIndex = GetIndexByIndex(nCoupleLayTmp,SK_LAYOUT_NUM,k+1);
							if (nIndex>=0)
							{
								nCoupleLayTmpTmp[nIndex]++;
							}
						}
					}

					if(SK_GetIndexPRIEx(i,GetCurrentRank(), 0)*100+GetLayPri(nCoupleLayTmpTmp,SK_LAYOUT_NUM)+nBaseValue>nValue)
					{
						if (nMinValue==-1
							||SK_GetIndexPRIEx(i,GetCurrentRank(), 0)*100+GetLayPri(nCoupleLayTmpTmp,SK_LAYOUT_NUM)+nBaseValue<nMinValue)
						{
							nStartIndex=i;
							nMinValue=SK_GetIndexPRIEx(i,GetCurrentRank(), 0)*100+GetLayPri(nCoupleLayTmpTmp,SK_LAYOUT_NUM)+nBaseValue;
							memcpy(nCoupleLay,nCoupleLayTmpTmp,sizeof(nCoupleLay));
						}
					}
				}
			}
		}
	}

	if (nStartIndex==-1) return FALSE;

	type.dwCardType=CARD_UNITE_TYPE_ABT_THREE_COUPLE;
	type.dwComPareType= COMPARE_UNITE_TYPE_ABT_THREE_COUPLE; 
	type.nMainValue=nMinValue;
	type.nCardCount=nMaxPair*5;
	
	XygInitChairCards(type.nCardIDs,MAX_CARDS_PER_CHAIR);
	int temp[MAX_CARDS_PER_CHAIR];
	memset(temp,0,sizeof(temp));
	memcpy(temp,nCardIDs,sizeof(int)*nCardLen);
	
	for(i=0;i<nMaxPair;i++)
	{
		PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,nStartIndex+i,3);
	}

	for (i=0;i<SK_LAYOUT_NUM;i++)
	{
		if (nCoupleLay[i]>0)
		{
			PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,i,2);
		}
	}
	
	return TRUE;
}

int CGameTable::GetIndexByIndex(int nCardLay[], int nLayLen, int nIndex)
{
	if (nIndex<1) 
		return -1;

	for (int i=0;i<nLayLen;i++)
	{
		if (nCardLay[i]>0)
			nIndex--;
		if (nIndex<=0)
			return i;
	}
	return -1;
}

int CGameTable::GetBit1Count(unsigned int bit)
{
	int nCount = 0;
	while (bit>0)
	{
		nCount++;
		bit&=(bit-1);
	}
	return nCount;
}

int CGameTable::GetLayPri(int nCardLay[], int nLayLen)
{
	int nPri=0;
	for (int i=0;i<nLayLen;i++)
	{
		if (nCardLay[i]>0)
		{
			nPri += SK_GetIndexPRIEx(i,GetCurrentRank(), 0);
		}
	}

	return nPri;
}


int CGameTable::GetLayPriEx(int nCardLay[], int nLayLen)
{
	int nPri=0;
	for (int i=0;i<nLayLen;i++)
	{
		if (nCardLay[i]>=0)
		{
			nPri += SK_GetIndexPRIEx(nCardLay[i],GetCurrentRank(), 0);
		}
	}
	
	return nPri;
}

BOOL    CGameTable::CaclCardType_ABT_Three_1(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail)
{
	if (nCardLen<=0)
		return FALSE;

	if (CardDetail->nTypeCount>=MAX_FIT_TYPE) return FALSE;
	if (nCardCount<8) return FALSE;           
	if (nCardCount%4!=0) return FALSE;
	
	int nCardLay[SK_LAYOUT_NUM];
	memset(nCardLay,0,sizeof(nCardLay));
	int nJokerCount=0;
	PreDealCards(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount);
	int& index=CardDetail->nTypeCount;
	
	memset(&CardDetail->uniteType[index],0,sizeof(UNITE_TYPE));
	if (!GetCardType_ABT_Three_1(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount,CardDetail->uniteType[index],nCardCount/4))
		return FALSE;

	index++;
	return TRUE;
}

BOOL   CGameTable::GetCardType_ABT_Three_1(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type,int nMaxPair)
{
	if (nCardLen<=0 || nLayLen<=0)
		return FALSE;

	if (nMaxPair<2)
		return FALSE;
	
	int nStartIndex=-1;
	int nValue=0;
	if (type.dwCardType==CARD_UNITE_TYPE_ABT_THREE_1)
	{
		if (type.nCardCount!=nMaxPair*4)//张数不同不能比较
			return FALSE;
		
		nValue=type.nMainValue;
	}

	//注意不支持财神
	int nTempLay[SK_LAYOUT_NUM];
	int nMinValue=-1;
	int nThisValue = 0;
	int nBaseValue=10000*nMaxPair;
	int nSingleLay[CHAIR_CARDS];
	int nSingleLayTmp[CHAIR_CARDS];
	int nSingleLayTmpTmp[CHAIR_CARDS];
	memset(nSingleLay,-1,sizeof(nSingleLay));

	for(int i=2;i<=14-nMaxPair;i++)//3到K
	{
		memcpy(nTempLay,nCardLay,sizeof(nTempLay));
		int Joker_Need=0;
		for(int j=0;j<nMaxPair;j++)//最多nMaxPair连对
		{
			if (nCardLay[i+j]<3)
			{
				Joker_Need+=3-nCardLay[i+j];
			}
			nTempLay[i+j]=0;
		}
		
		if (Joker_Need>nJokerCount) continue;
		
		int nSingleCount = 0;
		memset(nSingleLayTmp,-1,sizeof(nSingleLayTmp));
		for (int m=0;m<nLayLen;m++)
		{
			for (int j=0;j<nTempLay[m];j++)
			{
				nSingleLayTmp[nSingleCount++]=m;
			}
			nTempLay[m]=0;
		}

		/***********************************************************************
		* 当一共有nSingleCount个单张，需要从中遍历nMaxPair个单张的所有组合
		* 利用位运算，遍历0～2的nSingleCount次方中的数，如果该数的二进制有
		* nMaxPair个1，表示一种遍历情况
		************************************************************************/
		if (nSingleCount>=nMaxPair)
		{
			//遍历所有对子组合,在若干对子中，取出nMaxPair个对子
			int nTotal = 1<<nSingleCount;
			for (int j=3;j<nTotal;j++)
			{
				//有nMaxPair个对子
				if (GetBit1Count(j)==nMaxPair)
				{
					memset(nSingleLayTmpTmp,-1,sizeof(nSingleLayTmpTmp));
					for (int k=0;k<nSingleCount;k++)
					{
						//nSingleLayTmp中第k个数被随机组成nMaxPair个对子中的一对
						if ((j&(1<<k))!=0)
						{
							nSingleLayTmpTmp[k] = nSingleLayTmp[k];
						}
					}

					nThisValue =  SK_GetIndexPRIEx(i,GetCurrentRank(), 0)*100+GetLayPriEx(nSingleLayTmpTmp,SK_LAYOUT_NUM)+nBaseValue;
					if(nThisValue>nValue)
					{
						if (nMinValue==-1
							||nThisValue<nMinValue)
						{
							nStartIndex=i;
							nMinValue=nThisValue;
							memcpy(nSingleLay,nSingleLayTmpTmp,sizeof(nSingleLay));
						}
					}
				}
			}
		}
	}

	if (nStartIndex==-1) return FALSE;

	type.dwCardType=CARD_UNITE_TYPE_ABT_THREE_1;
	type.dwComPareType= COMPARE_UNITE_TYPE_ABT_THREE_1; 
	type.nMainValue=nMinValue;
	type.nCardCount=nMaxPair*4;
	
	XygInitChairCards(type.nCardIDs,MAX_CARDS_PER_CHAIR);
	int temp[MAX_CARDS_PER_CHAIR];
	memset(temp,0,sizeof(temp));
	memcpy(temp,nCardIDs,sizeof(int)*nCardLen);
	
	for(i=0;i<nMaxPair;i++)
	{
		PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,nStartIndex+i,3);
	}

	for (i=0;i<CHAIR_CARDS;i++)
	{
		if (nSingleLay[i]>=0)
		{
			PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,nSingleLay[i],1);
		}
	}
	
	return TRUE;
}


BOOL    CGameTable::CalcCardType_Four_2(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail)
{
	if (nCardLen<=0)
		return FALSE;

	if (CardDetail->nTypeCount>=MAX_FIT_TYPE) return FALSE;
	if (nCardCount!=6) return FALSE;           
	
	int nCardLay[SK_LAYOUT_NUM];
	memset(nCardLay,0,sizeof(nCardLay));
	int nJokerCount=0;
	PreDealCards(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount);
	int& index=CardDetail->nTypeCount;
	
	memset(&CardDetail->uniteType[index],0,sizeof(UNITE_TYPE));
	if (!GetCardType_Four_2(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount,CardDetail->uniteType[index]))
		return FALSE;
	if (CardDetail->uniteType[index].nCardCount!=nCardCount)	//此时牌数需要吻合，才能是此类型
		return FALSE;

	index++;
	return TRUE;
}

BOOL   CGameTable::GetCardType_Four_2(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type)
{
	if (nCardLen<=0 || nLayLen<=0)
		return FALSE;

	int nMaxPair=2;	//需要2个单张
	
	int nStartIndex=-1;

	int nValue=0;
	if (type.dwCardType==CARD_UNITE_TYPE_FOUR_2)
	{
		if (type.nCardCount!=6)//张数不同不能比较
			return FALSE;
		
		nValue=type.nMainValue;
	}

	//注意不支持财神
	int nTempLay[SK_LAYOUT_NUM];
	int nMinValue=-1;
	int nThisValue = 0;
	int nBaseValue=100000;
	int nSingleLay[CHAIR_CARDS];
	int nSingleLayTmp[CHAIR_CARDS];
	int nSingleLayTmpTmp[CHAIR_CARDS];
	memset(nSingleLay,-1,sizeof(nSingleLay));

	for(int i=1;i<14;i++)//2到A
	{
		if (nCardLay[i]<4) continue;

		memcpy(nTempLay,nCardLay,sizeof(nTempLay));
		nTempLay[i]-=4;
		
		int nSingleCount = 0;
		memset(nSingleLayTmp,-1,sizeof(nSingleLayTmp));
		for (int m=0;m<nLayLen;m++)
		{
			for (int j=0;j<nTempLay[m];j++)
			{
				nSingleLayTmp[nSingleCount++]=m;
			}
			nTempLay[m]=0;
		}

		/***********************************************************************
		* 当一共有nCoupleCount个对子，需要从中遍历nMaxPair个对子的所有组合
		* 利用位运算，遍历0～2的nCoupleCount次方中的数，如果该数的二进制有
		* nMaxPair个1，表示一种遍历情况
		************************************************************************/
		if (nSingleCount>=nMaxPair)
		{
			//遍历所有对子组合,在若干对子中，取出nMaxPair个对子
			int nTotal = 1<<nSingleCount;
			for (int j=3;j<nTotal;j++)
			{
				//有nMaxPair个对子
				if (GetBit1Count(j)==nMaxPair)
				{
					memset(nSingleLayTmpTmp,-1,sizeof(nSingleLayTmpTmp));
					CString tmp;
					for (int k=0;k<nSingleCount;k++)
					{
						//nSingleLayTmp中第k个数被随机组成nMaxPair个对子中的一对
						if ((j&(1<<k))!=0)
						{
							nSingleLayTmpTmp[k] = nSingleLayTmp[k];
						}
					}	
					
					nThisValue = SK_GetIndexPRIEx(i,GetCurrentRank(), 0)*100+GetLayPriEx(nSingleLayTmpTmp,SK_LAYOUT_NUM)+nBaseValue;
					if(nThisValue>nValue)
					{
						if (nMinValue==-1
							||nThisValue<nMinValue)
						{
							nStartIndex=i;
							nMinValue=nThisValue;
							memcpy(nSingleLay,nSingleLayTmpTmp,sizeof(nSingleLay));
						}
					}
				}
			}
		}
	}

	if (nStartIndex==-1) return FALSE;

	type.dwCardType=CARD_UNITE_TYPE_FOUR_2;
	type.dwComPareType= COMPARE_UNITE_TYPE_FOUR_2; 
	type.nMainValue=nMinValue;
	type.nCardCount=6;
	
	XygInitChairCards(type.nCardIDs,MAX_CARDS_PER_CHAIR);
	int temp[MAX_CARDS_PER_CHAIR];
	memset(temp,0,sizeof(temp));
	memcpy(temp,nCardIDs,sizeof(int)*nCardLen);
	
	PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,nStartIndex,4);

	for (i=0;i<CHAIR_CARDS;i++)
	{
		if (nSingleLay[i]>=0)
		{
			PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,nSingleLay[i],1);
		}
	}
	
	return TRUE;
}


BOOL    CGameTable::CalcCardType_Four_2_Couple(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail)
{
	if (nCardLen<=0)
		return FALSE;

	if (CardDetail->nTypeCount>=MAX_FIT_TYPE) return FALSE;
	if (nCardCount!=8) return FALSE;           
	
	int nCardLay[SK_LAYOUT_NUM];
	memset(nCardLay,0,sizeof(nCardLay));
	int nJokerCount=0;
	PreDealCards(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount);
	int& index=CardDetail->nTypeCount;
	
	memset(&CardDetail->uniteType[index],0,sizeof(UNITE_TYPE));
	if (!GetCardType_Four_2_Couple(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount,CardDetail->uniteType[index]))
		return FALSE;
	if (CardDetail->uniteType[index].nCardCount!=nCardCount)	//此时牌数需要吻合，才能是此类型
		return FALSE;

	index++;
	return TRUE;
}

BOOL   CGameTable::GetCardType_Four_2_Couple(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type)
{
	if (nCardLen<=0 || nLayLen<=0)
		return FALSE;

	int nMaxPair=2;	//需要2个对子
	
	int nStartIndex=-1;

	int nValue=0;
	if (type.dwCardType==CARD_UNITE_TYPE_FOUR_2_COUPLE)
	{
		if (type.nCardCount!=8)//张数不同不能比较
			return FALSE;
		
		nValue=type.nMainValue;
	}

	//注意不支持财神
	int nTempLay[SK_LAYOUT_NUM];
	int nMinValue=-1;
	int nBaseValue=100000;
	int nCoupleLay[SK_LAYOUT_NUM];
	int nCoupleLayTmp[SK_LAYOUT_NUM];
	int nCoupleLayTmpTmp[SK_LAYOUT_NUM];
	memset(nCoupleLay,0,sizeof(nCoupleLay));

	for(int i=1;i<14;i++)//2到A
	{
		if (nCardLay[i]<4) continue;

		memcpy(nTempLay,nCardLay,sizeof(nTempLay));
		nTempLay[i]-=4;
		
		int nCoupleCount = 0;
		memset(nCoupleLayTmp,0,sizeof(nCoupleLayTmp));
		for (int m=1;m<14;m++)
		{
			if (nTempLay[m]>=2)
			{
				nTempLay[m]-=2;
				nCoupleCount++;
				nCoupleLayTmp[m]++;
			}
		}

		/***********************************************************************
		* 当一共有nCoupleCount个对子，需要从中遍历nMaxPair个对子的所有组合
		* 利用位运算，遍历0～2的nCoupleCount次方中的数，如果该数的二进制有
		* nMaxPair个1，表示一种遍历情况
		************************************************************************/
		if (nCoupleCount>=nMaxPair)
		{
			//遍历所有对子组合,在若干对子中，取出nMaxPair个对子
			int nTotal = 1<<nCoupleCount;
			for (int j=3;j<nTotal;j++)
			{
				//有nMaxPair个对子
				if (GetBit1Count(j)==nMaxPair)
				{
					memset(nCoupleLayTmpTmp,0,sizeof(nCoupleLayTmpTmp));
					for (int k=0;k<nCoupleCount;k++)
					{
						//Lay中第k+1个数被随机组成nMaxPair个对子中的一对
						if ((j&(1<<k))!=0)
						{
							int nIndex = GetIndexByIndex(nCoupleLayTmp,SK_LAYOUT_NUM,k+1);
							if (nIndex>=0)
							{
								nCoupleLayTmpTmp[nIndex]++;
							}
						}
					}

					if(SK_GetIndexPRIEx(i,GetCurrentRank(), 0)*100+GetLayPri(nCoupleLayTmpTmp,SK_LAYOUT_NUM)+nBaseValue>nValue)
					{
						if (nMinValue==-1
							||SK_GetIndexPRIEx(i,GetCurrentRank(), 0)*100+GetLayPri(nCoupleLayTmpTmp,SK_LAYOUT_NUM)+nBaseValue<nMinValue)
						{
							nStartIndex=i;
							nMinValue=SK_GetIndexPRIEx(i,GetCurrentRank(), 0)*100+GetLayPri(nCoupleLayTmpTmp,SK_LAYOUT_NUM)+nBaseValue;
							memcpy(nCoupleLay,nCoupleLayTmpTmp,sizeof(nCoupleLay));
						}
					}
				}
			}
		}
	}

	if (nStartIndex==-1) return FALSE;

	type.dwCardType=CARD_UNITE_TYPE_FOUR_2_COUPLE;
	type.dwComPareType= COMPARE_UNITE_TYPE_FOUR_2_COUPLE; 
	type.nMainValue=nMinValue;
	type.nCardCount=8;
	
	XygInitChairCards(type.nCardIDs,MAX_CARDS_PER_CHAIR);
	int temp[MAX_CARDS_PER_CHAIR];
	memset(temp,0,sizeof(temp));
	memcpy(temp,nCardIDs,sizeof(int)*nCardLen);
	
	PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,nStartIndex,4);

	for (i=0;i<SK_LAYOUT_NUM;i++)
	{
		if (nCoupleLay[i]>0)
		{
			PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,i,2);
		}
	}
	
	return TRUE;
}

BOOL  CGameTable::CalcCardType_BOMB_2King(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail)
{
	if (nCardLen<=0)
		return FALSE;
	
	if (CardDetail->nTypeCount>=MAX_FIT_TYPE) return FALSE;
	if (nCardCount!=2) return FALSE;           //必须是2张大小王
	int nCardLay[SK_LAYOUT_NUM];
	memset(nCardLay,0,sizeof(nCardLay));
	int nJokerCount=0;
	PreDealCards(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount);
	
	int& index=CardDetail->nTypeCount;
	memset(&CardDetail->uniteType[index],0,sizeof(UNITE_TYPE));
	if (!GetCardType_BOMB_2King(nCardIDs,nCardLen,nCardLay,SK_LAYOUT_NUM,nJokerCount,CardDetail->uniteType[index]))
		return FALSE;
	
	index++;
	return TRUE;
}

BOOL CGameTable::GetCardType_BOMB_2King(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type)
{
	if (nCardLen<=0 || nLayLen<=0)
		return FALSE;
	
	if (nCardLay[14]!=1  //小王			
		||nCardLay[15]!=1) //大王
		return FALSE;
	
	if (type.dwCardType==CARD_UNITE_TYPE_2KING
		&&type.nMainValue>0)
		return FALSE;
	
	type.dwCardType=CARD_UNITE_TYPE_2KING;
	type.dwComPareType=  COMPARE_UNITE_TYPE_2KING;
	type.nMainValue=1;//唯一
	type.nCardCount=2;
	
	int temp[MAX_CARDS_PER_CHAIR];
	memset(temp,0,sizeof(temp));
	memcpy(temp,nCardIDs,sizeof(int)*nCardLen);
	XygInitChairCards(type.nCardIDs,MAX_CARDS_PER_CHAIR);
	PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,14,1);//大王
	PutCardToArray(type.nCardIDs,nCardLen,temp,nCardLen,15,1);//小王
	
	return TRUE;
	
}

BOOL CGameTable::IsGameMsg(UINT resquesID)
{
	if (resquesID>SYSMSG_BEGIN&&resquesID<SYSMSG_END)
		return TRUE;
	if (resquesID>LOCAL_GAME_MSG_BEGIN&&resquesID<LOCAL_GAME_MSG_END)
		return TRUE;
	
	if (resquesID>TCSG_GAME_MSG_BEGIN&&resquesID<TCSG_GAME_MSG_END)
		return TRUE;
	if (resquesID == TCSG_S2C_NEWTABLE_GRADE
		|| resquesID == TCSG_C2S_NEWTABLE_EX
		|| resquesID == TCSG_C2S_REQTABLE_GRADE)
	{
		return TRUE;
	}

	return FALSE;
}

void CGameTable::ActuallizeResults(void* pData, int nLen)
{
	CTable::ActuallizeResults(pData,nLen);

	//记录每局结果
	int nRecordIndex = m_nBoutCount-1;
	nRecordIndex = (m_nBoutCount-1)%MAX_RESULT_COUNT;
	if(m_nBoutCount>0)
	{
		GAME_WIN* pGameWinEx=(GAME_WIN*)pData;
		if(m_nBaseDeposit)
		{
			//银子房间记录银子得失
			for(int i=0;i<TOTAL_CHAIRS;i++){			
				m_nResultDiff[i][nRecordIndex]=pGameWinEx->nDepositDiffs[i];
				m_nTotalResult[i]+=pGameWinEx->nDepositDiffs[i];
			}
		}
		else
		{
			//记录分得失
			for(int i=0;i<TOTAL_CHAIRS;i++){			
				m_nResultDiff[i][nRecordIndex]=pGameWinEx->nScoreDiffs[i];
				m_nTotalResult[i]+=pGameWinEx->nScoreDiffs[i];
			}
		}		
	}
}

void CGameTable::OnPlayerPassiveEvent(int chairno)
{
	if (!ValidateChair(chairno))
		return;
	m_nAutoPlayCount[chairno]++;
}

void CGameTable::OnPlayerActiveEvent(int chairno)
{
	if (!ValidateChair(chairno))
		return;
	m_nAutoPlayCount[chairno] = 0;
}

BOOL CGameTable::IsTooManyAutoPlay(int chairno)
{
	if (!ValidateChair(chairno))
		return FALSE;

	if (m_nAutoPlayCount[chairno]>=MAX_AUTO_COUNT)
		return TRUE;
	else
		return FALSE;
}

//逃跑倍数加上炸弹
int CGameTable::CalcDoubleOfScore(int chairno, int breakchair, int defdouble)
{
	if (m_nObjectGains>0)
	{
		defdouble*=m_nObjectGains*UwlPow2(GetPublicInfo()->nBombFan);
	}
	
	return CTable::CalcDoubleOfScore(chairno,breakchair,defdouble);
}

int CGameTable::CalcBreakDeposit(int breakchair, int breakdouble, int& cut)
{	
	if (m_nObjectGains>0)
	{
		breakdouble*=m_nObjectGains*UwlPow2(GetPublicInfo()->nBombFan);
	}
	
	return CTable::CalcBreakDeposit(breakchair,breakdouble,cut);
}


BOOL  CGameTable::GetBestUnitType(UNITE_TYPE& first_card,CARD_UNITE& fight_card)
{
    DWORD dwDestType=first_card.dwCardType;
	DWORD dwDestMain=first_card.nMainValue;
	DWORD dwDestCompareType=fight_card.uniteType[0].dwComPareType;
	int nCardIDs[MAX_CARDS_PER_CHAIR];
	XygInitChairCards(nCardIDs,MAX_CARDS_PER_CHAIR);
	
	BOOL bnFindBig=FALSE;
	for(int i=0;i<fight_card.nTypeCount;i++)
	{
		if (IS_BIT_SET(fight_card.uniteType[i].dwComPareType,dwDestType))
		{
			if (fight_card.uniteType[i].dwCardType==dwDestType)
			{
				if (fight_card.uniteType[i].nMainValue>dwDestMain)
				{
					if ((  dwDestType == CARD_UNITE_TYPE_ABT_SINGLE
						|| dwDestType == CARD_UNITE_TYPE_ABT_COUPLE
						|| dwDestType == CARD_UNITE_TYPE_ABT_THREE
						|| dwDestType == CARD_UNITE_TYPE_ABT_THREE_1
						|| dwDestType == CARD_UNITE_TYPE_ABT_THREE_COUPLE)
						&& fight_card.uniteType[i].nCardCount!=first_card.nCardCount)
					{
						//去掉这5种类型，牌不相等的情况
					}
					else
					{
						bnFindBig=TRUE;
						dwDestType=fight_card.uniteType[i].dwCardType;
						dwDestMain=fight_card.uniteType[i].nMainValue;
						dwDestCompareType=fight_card.uniteType[i].dwComPareType;
						memcpy(nCardIDs,fight_card.uniteType[i].nCardIDs,sizeof(int)*MAX_CARDS_PER_CHAIR);
					}
				}
			}
			else
			{
				bnFindBig=TRUE;
				dwDestType=fight_card.uniteType[i].dwCardType;
				dwDestMain=fight_card.uniteType[i].nMainValue;
				dwDestCompareType=fight_card.uniteType[i].dwComPareType;
				memcpy(nCardIDs,fight_card.uniteType[i].nCardIDs,sizeof(int)*MAX_CARDS_PER_CHAIR);
			}
		}	
	}
	
	if (bnFindBig)
	{
		fight_card.uniteType[0].dwCardType=dwDestType;
		fight_card.uniteType[0].nMainValue=dwDestMain;
		fight_card.uniteType[0].dwComPareType=dwDestCompareType;
		memcpy(fight_card.uniteType[0].nCardIDs,nCardIDs,sizeof(int)*MAX_CARDS_PER_CHAIR);
		
		fight_card.nTypeCount=1;
		return TRUE;
	}
	else
		return FALSE;
}


BOOL  CGameTable::GetBestUnitType(CARD_UNITE& fight_card)
{
	if (fight_card.nTypeCount<=1)
		return TRUE;
	
    DWORD dwDestType=fight_card.uniteType[0].dwCardType;
	DWORD dwDestMain=fight_card.uniteType[0].nMainValue;
	DWORD dwDestCompareType=fight_card.uniteType[0].dwComPareType;
	int nCardIDs[MAX_CARDS_PER_CHAIR];
	XygInitChairCards(nCardIDs,MAX_CARDS_PER_CHAIR);
	
	BOOL bnFindBig=FALSE;
	for(int i=1;i<fight_card.nTypeCount;i++)
	{
		if (IS_BIT_SET(fight_card.uniteType[i].dwComPareType,dwDestType))
		{
			if (fight_card.uniteType[i].dwCardType==dwDestType)
			{
				if (fight_card.uniteType[i].nMainValue>dwDestMain)
				{
					bnFindBig=TRUE;
					dwDestType=fight_card.uniteType[i].dwCardType;
					dwDestMain=fight_card.uniteType[i].nMainValue;
					dwDestCompareType=fight_card.uniteType[i].dwComPareType;
					memcpy(nCardIDs,fight_card.uniteType[i].nCardIDs,sizeof(int)*MAX_CARDS_PER_CHAIR);
				}
			}
			else
			{
				bnFindBig=TRUE;
				dwDestType=fight_card.uniteType[i].dwCardType;
				dwDestMain=fight_card.uniteType[i].nMainValue;
				dwDestCompareType=fight_card.uniteType[i].dwComPareType;
				memcpy(nCardIDs,fight_card.uniteType[i].nCardIDs,sizeof(int)*MAX_CARDS_PER_CHAIR);
			}
		}	
	}
	
	if (bnFindBig)
	{
		fight_card.uniteType[0].dwCardType=dwDestType;
		fight_card.uniteType[0].nMainValue=dwDestMain;
		fight_card.uniteType[0].dwComPareType=dwDestCompareType;
		memcpy(fight_card.uniteType[0].nCardIDs,nCardIDs,sizeof(int)*MAX_CARDS_PER_CHAIR);
		fight_card.nTypeCount=1;
	}
	return TRUE;
}

//发起组牌
int  CGameTable::OnZuPai(ZUPAI* pZuPai)
{
	if (GetPublicInfo()->bFinishZuPai[pZuPai->nChairNO])
	{
		return FALSE;
	}

	SANGONG_DETAIL& detail = GetPlayerInfo(pZuPai->nChairNO)->stDetail;
	for(int i=0;i<CHAIR_CARDS;i++)
	{
		pZuPai->nCardID[i]=detail.stCardInfo[i].nCardID;
		CARDINFO* card=GetCard(pZuPai->nCardID[i]);
		if (card) 
			card->nCardStatus=CARD_STATUS_THROWDOWN;//代表已组牌
	}
	pZuPai->nCardType=detail.nCardType;
	GetPublicInfo()->bFinishZuPai[pZuPai->nChairNO]=TRUE;
	
	return TRUE;
}

BOOL CGameTable::AllZuPai()
{
	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		if (!m_bInGame[i]) 
		{
			continue;
		}

		if (!GetPublicInfo()->bFinishZuPai[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}

int CGameTable::CalcMaxCard(CARDINFO stCardInfo[], int nLen)
{
	CARDINFO	stMaxCardInfo;
	stMaxCardInfo = stCardInfo[0];
	int nMaxCardIndex = 0;

	for (int i=0;i<CHAIR_CARDS;i++)
	{
		int nCardTempValue = SK_GetCardIndexEx(stMaxCardInfo.nCardID,0);
		int nCardValue = SK_GetCardIndexEx(stCardInfo[i].nCardID,0);
		if (nCardTempValue == 13)
		{
			nCardTempValue = -1;
		}
		if (nCardValue == 13) 
		{
			nCardValue = -1;
		}
		if (nCardTempValue < nCardValue)
		{
			stMaxCardInfo = stCardInfo[i];
			nMaxCardIndex = i;
		}
		else if (nCardTempValue == nCardValue)
		{
			int nCardTempShpae = SK_GetCardShapeEx(stMaxCardInfo.nCardID,0);
			int nCardShpae = SK_GetCardShapeEx(stCardInfo[i].nCardID,0);
			if (nCardTempShpae < nCardShpae) 
			{
				stMaxCardInfo = stCardInfo[i];
				nMaxCardIndex = i;
			}
		}
	}

	return nMaxCardIndex;
}

int CGameTable::CalcMaxCard(int nCard[], int nLen)
{
	int nCardTemp = nCard[0];
	int nMaxCardIndex = 0;

	for (int i=0;i<CHAIR_CARDS;i++)
	{
		int nCardTempValue = SK_GetCardValueEx(nCardTemp,0);
		int nCardValue = SK_GetCardValueEx(nCard[i],0);
		if (nCardTempValue == 13)
		{
			nCardTempValue = -1;
		}
		if (nCardValue == 13) 
		{
			nCardValue = -1;
		}
		if (nCardTempValue < nCardValue)
		{
			nCardTemp = nCard[i];
			nMaxCardIndex = i;
		}
		else if (nCardTempValue == nCardValue)
		{
			int nCardTempShpae = SK_GetCardShapeEx(nCardTemp,0);
			int nCardShpae = SK_GetCardShapeEx(nCard[i],0);
			if (nCardTempShpae < nCardShpae) 
			{
				nCardTemp = nCard[i];
				nMaxCardIndex = i;
			}
		}
	}

	return nMaxCardIndex;
}

BOOL CGameTable::IsNeedCountdown()
{
	return TRUE;
}

int CGameTable::CalcBetDeposit()
{
	int nMinDeposit = 0;
	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		if (m_bInGame[i])
		{
			nMinDeposit = m_ptrPlayers[i]->m_nDeposit;
			break;
		}
	}
	for (i=0;i<TOTAL_CHAIRS;i++)
	{
		if (m_bInGame[i])
		{
			if (nMinDeposit > m_ptrPlayers[i]->m_nDeposit)
			 {
				 nMinDeposit = m_ptrPlayers[i]->m_nDeposit;
			 }
		}
	}

	//return nMinDeposit/10; //疯狂牛牛

	//牛牛
	if(nMinDeposit <= 100){
		nMinDeposit = 100;
	}
// 	nMinDeposit /= 100;
// 	nMinDeposit = UwlLog2(nMinDeposit);
// 	nMinDeposit = UwlPow2(nMinDeposit);
	//
	
	int nStartCount = XygGetStartCount(m_dwUserStatus, m_nTotalChairs);

	if(2 == nStartCount)
		nMinDeposit /= 40;
	else if (3 == nStartCount)
		nMinDeposit /= 80;
	else if (4 == nStartCount)
		nMinDeposit /= 160;

	if(nMinDeposit <= 0)
		nMinDeposit = 1;

	return nMinDeposit;
}

int CGameTable::CalcPlayerBaseDeposit(int chairno,int nBanker)
{
	if (!m_ptrPlayers[chairno]||!m_ptrPlayers[nBanker])
		return m_nBaseDeposit;
	
	int mindeposit=m_ptrPlayers[chairno]->m_nDeposit;
	if (mindeposit>m_ptrPlayers[nBanker]->m_nDeposit)
		mindeposit=m_ptrPlayers[nBanker]->m_nDeposit;
	
	if(mindeposit <= 100)
	{
		mindeposit = 100;
	}

	//最小携带的1%
	mindeposit /= 100;

	int nStartCount = XygGetStartCount(m_dwUserStatus, m_nTotalChairs);
	
	if(2 == nStartCount)
		mindeposit = (int)(mindeposit*1.5);

	if (mindeposit <= 0)
		mindeposit = 1;

	if (m_nDepositMult <=0)//增加可配置倍数
	{
		m_nDepositMult = 1;
	}
	return mindeposit * m_nDepositMult;
}

int CGameTable::CalcBaseDeposit(int nDeposits[], int tableno)
{
	assert(m_nTotalChairs <= MAX_CHAIRS_PER_TABLE);

	int nBase = 0;

	int mindeposit = 0;
	for(int i = 1; i < m_nTotalChairs; i++)
	{
		if (m_ptrPlayers[i] && !m_ptrPlayers[i]->m_bIdlePlayer)
		{
			mindeposit = nDeposits[i];
			break;
		}
	}
	
	for(i = 1; i < m_nTotalChairs; i++)
	{
		if (m_ptrPlayers[i] && !m_ptrPlayers[i]->m_bIdlePlayer)
		{
			if(nDeposits[i] < mindeposit)
			{
				mindeposit = nDeposits[i];
			}
		}
	}
	if(mindeposit <= 100){
		mindeposit = 100;
	}
	mindeposit /= 100;
	mindeposit = UwlLog2(mindeposit);

	nBase = UwlPow2(mindeposit);

	nBase *= (tableno / 10 + 1);

	return nBase;
}

int CGameTable::CalcOpeTime()
{
	if (IS_BIT_SET(m_dwStatus, TS_WAITING_QZ))
	{
		return m_nQZTime;
	}
	else if (IS_BIT_SET(m_dwStatus, TS_WAITING_YAZHU))
	{
		return m_nYaZhuTime;
	}
	else if (IS_BIT_SET(m_dwStatus, TS_WAITING_ZUPAI))
	{
		return m_nOperateTime;
	}

	return m_nOperateTime;
}

BOOL CGameTable::IsAllAllowExit(int askChairNo)
{
	//return CSkTable::IsAllAllowExit(askChairNo);

	for(int i = 0; i < m_nTotalChairs; i++)
	{
		if (!m_bInGame[i]) continue;

		if(askChairNo!=i	
			&&0 == m_dwBreakTime[i]
			&&m_bAllowExit[askChairNo][i]==FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}

DWORD CGameTable::SetStatusOnFinishQZ()
{
	RemoveStatus(TS_WAITING_QZ);
	m_dwActionYaZhu = GetTickCount();
	return AddStatus(TS_WAITING_YAZHU);
}

DWORD CGameTable::SetStatusOnFinishYaZhu()
{
	RemoveStatus(TS_WAITING_YAZHU);
	m_dwActionDealCard = GetTickCount();
	return AddStatus(TS_WAITING_ZUPAI);
}

void CGameTable::DealCardAfterYaZhu()
{
	CString strIniFile = GetINIFileName();
	TCHAR szRoomID[16];
	memset(szRoomID, 0, sizeof(szRoomID));
	_stprintf(szRoomID, _T("%ld"), m_nRoomID);

	GetPublicInfo()->nCurrentCatch=0;

	//初始化所有牌的信息
	int card[TOTAL_CARDS];
	for(int i=0;i<TOTAL_CARDS;i++)
		card[i]=i;
	
	CPlayer* ptrP = NULL;
	for (i=0;i<TOTAL_CHAIRS;i++)
	{
		if (m_ptrPlayers[i] && !m_ptrPlayers[i]->m_bIdlePlayer) 
		{
			ptrP = m_ptrPlayers[i];
			break;
		}
	}			
	SvrXygRandomSort(card,TOTAL_CARDS,GetTickCount()+ptrP->m_lTokenID * 10 + ptrP->m_hSocket);
	
	GAME_PUBLIC_INFO* pPublicInfo=GetPublicInfo();
	for(i=0;i<TOTAL_CARDS;i++)
	{
		
		pPublicInfo->GameCard[i].nCardID=card[i];
		pPublicInfo->GameCard[i].nCardIndex=SK_GetCardIndexEx(card[i],0);
		pPublicInfo->GameCard[i].nShape=SK_GetCardShapeEx(card[i],0);
		pPublicInfo->GameCard[i].nValue=SK_GetCardValueEx(card[i],0);
		pPublicInfo->GameCard[i].nCardStatus=CARD_STATUS_WAITDEAL;
		pPublicInfo->GameCard[i].nChairNO=-1;
		pPublicInfo->GameCard[i].nPositionIndex=i;
		pPublicInfo->GameCard[i].nUniteCount=0;
	}

#ifdef _DEBUG
	DWORD dwRead = GetPrivateProfileInt(_T("StartDeal"), _T("ReadCardsFromFile"), 0, strIniFile);
	if (dwRead)
	{
		ReadCardsFromFile();
	}
#endif

	DealCardNormal();

	m_dwActionDealCard = GetTickCount();
}

void CGameTable::FillupHandCardAfterYaZhu(LPYAZHU pYaZhu)
{
	if (!pYaZhu) return;

	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		if (!m_bInGame[i]) continue;

		XygInitChairCards(pYaZhu->nCardID[i],CHAIR_CARDS);
		GetInHandCard(i,pYaZhu->nCardID[i]);
	}
}

BOOL CGameTable::ValidateBetDbl(int nDbl)
{
	if (5 == nDbl || 7 == nDbl || 10 == nDbl)
	{
		return TRUE;
	}

	return FALSE;
}

int CGameTable::GetWinGains(SANGONG_DETAIL& chairDtl, SANGONG_DETAIL& bankerDtl)
{
	int nDbl=1;

	BOOL bBankerWin=FALSE;
	if (chairDtl.nCardType < bankerDtl.nCardType)
	{
		bBankerWin=TRUE;
	}
	else if (chairDtl.nCardType == bankerDtl.nCardType)
	{
		//坐庄场庄家和闲家都是0点，那么就是庄家赢
		if(chairDtl.nCardType==CARD_TYPE_POINT0 && !m_bTongBiRoom)
		{
			bBankerWin=TRUE;
		}
		else
		{
			int chairIndex=chairDtl.stCardInfo[chairDtl.nMaxCardIndex].nCardIndex;
			int bankerIndex=bankerDtl.stCardInfo[bankerDtl.nMaxCardIndex].nCardIndex;
			if (chairIndex == 13)
			{//A
				chairIndex = -1;
			}
			if (bankerIndex == 13)
			{
				bankerIndex = -1;
			}
			
			if (chairIndex < bankerIndex)
			{
				bBankerWin = TRUE;
			}
			else if(chairIndex > bankerIndex)
			{
				bBankerWin = FALSE;
			}
			else
			{
				int chairShape=chairDtl.stCardInfo[chairDtl.nMaxCardIndex].nShape;
				int bankerShape=bankerDtl.stCardInfo[bankerDtl.nMaxCardIndex].nShape;				
				if (chairShape < bankerShape)
				{
					bBankerWin=TRUE;
				}
				else if (chairShape > bankerShape) 
				{
					bBankerWin=FALSE;
				}
				else
				{//ERROR
					bBankerWin=TRUE;
				}
			}
		}
	}
	else
	{
		bBankerWin=FALSE;
	}
	
	int nBigType=0;
	if (bBankerWin)
		nBigType=bankerDtl.nCardType;
	else
		nBigType=chairDtl.nCardType;

	if (nBigType>=CARD_TYPE_POINT0 && nBigType<=CARD_TYPE_POINT6)
	{
		nDbl=1;
	}
	else if	(nBigType==CARD_TYPE_POINT7)
	{
		nDbl=2;
	}
	else if	(nBigType==CARD_TYPE_POINT8)
	{
		nDbl=3;
	}
	else if	(nBigType==CARD_TYPE_POINT9)
	{
		nDbl=4;
	}
	else if (nBigType == CARD_TYPE_SANGONG)
	{
		nDbl=5;
	}
	else if (nBigType == CARD_TYPE_ZHADAN)
	{
		nDbl=6;
	}
	else if (nBigType == CARD_TYPE_BAOJIU)
	{
		nDbl=9;
	}

	if (bBankerWin)
	{
		nDbl *= -1;
	}
		
	return nDbl;
}

int CGameTable::CompensateDeposits(int nOldDeposits[], int nDepositDiffs[])
{
	int totalwin = 0;
	double dblDeposits[MAX_CHAIRS_PER_TABLE];
	ZeroMemory(dblDeposits, sizeof(dblDeposits));
	
	for(int i = 0; i < m_nTotalChairs; i++){
		dblDeposits[i] = nDepositDiffs[i];
		if(nDepositDiffs[i] > 0){
			totalwin += nDepositDiffs[i];
		}
	}

	int nTotalLose = 0;
	for(i = 0; i < m_nTotalChairs; i++)
	{
		//dblDeposits[i] = nDepositDiffs[i];
		if(nDepositDiffs[i] < 0)
		{
			nTotalLose += nDepositDiffs[i];
		}
	}

	if(IS_BIT_SET(m_dwGameFlags, GF_LEVERAGE_ALLOWED))
	{ // 允许以小博大
		/*
		if(m_nDepositMult==1)
		{
			for(i = 0; i < m_nTotalChairs; i++)
			{
				int depositDiff = nDepositDiffs[i];
				int depositOld = nOldDeposits[i];
				if(depositDiff < 0)
				{ // 输家
					if(depositOld + depositDiff < 0)
					{ // 银子不够输
						nDepositDiffs[i] = -depositOld; // 输光已有银子
						for(int j = 0; j < m_nTotalChairs; j++)
						{
							if(dblDeposits[j] > 0)
							{
								double dblReturn = (-depositDiff - depositOld);
								nDepositDiffs[j] -= ceil(dblReturn / totalwin * dblDeposits[j] );
								if(nDepositDiffs[j] < 0)
								{
									nDepositDiffs[j] = 0;
								}
							}
						}
					}
				}
			}
		}
		else*/
		{
			//新版以小博大，输赢总和必须为0
			for(i = 0; i < m_nTotalChairs; i++)
			{
				//int depositDiff = nDepositDiffs[i];
				if (nDepositDiffs[i]>0)//赢家
				{
					if (nOldDeposits[i] < nDepositDiffs[i])
					{
						double fReturn = nDepositDiffs[i] - nOldDeposits[i];
						
						nDepositDiffs[i] = nOldDeposits[i];
						
						
						for(int j = 0; j < m_nTotalChairs; j++)
						{
							if(dblDeposits[j] < 0)
							{
								nDepositDiffs[j] += ceil(dblDeposits[j] / nTotalLose * fReturn );
								if(nDepositDiffs[j] > 0)
								{
									nDepositDiffs[j] = 0;
								}
							}
						}		
					}
				}
				else//输家
				{
					if(nOldDeposits[i] + nDepositDiffs[i] < 0)
					{
						double fReturn = -(nDepositDiffs[i] + nOldDeposits[i]);
						
						nDepositDiffs[i] = -nOldDeposits[i];
						
						for(int j = 0; j < m_nTotalChairs; j++)
						{
							if(dblDeposits[j] > 0)
							{
								nDepositDiffs[j] -= ceil(dblDeposits[j] / totalwin * fReturn );
								if(nDepositDiffs[j] < 0)
								{
									nDepositDiffs[j] = 0;
								}
							}
						}
					}
				}	
			}
			
			//最后再保证一下总和为0
			int nTotal = 0;
			for(i = 0; i < m_nTotalChairs; i++)
			{
				nTotal += nDepositDiffs[i];	
			}
			
			if (nTotal>0)
			{
				for(i = 0; i < m_nTotalChairs; i++)
				{
					if(nDepositDiffs[i]>0)
					{
						nDepositDiffs[i] -= nTotal;
						break;
					}
				}
			}
		}
	}
	else
	{ // 不允许以小博大 新规则
			int depositMin = 0;
			for(int i = 0; i < m_nTotalChairs; i++)
			{
				if (nOldDeposits[i]>0)
				{
					if ((0 == depositMin)
						|| (nOldDeposits[i] < depositMin))
						depositMin = nOldDeposits[i];
				}
			}
			for(i = 0; i < m_nTotalChairs; i++){
				int depositDiff = nDepositDiffs[i];
				if(depositDiff < 0){ // 输家
					depositMin = nOldDeposits[i];
					if(depositMin + depositDiff < 0){ // 银子输得太多
						nDepositDiffs[i] = -depositMin; // 输银进行限制
						for(int j = 0; j < m_nTotalChairs; j++){
							if(dblDeposits[j] > 0){
								double dblReturn = (-depositDiff - depositMin);
								nDepositDiffs[j] -= ceil(dblReturn / totalwin * dblDeposits[j] );
								if(nDepositDiffs[j] < 0){
									nDepositDiffs[j] = 0;
								}
							}
						}
					}
				}
			}
	}
	return CalcSurplus(nDepositDiffs);
}

BOOL CGameTable::UpdataGameWin(LPREFRESH_RESULT_EX lpRefreshResult,CPlayerLevelMap& mapPlayerLevel)
{
	return CTable::UpdataGameWin(lpRefreshResult,mapPlayerLevel);
}

int CGameTable::OnQiangZhuang(QZ* pQZ)
{
	if(!pQZ) 
		return BR_NULL_POINTER;
	int nChairNO = pQZ->nChairNO;
	if (!m_bInGame[nChairNO]) 
		return BR_NOT_INGAME;	 	
	if (GetPublicInfo()->bFinishQZ[nChairNO])
		return BR_FINISHED;

	GetPublicInfo()->bFinishQZ[nChairNO]=TRUE;
	GetPublicInfo()->nQZFlag[nChairNO]=pQZ->nQZFlag;
	return 1;
}

BOOL CGameTable::AllFinishQZ(QZ* pQZ)
{
	BOOL bAllGiveUpQZ=TRUE;
	int i=0;
	for (i=0;i<TOTAL_CHAIRS;i++)
	{
		if (!m_bInGame[i]) 
			continue;
		if (!GetPublicInfo()->bFinishQZ[i])
		{
			pQZ->nFinish=0;
			return FALSE;
		}
		else
		{
			if(GetPublicInfo()->nQZFlag[i]==1)
				bAllGiveUpQZ=FALSE;
		}
	}
	pQZ->bAllGiveUpQZ=bAllGiveUpQZ;
	GetPublicInfo()->bAllGiveUpQZ=bAllGiveUpQZ;
	
	int nTemp[TOTAL_CHAIRS];
	for(i=0;i<TOTAL_CHAIRS;i++)
		nTemp[i]=-1;
	for(i=0;i<TOTAL_CHAIRS;i++)
	{
		if (!m_bInGame[i]) 
			continue;
		int nCompareFlag=bAllGiveUpQZ?0:1;
		if (GetPublicInfo()->nQZFlag[i] == nCompareFlag)
		{
			nTemp[i]=i;
		}
	}

	CPlayer* ptrP = NULL;
	for (i=0;i<TOTAL_CHAIRS;i++)
	{
		if (m_ptrPlayers[i] && !m_ptrPlayers[i]->m_bIdlePlayer) 
		{
			ptrP = m_ptrPlayers[i];
			break;
		}
	}

	//取第一个随机数组的值(抢最大倍数的进行随机)		
	SvrXygRandomSort(nTemp,TOTAL_CHAIRS,GetTickCount()+ptrP->m_lTokenID * 10 + ptrP->m_hSocket);
	for(i=0;i<TOTAL_CHAIRS;i++)
	{
		if(nTemp[i]!=-1)
		{
			m_nBanker=nTemp[i];
			break;
		}
	}
	for(i=0;i<TOTAL_CHAIRS;i++)
	{
		if (!m_bInGame[i]) 
			continue;
		if (i == m_nBanker) 
			continue;
		m_nPlayerBaseDeposit[i]=CalcPlayerBaseDeposit(i,m_nBanker);
		pQZ->nBaseDeposit[i]=m_nPlayerBaseDeposit[i];
	}
	pQZ->nBanker=m_nBanker;
	pQZ->nFinish=1;
	return TRUE;
}

int CGameTable::OnYaZhu(YAZHU* pYaZhu)
{
	if(!pYaZhu) 
		return BR_NULL_POINTER;
	int nChairNO = pYaZhu->nChairNO;
	if (!m_bInGame[nChairNO]) 
		return BR_NOT_INGAME;
	if (nChairNO == m_nBanker) 
		return BR_IS_BANKER; 
	if (GetPublicInfo()->bFinishYaZhu[nChairNO])
		return BR_FINISHED;
	GetPublicInfo()->bFinishYaZhu[nChairNO]=TRUE;
	GetPublicInfo()->nYaZhuDbl[nChairNO]=pYaZhu->nDbl;
	return 1;
}

BOOL CGameTable::AllFinishYaZhu(YAZHU* pYaZhu)
{
	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		if (!m_bInGame[i]||m_nBanker==i) 
			continue;
		if (!GetPublicInfo()->bFinishYaZhu[i])
		{
			pYaZhu->nFinish=0;
			return FALSE;
		}
	}
	pYaZhu->nFinish=1;
	return TRUE;
}

void CGameTable::SetResetBanker(BOOL bReset)
{
	m_bResetBanker=bReset;
}

int CGameTable::TellBreakChair(int leavechair, DWORD waitsecs)
{
	return leavechair;
}

BOOL CGameTable::IsOperateTimeOver()
{
	if (IS_BIT_SET(m_dwStatus, TS_WAITING_QZ))
	{
		m_dwWaitOperateTick=(QZ_WAIT+1)*1000;
		if (GetTickCount()-m_dwActionStart>=m_dwWaitOperateTick)
			return TRUE;
		else 
			return FALSE;
	}
	else if (IS_BIT_SET(m_dwStatus, TS_WAITING_YAZHU))
	{
		m_dwWaitOperateTick=(YAZHU_WAIT+1)*1000;
		if (GetTickCount()-m_dwActionYaZhu>=m_dwWaitOperateTick)
			return TRUE;
		else 
			return FALSE;
	}		
	else if (IS_BIT_SET(m_dwStatus, TS_WAITING_ZUPAI))
	{
		m_dwWaitOperateTick=(m_nOperateTime+1)*1000;
		if (GetTickCount()-m_dwActionDealCard>=m_dwWaitOperateTick)
			return TRUE;
		else 
			return FALSE;
	}
	return FALSE;
}