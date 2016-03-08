#pragma once

#define		WM_GTR_CHANGE_SPECIFYTABLE_EX	 (WM_USER+2009)

#define		GR_ZU_PAI					(GAME_REQ_BASE_EX + 25110)		// ��ҳ�����Ϣ
#define     GR_QZ						(GAME_REQ_BASE_EX + 25151)		// ������ׯ ��ׯ��Ϣ
#define     GR_YAZHU					(GAME_REQ_BASE_EX + 25152)		// ������ׯ Ѻע��Ϣ

//����������
#define     TCSG_S2C_NEWTABLE_GRADE          (GAME_REQ_BASE_EX+25600)//���������ȼ�����
#define     TCSG_C2S_NEWTABLE_EX             (GAME_REQ_BASE_EX+25700)//��������������
#define     TCSG_C2S_REQTABLE_GRADE          (GAME_REQ_BASE_EX+25800)//����
#define     TCSG_WELFAREACTIVITYID			 (GAME_REQ_BASE_EX+25801)//�ͱ�
#define     TCSG_S2C_SENDPROPPRICE			 (GAME_REQ_BASE_EX+25802)//����˷��͵��߼۸���ͻ���
#define     TCSG_CHARGE_ALLURL               (GAME_REQ_BASE_EX+25803)//��ȡ��ֵ��ص���ҳ��ַ
#define     TCSG_ISTONGBIROOM			     (GAME_REQ_BASE_EX+25804)//�Ƿ���ͨ�ȷ���
//response

typedef struct _tagQZ
{
	int				nUserID;								
	int				nRoomID;								
	int				nTableNO;								
	int				nChairNO;
	int				nQZFlag;
	int				nFinish;
	int				nBanker;
	BOOL			bAllGiveUpQZ;
	int				nBaseDeposit[TOTAL_CHAIRS];
	int				nReserved[4];
}QZ,*LPQZ;

typedef struct _tagYAZHU
{
	int				nUserID;
	int				nRoomID;	
	int				nTableNO;
	int				nChairNO;
	int				nDbl;
	int				nFinish;
	int				nCardID[TOTAL_CHAIRS][CHAIR_CARDS];
	int				nReserved[4];
}YAZHU,*LPYAZHU;

typedef struct _tagZuPai
{
	int				nUserID;								// �û�ID
	int				nRoomID;								// ����ID
	int				nTableNO;								// ����
	int				nChairNO;								// λ��
	BOOL			bPassive;								// �Ƿ񱻶�
	int				nCardID[CHAIR_CARDS];					//��������
	int				nCardType;								//����
	int				nReserved[4];
}ZUPAI,*LPZUPAI;


typedef struct _tagSANGONG_DETAIL
{
	CARDINFO	stCardInfo[CHAIR_CARDS];				//��������
	int			nCardPoint[CHAIR_CARDS];				//�����ÿ���Ƶ���
	int			nMaxCardIndex;							//���������Ƶ�������stCardInfo[nMaxCardIndex]��
	int			nCardType;								//���� 
}SANGONG_DETAIL,*PSANGONG_DETAIL;

typedef struct _tagGAME_START_INFO{
	//��ͬSK_START_INFOһ�� 
	//begin
	TCHAR	szSerialNO[MAX_SERIALNO_LEN];//�������к�
	int		nBoutCount;				// �ڼ���
	int		nBaseDeposit[TOTAL_CHAIRS];			// ��������
	int		nBaseScore;				// ��������
	BOOL	bNeedDeposit;			// �Ƿ���Ҫ����
	BOOL	bForbidDesert;			// ��ֹǿ��
	int     nBanker;                // ׯ�ң���ʼ���
	int		nCurrentChair;			// ��ǰ����Ӻ� 
   	DWORD	dwStatus;				// ��ǰ״̬
	int     nThrowWait;             // ����ȴ�ʱ��
	int     nAutoGiveUp;            // �Զ�����ʱ��
	int     nOffline;               // ���߼��ʱ��
	int     nInHandCount;			// ��������
	//ͬSK_START_INFOһ�� 
	//end
	int     nHandID[CHAIR_CARDS];	// ÿ������
	int		nObjectGains;			//���
	int		nBottomIDs[BOTTOM_CARD];
	///////////////////////////////////////////////////
	BOOL	bInGame[TOTAL_CHAIRS];			// �Ƿ�����Ϸ��
	int		nQZWait;						// ��ׯ�ȴ�ʱ��
	int		nYaZhuWait;						// Ѻע�ȴ�ʱ��
	int		nZuPaiWait;						// ���Ƶȴ�ʱ��
	int		nTongBiBaseDeposit[TOTAL_CHAIRS][TOTAL_CHAIRS];//ͨ�ȳ�������
	
	int	    nReserved[4];					
}GAME_START_INFO, *LPGAME_START_INFO;


//��Ϸ������Ϣ,������Ҷ��ɼ�����Ϣ
typedef struct _tagGAME_PUBLIC_INFO
{
	//��ͬSK_PUBLIC_INFOһ�� 
	//begin
	int         nWaitChair;		 //�׼ҳ���
    UNITE_TYPE  WaitCardUnite;   //�׼ҵȴ�������
	int         nCurrentCatch;
	int         nCurrentRank;
	DWORD		dwUserStatus[MAX_CHAIRS_PER_TABLE];	//���״̬
	//ͬSK_PUBLIC_INFOһ�� 
	//end
	CARDINFO	GameCard[TOTAL_CARDS];							//������������Ϣ
	int			nAuctionRound;									//���˼���ׯ
	int			nBombFan;										//ը������
	int         nResultDiff[MAX_CHAIR_COUNT][MAX_RESULT_COUNT];
	int			nTotalResult[MAX_CHAIR_COUNT];
	/////////////////////////////////////////////////////////////
	BOOL		bFinishQZ[TOTAL_CHAIRS];
	int			nQZFlag[TOTAL_CHAIRS];
	BOOL		bAllGiveUpQZ;
	int			nYaZhuDbl[TOTAL_CHAIRS];
	BOOL		bFinishYaZhu[TOTAL_CHAIRS];
	BOOL		bFinishZuPai[TOTAL_CHAIRS]; 	//�Ƿ����������

	int			nReserved[4];
}GAME_PUBLIC_INFO,*LPGAME_PUBLIC_INFO;

