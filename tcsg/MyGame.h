#pragma once
using namespace ShinyNova;
#include "Game.h"
#include "Replay.h"
#include "MySubSystem.h"
#include "TgControls.h"

#include "WelfareDialog.h"
#include <VECTOR>
#include <qrencode.h>
using namespace std;

#define WELFARE_TXT_NAME "welfareRecord.ini"

class CMyGame:public CSKMainGame
{
public:
		 	 CMyGame();
	virtual ~CMyGame();

	//��������
	virtual	SK_PUBLIC_INFO*		GetPublicInfo();
	virtual	SK_PLAYER_INFO*		GetPlayerInfo(int chairno);
	virtual	int					GetTotalCardsCount(){return TOTAL_CARDS;}

	GAME_PUBLIC_INFO*	GetPublicInfoEx();
	virtual	GAME_PLAYER_INFO*	GetPlayerInfoEx(int chairno);
	virtual CGameInfoEx*		GetGameInfoEx();

	virtual void				OnGameSize();
	virtual void				OnIniData();//��ʼ��
	virtual void				GameInstance();//��ʼ��

	virtual BOOL				OnMsg(SNG_MSG& msg);

	virtual void				CreatePlayers(int nTotalChairs);
	virtual void				CreateMatchTable();        //������Ϸ����
	virtual void				CreateCard();

	virtual void				CreateBtn();
	virtual void				CreateClock();
			void				CreateCountDown();

	virtual void				CreateInterface();
	virtual void				CreateCardMaster();

//������ť
	virtual void				OnStart();
	virtual void				OnThrow();
	virtual void				OnPass();
	virtual void				OnReMind();
	virtual void				OnReMind2();
	virtual BOOL				OPE_ReMaindEx(int nInHand[],int nInLen,DWORD dwUniteType,BOOL bnSelect);
	virtual BOOL				OnBestReMind();
	virtual void				OnTimer();
	virtual void				OnCardMaster();
	virtual void				OnAskNewTable();

	virtual void				OnSureZuPai();

public:
//ͨѶ
	virtual void				NTF_StartGame(int chairno);
	virtual void				NTF_GameStart();
	virtual void				NTF_GameWin();
	virtual void				NTF_ZuPai(ZUPAI* pZuPai);
	virtual void				NTF_CardsThrow(CARDS_THROW* pCardsThrow);
	virtual void				NTF_CardsPass(CARDS_PASS* pCardsPass);
	virtual	void				NTF_GameStandOff();
	virtual	void				NTF_PlayerAbort(GAME_ABORT * pGameAbort);
	virtual void				OnGetNewPosition();
	virtual	void				OPE_EndTimer();
public:
//����

				void			OPE_ResetPublicInfo();
	virtual		void            OPE_InitData();  //��ʼ��
	virtual		void            OPE_ClearGame(); //��Ϸ����
	virtual		void			OPE_DXXW();
	virtual		void            OPE_CloseResult();

	virtual		void			OPE_SortHand(int dwFlags=SORT_FLAGS_COMMON);
	virtual		void            OPE_LeftUp();
	virtual		void            OPE_ShowAssistantButton(BOOL bn);
	virtual		void			OPE_ShowButtons();
	virtual		void			OPE_HideButtons();
	virtual		void            OPE_ShowGameInfo();//���ϽǱ�����Ϣ��
	virtual		void            OPE_AddBomb(CARDS_THROW* pCardsThrow);
	virtual		void            OPE_BulidResult();
	virtual		void            OPE_ShowResult();
				void	        OPE_WhileEndGame();
		
