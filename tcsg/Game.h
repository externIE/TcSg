// Game.h : main header file for the GAME application
//

#pragma once


#include "resource.h"       // main symbols
#include "GameInfo.h"
#include "GameRequest.h"
#include "GameNotifyEx.h"
//二维码充值start
#include "GameChargeDlg.h"
#include "QrcodeCharge.h"
//二维码充值end
#include "WelfareDialog.h"
using namespace ShinyNova;


class CGameApp : public CSKGameWinApp
{
public:
	CGameApp();
	~CGameApp();

	virtual  BOOL				CreateGameFrame();         //创建主窗口Cwnd
	virtual  CXyGameFrame*		CreateLogicFrame();
public:
	virtual  VOID*				GetTableInfo();
	virtual  VOID*				GetEnterInfo();
			 int				GetTableInfoSize();
			 int				GetEnterInfoSize();
	virtual  CGameInfo*			GetGameInfo();
	virtual  DWORD				GetUserConfig();
	virtual  CGameConnect*		GetGameConnect();
	virtual  CGameNotify*		GetGameNotify();
	virtual  BOOL				IsSilence();

	virtual  BOOL				ParseCommond(LPCTSTR str);
	virtual  void				CreateArrageTable();//组建新的桌子
	virtual  void				DisBandTable();

	virtual  int				GetTableChairCount();
	virtual	 int				GetThrowWait();
	virtual  int				GetCurrentRank();
	virtual	 int				GetChairCardsCount();

	virtual  BOOL				IsTakeDepositInGame();
	virtual	 BOOL				IsAllowAskExit();//该游戏房间是否允许协商退出

			 void				AchToUI();					 // 启动EnterGame时
			 void				ClearChairStatus();
			 void				SetUserStatus(DWORD dwUserStatus[], int nLen);
			 void               GetChargeUrl(LPCHARGEURL pChargeUrl);
public:
	void ReadGameSet();
	void SaveGameSet();
	void FreshGameSet();
	void EraseBkgnd();
	
	void FreshScores();
	virtual BOOL IsRejectKanPaiAsk();
	virtual BOOL IsAllowCardMaster();
	virtual BOOL IsRejectPassSound();
	virtual int	IsSaveReplay();
			BOOL	IsLikeRemaind();
			BOOL	IsUnlikeTip();
			BOOL	IsUseBigUI();
			void	SetUseBigUI(BOOL bUse);
	virtual void	ResetDataByNewPos(USER_POSITION* pUserPos);

	//log，用Dbgview可以接收
	inline void OutputDebugStringEx(CString format,...)
	{
		CString s;
		va_list args;
		va_start(args,format);
		s.FormatV(format,args);
		va_end(args); 
		::OutputDebugString(s);
	}

	//二维码充值start
    virtual void    SetRoomLimit(BOOL bLargeRoom){m_bLargeRoom = bLargeRoom;};
    virtual void    ShowGameCharge();//显示充值对话框
    virtual void    ShowQrChargeDlg(CString& strDstUrl);
    //二维码充值end 

	int            GetChargeStat() {return m_GameCharge.GetChargeStat();}
	void           SetChargeStat(int chargeStat) {m_GameCharge.SetChargeStat(chargeStat);}
public:
	GAME_SET           m_GameSet;    //游戏本地设置
    CGameRequest       m_GameRequest;//处理与服务器的连接并发送请求
	CGameNotifyEx      m_GameNotifyEx;//处理从服务器来的通知

	CGameInfoEx		   m_GameInfoEx;//本卓游戏信息,

	BOOL			   m_bGameAobrt;
	BOOL			   m_bUseBigUI;		//是否适应于大分辨率

	//二维码充值start
    BOOL               m_bLargeRoom;//是否为大额房间
    CGameChargeDlg     m_GameCharge;
    CQrcodeCharge*     m_pDlgQrcode;
    //二维码充值end

    //充值界面的URL信息
	CHARGEURL          m_ChargeUrl;
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameApp)
	public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CGameApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	protected:
		int   m_chargeStat;

};

extern CGameTable m_MyGameTable;
extern CGameApp theApp;
extern HHOOK   hMsgBoxHook;   
LRESULT   CALLBACK   CBTProc(int   nCode,   
							 WPARAM   wParam,   
							 LPARAM   lParam)   ;

extern int   MsgBoxEx(HWND   hwnd,   TCHAR   *szText,   TCHAR   *szCaption,   UINT   uType)     ;
