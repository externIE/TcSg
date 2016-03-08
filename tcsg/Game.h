// Game.h : main header file for the GAME application
//

#pragma once


#include "resource.h"       // main symbols
#include "GameInfo.h"
#include "GameRequest.h"
#include "GameNotifyEx.h"
//��ά���ֵstart
#include "GameChargeDlg.h"
#include "QrcodeCharge.h"
//��ά���ֵend
#include "WelfareDialog.h"
using namespace ShinyNova;


class CGameApp : public CSKGameWinApp
{
public:
	CGameApp();
	~CGameApp();

	virtual  BOOL				CreateGameFrame();         //����������Cwnd
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
	virtual  void				CreateArrageTable();//�齨�µ�����
	virtual  void				DisBandTable();

	virtual  int				GetTableChairCount();
	virtual	 int				GetThrowWait();
	virtual  int				GetCurrentRank();
	virtual	 int				GetChairCardsCount();

	virtual  BOOL				IsTakeDepositInGame();
	virtual	 BOOL				IsAllowAskExit();//����Ϸ�����Ƿ�����Э���˳�

			 void				AchToUI();					 // ����EnterGameʱ
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

	//log����Dbgview���Խ���
	inline void OutputDebugStringEx(CString format,...)
	{
		CString s;
		va_list args;
		va_start(args,format);
		s.FormatV(format,args);
		va_end(args); 
		::OutputDebugString(s);
	}

	//��ά���ֵstart
    virtual void    SetRoomLimit(BOOL bLargeRoom){m_bLargeRoom = bLargeRoom;};
    virtual void    ShowGameCharge();//��ʾ��ֵ�Ի���
    virtual void    ShowQrChargeDlg(CString& strDstUrl);
    //��ά���ֵend 

	int            GetChargeStat() {return m_GameCharge.GetChargeStat();}
	void           SetChargeStat(int chargeStat) {m_GameCharge.SetChargeStat(chargeStat);}
public:
	GAME_SET           m_GameSet;    //��Ϸ��������
    CGameRequest       m_GameRequest;//����������������Ӳ���������
	CGameNotifyEx      m_GameNotifyEx;//����ӷ���������֪ͨ

	CGameInfoEx		   m_GameInfoEx;//��׿��Ϸ��Ϣ,

	BOOL			   m_bGameAobrt;
	BOOL			   m_bUseBigUI;		//�Ƿ���Ӧ�ڴ�ֱ���

	//��ά���ֵstart
    BOOL               m_bLargeRoom;//�Ƿ�Ϊ����
    CGameChargeDlg     m_GameCharge;
    CQrcodeCharge*     m_pDlgQrcode;
    //��ά���ֵend

    //��ֵ�����URL��Ϣ
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
