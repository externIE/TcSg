#pragma once

class CGameServer : public CSkServer{
public:
	CGameServer( const TCHAR* szLicenseFile,
			 const TCHAR* szProductName,
			 const TCHAR* szProductVer,
			 const int nListenPort, const int nGameID,
			 DWORD flagEncrypt = 0, DWORD flagCompress = 0)
			 : CSkServer(szLicenseFile, szProductName, szProductVer,
				nListenPort, nGameID, flagEncrypt, flagCompress)
	{
		//////////////////////////////////////////////////////////////////////////
		//ChunkSvr相关
		//////////////////////////////////////////////////////////////////////////
		m_pSockClitChunk = new CMySockClit(this, KEY_HALL, flagEncrypt, flagCompress);
		lstrcpy(m_szChunkServer, _T(""));

		m_hThrdSendChunk     = NULL;
		m_hThrdConnectChunk  = NULL;
		m_hThrdSvrPulseChunk = NULL;
		m_hThrdTimerChunk    = NULL;
		m_uiThrdSendChunk    = 0;
		m_uiThrdConnectChunk = 0;
		m_uiThrdSvrPulseChunk= 0;
		m_uiThrdTimerChunk   = 0;
		m_bConnectChunkSvr   = FALSE;
		ZeroMemory(&m_ChunkSvrPulseInfo,sizeof(m_ChunkSvrPulseInfo));
	}
	~CGameServer()
	{
		
	}
	
protected:	

    //	
	virtual BOOL OnRequest(void* lpParam1, void* lpParam2);
	
	virtual CTable* OnNewTable(int roomid = INVALID_OBJECT_ID, int tableno = INVALID_OBJECT_ID, int score_mult = 1);
	virtual BOOL OnSendSysMsgToServer(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt);//消息处理
	virtual BOOL OnAskExit(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt);
public:

	virtual void OnSeverAutoPlay(CRoom* pRoom,CTable* pTable,int chairno);
	virtual	BOOL OnGameWin(LPCONTEXT_HEAD lpContext, CRoom* pRoom,CTable* pTable, int chairno, BOOL bout_invalid, int roomid);

			void NotifyPlayersAskExit(CGameTable* pTable, LPASK_EXIT pAskExit, LONG tokenExcept=0, BOOL compressed=FALSE);

	virtual	BOOL OnStartGameEx(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt);
	virtual BOOL OnChatToTableEx(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt);

	virtual void CheckGameWin();
			void CopyWaitResultMap(CWaitResultMap& mapWaitResult);
			void RemoveWaitResultByKey(DWORD dwKey);

	virtual int  OnNewTableChair(int roomid, int tableno, int chairno, int userid);	
	virtual int  OnUserStart(CTable* pTable,int chairno);

public:

	BOOL OnQiangZhuang(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt);
	BOOL OnYaZhu(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt);
	BOOL OnZuPai(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt);


public:
	virtual BOOL  Initialize();
	virtual void  Shutdown();
	
			void  SendWelfareActivityID(SOCKET hSocket,LONG lTokenID);
			void  SendIsTongbiRoom(int roomid,SOCKET hSocket,LONG lTokenID);
	//二维码充值start
	virtual BOOL OnRoomLimit(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt);
	virtual BOOL OnChargeBtn(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest, CWorkerContext* pThreadCxt);	
	void SendChargeUrl(SOCKET hSocket,LONG lTokenID);
	//二维码充值end	
	void ResetTableBanker(CTable* pTable, int chairno);
public:
#define RM_TABLEDEPOSIT 0x00000200	
	virtual BOOL IsTableDepositRoom(int roomid);						//房间设置要校验桌银
	virtual BOOL GetRoomSvrIniFile(int roomid, CString& sRoomSvrIniFile);
	virtual	int	 GetTableDepositMin(int roomid, int tableno);
	virtual int  GetMinPlayingDeposit(CTable *pTable, int roomid);		//玩游戏所需的最小银两下限

	//按额度配桌
	BOOL ReadTableGrade(int nRoomID,TABLEGRADEALL *pTableGrade);
	void NotifyTableGrade(int nRoomID,SOCKET sock, LONG token);	
	void PostAskNewTableByTableGrade(int nUserID,int nRoomID,int nTableNO,int nIndex);
	BOOL OnPlayerEnterGame(LPCONTEXT_HEAD lpContext,LPENTER_GAME_OK_EX lpEnterGameEx,LPREQUEST lpReqToClient);
	BOOL OnEnterGameDXXW(LPCONTEXT_HEAD lpContext, LPENTER_GAME_EX pEnterGame);

	/************************************************************************/
	/* AsitSvr/ChunkSvr Related                                                                   
	/************************************************************************/
public:
	BOOL InitSockToChunk();
	void ShutDownSockToChunk();
	BOOL ReadChunkSvrFromProfile();
	UINT GetThreadID_ChunkConnect();
	BOOL Chunk_BaseVerify(int userid, int gameid);
	BOOL Chunk_TransmitRequest(LPCONTEXT_HEAD lpContext, LPREQUEST lpRequest);
	void Chunk_SendPulseData();
	BOOL OnChunkSendPulse(LPREQUEST lpRequest);

protected:
	unsigned Chunk_SendThreadProc();
	unsigned Chunk_ConnectThreadProc();
	unsigned Chunk_ServerPulseThreadProc();
	unsigned Chunk_TimerThreadProc();
	
	static unsigned __stdcall Chunk_SendThreadFunc(LPVOID lpVoid);
	static unsigned __stdcall Chunk_ConnectThreadFunc(LPVOID lpVoid); //负责重连chunksvr
	static unsigned __stdcall Chunk_ServerPulseThreadFunc(LPVOID lpVoid); //
	static unsigned __stdcall Chunk_TimerThreadFunc(LPVOID lpVoid);
	
	BOOL Chunk_ValidateClientEx();			   //验证IP是否授权
	BOOL Chunk_ValidateClientInfo(int gameid); //验证ClientID是否授权
	
	BOOL Chunk_IsTimingOK(DWORD interval, int& sameHour, int& lastHour, int& cycleMin);
	int  Chunk_DoTimingWork();
	int  Chunk_OnTimerTriggered(DWORD interval, int& sameHour, int& lastHour, int& cycleMin);
	void Chunk_OnHourTriggered(int& lastHour);		//每小时触发一次
	void Chunk_SetIsConnectChunkSvr(BOOL bConnect);
	BOOL Chunk_GetIsConnectChunkSvr();

protected:
	CMySockClit*	m_pSockClitChunk;
	TCHAR			m_szChunkServer[MAX_PATH];

	HANDLE			m_hThrdSendChunk;
	HANDLE			m_hThrdTimerChunk; 
	HANDLE			m_hThrdConnectChunk;
	HANDLE			m_hThrdSvrPulseChunk;
	
	UINT			m_uiThrdSendChunk;
	UINT			m_uiThrdTimerChunk; 
	UINT			m_uiThrdConnectChunk;
	UINT			m_uiThrdSvrPulseChunk;
	
	CCritSec		 m_csChunkSvrPulseInfo;
	SERVERPULSE_INFO m_ChunkSvrPulseInfo;

	BOOL			 m_bConnectChunkSvr;
};

extern CGameServer* g_gameServer;

inline int Svr_RetrieveFields ( TCHAR *buf, TCHAR **fields, int maxfields, TCHAR**buf2 )
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