	virtual		BOOL            OPE_AssitSelect();//����ѡ��
	virtual		void            OPE_CheckSelect();
	virtual 	BOOL			OPE_BulidCard(int nInCards[], int nInCardLen,
										   int nOutCards[], int nOutCardLen,
										   UNITE_TYPE in_type,
										   UNITE_TYPE& out_type,
										   DWORD dwUniteSupport=CARD_UNITE_TYPE_TOTAL_EX,
										   BOOL bnUseJoker=TRUE);//ȡ��ѹ���ĵ�λ
	virtual 	BOOL			OPE_BulidCardEx(int nInCards[], int nInCardLen,
										     int nOutCards[], int nOutCardLen,
										     UNITE_TYPE in_type,
										     UNITE_TYPE& out_type,
										     DWORD dwUniteSupport=CARD_UNITE_TYPE_TOTAL_EX,
										     BOOL bnUseJoker=TRUE);//ȡ���λ

	virtual		UINT			OPE_ThrowCard(int nSelectCard[], int nCardLen);
	virtual		UINT			OPE_ZuPai();
	virtual		void            OnFreshGameSet();
	virtual		void			OPE_ShowPlayerRest(int chairno,int nRestCount);
	virtual		BOOL			RUL_IsFriend(int chairno);
	virtual		void	        OPE_PlaySoundByOperation(int chairno,DWORD dwOpeFlags,int nCount);//�������ͷ���
	virtual		void	        OPE_PlaySoundByOperation(BOOL bFirstHand, CARDS_THROW *pCardsThrow);//�������ͷ���
				void			OPE_ShowStartButton();
	virtual		void            OnEnterGameOK();
	virtual		void            ShowWaitArrageTable();
				void			OPE_PlaySoundForAuction(int chairno, BOOL bPass, int nGains, BOOL bAutoHide = TRUE);
				void			OPE_PlayAnimationByThrow(CARDS_THROW *pCardsThrow);
				void			FitButtonPosition();
				void			OnSortHandCard();   //��������������
	virtual		void            NTF_ChairCard(CARDS_INFO* pCardInfo);
				POINT			GetActorPosition(int chairno,int &nActorPos);
	virtual		void			OPE_ShowCardType(int chairno,DWORD type,int nCount); 
				void			OPE_HideAllPlayerAnmition();
				void			OPE_FreshUI();
	virtual		void			MoveGameRect(RECT& Rect);
	virtual		void            RUL_CalcGameStatus(int w,int h);

				UINT			GetIntNum(UINT nInteger);
				void			GetRushDigitPos(int chairno, UINT digitnum, POINT& pt);
	virtual		void			RushResultDigit();

public:
	virtual		void            GUI_MoveClock(int chairno,int second);
	virtual		void            GUI_MoveClock(int second);
	virtual		POINT			GetClockPos();
	virtual		void            OPE_GameClockZero();
	virtual		CGameCard*		GetThrowCard(int chairno, int nCardID);
	virtual		void			NTF_GameMsg(GAME_MSG* pGameMsg);

	virtual		void			NTF_StartCountdown(START_COUNTDOWN* pCountdown);
	virtual		void			NTF_SyncStartCountdown(SYNC_COUNTDOWN* pSyncCountdown);
	virtual		void			OPE_StartCountDownTimer();
	virtual		void			GUI_ShowCountDownClock(int second);
	virtual		void			OPE_StopCountDownTimer();
	virtual		void			GUI_HideCountDownClock();
	virtual		void			OnPlayerEnter(GAMEPLAYER* pGamePlayer);
	virtual		void			OnPlayerAbort(GAMEPLAYER* pGamePlayer);

	virtual		void			TakeDepositToGame(int nDestDeposit,int nKeyResult);
	virtual		void			AutoSetDeposit();
	virtual		void			ShowKeyResult(BOOL bShow,int nRandKey, int nFlag);
	virtual		int				GetAutoSetDeposit();

	BOOL	IsBanker(int nChairNO);
	BOOL	IsInGame(int nChairNO);
	BOOL	ValidateChair(int nChairNO);
	void	OPE_ShowBankerFlag(BOOL bShow=TRUE,BOOL bAnimate=FALSE);
	void	OPE_ShowWaitBet();
	void	OPE_HideWaitBet();
	void	FitWaitBetPostion();
	void	OPE_ShowSanGongType(int nChairNO, int nCardType, BOOL bSound);//���ƺ���ʾ����
	int		RUL_GetUnSelectCard(int nCardID[]);
	
