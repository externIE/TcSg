#pragma once

#define		WM_GTR_CHANGE_SPECIFYTABLE_EX	 (WM_USER+2009)

#define		GR_ZU_PAI					(GAME_REQ_BASE_EX + 25110)		// 玩家出牌信息
#define     GR_QZ						(GAME_REQ_BASE_EX + 25151)		// 看牌抢庄 抢庄消息
#define     GR_YAZHU					(GAME_REQ_BASE_EX + 25152)		// 看牌抢庄 押注消息

//按银两分桌
#define     TCSG_S2C_NEWTABLE_GRADE          (GAME_REQ_BASE_EX+25600)//银两分桌等级数据
#define     TCSG_C2S_NEWTABLE_EX             (GAME_REQ_BASE_EX+25700)//按银两分桌请求
#define     TCSG_C2S_REQTABLE_GRADE          (GAME_REQ_BASE_EX+25800)//请求
#define     TCSG_WELFAREACTIVITYID			 (GAME_REQ_BASE_EX+25801)//低保
#define     TCSG_S2C_SENDPROPPRICE			 (GAME_REQ_BASE_EX+25802)//服务端发送道具价格给客户端
#define     TCSG_CHARGE_ALLURL               (GAME_REQ_BASE_EX+25803)//获取充值相关的网页地址
#define     TCSG_ISTONGBIROOM			     (GAME_REQ_BASE_EX+25804)//是否是通比房间
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
	int				nUserID;								// 用户ID
	int				nRoomID;								// 房间ID
	int				nTableNO;								// 桌号
	int				nChairNO;								// 位置
	BOOL			bPassive;								// 是否被动
	int				nCardID[CHAIR_CARDS];					//排序后的牌
	int				nCardType;								//牌型
	int				nReserved[4];
}ZUPAI,*LPZUPAI;


typedef struct _tagSANGONG_DETAIL
{
	CARDINFO	stCardInfo[CHAIR_CARDS];				//排序后的牌
	int			nCardPoint[CHAIR_CARDS];				//排序后每张牌点数
	int			nMaxCardIndex;							//最大的那张牌的索引（stCardInfo[nMaxCardIndex]）
	int			nCardType;								//牌型 
}SANGONG_DETAIL,*PSANGONG_DETAIL;

typedef struct _tagGAME_START_INFO{
	//需同SK_START_INFO一致 
	//begin
	TCHAR	szSerialNO[MAX_SERIALNO_LEN];//本局序列号
	int		nBoutCount;				// 第几局
	int		nBaseDeposit[TOTAL_CHAIRS];			// 基本银子
	int		nBaseScore;				// 基本积分
	BOOL	bNeedDeposit;			// 是否需要银子
	BOOL	bForbidDesert;			// 禁止强退
	int     nBanker;                // 庄家，初始玩家
	int		nCurrentChair;			// 当前活动椅子号 
   	DWORD	dwStatus;				// 当前状态
	int     nThrowWait;             // 下棋等待时间
	int     nAutoGiveUp;            // 自动放弃时间
	int     nOffline;               // 掉线检测时间
	int     nInHandCount;			// 手牌数量
	//同SK_START_INFO一致 
	//end
	int     nHandID[CHAIR_CARDS];	// 每人手牌
	int		nObjectGains;			//标分
	int		nBottomIDs[BOTTOM_CARD];
	///////////////////////////////////////////////////
	BOOL	bInGame[TOTAL_CHAIRS];			// 是否在游戏中
	int		nQZWait;						// 抢庄等待时长
	int		nYaZhuWait;						// 押注等待时长
	int		nZuPaiWait;						// 组牌等待时长
	int		nTongBiBaseDeposit[TOTAL_CHAIRS][TOTAL_CHAIRS];//通比场基础银
	
	int	    nReserved[4];					
}GAME_START_INFO, *LPGAME_START_INFO;


