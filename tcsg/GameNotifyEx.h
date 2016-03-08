// GameNotify.h: interface for the CGameNotify class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMENOTIFY_H__A27FBBB7_FF64_4448_989B_EDF0F101A2C1__INCLUDED_)
#define AFX_GAMENOTIFY_H__A27FBBB7_FF64_4448_989B_EDF0F101A2C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CGameNotifyEx  : public CSKGameNotify
{
public:
	CGameNotifyEx();
	virtual ~CGameNotifyEx();

public:
	virtual void OnNotifyReceived( int nRequestID, VOID* pDataPtr,int nSize);
	virtual void NTF_GameAbort(GAME_ABORT* pGameAbort);//有玩家离开
 	virtual void NTF_StartGame(START_GAME* pStartGame);
 	virtual void NTF_GameStart(GAME_START_INFO* pStartData);
 	virtual	void NTF_GameMsg(GAME_MSG* pGameMsg);
 			void NTF_GameWin(GAME_WIN_RESULT* pGameWin);
			void NTF_GameTableInfo(GAME_TABLE_INFO* pGameTableInfo);
	virtual void NTF_PlayerOffline(LPPLAYER_OFFLINE lpOffline);
	virtual void NTF_UserPosition(LPUSER_POSITION pUserPos);

	virtual void NTF_CardPass(CARDS_PASS* pCardsPass);
	virtual void NTF_CardThrow(CARDS_THROW* pCarsThrow);
	virtual void NTF_GameStartSolo(LPVOID pStartInfo);
	virtual void NTF_UserDepositEvent(LPUSER_DEPOSITEVENT pUser_Deposit);


	virtual void NTF_StartCountdown(START_COUNTDOWN* pCountdown);
	virtual void NTF_SyncStartCountdown(SYNC_COUNTDOWN* pSyncCountdown);
	virtual void NTF_PlayerEnter(SOLO_PLAYER * pSoloPlayer);
	virtual	void NTF_PlayerAbort(GAME_ABORT * pGameAbort);

	void NTF_QZOK(QZ *pInfo);
	void NTF_YaZhuOK(YAZHU *pInfo);
	void NTF_ZuPai(ZUPAI* pZuPai);

	//二维码充值start
    virtual void NTF_RoomLimit(LPROOMLIMIT pRoomLimit);
    //二维码充值end

	//获取低保id
	virtual void NTF_WelfareActivity(int nActivityID);

	//获取充值界面的相关url
	void NTF_GetChargeUrl(LPCHARGEURL pChargeUrl);

	void NTF_IsTongBiRoom(int nIsTongBiRoom);
};

#endif // !defined(AFX_GAMENOTIFY_H__A27FBBB7_FF64_4448_989B_EDF0F101A2C1__INCLUDED_)
