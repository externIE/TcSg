#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGameServer* g_gameServer=NULL;
#include "Markup.h"
#include "DataRecord.h"

BOOL CGameServer::OnRequest(void* lpParam1, void* lpParam2)
{
	LPCONTEXT_HEAD	lpContext = LPCONTEXT_HEAD	(lpParam1);
	LPREQUEST		lpRequest = LPREQUEST		(lpParam2);

	UwlTrace(_T("----------------------start of request process-------------------"));
	
#if defined(_UWL_TRACE) | defined(UWL_TRACE)
	DWORD dwTimeStart = GetTickCount();
#else
	DWORD dwTimeStart = 0;
#endif
	CWorkerContext* pThreadCxt = reinterpret_cast<CWorkerContext*>(GetWorkerContext());

	assert(lpContext && lpRequest);
	UwlTrace(_T("socket = %ld requesting..."), lpContext->hSocket);
	switch(lpRequest->head.nRequest){
	case GR_SENDMSG_TO_SERVER:
		UwlTrace(_T("发送通知给服务器 requesting..."));
		OnSendSysMsgToServer(lpContext, lpRequest, pThreadCxt);
		break;

	case GR_QZ:
		UwlTrace(_T("GR_QZ requesting..."));
		OnQiangZhuang(lpContext, lpRequest, pThreadCxt);
		break;

	case GR_YAZHU:
		UwlTrace(_T("GR_YAZHU requesting..."));
		OnYaZhu(lpContext, lpRequest, pThreadCxt);
		break;

	case GR_ZU_PAI:
		UwlTrace(_T("GR_ZU_PAI requesting..."));
		OnZuPai(lpContext, lpRequest, pThreadCxt);
		break;

    //二维码充值start
	case GR_ROOMLIMIT:
		UwlTrace(_T("GR_ROOMLIMIT requesting..."));
		OnRoomLimit(lpContext, lpRequest, pThreadCxt);
		break;
		
	case GR_CHARGEBTN_LEFTDOWN:
		UwlTrace(_T("GR_CHARGEBTN_LEFTDOWN requesting..."));
		OnChargeBtn(lpContext, lpRequest, pThreadCxt);
		break;
    //二维码充值end
		
	default:
		UwlTrace(_T("goto default proceeding..."));
		CSkServer::OnRequest(lpParam1, lpParam2);
		break;
	}

	UwlClearRequest(lpRequest);

#if defined(_UWL_TRACE) | defined(UWL_TRACE)
	DWORD dwTimeEnd = GetTickCount();
#else
	DWORD dwTimeEnd = 0;
#endif
	UwlTrace(_T("request process time costs: %d ms"), dwTimeEnd - dwTimeStart);
	UwlTrace(_T("----------------------end of request process---------------------\r\n"));

	return TRUE;
}

CTable* CGameServer::OnNewTable(int roomid, int tableno, int score_mult)
{
	CGameTable* pTable = new CGameTable(roomid, tableno, score_mult);
	return (CTable*)pTable;
}


void CGameServer::OnSeverAutoPlay(CRoom* pRoom,CTable* ptr,int chairno)
{
	int nRoomID=pRoom->m_nRoomID;
	
	CGameTable* pTable=(CGameTable*)ptr;
	if (!IS_BIT_SET(pTable->m_dwStatus,TS_PLAYING_GAME))
		return;
	if (!pTable->ValidateChair(chairno)) 
		return;
	if (!pTable->ValidateChair(pTable->GetCurrentChair())) 
		return;

	CPlayer* pPlayer = pTable->m_ptrPlayers[chairno];
	if (!pPlayer) 
		return;
	
	DWORD dwTickWait=2000;
	
	if (GetTickCount()-pTable->m_dwActionBegin>dwTickWait)
		dwTickWait=0;//等待超时，那么立即执行

	if (IS_BIT_SET(pTable->m_dwStatus,TS_WAITING_QZ))
	{
		QZ stQZ;
		memset(&stQZ,0,sizeof(QZ));
		stQZ.nUserID = pPlayer->m_nUserID;
		stQZ.nRoomID=nRoomID;
		stQZ.nTableNO=pTable->m_nTableNO;
		stQZ.nChairNO=chairno;
		stQZ.nQZFlag=0;
		
		SimulateGameMsgFromUser(nRoomID,pPlayer,TCSG_GAME_MSG_AUTO_NO_QZ,sizeof(QZ),&stQZ,dwTickWait);
		return;
	}

	if (IS_BIT_SET(pTable->m_dwStatus,TS_WAITING_YAZHU))
	{
		YAZHU stYZ;
		memset(&stYZ,0,sizeof(YAZHU));
		stYZ.nUserID = pPlayer->m_nUserID;
		stYZ.nRoomID=nRoomID;
		stYZ.nTableNO=pTable->m_nTableNO;
		stYZ.nChairNO=chairno;
		stYZ.nDbl=5;//最低5倍
		
		SimulateGameMsgFromUser(nRoomID,pPlayer,TCSG_GAME_MSG_AUTO_YAZHU,sizeof(YAZHU),&stYZ,dwTickWait);
		return;
	}

	if (IS_BIT_SET(pTable->m_dwStatus,TS_WAITING_ZUPAI))
	{
		ZUPAI stZuPai;
		memset(&stZuPai,0,sizeof(ZUPAI));
		stZuPai.nUserID = pPlayer->m_nUserID;
		stZuPai.nRoomID=nRoomID;
		stZuPai.nTableNO=pTable->m_nTableNO;
		stZuPai.nChairNO=chairno;
		stZuPai.bPassive=TRUE;
		for (int i=0;i<CHAIR_CARDS;i++)
		{
			stZuPai.nCardID[i] = pTable->GetPlayerInfo(chairno)->stDetail.stCardInfo[i].nCardID;
		}
		
		SimulateGameMsgFromUser(nRoomID,pPlayer,TCSG_GAME_MSG_AUTO_ZUPAI,sizeof(ZUPAI),&stZuPai,dwTickWait);
		return;
	}
}


BOOL CGameServer::OnSendSysMsgToServer(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt)
{
	SOCKET sock = INVALID_SOCKET;
	LONG token = 0;
	int roomid = 0;
	int tableno = INVALID_OBJECT_ID;
	int userid = 0;
	int chairno = INVALID_OBJECT_ID;
	int cardchair = INVALID_OBJECT_ID;
	int r_id = 0;
	int t_no = INVALID_OBJECT_ID;
	int u_id = 0;
	int c_no = INVALID_OBJECT_ID;
	BOOL lookon = FALSE;
	BOOL bPassive=IS_BIT_SET(lpContext->dwFlags,CH_FLAG_SYSTEM_EJECT);//是否系统自己生成的消息
	
	CRoom* pRoom = NULL;
	CGameTable* pTable = NULL;
	CPlayer* pPlayer = NULL;
	
	sock = lpContext->hSocket;
	token = lpContext->lTokenID;

	//数据长度验证
	if (lpRequest->nDataLen<sizeof(GAME_MSG))
	{
		UwlLogFile(_T("%ld发送了不合法的消息结构，结构长度:%ld"),lpContext->lTokenID,lpRequest->nDataLen);
		return NotifyResponseFaild(lpContext);
	}
	GAME_MSG* pMsg = LPGAME_MSG(PBYTE(lpRequest->pDataPtr));
	BYTE* pData=PBYTE(lpRequest->pDataPtr)+sizeof(GAME_MSG);

	UwlTrace("OnSendSysMsgToServer SubID:%ld ...",pMsg->nMsgID);

	roomid = pMsg->nRoomID;
	userid = pMsg->nUserID;

	{
		CAutoLock lock(&m_csRoomMap);
		if(!m_mapRoom.Lookup(roomid, pRoom))
		{
			return FALSE;
		}
	}


	USER_DATA user_data;
	memset(&user_data,0,sizeof(user_data));
	
	if(pRoom)
	{
		CAutoLock lock(&(pRoom->m_csRoom));
		
		if (!m_mapUserData.Lookup(userid,user_data)
			&&bPassive==FALSE)
		{
			UwlLogFile(_T("user:%ld未在服务器注册，试图发送消息号为:%ld"),userid,pMsg->nMsgID);
			return NotifyResponseFaild(lpContext);
		}
		else
		{
			roomid=user_data.nRoomID;
			tableno=user_data.nTableNO;
			chairno=user_data.nChairNO;
		}

		if(!pRoom->m_mapTable.Lookup(tableno, (CTable*&)pTable))
		{
			return NotifyResponseFaild(lpContext);
		}
	}
	
	if(pTable) 
	{
		{
			LOCK_TABLE(pTable, chairno, bPassive, userid, token);

			//消息集验证
			if (!pTable->IsGameMsg(pMsg->nMsgID))
			{
		 		UwlLogFile(_T("%ld发送了不合法的消息结构，消息号为:%ld"),token,pMsg->nMsgID);
				return NotifyResponseFaild(lpContext);	
			}
			
			if(!pTable->IsPlayer(userid))
			{ // 不是玩家
				UwlLogFile(_T("user not player. user %ld SendMsgToPlayer Failed,dwFlags:%ld"), userid,pMsg->nMsgID);
				return NotifyResponseFaild(lpContext);
			}

			if (!pTable->ValidateChair(chairno))
			{
				UwlLogFile(_T("chairno Error. user %ld SendMsgToPlayer Failed,chairno:%ld"), userid,chairno);
				return NotifyResponseFaild(lpContext);
			}

			switch(pMsg->nMsgID) 
			{
				case SYSMSG_GAME_CLOCK_STOP:
					{
						if (pTable->IsOperateTimeOver()&&GetTickCount()-pTable->m_dwLastClockStop>3000)
						{
							pTable->m_dwLastClockStop=GetTickCount();
	// 						int offline=pTable->GetCurrentChair();
							for (int i=0;i<TOTAL_CHAIRS;i++)
							{
								if (pTable->m_bInGame[i] && !pTable->GetPublicInfo()->bFinishZuPai[i])
								{
									if ((IS_BIT_SET(pTable->m_dwStatus,TS_WAITING_QZ) && !pTable->GetPublicInfo()->bFinishQZ[i]) ||
										(IS_BIT_SET(pTable->m_dwStatus,TS_WAITING_YAZHU) && pTable->m_nBanker !=i && !pTable->GetPublicInfo()->bFinishYaZhu[i]) ||
										(IS_BIT_SET(pTable->m_dwStatus,TS_WAITING_ZUPAI) && !pTable->GetPublicInfo()->bFinishZuPai[i])
										)
									{
										int offline = i;
										if (pTable->ValidateChair(offline) && pTable->CheckOffline(offline))
										{
											if (pTable->IsTooManyAutoPlay(offline))
											{
												CPlayer* pPlayer = pTable->m_ptrPlayers[offline];
												if (pPlayer)
												{
													OnTooManyBreak(pPlayer->m_nUserID,roomid,tableno,offline,pPlayer->m_hSocket,pPlayer->m_lTokenID);
													return TRUE;
												}
											}
											else
											{
												OnPlayerOffline(pTable,offline);
												OnSeverAutoPlay(pRoom,pTable,offline);
											}
										}
									}
								}
							}
						}
					}
					break;
				case TCSG_GAME_MSG_AUTO_NO_QZ:
					{				
						if(!pTable->IsPlayer(userid)) // 不是玩家
							return NotifyResponseFaild(lpContext);												
						
						if(!IS_BIT_SET(pTable->m_dwStatus, TS_PLAYING_GAME)) // 游戏未在进行中
							return NotifyResponseFaild(lpContext);
						
						if(!IS_BIT_SET(pTable->m_dwStatus, TS_WAITING_QZ)) // 不是抢庄状态
							return NotifyResponseFaild(lpContext);						
						
						QZ* pQZ=(QZ*)pData;
						int nRet = pTable->OnQiangZhuang(pQZ);
						if (nRet <= 0)
						{
							UwlLogFile(_T("Auto QiangZhuang failed, errorID:%d, room:%ld, table:%ld, chair:%ld, user:%ld"), 
								nRet, roomid, tableno, chairno, userid);
							return NotifyResponseFaild(lpContext);
						}					
						
						//////////////////////////////////////////////////////////////////////////
						if (pTable->AllFinishQZ(pQZ))
							pTable->SetStatusOnFinishQZ();
						NotifyTablePlayers(pTable, GR_QZ, pQZ, sizeof(QZ));
						NotifyTableVisitors(pTable, GR_QZ, pQZ, sizeof(QZ));
					}
					break;
				case TCSG_GAME_MSG_AUTO_YAZHU:
					{				
						if(!pTable->IsPlayer(userid)) // 不是玩家
							return NotifyResponseFaild(lpContext);												
						
						if(!IS_BIT_SET(pTable->m_dwStatus, TS_PLAYING_GAME)) // 游戏未在进行中
							return NotifyResponseFaild(lpContext);
						
						if(!IS_BIT_SET(pTable->m_dwStatus, TS_WAITING_YAZHU)) // 不是押注状态
							return NotifyResponseFaild(lpContext);						
						
						YAZHU* pYaZhu=(YAZHU*)pData;
						int nRet = pTable->OnYaZhu(pYaZhu);
						if (nRet <= 0)
						{
							UwlLogFile(_T("Auto YaZhu failed, errorID:%d, room:%ld, table:%ld, chair:%ld, user:%ld"), 
								nRet, roomid, tableno, chairno, userid);
							return NotifyResponseFaild(lpContext);
						}					
						
						//////////////////////////////////////////////////////////////////////////
						if (pTable->AllFinishYaZhu(pYaZhu))
						{
							pTable->SetStatusOnFinishYaZhu();
							pTable->DealCardAfterYaZhu();
							pTable->FillupHandCardAfterYaZhu(pYaZhu);
						}
						NotifyTablePlayers(pTable, GR_YAZHU, pYaZhu, sizeof(YAZHU));
						NotifyTableVisitors(pTable, GR_YAZHU, pYaZhu, sizeof(YAZHU));
					}
					break;
				case TCSG_GAME_MSG_AUTO_ZUPAI:
					{
						if(!pTable->IsPlayer(userid)) // 不是玩家
							return NotifyResponseFaild(lpContext);												
						
						if(!IS_BIT_SET(pTable->m_dwStatus, TS_PLAYING_GAME)) // 游戏未在进行中
							return NotifyResponseFaild(lpContext);
						
						if(!IS_BIT_SET(pTable->m_dwStatus, TS_WAITING_ZUPAI)) // 不是组牌状态
							return NotifyResponseFaild(lpContext);	
						
						ZUPAI* pZuPai=(ZUPAI*)pData;	
						
						if (!pTable->OnZuPai(pZuPai))
						{
							UwlLogFile(_T("TCSG_GAME_MSG_AUTO_ZUPAI failed, chair %ld, auto: %ld"), pZuPai->nChairNO, bPassive);
							return NotifyResponseFaild(lpContext);
						}
						
						NotifyTablePlayers(pTable, GR_ZU_PAI, pZuPai, sizeof(ZUPAI));
						NotifyTableVisitors(pTable, GR_ZU_PAI, pZuPai, sizeof(ZUPAI));
						
						if (pTable->AllZuPai())
						{
							OnGameWin(lpContext,pRoom,pTable,chairno,FALSE,roomid);
						}
					}
					break;
				case SYSMSG_PLAYER_ONLINE:
					{
						pPlayer=pTable->m_ptrPlayers[chairno];
						if (pTable->IsOffline(chairno))
						{
							//断线续完
							pTable->m_dwUserStatus[chairno]&=~US_USER_OFFLINE;
							/////////////////////////////////////////////////////////////////////////
							NotifyTableMsg(pTable,GAME_MSG_SEND_OTHER,SYSMSG_RETURN_GAME,4,&pPlayer->m_nChairNO,pPlayer->m_lTokenID);
							/////////////////////////////////////////////////////////////////////////
						}
					}
					break;
				case SYSMSG_GAME_ON_AUTOPLAY:
					{
						if (!pTable->IsAutoPlay(chairno))
						{
							pTable->m_dwUserStatus[chairno]|=US_USER_AUTOPLAY;
							NotifyPlayerMsgAndResponse(lpContext,pTable,GAME_MSG_SEND_EVERYONE,SYSMSG_GAME_ON_AUTOPLAY,sizeof(int),&chairno);
						}
						else
						{
							NotifyPlayerMsgAndResponse(lpContext,pTable,chairno,SYSMSG_GAME_ON_AUTOPLAY,sizeof(int),&chairno);
						}
					}
					break;
				case SYSMSG_GAME_CANCEL_AUTOPLAY:
					{
 						if (pTable->IsAutoPlay(chairno))
						{
							pTable->m_dwUserStatus[chairno]&=~US_USER_AUTOPLAY;
							NotifyPlayerMsgAndResponse(lpContext,pTable,GAME_MSG_SEND_EVERYONE,SYSMSG_GAME_CANCEL_AUTOPLAY,sizeof(int),&chairno);
						}
						else
						{
							NotifyPlayerMsgAndResponse(lpContext,pTable,chairno,SYSMSG_GAME_CANCEL_AUTOPLAY,sizeof(int),&chairno);
						}
					}
					break;
				case TCSG_C2S_NEWTABLE_EX://
					{
						int nIndex = *(int *)pData;
						if(nIndex<0||nIndex>=MAX_TABLE_GRADE)
							return FALSE;
						PostAskNewTableByTableGrade(userid,roomid,tableno,nIndex);
						
					}
					break;
				case TCSG_C2S_REQTABLE_GRADE:
					{
						NotifyTableGrade(roomid,sock,token);
					}
					break;
				default:
					return NotifyResponseSucceesd(lpContext);
			}
		}
	}
	return TRUE;
}

