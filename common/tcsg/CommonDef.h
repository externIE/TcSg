#pragma once


#define		PORT_OF_CHUNKSVR	  0
#define		PORT_OF_ASSITSVR 	  0 

#define CONNECT_CHUNKSVR_WAIT	10			// waittime(seconds)
#define CHUNKSVR_RESPONSE_WAIT	10000		// waittime(millisec)
#define	CONNECTS_TO_CHUNKSVR	1			// connects
#define	CLIENT_INITIAL_RECVS	10			// recvs
#define DEF_SERVERPULSE_INTERVAL 60			//seconds 定时检查Svr发过来的脉搏(发送间隔默认10秒)
#define DEF_TIMER_INTERVAL		5			// minitues
#define MIN_TIMER_INTERVAL		1			// minitues
#define DEF_KICKOFF_MODE		2			// clock mode
#define DEF_KICKOFF_TIMING		6			// AM 6:00 clock at morning
#define DEF_KICKOFF_ELAPSE		(DEF_TIMER_INTERVAL * 12) // minitues

#define MAX_DB_CATALOG_LEN 32
#define MAX_CHUNKDB_SOURCE_LEN  128



#define NAME_CHUNKDB_MAIN			_T("MAIN")
#define NAME_CHUNKDB_GAME			_T("GAME")
#define NAME_CHUNKDB_LOG			_T("LOG")



enum{
	TYPE_CHUNKDB_MAIN = 0,
	TYPE_CHUNKDB_GAME,
	TYPE_CHUNKDB_LOG,
	TYPE_CHUNKDB_MAX
};

typedef struct _tagCHUNK_DB{
	int nID;
	int nType;
	TCHAR szName[MAX_SERVERNAME_LEN];
	TCHAR szSource[MAX_CHUNKDB_SOURCE_LEN];
	TCHAR szCatalog[MAX_DB_CATALOG_LEN];
	TCHAR szUserName[MAX_USERNAME_LEN];
	TCHAR szPassword[MAX_PASSWORD_LEN];
	int nSecurityMode;
	int nReserved[4];
}CHUNK_DB, *LPCHUNK_DB;



typedef CList<int ,int&>         CIntList;
typedef CMap<LONG, LONG, SOCKET, SOCKET> CTokenSockMap;
typedef CMap<int, int, LONG, LONG> CClientTokenMap;
typedef CMap<CString,LPCTSTR,LONG,LONG&>  CStringLONGMap;
typedef CMap<int, int, CClientTokenMap*, CClientTokenMap*&> CGameSvrTokenMap;

/************************************************************************/
/*	define of string res : AsitSvr/GameSvr Strings                       
/************************************************************************/
#define ASS_USERID_MISMATCH								_T("用户ID不匹配")
#define ASS_HARDID_MISMACCH								_T("硬件ID不匹配")
#define ASS_PARAM_NOTVERIFY								_T("参数验证失败")
#define ASS_USER_NOTLOGON								_T("用户未登录")