	void	CreatePaiXing();
	void	FitPaiXing(int nIndex=-1);
	void    FitStartBtn();
	void	FitCountDownPostion();
	void    FitClockPos();
	void    FitZupaiBtnPos();
	void	ShowClock();
	void	HideClock();
	int		GetBaseValue(int nChairNo);
	double  GetSpeed(int x1, int y1, int x2, int y2, DWORD dwTimes);//dwTimes-ms
	int		GetInHandCardPosCE(int nDrawIndex, int nCardIndex, POINT& pt);
	int		GetInHandCardWidth(int nIndex);
	int		GetInHandCardHeight(int nIndex);
	int		GetThrowCardPosCE(int nIndex, POINT& pt);
	int		GetThrowCardWidth(int nIndex);
	int		GetThrowCardHeight(int nIndex);
	void	GetPlayerPanePos(int index, POINT& pt);
	void	HideWaitNextBoutAll();

	void	ShowHuanZhuoBtn(BOOL bShow);
	int		GetNextChair(int chairno);
	void    EncodeTextToQRBitmapData(LPCTSTR lpText);

	void    OPE_QZ();
	void    OnQiangZhuang(BOOL bQZ);
	void    NTF_QZOK(QZ *pInfo);
	BOOL    IsFinishQZ(int nChairNO);
	void    SetFinishQZ(int nChairNO);
	int		GetQZFlag(int nChairNO);
	void    SetQZFlag(int nChairNO,int nFlag);
	BOOL	GetIsAllGiveUpQZ();
	void	SetIsAllGiveUpQZ(BOOL bAllGiveUpQZ);
	void    QZFinish(QZ *pInfo);
	void    PlayZJAni(int Banker);
	void    BeginYaZhu();
	void    OPE_YaZhu(int nDbl);
	void    NTF_YaZhuOK(YAZHU *pInfo);
	void	YaZhuFinish(YAZHU *pInfo);
	void    OPE_DealSanGongCard();
	void	MOVIE_DealSanGongCard();
	void    BeginZuPai();
	void    CreateIndicatorAni();
	void    FixIndicatorPosition();
	void    UpdataIndicatorIightAni(int nBanker, int nCount);

	void    CreateQZFlag();
	void    FitQZFlag();
	void    OPE_ShowQZ(int nChairNO, int nQZFlag);

	void    CreateYinzi();
	void    OPE_ShowYZYinzi(int nChairNO, int nDbl);
	void    OPE_HideYZYinzi();
	void    OPE_ShowQZButton();
	void    OPE_HideQZButton();
	int     GetYinziPos(int nDrawIndex, POINT& pt);

	void    OPE_HideYZYinziDigit();

	bool    LeftMoneyEnough(int nMoney);

    void    OnGetWelfareConfig(int nActivityID);

	void    ShowCanGetWelfare();
    void    ShowCanGetCallBack();

	void    StopCanGetWelfare();
	void    CreateQZRemaind();
	void    OPE_ShowQZRemaind();
	void    OPE_HideQZRemaind();
	void    OPE_FitQZReamind();
	void    OPE_ShowWelfRushDigit(int nDigitNum);
	void    OPE_FixYZYinzi();
	void    CreateYaZhuDlg();
	void    OPE_ShowYaZhuDlg();
	void    OPE_HideYaZhuDlg();
	void    FreshYaZhuDigit();
	void    OPE_PreFixYinzi(int nChairNO, int nDbl);

	void    FitYZYinZiDigitPos();

	virtual void OnEnterDXXW();
	virtual void OnEnterIdle();

	void	OPE_OnStart();
	void	OPE_ShowBaseDeposit();

public:

//���ÿ�
	 CGameSetBoard*    m_GameSetBoard;

//������
	 CXyCardMaster*    m_CardMaster;
	 CXyStandButton*   m_btnCardMaster;