BOOL CGameServer::OnZuPai(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt)
{
	SOCKET sock = INVALID_SOCKET;
	LONG token = 0;
	int roomid = 0;
	int tableno = INVALID_OBJECT_ID;
	int userid = 0;
	int chairno = INVALID_OBJECT_ID;
	int r_id = 0;
	int t_no = INVALID_OBJECT_ID;
	int u_id = 0;
	int c_no = INVALID_OBJECT_ID;
	
	CRoom* pRoom = NULL;
	CGameTable* pTable = NULL;
	CPlayer* pPlayer = NULL;
	
	sock = lpContext->hSocket;
	token = lpContext->lTokenID;

	LPZUPAI pZuPai = LPZUPAI(PBYTE(lpRequest->pDataPtr));
	roomid = pZuPai->nRoomID;
	tableno = pZuPai->nTableNO;
	userid = pZuPai->nUserID;
	chairno = pZuPai->nChairNO;

	BOOL bPassive=IS_BIT_SET(lpContext->dwFlags,CH_FLAG_SYSTEM_EJECT);//是否系统自己生成的消息

	if(bPassive){
		int passive = GetPrivateProfileInt(
							_T("action"),			// section name
							_T("passive"),			// key name
							1,						// default int
							m_szIniFile				// initialization file name
							);
		if(!passive) {
			return NotifyResponseFaild(lpContext);
		}
	}
	{
		CAutoLock lock(&m_csRoomMap);

		if(!m_mapRoom.Lookup(roomid, pRoom)){
			return NotifyResponseFaild(lpContext);
		}
	}
	USER_DATA user_data;
	memset(&user_data,0,sizeof(user_data));
	if(pRoom){
		CAutoLock lock(&(pRoom->m_csRoom));

		if (!m_mapUserData.Lookup(userid,user_data)
			&&bPassive==FALSE)
		{
			return NotifyResponseFaild(lpContext);
		}
		else
		{
			roomid=user_data.nRoomID;
			tableno=user_data.nTableNO;
			chairno=user_data.nChairNO;
		}

		if(!pRoom->m_mapTable.Lookup(tableno, (CTable*&)pTable)){
			return NotifyResponseFaild(lpContext);
		}
	}
	if(pTable){
		LOCK_TABLE(pTable, chairno, bPassive, userid, token);

		if(!pTable->IsPlayer(userid)){ // 不是玩家
			UwlLogFile(_T("ZuPai failed: user not player! user %ld, auto: %ld"), userid, bPassive);
			return NotifyResponseFaild(lpContext);
		}	
		UwlTrace(_T("chair %ld, user %ld zupai. auto: %ld"), chairno, userid, bPassive);
		if(!IS_BIT_SET(pTable->m_dwStatus, TS_PLAYING_GAME)){ // 游戏未在进行中
			return NotifyResponseFaild(lpContext);
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		if(!IS_BIT_SET(pTable->m_dwStatus, TS_WAITING_ZUPAI)){ // 不是组牌状态
			UwlLogFile(_T("ZuPai failed: status not TS_WAITING_ZUPAI! chair %ld, auto: %ld"), chairno, bPassive);
			return NotifyResponseFaild(lpContext);
		}
		UwlTrace(_T("chair %ld, user %ld zupai OK! auto: %ld"), chairno, userid, bPassive);
		
		if (!pTable->OnZuPai(pZuPai))
		{
			UwlLogFile(_T("ZuPai failed: pTable->OnZuPai, chair %ld, auto: %ld"), chairno, bPassive);
			return NotifyResponseFaild(lpContext);
		}

		NotifyResponseSucceesd(lpContext);
		
		NotifyTablePlayers(pTable, GR_ZU_PAI, pZuPai, sizeof(ZUPAI));
		NotifyTableVisitors(pTable, GR_ZU_PAI, pZuPai, sizeof(ZUPAI));

		//过牌后检查状态
		pPlayer=pTable->m_ptrPlayers[chairno];
		if (pTable->IsOffline(chairno))
		{
			//断线续完
			pTable->m_dwUserStatus[chairno]&=~US_USER_OFFLINE;
			/////////////////////////////////////////////////////////////////////////
			NotifyTableMsg(pTable,GAME_MSG_SEND_OTHER,SYSMSG_RETURN_GAME,4,&pPlayer->m_nChairNO,pPlayer->m_lTokenID);
			/////////////////////////////////////////////////////////////////////////
		}

		//自动出牌统计清零
		pTable->OnPlayerActiveEvent(chairno);

		if (pTable->AllZuPai())
		{		
			OnGameWin(lpContext,pRoom,pTable,chairno,FALSE,roomid);
		}
	}
	return TRUE;
}

BOOL CGameServer::OnQiangZhuang(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt)
{
	SOCKET sock = INVALID_SOCKET;
	LONG token = 0;
	int roomid = 0;
	int tableno = INVALID_OBJECT_ID;
	int userid = 0;
	int chairno = INVALID_OBJECT_ID;
	int qzflag = 0;
	int r_id = 0;
	int t_no = INVALID_OBJECT_ID;
	int u_id = 0;
	int c_no = INVALID_OBJECT_ID;
	
	CRoom* pRoom = NULL;
	CGameTable* pTable = NULL;
	CPlayer* pPlayer = NULL;
	
	sock = lpContext->hSocket;
	token = lpContext->lTokenID;

	LPQZ pQZ = LPQZ(PBYTE(lpRequest->pDataPtr));
	roomid = pQZ->nRoomID;
	tableno = pQZ->nTableNO;
	userid = pQZ->nUserID;
	chairno = pQZ->nChairNO;
	qzflag = pQZ->nQZFlag;
	BOOL bPassive=IS_BIT_SET(lpContext->dwFlags,CH_FLAG_SYSTEM_EJECT);//是否系统自己生成的消息

	if(bPassive){
		int passive = GetPrivateProfileInt(
							_T("action"),			// section name
							_T("passive"),			// key name
							1,						// default int
							m_szIniFile				// initialization file name
							);
		if(!passive) {
			return NotifyResponseFaild(lpContext);
		}
	}
	{
		CAutoLock lock(&m_csRoomMap);

		if(!m_mapRoom.Lookup(roomid, pRoom)){
			return NotifyResponseFaild(lpContext);
		}
	}
	USER_DATA user_data;
	memset(&user_data,0,sizeof(user_data));
	if(pRoom){
		CAutoLock lock(&(pRoom->m_csRoom));

		if (!m_mapUserData.Lookup(userid,user_data)
			&&bPassive==FALSE)
		{
			return NotifyResponseFaild(lpContext);
		}
		else
		{
			roomid=user_data.nRoomID;
			tableno=user_data.nTableNO;
			chairno=user_data.nChairNO;
		}

		if(!pRoom->m_mapTable.Lookup(tableno, (CTable*&)pTable)){
			return NotifyResponseFaild(lpContext);
		}
	}
	if(pTable){
		LOCK_TABLE(pTable, chairno, bPassive, userid, token);

		if(!pTable->IsPlayer(userid)){ // 不是玩家
			UwlLogFile(_T("QiangZhuang failed: user not player! user %ld, auto: %ld"), userid, bPassive);
			return NotifyResponseFaild(lpContext);
		}		

		if(!IS_BIT_SET(pTable->m_dwStatus, TS_PLAYING_GAME)){ // 游戏未在进行中
			return NotifyResponseFaild(lpContext);
		}
		if(!IS_BIT_SET(pTable->m_dwStatus, TS_WAITING_QZ)){ // 不是抢庄状态
			UwlLogFile(_T("QiangZhuang failed: status not TS_WAITING_QZ! chair %ld, auto: %ld"), chairno, bPassive);
			return NotifyResponseFaild(lpContext);
		}

		int nRet = pTable->OnQiangZhuang(pQZ);
		if (nRet <= 0)
		{
			UwlLogFile(_T("QiangZhuang failed, errorID:%d, room:%ld, table:%ld, chair:%ld, user:%ld, auto: %ld"), 
				nRet, roomid, tableno, chairno, userid, bPassive);
			return NotifyResponseFaild(lpContext);
		}

		UwlTrace(_T("chair %ld, user %ld qiangzhuang OK! auto: %ld"), chairno, userid, bPassive);
		NotifyResponseSucceesd(lpContext);
	
		//////////////////////////////////////////////////////////////////////////
		if (pTable->AllFinishQZ(pQZ))
			pTable->SetStatusOnFinishQZ();
		NotifyTablePlayers(pTable, GR_QZ, pQZ, sizeof(QZ));
		NotifyTableVisitors(pTable, GR_QZ, pQZ, sizeof(QZ));
		//////////////////////////////////////////////////////////////////////////

		//检查断线状态
		pPlayer=pTable->m_ptrPlayers[chairno];
		if (pTable->IsOffline(chairno))
		{
			//断线续完
			pTable->m_dwUserStatus[chairno]&=~US_USER_OFFLINE;
			/////////////////////////////////////////////////////////////////////////
			NotifyTableMsg(pTable,GAME_MSG_SEND_OTHER,SYSMSG_RETURN_GAME,4,&pPlayer->m_nChairNO,pPlayer->m_lTokenID);
			/////////////////////////////////////////////////////////////////////////
		}

		//自动操作统计清零
		pTable->OnPlayerActiveEvent(chairno);

	}
	return TRUE;
}

BOOL CGameServer::OnYaZhu(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt)
{
	SOCKET sock = INVALID_SOCKET;
	LONG token = 0;
	int roomid = 0;
	int tableno = INVALID_OBJECT_ID;
	int userid = 0;
	int chairno = INVALID_OBJECT_ID;
	int r_id = 0;
	int t_no = INVALID_OBJECT_ID;
	int u_id = 0;
	int c_no = INVALID_OBJECT_ID;
	
	CRoom* pRoom = NULL;
	CGameTable* pTable = NULL;
	CPlayer* pPlayer = NULL;
	
	sock = lpContext->hSocket;
	token = lpContext->lTokenID;

	LPYAZHU pYaZhu = LPYAZHU(PBYTE(lpRequest->pDataPtr));
	roomid = pYaZhu->nRoomID;
	tableno = pYaZhu->nTableNO;
	userid = pYaZhu->nUserID;
	chairno = pYaZhu->nChairNO;
	BOOL bPassive=IS_BIT_SET(lpContext->dwFlags,CH_FLAG_SYSTEM_EJECT);//是否系统自己生成的消息

	if(bPassive){
		int passive = GetPrivateProfileInt(
							_T("action"),			// section name
							_T("passive"),			// key name
							1,						// default int
							m_szIniFile				// initialization file name
							);
		if(!passive) {
			return NotifyResponseFaild(lpContext);
		}
	}
	{
		CAutoLock lock(&m_csRoomMap);

		if(!m_mapRoom.Lookup(roomid, pRoom)){
			return NotifyResponseFaild(lpContext);
		}
	}
	USER_DATA user_data;
	memset(&user_data,0,sizeof(user_data));
	if(pRoom){
		CAutoLock lock(&(pRoom->m_csRoom));

		if (!m_mapUserData.Lookup(userid,user_data)
			&&bPassive==FALSE)
		{
			return NotifyResponseFaild(lpContext);
		}
		else
		{
			roomid=user_data.nRoomID;
			tableno=user_data.nTableNO;
			chairno=user_data.nChairNO;
		}

		if(!pRoom->m_mapTable.Lookup(tableno, (CTable*&)pTable)){
			return NotifyResponseFaild(lpContext);
		}
	}
	if(pTable){
		LOCK_TABLE(pTable, chairno, bPassive, userid, token);

		if(!pTable->IsPlayer(userid)){ // 不是玩家
			UwlLogFile(_T("YaZhu failed: user not player! user %ld, auto: %ld"), userid, bPassive);
			return NotifyResponseFaild(lpContext);
		}		

		if(!IS_BIT_SET(pTable->m_dwStatus, TS_PLAYING_GAME)){ // 游戏未在进行中
			return NotifyResponseFaild(lpContext);
		}
		if(!IS_BIT_SET(pTable->m_dwStatus, TS_WAITING_YAZHU)){ // 不是押注状态
			UwlLogFile(_T("YaZhu failed: status not TS_WAITING_YAZHU! chair %ld, auto: %ld"), chairno, bPassive);
			return NotifyResponseFaild(lpContext);
		}

		int nRet = pTable->OnYaZhu(pYaZhu);
		if (nRet <= 0)
		{
			UwlLogFile(_T("YaZhu failed, errorID:%d, room:%ld, table:%ld, chair:%ld, user:%ld, auto: %ld"), 
				nRet, roomid, tableno, chairno, userid, bPassive);
			return NotifyResponseFaild(lpContext);
		}

		UwlTrace(_T("chair %ld, user %ld YaZhu OK! auto: %ld"), chairno, userid, bPassive);
		NotifyResponseSucceesd(lpContext);
	
		//////////////////////////////////////////////////////////////////////////
		if (pTable->AllFinishYaZhu(pYaZhu))
		{
			pTable->SetStatusOnFinishYaZhu();
			pTable->DealCardAfterYaZhu();
			pTable->FillupHandCardAfterYaZhu(pYaZhu);
		}
		NotifyTablePlayers(pTable, GR_YAZHU, pYaZhu, sizeof(YAZHU));
		NotifyTableVisitors(pTable, GR_YAZHU, pYaZhu, sizeof(YAZHU));
		//////////////////////////////////////////////////////////////////////////

		//检查断线状态
		pPlayer=pTable->m_ptrPlayers[chairno];
		if (pTable->IsOffline(chairno))
		{
			//断线续完
			pTable->m_dwUserStatus[chairno]&=~US_USER_OFFLINE;
			/////////////////////////////////////////////////////////////////////////
			NotifyTableMsg(pTable,GAME_MSG_SEND_OTHER,SYSMSG_RETURN_GAME,4,&pPlayer->m_nChairNO,pPlayer->m_lTokenID);
			/////////////////////////////////////////////////////////////////////////
		}

		//自动操作统计清零
		pTable->OnPlayerActiveEvent(chairno);

	}
	return TRUE;
}

BOOL CGameServer::Initialize()
{
	BOOL bRet = CMainServer::Initialize();
	if (bRet)
	{
		//InitSockToChunk();
		DataRecord::Instance()->init(m_szIniFile, this);
		DataRecord::Instance()->ReadInfos();
	}

	return bRet;
}

BOOL CGameServer::ReadChunkSvrFromProfile()
{
	lstrcpy(m_szChunkServer, (""));
	GetPrivateProfileString(_T("ChunkServer"), _T("Name"), _T(""), m_szChunkServer,	sizeof(m_szChunkServer), m_szIniFile);
	return TRUE;
}

BOOL CGameServer::InitSockToChunk()
{
	{
		ReadChunkSvrFromProfile();
		if(!lstrcmpi(m_szChunkServer, _T(""))){
			
			CString strMsg;
			UwlGetErrMsg(ERR_SERVER_NOTFOUND, strMsg);
			
			CString strErr;
			UwlGetErrMsg(ERR_SERVER_ABORTED, strErr);
			strErr += strMsg;
			UwlLogFile(_T("%s"), strErr);
			UwlShowServiceMsg(strErr, NULL, MB_OK);
			return FALSE;
		}
		UwlTrace(_T("chunk server name: %s"), m_szChunkServer);
	}
	
	if (!m_pSockClitChunk)
		return FALSE;
	
	m_hThrdTimerChunk = (HANDLE)_beginthreadex(NULL,			// Security
		0,						// Stack size - use default
		Chunk_TimerThreadFunc,     	// Thread fn entry point
		(void*) this,			// Param for thread
		0,						// Init flag
		&m_uiThrdTimerChunk);		// Thread address;	
	
	// create send and recv thread for client
	m_hThrdSendChunk = (HANDLE)_beginthreadex(NULL,				// Security
		0,						// Stack size - use default
		Chunk_SendThreadFunc,     	// Thread fn entry point
		(void*) this,			// Param for thread
		0,						// Init flag
		&m_uiThrdSendChunk);			// Thread address;
	m_hThrdConnectChunk = (HANDLE)_beginthreadex(NULL,			// Security
		0,						// Stack size - use default
		Chunk_ConnectThreadFunc,     	// Thread fn entry point
		(void*) this,			// Param for thread
		0,						// Init flag
		&m_uiThrdConnectChunk);			// Thread address;
	m_hThrdSvrPulseChunk = (HANDLE)_beginthreadex(NULL,			// Security
		0,						// Stack size - use default
		Chunk_ServerPulseThreadFunc,  // Thread fn entry point
		(void*) this,			// Param for thread
		0,						// Init flag
		&m_uiThrdSvrPulseChunk);		// Thread address;
	
	if(FALSE == m_pSockClitChunk->Create(m_szChunkServer, PORT_OF_CHUNKSVR, CONNECT_CHUNKSVR_WAIT, 
		TRUE, GetHelloData(), GetHelloLength()+1,
		CONNECTS_TO_CHUNKSVR, CLIENT_INITIAL_RECVS))
	{
		m_pSockClitChunk->Destroy();
		
		UwlTrace(_T("Can not connect to chunk server."));
		UwlLogFile(_T("Can not connect to chunk server."));
		
		CString strMsg;
		UwlGetErrMsg(ERR_CONNECT_FAILED, strMsg);
		
		CString strErr;
		UwlGetErrMsg(ERR_SERVER_ABORTED, strErr);
		strErr += strMsg;
		UwlLogFile(_T("%s"), strErr);
		UwlShowServiceMsg(strErr, NULL, MB_OK);
		return FALSE;
	}
	Chunk_SetIsConnectChunkSvr(TRUE);
	Chunk_ValidateClientInfo(m_nGameID);
	
	return TRUE;
}

void CGameServer::Shutdown()
{
	ShutDownSockToChunk();
	DataRecord::Instance()->Purge();
	CMainServer::Shutdown();
}

void CGameServer::ShutDownSockToChunk()
{
	SetEvent(m_hExitServer);

	PostThreadMessage(m_uiThrdSendChunk, WM_QUIT, 0, 0);
	WaitForSingleObject(m_hThrdSendChunk, WAITTIME_EXIT);	
	if(m_hThrdSendChunk){
		CloseHandle(m_hThrdSendChunk);
		m_hThrdSendChunk = NULL;
	}
	PostThreadMessage(m_uiThrdConnectChunk, WM_QUIT, 0, 0);
	WaitForSingleObject(m_hThrdConnectChunk, WAITTIME_EXIT);	
	if(m_hThrdConnectChunk){
		CloseHandle(m_hThrdConnectChunk);
		m_hThrdConnectChunk = NULL;
	}
	PostThreadMessage(m_uiThrdSvrPulseChunk, WM_QUIT, 0, 0);
	WaitForSingleObject(m_hThrdSvrPulseChunk, WAITTIME_EXIT);	
	if(m_hThrdSvrPulseChunk){
		CloseHandle(m_hThrdSvrPulseChunk);
		m_hThrdSvrPulseChunk = NULL;
	}
	PostThreadMessage(m_uiThrdTimerChunk, WM_QUIT, 0, 0);
	WaitForSingleObject(m_hThrdTimerChunk, WAITTIME_EXIT);	
	if(m_hThrdTimerChunk){
		CloseHandle(m_hThrdTimerChunk);
		m_hThrdTimerChunk = NULL;
	}

	if (m_pSockClitChunk){ m_pSockClitChunk->Destroy();}
}

BOOL CGameServer::OnAskExit(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt)
{
	LONG token = 0;
	int roomid = 0;
	int tableno = INVALID_OBJECT_ID;
	int userid = 0;
	int chairno = INVALID_OBJECT_ID;
	int r_id = 0;
	int t_no = INVALID_OBJECT_ID;
	int u_id = 0;
	int c_no = INVALID_OBJECT_ID;
	BOOL lookon = FALSE;
	
	CRoom* pRoom = NULL;
	CTable* pTable = NULL;
	CPlayer* pPlayer = NULL;
	
	token = lpContext->lTokenID;
	
	LPASK_EXIT pAskExit = LPASK_EXIT(PBYTE(lpRequest->pDataPtr));
	roomid = pAskExit->nRoomID;
	tableno = pAskExit->nTableNO;
	userid = pAskExit->nUserID;
	chairno = pAskExit->nChairNO;	
	{
		CAutoLock lock(&m_csRoomMap);
		
		if(!m_mapRoom.Lookup(roomid, pRoom)){
			return TRUE;
		}
	}
	if(pRoom){
		CAutoLock lock(&(pRoom->m_csRoom));
		
		if(!pRoom->m_mapTable.Lookup(tableno, (CTable*&)pTable)){
			return TRUE;
		}
	}
	if(pTable) 
	{
		if (chairno<=-1||chairno>=pTable->m_nTotalChairs)
		{
			return TRUE;
		}
		
		LOCK_TABLE(pTable, chairno, FALSE, userid, token);	
		
		if(pTable->IsVisitor(userid)){ // 是旁观者
			UwlLogFile(_T("user is visitor. user %ld ask cardview failed."), userid);
			return TRUE;
		}
		
		if(!IS_BIT_SET(pTable->m_dwStatus, TS_PLAYING_GAME)){ // 游戏未在进行中
			return TRUE;
		}
		
		//Add on 20130320
		if (IsRandomRoom(roomid)){	//随机房间不得请求退出
			return TRUE;
		}
		
		if (pTable->m_nAskExit[chairno]<MAX_ASKEXIT)
		{			
			pTable->m_nAskExit[chairno]++;
			
			memset(pTable->m_bAllowExit[chairno],0,sizeof(pTable->m_bAllowExit[chairno]));			
			
			NotifyPlayersAskExit((CGameTable*)pTable, pAskExit, token);
			
			//增加一个检测，其它3家都掉线的情况下，直接散桌
			if (pTable->IsAllAllowExit(chairno))
			{			
				GAME_ABORT GameAbort;
				ZeroMemory(&GameAbort, sizeof(GameAbort));
				GameAbort.nUserID = userid;
				GameAbort.nChairNO = chairno;
				GameAbort.bForce=FALSE;	
				
				RemoveClients(pTable, 0, FALSE);
				NotifyTablePlayers(pTable, GR_GAME_ABORT, &GameAbort, sizeof(GameAbort));
				NotifyTableVisitors(pTable, GR_GAME_ABORT, &GameAbort, sizeof(GameAbort));
				pTable->Reset(); // 清空桌子
				
				return TRUE;
			}	
		}			
	}
	return TRUE;
}

void CGameServer::NotifyPlayersAskExit(CGameTable* pTable, LPASK_EXIT pAskExit, LONG tokenExcept/*=0*/, BOOL compressed/*=FALSE*/)
{
	if (!pTable)
		return;

	UINT nRequest=GR_ASK_EXIT;
	void* pData=(void*)pAskExit;
	int nLen=sizeof(ASK_EXIT);

	for (int i = 0; i < pTable->m_nTotalChairs; i++)
	{
		if (!pTable->m_bInGame[i]) continue;
			
		CPlayer* ptrP = pTable->m_ptrPlayers[i];
		if(ptrP && ptrP->m_lTokenID != tokenExcept)
		{
			NotifyOneUser(ptrP->m_hSocket, ptrP->m_lTokenID, nRequest, pData, nLen, compressed);
		}
	}
}

BOOL CGameServer::OnGameWin(LPCONTEXT_HEAD lpContext, CRoom* pRoom,CTable* pTable, int chairno, BOOL bout_invalid, int roomid)
{
	if (pRoom&&pRoom->IsOnMatch())
	{//比赛
		return OnMatchGameWin(lpContext,pRoom,pTable,chairno,bout_invalid,roomid);
	}
	

	BOOL bWaitCheckRult=IsWaitCheckResult(roomid);
	pTable->SetStatusOnEnd();
	
	int nLen = pTable->GetGameWinSize();
	void* pData = new_byte_array(nLen);

	CGameTable* pGameTable=(CGameTable*)pTable;
	pTable->BuildUpGameWin(pData, nLen, chairno, 
							bout_invalid, m_mapPlayerLevel);

	if(!bout_invalid && pTable->CheckInDB()){ // 本局结果有效并且需要提交到数据库
		REFRESH_RESULT_EX RefreshResult;
		ZeroMemory(&RefreshResult, sizeof(RefreshResult));
		GAME_RESULT_EX GameResults[MAX_CHAIRS_PER_TABLE];
		ZeroMemory(&GameResults, sizeof(GameResults));
		RefreshResult.nTableNO	=	pTable->m_nTableNO;

		if (!IsVariableChairRoom(roomid))
		{
			pTable->ConstructGameResults(pData, nLen, roomid, m_nGameID, 
				&RefreshResult, GameResults);
			pTable->RefreshPlayerData(GameResults);
			TransmitGameResult(lpContext, &RefreshResult, GameResults, pTable->m_nTotalChairs * sizeof(GAME_RESULT_EX));
		}
		else
		{
			int nPlayerCount = 0;
			for(int i = 0; i < pTable->m_nTotalChairs; i++)
			{
				if (pTable->m_ptrPlayers[i] && !(pTable->m_ptrPlayers[i]->m_bIdlePlayer))
				{
					nPlayerCount++;
				}
			}
			pTable->ConstructEndSaveResult(pData, nLen, roomid, m_nGameID, 
				&RefreshResult, GameResults);

			int nIndex = 0;
			for(i = 0; i < pTable->m_nTotalChairs; i++)
			{
				if (pTable->m_ptrPlayers[i] && !(pTable->m_ptrPlayers[i]->m_bIdlePlayer))
				{
					pTable->RefreshOnePlayerData(&(GameResults[nIndex++]));
				}
			}
			
			TransmitGameResultEx(pTable, lpContext, &RefreshResult, GameResults, nPlayerCount * sizeof(GAME_RESULT_EX));
		}
		DataRecord::Instance()->UpdateInfos(pTable, &RefreshResult, GameResults);
		DataRecord::Instance()->UpdateRoomBout(roomid, pTable->m_nTableNO);
	}
	
	if (!bWaitCheckRult)
	{
		//不等待checksvr验证
		NotifyTablePlayers(pTable, GR_GAME_WIN, pData, nLen, 0, TRUE);
		NotifyTableVisitors(pTable, GR_GAME_WIN, pData, nLen, 0, TRUE);
		
		// add on 20130723 by thg
		SCORE_PROTECTOR spt; 
		memset(&spt,0,sizeof(spt));
		spt.dwFlags=FLAGS_SCOREPRT_MEMBER;
		if (pTable->IsMemberReduceNGScore()
			&& pTable->SetScoreProtect(spt.nUserID, spt.nScoreDiffs))
		{
			NotifyTablePlayers(pTable,  GR_SCORE_PROTECTOR, &spt, sizeof(spt), 0);
			NotifyTableVisitors(pTable, GR_SCORE_PROTECTOR, &spt, sizeof(spt), 0);
		}
		// add end

		// add on 20140213 by pengsy
		if (IsCloakingRoom(roomid))
		{	
			TCHAR szRoomID[16];
			memset(szRoomID, 0, sizeof(szRoomID));
			_stprintf(szRoomID, _T("%ld"), roomid);
			
			int max_table_bout = GetPrivateProfileInt(_T("maxtablebout"), szRoomID, MAX_TABLE_BOUT,	m_szIniFile);
			if (pTable->m_nBoutCount>=max_table_bout) 			
			{
				//已经达到桌局数上限，发送真实信息
				int nRealSize = pTable->m_nCloakRealSize;
				void* pRealData = new BYTE[nRealSize];
				memset(pRealData,0,nRealSize);
				memcpy(pRealData,pTable->m_pCloakRealData,nRealSize);
				
				NotifyTablePlayers(pTable,  GR_CLOAK_SOLOPLAYERREAL, pRealData, nRealSize, 0);
				//NotifyTableVisitors(pTable, GR_CLOAK_SOLOPLAYERREAL, pRealData, nRealSize, 0);		
				SAFE_DELETE_ARRAY(pRealData);
			}
		}
		// add end

		DWORD dwBreakTime[MAX_CHAIRS_PER_TABLE];
		memset(dwBreakTime, 0, sizeof(dwBreakTime));
		memcpy(dwBreakTime, pTable->m_dwBreakTime,sizeof(dwBreakTime));
		
		pTable->PrepareNextBout(pData, nLen);
		
		SAFE_DELETE(pData);

		if (pTable->IsGameOver())
		{
			PostGameBoutEnd(roomid, pTable->m_nTableNO, pTable);

			//Solo 独离不随机的房间，主动退掉已经掉线的玩家
			if (IsSoloRoom(roomid)
				&& IsLeaveAlone(roomid)
				&& !IsRandomRoom(roomid))
			{
				for (int i=0; i<pTable->m_nTotalChairs;i++)
				{
					if (pTable->m_ptrPlayers[i]
						&& dwBreakTime[i]>0)
					{
						RemoveOneClients(pTable,pTable->m_ptrPlayers[i]->m_nUserID,TRUE);
						EjectLeaveGameEx(pTable->m_ptrPlayers[i]->m_nUserID, roomid, pTable->m_nTableNO, 
							i, pTable->m_ptrPlayers[i]->m_hSocket, pTable->m_ptrPlayers[i]->m_lTokenID);
					}
				}
			}
		}
	}
	else
	{
		//等待checksvr验证
		StoreUpGameWin(pTable,pData,nLen,chairno);
	}

	return TRUE;
}

BOOL CGameServer::GetRoomSvrIniFile(int roomid, CString& sRoomSvrIniFile)
{
	if (roomid<=0)
		return FALSE;
	
	HWND hRoomSvrWnd = GetRoomSvrWnd(roomid);
	
	if (!hRoomSvrWnd) 
		return FALSE;
	
	TCHAR szTemp[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), szTemp, sizeof(szTemp));
	
	//获取游戏目录
	CString sFolder;
	sFolder.Format(_T("%s"), szTemp);
	int nIndex = sFolder.ReverseFind('\\');
	if (nIndex<0)
		return FALSE;
	sFolder = sFolder.Left(nIndex);
	nIndex = sFolder.ReverseFind('\\');
	if (nIndex<0)
		return FALSE;
	sFolder = sFolder.Left(nIndex+1);
	
	int hWnd = 0;
	
	for (int i=0;i<16;i++)
	{
		if (i==0)
			sRoomSvrIniFile.Format(_T("%sroomsvr\\roomsvr.ini"), sFolder);
		else
			sRoomSvrIniFile.Format(_T("%sroomsvr%d\\roomsvr.ini"), sFolder, i+1);
		
		memset(szTemp, 0, sizeof(szTemp));
		GetPrivateProfileString(_T("listen"), _T("hwnd"), _T(""),szTemp,sizeof(szTemp) ,sRoomSvrIniFile);
		if (strlen(szTemp)>0)
		{	
			hWnd = atoi(szTemp);
			if (hWnd == (int)hRoomSvrWnd)
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

int	 CGameServer::GetTableDepositMin(int roomid, int tableno)
{
	CString sRoomSvrIniFile;
	if (!GetRoomSvrIniFile(roomid, sRoomSvrIniFile))
		return -1;
	
	TCHAR szSection[32];
	memset(szSection,0,sizeof(szSection));
	sprintf(szSection,"TableDeposit%d",roomid);
	
	int nCount = GetPrivateProfileInt(szSection,_T("Count"),0, sRoomSvrIniFile);
	if(nCount<=0 || nCount>32)
		return -1;
	
	CString sTmp;
	for(int i=0;i<nCount;i++)
	{
		TCHAR szValue[64];
		memset(szValue,0,sizeof(szValue));
		sTmp.Format(_T("%d"),i);
		GetPrivateProfileString(szSection,sTmp,_T(""),szValue,sizeof(szValue) ,sRoomSvrIniFile);	
		if(szValue[0]==0)
			return -1;
		TCHAR *fields[8];
		memset(fields,0,sizeof(fields));
		TCHAR *p1, *p2;
		p1=szValue;
		int nFields=Svr_RetrieveFields ( p1, fields, 8, &p2 ) ;
		if(nFields<3)
			return FALSE;
		
		int nTableNO1=atoi(fields[0]);
		int nTableNO2=atoi(fields[1]);
		int nMinDeposit=atoi(fields[2]);
		if(tableno>=nTableNO1 && tableno<=nTableNO2)
			return nMinDeposit;	
	}
	
	return 0;
}

int CGameServer::GetMinPlayingDeposit(CTable *pTable, int roomid)
{
	TCHAR szRoomID[16];
	memset(szRoomID, 0, sizeof(szRoomID));
	_stprintf(szRoomID, _T("%ld"), pTable->m_nRoomID);
	
	int deposit_min = GetPrivateProfileInt(
		_T("deposit"),			// section name
		_T("min"),				// key name
		MIN_DEPOSIT_NEED,		// default int
		m_szIniFile				// initialization file name
		);
	int fee_mode = GetPrivateProfileInt(
		_T("feemode"),	// section name
		szRoomID,		// key name
		FEE_MODE_TEA,	// default int
		m_szIniFile		// initialization file name
		);
	int fee_value = GetPrivateProfileInt(
		_T("feevalue"),	// section name
		szRoomID,		// key name
		1,				// default int
		m_szIniFile		// initialization file name
		);
	
	//如果是服务费模式，银子的最低要求为deposit_min+服务费
	if (FEE_MODE_SERVICE_FIXED == fee_mode
		&& IsNeedDepositRoom(roomid))
	{
		deposit_min += fee_value;
	}
	
	//判断桌子银两最低要求
	int nMinTblDeposit = 0;
	if (IsTableDepositRoom(roomid))
	{
		nMinTblDeposit = GetTableDepositMin(roomid, pTable->m_nTableNO);	//取桌银限制
	}
	if (nMinTblDeposit>deposit_min)
	{
		deposit_min = nMinTblDeposit;
	}

	int nMin = GetMinDeposit(roomid);
	if(nMin > deposit_min)
		deposit_min = nMin;
	
	return deposit_min;
}

BOOL CGameServer::IsTableDepositRoom(int roomid)
{
	return IS_BIT_SET(GetRoomManage(roomid),RM_TABLEDEPOSIT);
}

BOOL CGameServer::OnStartGameEx(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt)
{
	REQUEST response;
	memset(&response, 0, sizeof(response));

	LONG token = 0;
	int roomid = 0;
	int tableno = INVALID_OBJECT_ID;
	int userid = 0;
	int chairno = INVALID_OBJECT_ID;
	int r_id = 0;
	int t_no = INVALID_OBJECT_ID;
	int u_id = 0;
	int c_no = INVALID_OBJECT_ID;
	
	CRoom* pRoom = NULL;
	CTable* pTable = NULL;
	CPlayer* pPlayer = NULL;
	
	token = lpContext->lTokenID;

	LPSTART_GAME pStartGame = LPSTART_GAME(PBYTE(lpRequest->pDataPtr));
	roomid = pStartGame->nRoomID;
	tableno = pStartGame->nTableNO;
	userid = pStartGame->nUserID;
	chairno = pStartGame->nChairNO;

	BOOL bPassive = IS_BIT_SET(lpContext->dwFlags,CH_FLAG_SYSTEM_EJECT);
	response.head.nRequest = UR_OPERATE_FAILED;

	{
		CAutoLock lock(&m_csRoomMap);

		if(!m_mapRoom.Lookup(roomid, pRoom)){
			return SendUserResponse(lpContext, &response,bPassive);
		}
	}
	if(pRoom){
		CAutoLock lock(&(pRoom->m_csRoom));

		if(!pRoom->m_mapTable.Lookup(tableno, pTable)){
			return SendUserResponse(lpContext, &response,bPassive);
		}
	}
	if(pTable) {
		LOCK_TABLE(pTable, chairno, FALSE, userid, token);
		
		if(!pTable->IsPlayer(userid)){ // 不是玩家
			UwlLogFile(_T("user not player. user %ld start game failed."), userid);
			return SendUserResponse(lpContext, &response,bPassive);
		}

		if(IS_BIT_SET(pTable->m_dwStatus, TS_PLAYING_GAME)){ // 游戏进行中
			return SendUserResponse(lpContext, &response,bPassive);
		}

		if(IS_BIT_SET(pTable->m_dwUserStatus[chairno], US_GAME_STARTED)){ // 已经开始
			return SendUserResponse(lpContext, &response,bPassive);
		}

		if(IS_BIT_SET(pTable->m_dwUserStatus[chairno], US_USER_WAITNEWTABLE)){ // 已经进入分桌队列
			return SendUserResponse(lpContext, &response,bPassive);
		}

		BOOL bSoloRoom=IsSoloRoom(roomid);
		BOOL bRandomRoom=IsRandomRoom(roomid);
		
		if(!VerifyRoomTableChair(roomid, tableno, chairno, userid)){ // 该位置上用户不匹配
			return SendUserResponse(lpContext, &response,bPassive);
		}
		//
		if (bSoloRoom) 
		{			
			if (IsLeaveAlone(roomid))
			{
				//solo独离房, 按下开始时, 判断银子是否在房间限定范围内,否则返回超出银两值				
				//add by thg, 20130802
				pPlayer=pTable->m_ptrPlayers[chairno];
				if (pPlayer && IsNeedDepositRoom(roomid))
				{
					//检查银两最低要求
					BOOL bDepositNotEnough = FALSE;
					int nMinDeposit = 0;
					if (IsCheckDepositMin(roomid)) 
					{
						nMinDeposit=GetMinPlayingDeposit(pTable, roomid); 
						if (pPlayer->m_nDeposit < nMinDeposit) //银子不够
						{
							bDepositNotEnough = TRUE;
						}
					}
					else
					{
						nMinDeposit = GetMinPlayingDeposit(pTable, roomid);
						if (pPlayer->m_nDeposit < nMinDeposit) //银子不够
						{
							bDepositNotEnough = TRUE;
						}
					}
					if (bDepositNotEnough)
					{
						DEPOSIT_NOT_ENOUGH dne;
						memset(&dne, 0, sizeof(dne));
						dne.nUserID = userid;
						dne.nChairNO = chairno;
						dne.nDeposit = pPlayer->m_nDeposit;
						dne.nMinDeposit = nMinDeposit;
						
						response.head.nRequest = GR_RESPONE_DEPOSIT_NOTENOUGH;
						response.nDataLen = sizeof(dne);
						response.pDataPtr = &dne;
						return SendUserResponse(lpContext,&response);
					}
					
					//检查银两是否超出
					if (IsCheckDepositMax(roomid))
					{
						int nMaxDeposit=GetMaxDeposit(roomid); //银子超出						
						if (nMaxDeposit < pPlayer->m_nDeposit)
						{
							DEPOSIT_TOO_HIGH dth;
							memset(&dth, 0, sizeof(dth));
							dth.nUserID = userid;
							dth.nChairNO = chairno;
							dth.nDeposit = pPlayer->m_nDeposit;
							dth.nMaxDeposit = nMaxDeposit;
							
							response.head.nRequest = GR_RESPONE_DEPOSIT_TOOHIGH;
							response.nDataLen = sizeof(dth);
							response.pDataPtr = &dth;
							return SendUserResponse(lpContext,&response,bPassive);
						}
					}
				}
			}
			
			if (bRandomRoom
				&& (pTable->IsFirstBout()
				||(IsLeaveAlone(roomid)
				&&IsNeedWaitArrageTable(pTable,roomid,userid))))//已经达到桌局数上限，那么重新向RoomSvr请求分桌
			{
				pTable->m_dwUserStatus[chairno]|=US_USER_WAITNEWTABLE; 
				PostAskNewTable(userid,roomid,tableno,chairno);
				
				pPlayer=pTable->m_ptrPlayers[chairno];
				if (pPlayer)
				{
					NotifyOneUser(pPlayer->m_hSocket,pPlayer->m_lTokenID,GR_WAIT_NEWTABLE,NULL,0);
				}
				
				response.head.nRequest = UR_OPERATE_SUCCEEDED;
				return SendUserResponse(lpContext, &response,bPassive);
			}
		}
		//
		if (IS_BIT_SET(GetGameOption(roomid),GO_NOT_VERIFYSTART))
		{
			OnUserStart(pTable,chairno);
		}
		else
		{
			PostVerifyStart(userid,roomid,tableno,chairno);
		}
	}

	response.head.nRequest = UR_OPERATE_SUCCEEDED;
    return SendUserResponse(lpContext, &response,bPassive);
}


//按额度配桌相关
//..../gamesvr/xxx.exe
//..../roomsvr/roomsvr.ini
//..../roomsvr2/roomsvr.ini
//			...
//..../roomsvr16/roomsvr.ini
BOOL CGameServer::ReadTableGrade(int nRoomID,TABLEGRADEALL *pTableGrade)
{
	CString sRoomSvrIniFile;
	if (!GetRoomSvrIniFile(nRoomID, sRoomSvrIniFile))
		return FALSE;
	
	TCHAR szSection[32];
	memset(szSection,0,sizeof(szSection));
	sprintf(szSection,"TableDeposit%d",nRoomID);
	
	int nCount = GetPrivateProfileInt(szSection,_T("Count"),0, sRoomSvrIniFile);
	if(nCount<=0 || nCount>MAX_TABLE_GRADE)
		return FALSE;

	pTableGrade->nCount = nCount;
	
	CString sTmp;
	for(int i=0;i<nCount;i++)
	{
		TCHAR szValue[64];
		memset(szValue,0,sizeof(szValue));
		sTmp.Format(_T("%d"),i);
		GetPrivateProfileString(szSection,sTmp,_T(""),szValue,sizeof(szValue) ,sRoomSvrIniFile);	
		if(szValue[0]==0)
			return FALSE;
		TCHAR *fields[8];
		memset(fields,0,sizeof(fields));
		TCHAR *p1, *p2;
		p1=szValue;
		int nFields=Svr_RetrieveFields ( p1, fields, 8, &p2 ) ;
		if(nFields<3)
			return FALSE;
		
		int nTableNO1=atoi(fields[0]);
		int nTableNO2=atoi(fields[1]);
		int nMinDeposit=atoi(fields[2]);
		pTableGrade->grade[i].nStartTable = nTableNO1;
		pTableGrade->grade[i].nEndTable = nTableNO2;
		pTableGrade->grade[i].nMinDeposit = nMinDeposit;
	}

	return TRUE;
}
//进入游戏的时候才发
void CGameServer::NotifyTableGrade(int nRoomID,SOCKET sock, LONG token)//读取配置
{
	TABLEGRADEALL gradeAll;
	memset(&gradeAll,0,sizeof(gradeAll));
	if(ReadTableGrade(nRoomID,&gradeAll))
	{	
		CRoom *pRoom = NULL;
		if(!m_mapRoom.Lookup(nRoomID, pRoom))
		{
			return;
		}
		
		
		for (int i = 0; i < gradeAll.nCount;i++)
		{
			for(int tableno = gradeAll.grade[i].nStartTable; tableno <= gradeAll.grade[i].nEndTable;tableno++)
			{
				CTable *pTable = NULL;
				if(!pRoom->m_mapTable.Lookup(tableno, pTable))
				{
					continue;
				}

				if (pTable)
				{
					gradeAll.grade[i].nPlayerCount += pTable->GetPlayerCount();
				}	
			}				
		}
		
		int nSize = sizeof(int) + gradeAll.nCount * sizeof(TABLEGRADE);
		NotifyOneUser(sock,token,TCSG_S2C_NEWTABLE_GRADE,&gradeAll,nSize);
	}
}


void CGameServer::PostAskNewTableByTableGrade(int nUserID,int nRoomID,int nTableNO,int nIndex)
{
	HWND hWnd=GetRoomSvrWnd(nRoomID);
	if(IsWindow(hWnd))
	{
		DWORD dwRoomTableIndex = MakeRoomTableChair(nRoomID,nTableNO,nIndex);//把原来的chairNo,替换为银两等级
		PostMessage(hWnd,WM_GTR_CHANGE_SPECIFYTABLE_EX,dwRoomTableIndex,nUserID);
	}
}
BOOL CGameServer::OnEnterGameDXXW(LPCONTEXT_HEAD lpContext, LPENTER_GAME_EX pEnterGame)
{
	NotifyTableGrade(pEnterGame->nRoomID,lpContext->hSocket,lpContext->lTokenID);

	SendWelfareActivityID(lpContext->hSocket,lpContext->lTokenID);
				
	SendChargeUrl(lpContext->hSocket,lpContext->lTokenID);
	
	SendIsTongbiRoom(pEnterGame->nRoomID,lpContext->hSocket,lpContext->lTokenID);

	return TRUE;
}


BOOL  CGameServer::OnPlayerEnterGame(LPCONTEXT_HEAD lpContext,LPENTER_GAME_OK_EX lpEnterGameEx,LPREQUEST lpReqToClient)
{
	int roomid=lpEnterGameEx->nRoomID;
	int userid=lpEnterGameEx->nUserID;
	int usertype=lpEnterGameEx->nUserType;
	int tableno=lpEnterGameEx->nTableNO;
	int chairno=lpEnterGameEx->nChairNO;
	int token=lpContext->lTokenID;
	int sock=lpContext->hSocket;
	DWORD auto_startgame = 0;

	CRoom* pRoom = NULL;
	CTable* pTable = NULL;
	CPlayer* pPlayer = NULL;
	{
		CAutoLock lock(&m_csRoomMap);
		
		if(!m_mapRoom.Lookup(roomid, pRoom)){
			return TRUE;
		}
	}

	if(pRoom){
		CAutoLock lock(&(pRoom->m_csRoom));
		
		if(!pRoom->m_mapTable.Lookup(tableno, pTable)){
			return TRUE;
		}
	}	

	if (pTable)
	{	
		CAutoLock lock(&(pTable->m_csTable));
		
		if(IS_BIT_SET(pTable->m_dwRoomOption[chairno], RO_AUTO_STARTGAME)){ //
			auto_startgame = 1;
		}

		if(pTable->IsGameOver())
			ResetTableBanker(pTable,chairno);

		pPlayer = pTable->m_ptrPlayers[chairno];
		if(pPlayer)
		{
			userid = pPlayer->m_nUserID;
			
			sock = pPlayer->m_hSocket;
			token = pPlayer->m_lTokenID;
			
			pPlayer->m_nDeposit = lpEnterGameEx->nDeposit;
			pPlayer->m_nScore = lpEnterGameEx->nScore;
			pPlayer->m_nLevelID = lpEnterGameEx->nLevelID;
			pPlayer->m_nUserType = lpEnterGameEx->nUserType;
			pPlayer->m_nExperience = lpEnterGameEx->nExperience;
			pPlayer->m_nBout = lpEnterGameEx->nBout;

			int nUserDataLen=CalcEnterUserDatelen(pRoom,pTable,pPlayer);
			
			int nInfoLen = 0;
			BYTE* pSendClient=NULL;
			if (IsVariableChairRoom(roomid)
				&& IS_BIT_SET(pTable->m_dwStatus, TS_PLAYING_GAME))
			{
				lpReqToClient->head.nRequest = GR_RESPONE_ENTER_GAME_IDLE;
				nInfoLen = pTable->GetGameTableInfoSize(); 
				lpReqToClient->nDataLen = nInfoLen + nUserDataLen;
				pSendClient=new BYTE[lpReqToClient->nDataLen];
				ZeroMemory(pSendClient, lpReqToClient->nDataLen);
									
				//GameTableInfo
				pTable->FillupGameTableInfo(pSendClient, nInfoLen, chairno, FALSE);
			}
			else
			{
				lpReqToClient->head.nRequest=GR_RESPONE_ENTER_GAME_OK;
				nInfoLen = pTable->GetEnterGameInfoSize();
				lpReqToClient->nDataLen = nInfoLen + nUserDataLen;
				pSendClient=new BYTE[lpReqToClient->nDataLen];
				ZeroMemory(pSendClient, lpReqToClient->nDataLen);
									
				//EnterInfo
				pTable->FillupEnterGameInfo(pSendClient, nInfoLen, chairno, FALSE);
			}
			
			CopyEnterUserData(pRoom,pTable,pPlayer,pSendClient,nInfoLen);
						
			lpReqToClient->pDataPtr=pSendClient;
			SendUserResponse(lpContext, lpReqToClient, FALSE, TRUE);				
			
			if (IsVariableChairRoom(roomid)
				&& IS_BIT_SET(pTable->m_dwStatus, TS_PLAYING_GAME))
			{
				//...
			}
			else
			{
				//?ó??standby???￠￡???ààòaó?μ?
				LPALL_STANDBY ptr_all_standby = NULL;
				int len_all_standby = sizeof(ALL_STANDBY) + sizeof(STANDBY_OK) * pTable->m_nTotalChairs;
				ptr_all_standby = LPALL_STANDBY(new BYTE[len_all_standby]);
				memset(ptr_all_standby, 0, len_all_standby);
				//
				pTable->m_dwFirstEnter[chairno] = GetTickCount();
				pTable->m_dwUserStatus[chairno] |= US_USER_ENTERED;
				
				//???ú·???è?ó??·?í?óè?US_USER_WAITNEWTABLE×′ì?
				if (IsRandomRoom(roomid)) 
					pTable->m_dwUserStatus[chairno] |= US_USER_WAITNEWTABLE;
				
				if(pTable->IsAllStandBy(ptr_all_standby, m_mapPlayerLevel)){
					NotifyTablePlayers(pTable, GR_ALL_STANDBY, ptr_all_standby, len_all_standby);
					NotifyTableVisitors(pTable, GR_ALL_STANDBY, ptr_all_standby, len_all_standby);
				}
				SAFE_DELETE_ARRAY(ptr_all_standby);
			}

			
			//Solo模式中通知其他客户端
			if (IsSoloRoom(roomid))
			{
				//Solo模式中通知其他客户端
				SOLO_PLAYER sp;
				memset(&sp,0,sizeof(sp));
				
				if (LookupSoloPlayer(userid,sp))
				{
					if (!IsCloakingRoom(roomid)) 
						NotifyPlayersSomeoneEnter(pTable,&sp,token);//modify 2014.1.27 by pgl
								
					NotifyVistorsSomeoneEnter(pTable,&sp,token);//modify 2014.1.27 by pgl
				}
			}
			
			//发送排行榜网址
			NotifyMatchWWW(roomid,pPlayer->m_nUserID,pPlayer->m_hSocket,pPlayer->m_lTokenID);

			NotifyHandPhoneQRCodeURL(roomid,pPlayer->m_nUserType,pPlayer->m_hSocket,pPlayer->m_lTokenID);
			
			//通知房间,1.0模式的延续
			OnGameEntered(pTable, userid, roomid, tableno, chairno, sock, token);

			//开始游戏倒计时
			if (IsVariableChairRoom(roomid))
			{
				if (pTable->IsGameOver()
					&& pTable->IsInCountDown())
				{
					START_COUNTDOWN  sc;
					memset(&sc,0,sizeof(START_COUNTDOWN));
					sc.nUserID = userid;
					sc.nRoomID = roomid;
					sc.nTableNO = tableno;
					sc.nChairNO = chairno;
					sc.bStartorStop = TRUE;
					sc.nCountDown = pTable->CalcRemainCountdown(GetTickCount());
						NotifyOneUser(pPlayer->m_hSocket, pPlayer->m_lTokenID, GR_START_COUNTDOWN, &sc, sizeof(START_COUNTDOWN));
				}
			}

			//发送桌椅信息
			NotifyTableGrade(roomid,pPlayer->m_hSocket,pPlayer->m_lTokenID); 

			SendWelfareActivityID(pPlayer->m_hSocket,pPlayer->m_lTokenID);
			
			SendChargeUrl(pPlayer->m_hSocket,pPlayer->m_lTokenID);

			SendIsTongbiRoom(roomid,lpContext->hSocket,lpContext->lTokenID);
		}
		else
		{
			lpReqToClient->head.nRequest = UR_OPERATE_FAILED;
			SendUserResponse(lpContext, lpReqToClient, FALSE, FALSE);
		}
	}
	else {
		lpReqToClient->head.nRequest = UR_OPERATE_FAILED;
		SendUserResponse(lpContext, lpReqToClient, FALSE, FALSE);
	}

	if(GR_RESPONE_ENTER_GAME_OK == lpReqToClient->head.nRequest && auto_startgame){  
		EjectStartGame(userid, roomid, tableno, chairno, sock, token);
	}

	return TRUE;
}

void CGameServer::SendWelfareActivityID(SOCKET hSocket,LONG lTokenID)
{
	int nActivityID = GetPrivateProfileInt(_T("ActivityID"),"ID",0,m_szIniFile);
	if(nActivityID>0)//有活动才发。
	{
		NotifyOneUser(hSocket,lTokenID,TCSG_WELFAREACTIVITYID,&nActivityID,sizeof(int));
	}
}

void CGameServer::SendIsTongbiRoom(int roomid,SOCKET hSocket,LONG lTokenID)
{
	TCHAR szRoomID[16];
	memset(szRoomID, 0, sizeof(szRoomID));
	_stprintf(szRoomID, _T("%ld"), roomid);
	
	int nTongBiRoom=GetPrivateProfileInt(_T("TongBiRoom"),szRoomID,0,m_szIniFile);
	NotifyOneUser(hSocket,lTokenID,TCSG_ISTONGBIROOM,&nTongBiRoom,sizeof(int));
}

BOOL CGameServer::OnChatToTableEx(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt)
{
	SOCKET sock = INVALID_SOCKET;
	LONG token = 0;
	int roomid = 0;
	int tableno = INVALID_OBJECT_ID;
	int userid = 0;
	int chairno = INVALID_OBJECT_ID;
	int r_id = 0;
	int t_no = INVALID_OBJECT_ID;
	int u_id = 0;
	int c_no = INVALID_OBJECT_ID;
	BOOL lookon = FALSE;
	
	CRoom* pRoom = NULL;
	CTable* pTable = NULL;
	CPlayer* pPlayer = NULL;
	
	token = lpContext->lTokenID;
	sock = lpContext->hSocket;
	
	LPCHAT_TO_TABLE pChatToTable = LPCHAT_TO_TABLE(PBYTE(lpRequest->pDataPtr));
	roomid = pChatToTable->nRoomID;
	tableno = pChatToTable->nTableNO;
	userid = pChatToTable->nUserID;
	chairno = pChatToTable->nChairNO;
	{
		CAutoLock lock(&m_csRoomMap);
		
		if(!m_mapRoom.Lookup(roomid, pRoom)){
			return TRUE;
		}
	}
	if(pRoom){
		CAutoLock lock(&(pRoom->m_csRoom));
		
		if(!pRoom->m_mapTable.Lookup(tableno, pTable)){
			return TRUE;
		}
	}
	if(pTable) 
	{
		LOCK_TABLE(pTable, chairno, FALSE, userid, token);

		if(lstrlen(pChatToTable->szChatMsg) > (MAX_CHATMSG_LEN-2) )
			return FALSE;
		
		int nMinExp=0;
		BOOL bAllowed=TRUE;
  		CString sForbid;

		CHAT_FROM_TABLE cft;
		memset(&cft,0,sizeof(cft));
		cft.nRoomID = roomid;
		cft.nUserID = userid;

		DWORD dwRoomOption=GetRoomOption(roomid);

		if(IS_BIT_SET(dwRoomOption,RO_FORBID_CHAT))// 禁止聊天
		{ 
			bAllowed=FALSE;
 			sForbid.Format(_T(" 本游戏禁止发消息"));
		}
		else if(pTable->IsForbidLookChat() && pTable->IsVisitor(userid))// 禁止旁观者聊天
		{ 
	 		bAllowed=FALSE;
 			sForbid.Format(_T(" 本游戏禁止旁观者发消息"));
		}
		else if(pTable->IsDarkRoom())// 隐名房间
		{ 
 			bAllowed=FALSE;
 			sForbid.Format(_T(" 本游戏禁止发消息"));
		}			
		else if(IsForbidChatByExp(pTable,userid,nMinExp))
		{
 			bAllowed=FALSE;
 			sForbid.Format(_T(" 新手(经验值＜%d分钟)禁止发消息"),nMinExp);
		}
		else if (pTable->IsVisitor(userid)/*&&((nRet=VerifyVistorSpeak(userid,roomid,tableno))<0)*/)
		{
			//验证旁观发言
			int nRet = VerifyVistorSpeak(userid,roomid,tableno,pChatToTable->szChatMsg);
			if (-1==nRet)
			{
				bAllowed=FALSE;
				sForbid.Format(_T(" 您说话太快了，请休息一会儿"));
			}
			else if (-2==nRet)
			{
				bAllowed=FALSE;
				sForbid.Format(_T(" 进入房间10分钟后，才可以旁观时发言"));
			}
			else if (-3==nRet)
			{
				bAllowed=FALSE;
				sForbid.Format(_T(" 禁止发送违禁单词或符号"));
			}
		}
 	 	else if(IsIncludeFilterWord(pChatToTable->szChatMsg))
		{
 			bAllowed=FALSE;
 			sForbid.Format(_T(" 禁止发送违禁单词或符号"));
		}
		else 
		{
		}
		 
		if(bAllowed==FALSE )
		{
 			sprintf(cft.szChatMsg,_T("%s%s\r\n"),GetChatTags(FLAG_CHAT_SYSNOTIFY),sForbid);
 			cft.nMsgLen = lstrlen(cft.szChatMsg)+1;  
 			cft.dwFlags	= FLAG_CHAT_SYSNOTIFY;
  			int nSendLen=sizeof(CHAT_FROM_TABLE)-sizeof(cft.szChatMsg)+cft.nMsgLen;
 			NotifyOneUser(sock,token,GR_CHAT_FROM_TALBE,&cft,nSendLen);
			return TRUE;
		}

		//Add on 20130122
		//记录旁观发言时间地点人物
		if (pTable->IsVisitor(userid))
		{
			SetVisitorLastSpeakInfo(userid,roomid,tableno);
		}
		//Add end
		
		sprintf(cft.szChatMsg,_T("%s%s\r\n"),GetChatTags(FLAG_CHAT_PLAYERMSG),pChatToTable->szChatMsg);
  		cft.nMsgLen=lstrlen(cft.szChatMsg)+1;
  		cft.dwFlags=FLAG_CHAT_PLAYERMSG;
		int nSendLen=sizeof(CHAT_FROM_TABLE)-sizeof(cft.szChatMsg)+cft.nMsgLen;
		cft.nReserved[0] = pChatToTable->nReserved[0];
		cft.nReserved[1] = pChatToTable->nReserved[1];
		cft.nReserved[2] = pChatToTable->nReserved[2];

  		NotifyTablePlayers(pTable, GR_CHAT_FROM_TALBE, &cft,nSendLen);
		NotifyTableVisitors(pTable, GR_CHAT_FROM_TALBE,&cft, nSendLen);
	}
	return TRUE;
}

void CGameServer::CopyWaitResultMap(CWaitResultMap& mapWaitResult)
{
	CAutoLock lock(&m_csWaitResult);
	
	int client = 0;
	DWORD key=0;
	POSITION pos = m_mapWaitResult.GetStartPosition();
	while (pos)	{
		WAIT_FRESHRESULT wf;
		m_mapWaitResult.GetNextAssoc(pos, key, wf);
		mapWaitResult.SetAt(key, wf);
	}
}

void CGameServer::RemoveWaitResultByKey(DWORD dwKey)
{
	CAutoLock lock(&m_csWaitResult);
	m_mapWaitResult.RemoveKey(dwKey);
}

void CGameServer::CheckGameWin()
{
	DWORD t=GetTickCount();
	if (t-m_dwLastCheckGameWin<1000)
		return;
	
	m_dwLastCheckGameWin=t;
	
	CWaitResultMap mapWaitResult;
	CopyWaitResultMap(mapWaitResult);
	
	if (mapWaitResult.GetCount()==0)
		return;
	
	WAIT_FRESHRESULT wf;
	memset(&wf, 0, sizeof(wf));
	
	DWORD key=0;
	POSITION pos = mapWaitResult.GetStartPosition();
	while(pos){
		mapWaitResult.GetNextAssoc(pos, key, wf);
		if (key/1000==wf.nRoomID
			&&key%1000==wf.nTableNO
			&&t-wf.dwTickCount>=5000)
		{
			
			CRoom* pRoom = NULL;
			CTable* pTable = NULL;
			{
				CAutoLock lock(&m_csRoomMap);
				
				if(!m_mapRoom.Lookup(wf.nRoomID, pRoom)){
					RemoveWaitResultByKey(key);
					continue;
				}
			}
			if(pRoom){
				CAutoLock lock(&(pRoom->m_csRoom));
				
				if(!pRoom->m_mapTable.Lookup(wf.nTableNO, pTable)){
					RemoveWaitResultByKey(key);
					continue;
				}
			}	
			if(pTable) {
				CAutoLock lock(&(pTable->m_csTable));
				NotifyClientGameWin(pTable);
			}
		}
		
	}
}

UINT CGameServer::GetThreadID_ChunkConnect()
{
	return m_uiThrdConnectChunk;
}

BOOL CGameServer::Chunk_BaseVerify(int userid, int gameid)
{
	if (userid <= 0 || gameid <= 0 || gameid >= MAX_GAME_NUM)
		return FALSE;
	
	return TRUE;
}

BOOL CGameServer::Chunk_TransmitRequest(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest)
{
	LPREQUEST pRequest = new REQUEST;
	memcpy(pRequest, lpRequest, sizeof(REQUEST));
	
	int nDataLen = lpRequest->nDataLen;
	pRequest->pDataPtr = new BYTE[nDataLen];
	pRequest->nDataLen = nDataLen;
	memcpy(pRequest->pDataPtr, lpRequest->pDataPtr, nDataLen);
	
	LPCONTEXT_HEAD pContext = new CONTEXT_HEAD;
	memcpy(pContext, lpContext, sizeof(CONTEXT_HEAD));
	
	if(!PostThreadMessage(m_uiThrdSendChunk, UM_DATA_TOSEND, (WPARAM)pContext, (LPARAM)pRequest)){
		UwlClearRequest(pRequest);
		SAFE_DELETE(pRequest);
		SAFE_DELETE(pContext);
		return FALSE;
	}else{
		return TRUE;
	}
}

void CGameServer::Chunk_SendPulseData()
{
	if (!m_pSockClitChunk) return;
	
	REQUEST request;
	memset(&request, 0, sizeof(request));
	REQUEST response;
	memset(&response, 0, sizeof(response));
	
	CONTEXT_HEAD context;
	memset(&context, 0, sizeof(context));
	context.hSocket = m_pSockClitChunk->GetSocket();
	
	request.head.nRequest = GR_SEND_PULSE_EX;
	
	BOOL bSendOK = m_pSockClitChunk->SendCast(&context, &request, &response);
}

unsigned CGameServer::Chunk_SendThreadFunc(LPVOID lpVoid)
{
	CGameServer * pThread = (CGameServer *) lpVoid;
	
    return pThread->Chunk_SendThreadProc();
} 

unsigned CGameServer::Chunk_ConnectThreadFunc(LPVOID lpVoid)
{
	CGameServer * pThread = (CGameServer *) lpVoid;
	
    return pThread->Chunk_ConnectThreadProc();
} 

unsigned CGameServer::Chunk_ServerPulseThreadFunc(LPVOID lpVoid)
{
	CGameServer * pThread = (CGameServer *) lpVoid;
	
    return pThread->Chunk_ServerPulseThreadProc();
}

unsigned CGameServer::Chunk_TimerThreadFunc(LPVOID lpVoid)
{
	CGameServer * pThread = (CGameServer *) lpVoid;
	
    return pThread->Chunk_TimerThreadProc();
} 

unsigned CGameServer::Chunk_SendThreadProc()
{
	UwlTrace(_T("chunk_send thread started. id = %d"), GetCurrentThreadId());
	
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		if(UM_DATA_TOSEND == msg.message)
		{
			LPCONTEXT_HEAD pContext = LPCONTEXT_HEAD(msg.wParam);
			LPREQUEST pRequest = LPREQUEST(msg.lParam);
			
			int nLen = sizeof(CONTEXT_HEAD) + pRequest->nDataLen;
			PBYTE pData = new BYTE[nLen];
			memcpy(pData, pContext, sizeof(CONTEXT_HEAD));
			memcpy(pData + sizeof(CONTEXT_HEAD), pRequest->pDataPtr, pRequest->nDataLen);
			
			REQUEST request;
			memset(&request, 0, sizeof(request));
			request.nDataLen = nLen;
			request.pDataPtr = pData;
			request.head.nRequest = pRequest->head.nRequest;
			request.head.nRepeated = 1;
			
			UwlClearRequest(pRequest);
			SAFE_DELETE(pContext);
			SAFE_DELETE(pRequest);
			
			CONTEXT_HEAD context;
			memset(&context, 0, sizeof(context));
			
			BOOL bTimeout = FALSE;
			BOOL bSendOK = m_pSockClitChunk->SendRequest(&context, &request, NULL, bTimeout);
			
			UwlClearRequest(&request);
		}else{
			DispatchMessage(&msg);
		}
	}
	UwlTrace(_T("chunk_send thread exiting. id = %d"), GetCurrentThreadId());
   	return 0;
} 


unsigned CGameServer::Chunk_ConnectThreadProc()
{
	UwlTrace(_T("chunk_connect thread started. id = %d"), GetCurrentThreadId());
	
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		if(UM_DATA_TOSEND == msg.message)
		{
			if(UR_SOCKET_CLOSE == msg.wParam)
			{
				Chunk_SetIsConnectChunkSvr(FALSE);
				while(TRUE)
				{
					m_pSockClitChunk->CloseSockets();					
					ReadChunkSvrFromProfile();
					
					if(FALSE == m_pSockClitChunk->BeginConnect(m_szChunkServer, PORT_OF_CHUNKSVR, 1))
					{
						UwlTrace(_T("Can not connect to chunk server."));
						UwlLogFile(_T("Can not connect to chunk server."));
						if(!m_hExitServer || 
							WAIT_OBJECT_0 == WaitForSingleObject(m_hExitServer, 0)){
							break;
						}
					}else{
						UwlTrace(_T("reconnect to chunk server OK."));
						UwlLogFile(_T("reconnect to chunk server OK."));
						Chunk_ValidateClientInfo(m_nGameID);
						break;
					}
				}
				Chunk_SetIsConnectChunkSvr(TRUE);
			}
			else if(UR_RECONNECT_SVR == msg.wParam)
			{
				m_pSockClitChunk->CloseSockets();	
				ReadChunkSvrFromProfile();	
				
				if(FALSE == m_pSockClitChunk->BeginConnect(m_szChunkServer, PORT_OF_CHUNKSVR, 1))
				{
					UwlTrace(_T("Can not reconnect to chunk server as received cmd."));
					UwlLogFile(_T("Can not reconnect to chunk server as received cmd."));
				}else{
					UwlTrace(_T("reconnect to chunk server OK as received cmd."));
					UwlLogFile(_T("reconnect to chunk server OK as received cmd."));					
					Chunk_ValidateClientInfo(m_nGameID);
				}
			}					
		}
		else
		{
			DispatchMessage(&msg);
		}
	}
	UwlTrace(_T("chunk_connect thread exiting. id = %d"), GetCurrentThreadId());
   	return 0;
}

unsigned CGameServer::Chunk_ServerPulseThreadProc()
{
	UwlTrace(_T("chunksvr pulse thread started. id = %d"), GetCurrentThreadId());
	
	//重连失败大概耗时31秒左右，这样定时间隔60秒，即可实现重连失败之后定时重连
	while(TRUE)
	{
		int interval = GetPrivateProfileInt(_T("ServerPulse"), _T("Interval"), DEF_SERVERPULSE_INTERVAL, m_szIniFile);
		DWORD dwRet = WaitForSingleObject(m_hExitServer, interval  * 1000);
		if(WAIT_OBJECT_0 == dwRet)// exit event
			break;
		
		if(WAIT_TIMEOUT == dwRet)// timeout
		{ 	
			int nCheck = GetPrivateProfileInt(_T("ServerPulse"), _T("Check"),1, m_szIniFile);//默认=1表示要检查
			if(nCheck==0)
				continue;
			
			CAutoLock lock(&m_csChunkSvrPulseInfo);
			int nLatest=m_ChunkSvrPulseInfo.nLatestTime;
			
			if(nLatest!=0)
			{
				int nNow=UwlGetCurrentSeconds();
				if(nNow-nLatest > interval )
				{
					int nDate,nTime;
					UwlGetCurrentDateTime(nDate,nTime);
					
					if(m_ChunkSvrPulseInfo.nCurrentDate == nDate )
					{
						m_ChunkSvrPulseInfo.nReconnectCount ++;
					}
					else
					{
						m_ChunkSvrPulseInfo.nCurrentDate=nDate;
						m_ChunkSvrPulseInfo.nReconnectCount=0;
					}
					
					int nNowTick=GetTickCount();
					UwlLogFile(_T("[%u] Begin reconnect as chunksvr pulse lost [%d] 秒."),nNowTick,nNow-nLatest);
					
					PostThreadMessage(m_uiThrdConnectChunk, UM_DATA_TOSEND, UR_RECONNECT_SVR, nNowTick);					
				}
			}
		}
	}
	
	UwlTrace(_T("chunksvr pulse thread exiting. id = %d"), GetCurrentThreadId());
	UwlLogFile(_T("chunksvr pulse thread exiting. id = %d"), GetCurrentThreadId());
	
   	return 0;
} 

unsigned CGameServer::Chunk_TimerThreadProc()
{
	UwlTrace(_T("chunk_timer thread started. id = %d"), GetCurrentThreadId());
	
	int sameHour = 0;
	int lastHour = -1;
	int cycleMin = 0;
	int dayhour = -1;
	
	while(TRUE){
		int interval = GetPrivateProfileInt(_T("Timer"), _T("Interval"), DEF_TIMER_INTERVAL, m_szIniFile);
		if(interval < MIN_TIMER_INTERVAL){ // 至少1分钟周期
			interval = MIN_TIMER_INTERVAL;
		}
		DWORD dwRet = WaitForSingleObject(m_hExitServer, interval * 60 * 1000);
		if(WAIT_OBJECT_0 == dwRet){ // exit event
			break;
		}
		if(WAIT_TIMEOUT == dwRet){ // timeout
			UwlTrace(_T("chunk_timer thread triggered. do something. interval = %ld minutes."), interval);
			
			Chunk_SendPulseData();
			
			Chunk_OnTimerTriggered(interval, sameHour, lastHour, cycleMin);
			
			Chunk_OnHourTriggered(dayhour);
		}
	}
	UwlTrace(_T("chunk_timer thread exiting. id = %d"), GetCurrentThreadId());
   	return 0;
}

BOOL CGameServer::Chunk_ValidateClientEx()
{
// 	REQUEST request;
// 	memset(&request, 0, sizeof(request));
// 	REQUEST response;
// 	memset(&response, 0, sizeof(response));
// 	
// 	CONTEXT_HEAD context;
// 	context.hSocket = m_pSockClitChunk->GetSocket();
// 	context.lSession = 0;
// 	context.bNeedEcho = TRUE;
// 	
// 	VALIDATE_CLIENT_EX vce;
// 	ZeroMemory(&vce,sizeof(vce));
// 	vce.nClientID   = m_nGameID;
// 	vce.nClientType = CLIENT_TYPE_GAME_EX; // 
// 	xyGetHardID(vce.szHardID);
// 	xyGetVolumeID(vce.szVolumeID);
// 	xyGetMachineID(vce.szMachineID);
// 	request.head.nRequest = GR_VALIDATE_CLIENT_EX;
// 	request.nDataLen = sizeof(vce);
// 	request.pDataPtr =&vce;
// 	
// 	BOOL bTimeout = FALSE;
// 	BOOL bSendOK = m_pSockClitChunk->SendRequest(&context, &request, &response, bTimeout, 10000);
// 	if(!bSendOK || UR_FETCH_SUCCEEDED != response.head.nRequest)
// 	{
// 		UwlLogFile(_T("ChunkSvr ValidateClientEx() failed!"));
// 		UwlClearRequest(&response);
// 		return FALSE;
// 	}
// 	
// 	UwlClearRequest(&response);
// 	UwlLogFile(_T("ChunkSvr ValidateClientEx() OK!"));
	return TRUE;
}

BOOL CGameServer::Chunk_ValidateClientInfo(int gameid)
{
	TCHAR szYxpdsvr[MAX_PATH];
	memset(szYxpdsvr,0,sizeof(szYxpdsvr));
	GetSystemDirectory(szYxpdsvr,MAX_PATH);
	lstrcat(szYxpdsvr,_T("\\yxpdsvr.ini"));
	
	REQUEST request;
	memset(&request, 0, sizeof(request));
	REQUEST response;
	memset(&response, 0, sizeof(response));
	
	CONTEXT_HEAD context;
	memset(&context, 0, sizeof(context));
	context.hSocket = m_pSockClitChunk->GetSocket();
	
	VALIDATE_GAMESVR vg;
	memset(&vg,0,sizeof(vg));
	
	vg.nClientID = GetPrivateProfileInt(_T("HOST"),_T("ID"),0, szYxpdsvr); // 
	vg.nClientType = CLIENT_TYPE_GAME_EX; // 
	vg.nGameID = gameid;
	vg.nGamePort = m_nListenPort;
	
	request.head.nRequest = GR_VALIDATE_GAMESVR_EX;
	request.nDataLen = sizeof(vg);
	request.pDataPtr = &vg;
	
	BOOL bTimeout = FALSE;
	BOOL bSendOK = m_pSockClitChunk->SendRequest(&context, &request, &response, bTimeout);
	return bSendOK;

// 	REQUEST request;
// 	memset(&request, 0, sizeof(request));
// 	REQUEST response;
// 	memset(&response, 0, sizeof(response));
// 	
// 	CONTEXT_HEAD context;
// 	memset(&context, 0, sizeof(context));
// 	context.hSocket = m_pSockClient->GetSocket();
// 	
// 	VALIDATE_CLIENT vc;
// 	ZeroMemory(&vc,sizeof(vc));
// 	vc.nClientID = gameid; 
// 	vc.nClientType = CLIENT_TYPE_ASSIST; // 
// 	
// 	request.head.nRequest = GR_VALIDATE_CLIENT;
// 	request.nDataLen = sizeof(vc);
// 	request.pDataPtr = &vc;
// 	
// 	BOOL bTimeout = FALSE;
// 	BOOL bSendOK = m_pSockClient->SendRequest(&context, &request, &response,bTimeout);
// 	return bSendOK;
}

BOOL CGameServer::Chunk_IsTimingOK(DWORD interval, int& sameHour, int& lastHour, int& cycleMin)
{
	int mode = GetPrivateProfileInt(
		_T("KickOff"),			// section name
		_T("Mode"),				// key name
		DEF_KICKOFF_MODE,		// default int
		m_szIniFile				// initialization file name
		);
	int timing = GetPrivateProfileInt(
		_T("KickOff"),			// section name
		_T("Timing"),			// key name
		DEF_KICKOFF_TIMING,		// default int
		m_szIniFile				// initialization file name
		);
	int elapse = GetPrivateProfileInt(
		_T("KickOff"),			// section name
		_T("Elapse"),			// key name
		DEF_KICKOFF_ELAPSE,		// default int
		m_szIniFile				// initialization file name
		);
	int timingOK = 0;
	if(1 == mode){
		cycleMin += interval;
		if(cycleMin >= elapse){
			cycleMin = 0;
			timingOK = 1;
		}
	}
	if(2 == mode){
		SYSTEMTIME time;
		GetLocalTime(&time);
		if(time.wHour != lastHour){
			sameHour = 0;
		}else{
			sameHour++;
		}
		lastHour = time.wHour;
		if(time.wHour == timing){
			if(0 == sameHour){
				timingOK = 1;
			}
		}
	}
	return timingOK;
}

int CGameServer::Chunk_DoTimingWork()
{
// 	int count = ReleaseSockBufPool();
// 	UwlLogFile(_T("release server buffer pool done. count = %ld."), count);
	
	int count = 0;
	if (m_pSockClitChunk)
	{
		count = m_pSockClitChunk->ReleaseSockBufPool();
		UwlLogFile(_T("release chunk_client buffer pool done. count = %ld."), count);	
	}
	
// 	int deadtime = GetPrivateProfileInt(
// 						_T("KickOff"),			// section name
// 						_T("DeadTime"),			// key name
// 						DEF_KICKOFF_DEADTIME,	// default int
// 						m_szIniFile				// initialization file name
// 						);
// 	CDWordArray aryToken;
// 	count = CleanStales(deadtime * 60, aryToken);
// 	UwlLogFile(_T("CleanStales() called. deadtime = %ld minutes, count = %ld."), deadtime, count);
// 	
// 	int roomid = 0;
// 	int userid = 0;
// 	for(int i = 0; i < aryToken.GetSize(); i++){
// 		RemoveTokenData(aryToken[i], userid);
// 	}
// 	
// 	ClearUserToken();
	
// 	int stifftime = GetPrivateProfileInt(
// 		_T("KickOff"),			// section name
// 		_T("StiffTime"),		// key name
// 		DEF_KICKOFF_STIFFTIME,	// default int
// 		m_szIniFile				// initialization file name
// 		);
// 	count = CloseStiffs(stifftime);
// 	UwlLogFile(_T("CloseStiffs() called. stifftime = %ld seconds, stiffs = %ld."), stifftime, count);
	
// 	int staletime = GetPrivateProfileInt(
// 		_T("UserData"),			// section name
// 		_T("StaleTime"),		// key name
// 		DEF_KICKOFF_STALETIME,	// default int
// 		m_szIniFile				// initialization file name
// 		);
// 	count = ClearStaleUserData(staletime);
// 	UwlLogFile(_T("ClearStaleUserData() called. staletime = %ld seconds, count = %ld."), staletime, count);
	return 1;
}

int CGameServer::Chunk_OnTimerTriggered(DWORD interval, int& sameHour, int& lastHour, int& cycleMin)
{
	if(Chunk_IsTimingOK(interval, sameHour, lastHour, cycleMin)){
		return Chunk_DoTimingWork();
	}
	return 0;
}


void CGameServer::Chunk_OnHourTriggered(int& lastHour)
{
// 	SYSTEMTIME time;
// 	GetLocalTime(&time);
// 	
// 	if (lastHour < 0 
// 		|| time.wHour != lastHour)
// 	{
// 		//每小时记录一次..
// 		//添加在这里..
// 	}
// 	
// 	lastHour = time.wHour;
}

BOOL CGameServer::OnChunkSendPulse(LPREQUEST lpRequest)
{
	CAutoLock lock(&m_csChunkSvrPulseInfo);
	m_ChunkSvrPulseInfo.nLatestTime = UwlGetCurrentSeconds();
	
	return TRUE;
}

void CGameServer::Chunk_SetIsConnectChunkSvr(BOOL bConnect)
{
	m_bConnectChunkSvr=bConnect;
}

BOOL CGameServer::Chunk_GetIsConnectChunkSvr()
{
	return m_bConnectChunkSvr;
}

//二维码充值start
BOOL    CGameServer::OnRoomLimit(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt)
{
    LONG token = 0;
    int roomid = 0;
    int tableno = INVALID_OBJECT_ID;
    int userid = 0;
    int chairno = INVALID_OBJECT_ID;
    int askid = 0;
    int r_id = 0;
    int t_no = INVALID_OBJECT_ID;
    int u_id = 0;
    int c_no = INVALID_OBJECT_ID;
    BOOL lookon = FALSE;

    CRoom* pRoom = NULL;
    CGameTable* pTable = NULL;

    token = lpContext->lTokenID;
    LPROOMLIMIT pRoomLimit = LPROOMLIMIT(PBYTE(lpRequest->pDataPtr));
    roomid  = pRoomLimit->nRoomID;
    tableno = pRoomLimit->nTableNO;
    userid  = pRoomLimit->nUserID;
    chairno = pRoomLimit->nChairNO;

    {
        CAutoLock lock(&m_csRoomMap);
        
        if(!m_mapRoom.Lookup(roomid, pRoom)){
            return TRUE;
        }
    }
    if(pRoom){
        CAutoLock lock(&(pRoom->m_csRoom));		
        if(!pRoom->m_mapTable.Lookup(tableno, (CTable*&)pTable)){
            return TRUE;
        }
    }

    if (pTable)
    {
        if (chairno<=-1||chairno>=pTable->m_nTotalChairs)
        {
            return TRUE;
        }

        LOCK_TABLE(pTable, chairno, FALSE, userid, token);
        if(!pTable->IsPlayer(userid) && !pTable->IsVisitor(userid)){ // 不是玩家，不是旁观
            UwlLogFile(_T("user not player. user %ld allow cardview failed."), userid);
            return TRUE;
        }

        CPlayer* pPlayer = pTable->GetPlayer(userid);
        CPlayer* pVisitor = pTable->GetVisitor(userid);

        CPlayer* pPlayerResponse = NULL;
        if (pPlayer)
        {
            pPlayerResponse = pPlayer;
        }
        else if (pVisitor)
        {
            pPlayerResponse = pVisitor;
        }

        if (pPlayerResponse)
        {
            CString strKey;
            strKey.Format("%d", roomid);
            int nLargeRoom = GetPrivateProfileInt(_T("LargeRoomCon"), strKey, 0, GetINIFileName());
            if (nLargeRoom > 0)
            {
                pRoomLimit->bLargeRoom = TRUE;
            }
            NotifyOneUser(pPlayerResponse->m_hSocket, pPlayerResponse->m_lTokenID, GR_ROOMLIMIT, pRoomLimit, sizeof(ROOMLIMIT));
        }
    }

    return TRUE;
}


BOOL    CGameServer::OnChargeBtn(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt)
{
    LONG token = 0;
    int roomid = 0;
    int tableno = INVALID_OBJECT_ID;
    int userid = 0;
    int chairno = INVALID_OBJECT_ID;
    int askid = 0;
    int r_id = 0;
    int t_no = INVALID_OBJECT_ID;
    int u_id = 0;
    int c_no = INVALID_OBJECT_ID;
    BOOL lookon = FALSE;
    
    CRoom* pRoom = NULL;
    CGameTable* pTable = NULL;
    CPlayer* pVisitor = NULL;
    
    token = lpContext->lTokenID;
    
    LPCHARGEBTN pChargeBtn = LPCHARGEBTN(PBYTE(lpRequest->pDataPtr));
    if (NULL == pChargeBtn)
    {
        return FALSE;
    }
    roomid = pChargeBtn->nRoomID;
    tableno = pChargeBtn->nTableNO;
    userid = pChargeBtn->nUserID;
    chairno = pChargeBtn->nChairNO;
    
    {
        CAutoLock lock(&m_csRoomMap);
        
        if(!m_mapRoom.Lookup(roomid, pRoom)){
            return TRUE;
        }
    }
    if(pRoom){
        CAutoLock lock(&(pRoom->m_csRoom));		
        if(!pRoom->m_mapTable.Lookup(tableno, (CTable*&)pTable)){
            return TRUE;
        }
    }
    
    if(pTable) {
        if (chairno<=-1||chairno>=pTable->m_nTotalChairs)
        {
            return TRUE;
        }
        
        {
            LOCK_TABLE(pTable, chairno, FALSE, userid, token);
            
            if(!pTable->IsPlayer(userid) && !pTable->IsVisitor(userid)){ // 不是玩家，不是旁观
                UwlLogFile(_T("user not player. user %ld allow cardview failed."), userid);
                return TRUE;
            }
        }
        
        if (pChargeBtn->nChargeNumber < 0 || pChargeBtn->nChargeNumber > 1000)
        {
            return FALSE;
        }
        
        {
            if (OTHERWAY_CHARGE_BTN == pChargeBtn->enType)
            {
                UwlLogFile("Game charge info, %d will charge from web.", pChargeBtn->nUserID);
            }
            else if (QRCODE_CHARGE_BTN == pChargeBtn->enType)
            {
                UwlLogFile("Game charge info, %d will charge %d(yuan) from qrcode.", pChargeBtn->nUserID, pChargeBtn->nChargeNumber);
            }
			else if (QRCODE_CHARGE_BTN_WECHAT == pChargeBtn->enType)
            {
                UwlLogFile("Game charge info, %d will charge %d(yuan) from qrcode_wechat.", pChargeBtn->nUserID, pChargeBtn->nChargeNumber);
            }
        }
    }
    
    return TRUE;
}

//获取网页充值的相关地址
void CGameServer::SendChargeUrl(SOCKET hSocket,LONG lTokenID)
{
    CHARGEURL ChargeUrl;
	GetPrivateProfileString(_T("ChargeURL"), _T("WebUrl"), _T(""), ChargeUrl.WebUrl, sizeof(ChargeUrl.WebUrl), m_szIniFile);
	GetPrivateProfileString(_T("ChargeURL"), _T("OrCodeUrl"), _T(""), ChargeUrl.OrCodeUrl, sizeof(ChargeUrl.OrCodeUrl), m_szIniFile);
	GetPrivateProfileString(_T("ChargeURL"), _T("OrCodeUrl_WeChat"), _T(""), ChargeUrl.OrCodeUrl_WeChat, sizeof(ChargeUrl.OrCodeUrl_WeChat), m_szIniFile);
	GetPrivateProfileString(_T("ChargeURL"), _T("TongBaoUrl"), _T(""), ChargeUrl.TongBaoUrl, sizeof(ChargeUrl.TongBaoUrl), m_szIniFile);
	GetPrivateProfileString(_T("ChargeURL"), _T("LearnMoreUrl"), _T(""), ChargeUrl.LearnMoreUrl, sizeof(ChargeUrl.LearnMoreUrl), m_szIniFile);
	NotifyOneUser(hSocket,lTokenID,TCSG_CHARGE_ALLURL,&ChargeUrl,sizeof(ChargeUrl));
}
//二维码充值end

int CGameServer::OnNewTableChair(int roomid, int tableno, int chairno, int userid)
{
	int nRet=CMainServer::OnNewTableChair(roomid,tableno,chairno,userid);
	//换桌成功后，如果刚好换到这局庄家的位置,且游戏不在进行中，那么需要重置庄家
	if(nRet==1)
	{
		CRoom* pRoom = NULL;
		CTable* pTable = NULL;
		{
			CAutoLock lock(&m_csRoomMap);
			
			if(!m_mapRoom.Lookup(roomid, pRoom)){
				return nRet;
			}
		}
		
		if(pRoom){
			CAutoLock lock(&(pRoom->m_csRoom));
			
			if(!pRoom->m_mapTable.Lookup(tableno, pTable)){
				return nRet;
			}
		}	
		
		if (pTable)
		{	
			CAutoLock lock(&(pTable->m_csTable));
			if(pTable->IsGameOver())
				ResetTableBanker(pTable,chairno);
		}
	}
	return nRet;
}

void CGameServer::ResetTableBanker(CTable* pTable, int chairno)
{
	BOOL bResetBanker=TRUE;
	int nOtherInGameCount=0;
	int nBanker=((CGameTable*)pTable)->m_nTempBanker;
	//第一种情况 如果进来的时候桌子只有自己一个人 就要重置庄家
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		if(i==chairno)
			continue;
		if(pTable->m_ptrPlayers[i])
		{
			nOtherInGameCount++;
			bResetBanker=FALSE;
		}
	}
	
	//第一种不符合 考虑第二种情况 
	//如果进来的时候刚好坐的是庄家的位置 那么说明刚才的庄家退出来 则要重置庄家
	if(bResetBanker==FALSE&&chairno==nBanker)
		bResetBanker=TRUE;
	
	//第二种不符合 考虑第三种情况 
	//如果进来的时候只有庄家一个人
	if(bResetBanker==FALSE&&nOtherInGameCount==1&&pTable->m_ptrPlayers[nBanker])
		bResetBanker=TRUE;
	
	if(bResetBanker)
		((CGameTable*)pTable)->SetResetBanker(bResetBanker);
}

