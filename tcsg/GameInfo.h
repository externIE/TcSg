// GameInfo.h: interface for the CGameInfoEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEINFO_H__17F16CEC_D35E_47E0_A4D6_4A88F209871B__INCLUDED_)
#define AFX_GAMEINFO_H__17F16CEC_D35E_47E0_A4D6_4A88F209871B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGameInfoEx:public CGameInfo
{
public:
	CGameInfoEx();
	virtual ~CGameInfoEx();
public:
public:  
	GAME_ENTER_INFO			       m_GameEnterInfo;
	GAME_TABLE_INFO				   m_GameTableInfo;//改结构保存DXXW时从服务器返回的游戏信息
    GAME_WIN_RESULT                m_GameWinResult;
	BOOL						   m_bnAlowLook;//是否允许看牌

	virtual  GAME_WIN_RESULT*      GetGameWin();
	virtual  GAME_START_INFO*      GetGameStartInfo();
	virtual  GAME_TABLE_INFO*      GetGameTableInfo();   

	virtual BOOL  IsGamePlaying();
	virtual BOOL  IsForbidDesert();
	virtual BOOL  IsChairReady(int chairno);
			void  ResetChairStatus(int chairno);
			void  ResetChairReady();
			void  SetUserStatus(DWORD dwUS[], int len);
	virtual DWORD GetStatus();
	virtual void  SetStatus(DWORD dwStatus);
	virtual int   GetBoutCount();
	virtual int   GetBaseDeposit();
	virtual int   GetBaseScore();
	virtual	void  SetCurrentChairNO(int chairno);
	virtual	int   GetCurrentChairNO();
	virtual	void  SetBankerChairNO(int chairno);

	virtual int	  GetBanker();
	virtual int   GetAutoPasTime();
	virtual int   GetOfflineTime();
	virtual int   GetCurrentRank();
	virtual int   GetThrowWait();
             

	virtual void  NTF_StartGame(START_GAME* pStartGame);
	virtual void  NTF_GameStart(GAME_START_INFO *pStartData);
	virtual void  NTF_Zupai(ZUPAI* pZupai);
	virtual void  NTF_GameWin(GAME_WIN_RESULT* pGameWinMJ);
	virtual void  ClearTableInfo();//游戏结束后清理数据
	virtual void  ResetStatusWhenStart();//游戏开始时清理数据

	virtual	void  SetObjectGains(int nGains);
	virtual int   GetObjectGains();
};

#endif // !defined(AFX_GAMEINFO_H__17F16CEC_D35E_47E0_A4D6_4A88F209871B__INCLUDED_)