//游戏公共信息,所有玩家都可见的信息
typedef struct _tagGAME_PUBLIC_INFO
{
	//需同SK_PUBLIC_INFO一致 
	//begin
	int         nWaitChair;		 //首家出牌
    UNITE_TYPE  WaitCardUnite;   //首家等待的牌型
	int         nCurrentCatch;
	int         nCurrentRank;
	DWORD		dwUserStatus[MAX_CHAIRS_PER_TABLE];	//玩家状态
	//同SK_PUBLIC_INFO一致 
	//end
	CARDINFO	GameCard[TOTAL_CARDS];							//储存所有牌信息
	int			nAuctionRound;									//叫了几轮庄
	int			nBombFan;										//炸弹翻倍
	int         nResultDiff[MAX_CHAIR_COUNT][MAX_RESULT_COUNT];
	int			nTotalResult[MAX_CHAIR_COUNT];
	/////////////////////////////////////////////////////////////
	BOOL		bFinishQZ[TOTAL_CHAIRS];
	int			nQZFlag[TOTAL_CHAIRS];
	BOOL		bAllGiveUpQZ;
	int			nYaZhuDbl[TOTAL_CHAIRS];
	BOOL		bFinishYaZhu[TOTAL_CHAIRS];
	BOOL		bFinishZuPai[TOTAL_CHAIRS]; 	//是否完成组牌了

	int			nReserved[4];
}GAME_PUBLIC_INFO,*LPGAME_PUBLIC_INFO;

//玩家私人信息，只有自己可见
typedef struct _tagGAME_PLAYER_INFO
{	//begin
	//需同SK_PLAYER_INFO一致	
	int         nWaitTime;						//当前等待时间
	int         nThrowTime;						//出牌时间
	int         nTotalThrowCost;				//总出牌
	int         nInHandCount;					//手牌数量
	int  	    nAutoThrowCount;				// 托管出牌次数
	//需同SK_PLAYER_INFO一致 
	//end
	int         nBombCount;						//炸弹数量
	int			nThrowCount;					//出牌次数
	int			nAskExitCount;					//	
	/////////////////////////////////////////////////////////////
	SANGONG_DETAIL	stDetail;					//组牌详细信息	

    int			nReserved[4];		
}GAME_PLAYER_INFO,*LPGAME_PLAYER_INFO;

typedef struct _tagGAME_TABLE_INFO{
	GAME_START_INFO		gamestart;
	//公共信息
	GAME_PUBLIC_INFO    m_Public;
	//玩家私密信息
	GAME_PLAYER_INFO    m_Player[TOTAL_CHAIRS];
	//保留字
	int					nReserved[4];
}GAME_TABLE_INFO, *LPGAME_TABLE_INFO;

typedef struct _tagGAME_WIN_RESULT{
	GAME_WIN	gamewin;
	int         nCardID[TOTAL_CHAIRS][CHAIR_CARDS];//所有玩家的剩余手牌
	int         nCardCount[TOTAL_CHAIRS];
	int			nBottomIDs[BOTTOM_CARD];
	int			nNextBaseScore;			//下一局基础分
	int			nBombFan;
	int			nSpring;				//春天翻倍
	//////////////////////////////////////////////////////
	int         nGainsWin[TOTAL_CHAIRS];//输赢详细
	int			nReserved[4];
}GAME_WIN_RESULT, *LPGAME_WIN_RESULT;

enum{
	TCSG_GAME_MSG_BEGIN = LOCAL_GAME_MSG_END,
	TCSG_GAME_MSG_ENDGAME,
	TCSG_GAME_MSG_AUTO_NO_QZ,		//抢庄阶段：自动不抢庄
	TCSG_GAME_MSG_AUTO_YAZHU,		//押注阶段：自动押1倍
	TCSG_GAME_MSG_AUTO_ZUPAI,		//组牌阶段：自动摊牌
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

//二维码充值start
#define GR_ROOMLIMIT            (GAME_REQ_BASE_EX + 20612)//房间额度信息，小额房间还是大额房间
#define GR_CHARGEBTN_LEFTDOWN   (GAME_REQ_BASE_EX + 20613)//充值按钮被点击，发送给服务端
typedef struct  _tagROOMLIMIT
{
    int     nUserID;
    int     nRoomID;
    int     nTableNO;
    int     nChairNO;
	
    BOOL    bLargeRoom;//是否为大额房间
}ROOMLIMIT, *LPROOMLIMIT;

enum CHARGE_BTN_TYPE
{
    QRCODE_CHARGE_BTN = 0,  //二维码充值
	QRCODE_CHARGE_BTN_WECHAT = 1,  //微信二维码充值
	OTHERWAY_CHARGE_BTN,    //其他方式充值
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
//二维码充值end

//充值界面信息
typedef struct _tagCHARGEURL{
	TCHAR   WebUrl[MAX_PATH];
	TCHAR   OrCodeUrl[MAX_PATH];
	TCHAR   OrCodeUrl_WeChat[MAX_PATH];
	TCHAR   TongBaoUrl[MAX_PATH];
	TCHAR   LearnMoreUrl[MAX_PATH];
}CHARGEURL, *LPCHARGEURL;