int CGameServer::OnUserStart(CTable* pTable,int chairno)
{
	if (!pTable||chairno<0||chairno>pTable->m_nTotalChairs)
		return -1;
	
	CPlayer* pPlayer=pTable->m_ptrPlayers[chairno];
	if (!pPlayer)
		return -1;
	
	pTable->OnUserStart(chairno);
	int startcount=XygGetStartCount(pTable->m_dwUserStatus, pTable->m_nTotalChairs);
	
	if (startcount<pTable->m_nTotalChairs)
	{
		if (IsVariableChairRoom(pTable->m_nRoomID)
			&& IsAllowStartGame(pTable, startcount))
		{
			// 目前桌上所有人都按下了开始
			if (pTable->GetPlayerCountOnTable() == startcount)
			{
				OnGameStarted(pTable,0);
				return 1;
			}
			//准备玩家足够多了，游戏自动开始倒计时
			else if (pTable->IsNeedCountdown() && !pTable->IsInCountDown())
			{
				START_COUNTDOWN  sc;
				memset(&sc,0,sizeof(START_COUNTDOWN));
				sc.nRoomID = pTable->m_nRoomID;
				sc.nTableNO = pTable->m_nTableNO;
				sc.bStartorStop = TRUE;
				sc.nCountDown = GetCountdown(pTable,pTable->m_nRoomID);
				pTable->SetCountDown(TRUE, sc.nCountDown);
				NotifyTablePlayers(pTable,GR_START_COUNTDOWN,&sc,sizeof(START_COUNTDOWN));
			}
		}
		
		START_GAME sg;
		memset(&sg,0,sizeof(START_GAME));
		sg.nRoomID=pTable->m_nRoomID;
		sg.nTableNO=pTable->m_nTableNO;
		sg.nChairNO=chairno;
		sg.nUserID=pPlayer->m_nUserID;	
		sg.nReserved[0]=((CGameTable*)pTable)->m_bResetBanker;
		NotifyTablePlayers(pTable, GR_START_GAME, &sg, sizeof(START_GAME));
	}
	else
	{
		OnGameStarted(pTable,0);
	}
	
	
	return 1;
}