	  //�Ƿ��ڷ�����
	 CXyStandButton*   m_btnQZ;
	 CXyStandButton*   m_btnNOQZ;
	 CMyCard*          m_nBottomCard[BOTTOM_CARD];
	 CXyStandButton*   m_btnSort;
	 BITObject*		   m_bitActor[TOTAL_CHAIRS];
	 CXyDigit*		   m_AuctionScore;
	 CXyDigit*		   m_BombFanScore;
	 CXyDigit*		   m_digitBaseBig;
	 CXyDigit*		   m_AuctionScoreBig;
	 CXyDigit*		   m_BombFanScoreBig;

	 EVENTHANDLE	   m_hEventCountDownTimer;				// 
	 int			   m_nZeroCountDown;					//
	 CXyDigit*		   m_digitCntDwn;						// ����ʱ����
	 BITObject*		   m_bitCountDown;						// ����ʱ��ͼ
	 BITObject*		   m_bitWaitNextBout[TOTAL_CHAIRS];		// �ȴ�һ�¾�


	 CXyStandButton*    m_btnSureZuPai;						// ȷ������
	 CXyStandButton*	m_btnHuanZhuo;						// ������ť
	 BITObject*			m_bitWaitBet;						// �ȴ�Ѻע

	 BITObject*			m_bitPaiXing[TOTAL_CHAIRS];			// ����	��drawindex
	 BITObject*			m_bitClockBase;						// ʱ�ӵ�ͼ

	 CXyStandButton*	m_btnAutoSetDeposit;				// �Զ�������ť
	 CAutoSetDepositDlg*m_dlgAutoSetDeposit;				// �Զ������Ի���
	 CMyKeyResult*		m_pMyKeyResult;

	 PObject*			m_objRushFrame[TOTAL_CHAIRS];	//��Ӯ�������壬��ʵ��ʵ�ʿ�
	 CXyDigit*			m_digtRush[TOTAL_CHAIRS][2];	//��Ӯ���� 
	 PlaneObject*		m_plRush[TOTAL_CHAIRS];			//��Ӯ������

	 TCHAR				m_szUsername[TOTAL_CHAIRS][MAX_USERNAME_LEN];

	 CShinyNovaPipeline*	m_CEventPipe;

	 ///�
	 int               m_nNewTableCountDown;

	 //��ά���ֵstart
     CXyStandButton*   m_btnCharge;
     //��ά���ֵend

	 CXyStandButton    *m_indicator[TOTAL_CHAIRS+1];
	 BITObject		   *m_bitQZFlag[TOTAL_CHAIRS];

	 BITObject         *m_objYinzi[TOTAL_CHAIRS];//��drawindex
	 CXyDigit          *m_digitYZYinzi[TOTAL_CHAIRS];//��drawindex
	 PlaneObject       *m_plQZRemaind;
	 
	 //welfare begin
	 CWelfareDialog    *m_welfDialog;
	 CXyStandButton    *m_bitGetWelfare;	//������ť
     BITObject         *m_welfLight;
	 PObject           *m_obWelfFrame;
	 PlaneObject       *m_plWelfRush;
	 CXyDigit          *m_digtWelfRush;
	 //welfare	end
	 int                m_welfareSurplusNum;   //���ؼ�¼��ȡ�ͱ��Ĵ���
	 int                m_nDailyNum;
	 EVENTHANDLE        m_eventId;
	 int                m_nActivityID;

	 DWORD              m_dwSoundId;

	 PlaneObject       *m_plYaZhuFrame;
	 CXyStandButton    *m_btnYaZhu[YAZHU_BTN_COUNT];
	 CXyDigit          *m_digitYaZhuDbl[YAZHU_BTN_COUNT];
	 BOOL				m_bTongBi;

	 CXyStandButton    *m_btnCareType;
	 BITObject         *m_bitCardType;
	 BITObject         *m_bitBaseDeposit;
	 CXyDigit		   *m_digitBaseDeposit[TOTAL_CHAIRS-1];
};