//���˽����Ϣ��ֻ���Լ��ɼ�
typedef struct _tagGAME_PLAYER_INFO
{	//begin
	//��ͬSK_PLAYER_INFOһ��	
	int         nWaitTime;						//��ǰ�ȴ�ʱ��
	int         nThrowTime;						//����ʱ��
	int         nTotalThrowCost;				//�ܳ���
	int         nInHandCount;					//��������
	int  	    nAutoThrowCount;				// �йܳ��ƴ���
	//��ͬSK_PLAYER_INFOһ�� 
	//end
	int         nBombCount;						//ը������
	int			nThrowCount;					//���ƴ���
	int			nAskExitCount;					//	
	/////////////////////////////////////////////////////////////
	SANGONG_DETAIL	stDetail;					//������ϸ��Ϣ	

    int			nReserved[4];		
}GAME_PLAYER_INFO,*LPGAME_PLAYER_INFO;

typedef struct _tagGAME_TABLE_INFO{
	GAME_START_INFO		gamestart;
	//������Ϣ
	GAME_PUBLIC_INFO    m_Public;
	//���˽����Ϣ
	GAME_PLAYER_INFO    m_Player[TOTAL_CHAIRS];
	//������
	int					nReserved[4];
}GAME_TABLE_INFO, *LPGAME_TABLE_INFO;

typedef struct _tagGAME_WIN_RESULT{
	GAME_WIN	gamewin;
	int         nCardID[TOTAL_CHAIRS][CHAIR_CARDS];//������ҵ�ʣ������
	int         nCardCount[TOTAL_CHAIRS];
	int			nBottomIDs[BOTTOM_CARD];
	int			nNextBaseScore;			//��һ�ֻ�����
	int			nBombFan;
	int			nSpring;				//���췭��
	//////////////////////////////////////////////////////
	int         nGainsWin[TOTAL_CHAIRS];//��Ӯ��ϸ
	int			nReserved[4];
}GAME_WIN_RESULT, *LPGAME_WIN_RESULT;

enum{
	TCSG_GAME_MSG_BEGIN = LOCAL_GAME_MSG_END,
	TCSG_GAME_MSG_ENDGAME,
	TCSG_GAME_MSG_AUTO_NO_QZ,		//��ׯ�׶Σ��Զ�����ׯ
	TCSG_GAME_MSG_AUTO_YAZHU,		//Ѻע�׶Σ��Զ�Ѻ1��
	TCSG_GAME_MSG_AUTO_ZUPAI,		//���ƽ׶Σ��Զ�̯��
	TCSG_GAME_MSG_END,
};

typedef struct _tagPreDeal{
	int		    chairno;
	int			nValueIndex;
}PRE_DEAL, *LPPRE_DEAL;


#define MAX_TABLE_GRADE 32
typedef struct _tagTABLEGRADE
{
	int nStartTable;
	int nEndTable;
	int nMinDeposit;
	int nPlayerCount;
}TABLEGRADE,*LPTABLERGRADE;

typedef struct _tagTABLESILVERGRADEALL
{
	int nCount;
	TABLEGRADE grade[MAX_TABLE_GRADE];
}TABLEGRADEALL,*LPTABLEGRADEALL;

//��ά���ֵstart
#define GR_ROOMLIMIT            (GAME_REQ_BASE_EX + 20612)//��������Ϣ��С��仹�Ǵ���
#define GR_CHARGEBTN_LEFTDOWN   (GAME_REQ_BASE_EX + 20613)//��ֵ��ť����������͸������
typedef struct  _tagROOMLIMIT
{
    int     nUserID;
    int     nRoomID;
    int     nTableNO;
    int     nChairNO;
	
    BOOL    bLargeRoom;//�Ƿ�Ϊ����
}ROOMLIMIT, *LPROOMLIMIT;

enum CHARGE_BTN_TYPE
{
    QRCODE_CHARGE_BTN = 0,  //��ά���ֵ
	QRCODE_CHARGE_BTN_WECHAT = 1,  //΢�Ŷ�ά���ֵ
	OTHERWAY_CHARGE_BTN,    //������ʽ��ֵ
};

typedef struct _tagCHARGEBTN
{
    int             nUserID;
    int             nRoomID;
    int             nTableNO;
    int             nChairNO;
	
    CHARGE_BTN_TYPE enType;
    int             nChargeNumber;
}CHARGEBTN, *LPCHARGEBTN;
//��ά���ֵend

//��ֵ������Ϣ
typedef struct _tagCHARGEURL{
	TCHAR   WebUrl[MAX_PATH];
	TCHAR   OrCodeUrl[MAX_PATH];
	TCHAR   OrCodeUrl_WeChat[MAX_PATH];
	TCHAR   TongBaoUrl[MAX_PATH];
	TCHAR   LearnMoreUrl[MAX_PATH];
}CHARGEURL, *LPCHARGEURL;