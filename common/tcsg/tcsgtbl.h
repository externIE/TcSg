#pragma once


#pragma warning(disable : 4786)
#include <map>

extern void InitialGameTableInfo(GAME_TABLE_INFO* table);
extern int  My_GetIndexPRI(int nCardID,int nRank, DWORD gameflags); //大小

extern int  TCSG_CalcCardValue(int nCardID);
extern int	TCSG_CalcCardPoint(int nCardID);

class CGameTable : public CSkTable{
public:
	CGameTable(int roomid = INVALID_OBJECT_ID, int tableno = INVALID_OBJECT_ID, int score_mult = 1, 
			int totalchairs = TOTAL_CHAIRS, DWORD gameflags = GAME_FLAGS,
			DWORD gameflags2 = 0, 
			int max_asks = MAX_ASK_REPLYS,
			int totalcards = TOTAL_CARDS, 
			int totalpacks = CARD_PACKS, int chaircards = CHAIR_CARDS, int bottomcards = 0,
			int layoutnum = LAYOUT_NUM, int layoutmod = SK_LAYOUT_MOD, int layoutnumex = 0, 
			int abtpairs[] = NULL,
			int throwwait = DEF_THROW_WAIT, int maxautothrow = MAX_AUTO_THROW,
			int entrustwait = DEF_ENTRUST_WAIT,
			int max_auction = 3, int min_auction = MIN_AUCTION_GAINS,
			int def_auction = 1,
			FP_SK_GetXXX fpSKGetCardIndex = SK_GetCardIndex,
			FP_SK_GetXXX fpSKGetCardShape = SK_GetCardShape,
			FP_SK_GetXXX fpSKGetCardValue = SK_GetCardValue,
			FP_SK_GetXXX fpSKGetCardScore = SK_GetCardScore,
			FP_SK_GetXXXEx fpSKGetCardPRI = SK_GetCardPRI,
			FP_SK_GetXXXEx fpSKGetIndexPRI = My_GetIndexPRI);
	virtual ~CGameTable();

public:
	//////////////////////////////////////////////////////////////////////////
	//重载区
	virtual void ResetMembers(BOOL bResetAll = TRUE);
	//各种fill，需要重载
	virtual int				GetGameTableInfoSize();
	virtual int				GetGameStartSize();
	virtual int				GetGameWinSize();
	virtual int				GetEnterGameInfoSize();
	virtual void			FillupEnterGameInfo(void* pData, int nLen, int chairno, BOOL lookon = FALSE);
	virtual void			FillupGameTableInfo(void* pData, int nLen, int chairno, BOOL lookon = FALSE);
	virtual void			FillupGameStart(void* pData, int nLen, int chairno, BOOL lookon = FALSE);
	virtual int				FillupGameWin(void* pData, int nLen, int chairno);
	virtual void			FillupStartData(void* pData, int nLen);
	//牌操作相关，需要重载
	virtual int				CatchOneCard(int chairno);					 
	virtual BOOL			GiveCard(int chairno,int destchair,int nCardID);	 			 
	virtual BOOL			SetCardStatus(int nCardID,int chairno,int nStatus);		 
	virtual void			PutThrowCardsToCost(int chairno);					 
	virtual void			PutAllCardsToCost();								 
    virtual int				GetTributeCard(int chairno);	
	virtual CARDINFO*		GetCard(int nCardID);								 
	virtual int				GetInHandCard(int chairno,int nCardIDs[]);		 
	virtual BOOL			IsCardInHand(int nChairNO,int nCardIDs[],int nCount);
	//出牌过程，需要重载
	virtual BOOL			ValidatePass(CARDS_PASS* pCardsPass);
	virtual BOOL			ValidateThrow(CARDS_THROW* pCardsThrow);		 
	virtual int				ThrowCards(CARDS_THROW* pCardsThrow);			 
	virtual int				CalcChairThrowTime(int chairno);
	virtual void			CalcBombInThrow(CARDS_THROW* pCardsThrow);	 				 
	virtual void			OnPass(CARDS_PASS* pCardsPass);
	virtual BOOL			CalcWinOnThrow(CARDS_THROW* pCardsThrow);			 
	virtual BOOL			CalcWinOnPass(CARDS_PASS* pCardsPass);			 
	virtual void			SetCurrentRank(int nRank);						 
	virtual int				GetCurrentRank();	
	//选择重载
	virtual int				CalcBankerChairBefore();
	virtual int				CalcBankerChairAfter(void* pData, int nLen);
	virtual int				CalcResultDiffs(void* pData, int nLen, int nScoreDiffs[], int nDepositDiffs[]);
	virtual BOOL			CalcWinPoints(void* pData, int nLen, int chairno, int nWinPoints[]);
	virtual void			FillupNextBoutInfo(void* pData, int nLen, int chairno);
	virtual DWORD			SetStatusOnStart();
	virtual int				SetCurrentChairOnStart();
	virtual BOOL			IsGameMsg(UINT resquesID);//查询是不是游戏消息

	virtual BOOL			CaclCardType_ABT_Couple(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail);
    virtual BOOL			GetCardType_ABT_Couple(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type,int nMaxPair=3);
	virtual BOOL			CaclCardType_ABT_Single(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail);
	virtual BOOL			GetCardType_ABT_Single(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type,int nMaxCount=12);
	virtual BOOL			GetCardType_Bomb(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type,int nUseCount=0);
	virtual BOOL            GetUniteDetails(int chairno, int nCardIDs[],int nCardsLen,CARD_UNITE& unit_detail,DWORD dwFlags=CARD_UNITE_TYPE_TOTAL_EX);
	virtual BOOL			CaclCardType_ABT_Three_Couple(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail);
	virtual BOOL			GetCardType_ABT_Three_Couple(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type,int nMaxPair=2);
			int				GetLayPri(int nCardLay[], int nLayLen);	//lay中有值的权值,（nCardLay值表示个数）
			int				GetBit1Count(unsigned int bit);			//得到数值二进制里1的个数
			int				GetIndexByIndex(int nCardLay[], int nLayLen, int nIndex);		//得到第nIndex个数的index
			int				GetLayPriEx(int nCardLay[], int nLaylen);	//lay中有值的权值,（nCardLay值表示这个值）


	//三带一
	virtual BOOL			CaclCardType_Three_1(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail);
	virtual BOOL			GetCardType_Three_1(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,
									int nJokerCount,UNITE_TYPE& type);
	//飞机带单张
	virtual BOOL			CaclCardType_ABT_Three_1(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail);
	virtual BOOL			GetCardType_ABT_Three_1(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type,int nMaxPair=2);

	//4带2张单
	virtual BOOL			CalcCardType_Four_2(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail);
	virtual BOOL			GetCardType_Four_2(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type);

	//4带2对子
	virtual BOOL			CalcCardType_Four_2_Couple(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail);
	virtual BOOL			GetCardType_Four_2_Couple(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type);

	//大小王
	virtual BOOL			CalcCardType_BOMB_2King(int nCardIDs[],int nCardLen,int nCardCount,CARD_UNITE* CardDetail);
	virtual BOOL			GetCardType_BOMB_2King(int nCardIDs[],int nCardLen,int nCardLay[], int nLayLen,int nJokerCount,UNITE_TYPE& type);

	virtual BOOL			GetBestUnitType(UNITE_TYPE& first_card,CARD_UNITE& fight_card);
	virtual BOOL			GetBestUnitType(CARD_UNITE& fight_card);

	virtual	void			ActuallizeResults(void* pData, int nLen);
			
	//断线处理：4次掉线出牌，断桌子
			void			OnPlayerPassiveEvent(int chairno);
			void			OnPlayerActiveEvent(int chairno);
			BOOL			IsTooManyAutoPlay(int chairno);
	virtual int				CalcDoubleOfScore(int chairno, int breakchair, int defdouble);
	virtual int				CalcBreakDeposit(int breakchair, int breakdouble, int& cut);
	virtual void			ResetTable();//清除桌子游戏，局数重新开始

	//做炸弹牌
			BOOL			MakeDealCards(int nMakeChance);
			int				GetRandomBombNum(int seed);
			void			MakeCardsLayIn(int nChair,int nBombNum,int& nCurIndex,int nBombValue[]);
			void			DealCardNormal();
			int				IsMakedCard(int nCardID);
	//////////////////////////////////////////////////////////////////////////

	GAME_TABLE_INFO*  GetGameTableInfo();
	GAME_START_INFO*  GetStartInfo();
	GAME_PUBLIC_INFO* GetPublicInfo();
	GAME_PLAYER_INFO* GetPlayerInfo(int chairno);

			void	ConstructGameData();
	virtual void	StartDeal();
	virtual	BOOL	CheckCards();	//发完牌后，检查下牌

	virtual int		CalcMaxCard(CARDINFO stCardInfo[], int nLen);			//计算最大的牌的索引
	virtual int		CalcMaxCard(int nCard[], int nLen);						//计算最大的牌的索引
	virtual	int		CalcCardType(int nChairNO, PSANGONG_DETAIL pDetail);								//计算牌型
	virtual int		OnZuPai(ZUPAI* pZuPai);
	virtual BOOL	AllZuPai();
	virtual BOOL	IsNeedCountdown();
	virtual int		CalcBetDeposit();
	
	virtual	int		CalcBaseDeposit(int nDeposits[], int tableno);
			int		CalcPlayerBaseDeposit(int chairno,int nBanker);

	virtual int		CalcOpeTime();				//流程操作时间
	virtual	BOOL	IsAllAllowExit(int askChairNo);
	virtual BOOL	UpdataGameWin(LPREFRESH_RESULT_EX lpRefreshResult,CPlayerLevelMap& mapPlayerLevel);

	BOOL			ValidateBetDbl(int nDbl);
	int				GetWinGains(SANGONG_DETAIL& chairDtl, SANGONG_DETAIL& bankerDtl);//闲家赢庄家牌型倍数
	virtual int		TellBreakChair(int leavechair, DWORD waitsecs);
	virtual BOOL	IsOperateTimeOver();//操作时间是否已经耗尽

	GAME_TABLE_INFO* m_GameTalbeInfo;
	int			  m_nAutoPlayCount[TOTAL_CHAIRS];
	int           m_nResultDiff[MAX_CHAIR_COUNT][MAX_RESULT_COUNT];
	int			  m_nTotalResult[MAX_CHAIR_COUNT];
	PRE_DEAL	  m_nBombHadDeal[LAYOUT_NUM];
	int			  m_nBottomCatch[BOTTOM_CARD];

	BOOL		  m_bInGame[TOTAL_CHAIRS];
	int			  m_nPlayerBaseDeposit[TOTAL_CHAIRS];
	int			  m_nQZTime;
	int			  m_nYaZhuTime;
	int			  m_nOperateTime;
	int			  m_nWinGains[TOTAL_CHAIRS];

	//当某一个玩家离开时 会使庄家重置 修正这个问题
	BOOL		  m_bResetBanker;
	//m_nBanker会被置为0
	int			  m_nTempBanker;
	void		  SetResetBanker(BOOL bReset);


	int				OnQiangZhuang(QZ* pQZ);
	BOOL			AllFinishQZ(QZ* pQZ);
	DWORD			SetStatusOnFinishQZ();
	int				OnYaZhu(YAZHU* pYaZhu);
	BOOL			AllFinishYaZhu(YAZHU* pYaZhu);
	DWORD			SetStatusOnFinishYaZhu();
	void			DealCardAfterYaZhu();
	void			FillupHandCardAfterYaZhu(LPYAZHU pYaZhu);
	void			DealCardNormal_Begin();
	void			DealCardNormal_End();

	DWORD           m_dwActionDealCard;
	DWORD			m_dwActionYaZhu;
	DWORD			m_dwStatusOnStart;
	BOOL			m_bTongBiRoom;
	int				m_nTongBiWinGains[TOTAL_CHAIRS][TOTAL_CHAIRS];
	int				m_nTongBiBaseDeposit[TOTAL_CHAIRS][TOTAL_CHAIRS];
	
public://通讯相关
	DWORD   m_dwLastClockStop;

//读取配置文件
	BOOL    ReadCardsFromFile();
	CString GetINIFileName();
	int     RetrieveFields ( TCHAR *buf, TCHAR **fields, int maxfields, TCHAR**buf2 );
public:
	int CompensateDeposits(int nOldDeposits[], int nDepositDiffs[]);
/////////////////////////////////////////////////////////////////
	
};

inline void SvrReversalMoreByValue(int array[],int value[],int length)
{
	
	int i,j,temp; 
	for(i=0;i<length-1;i++) 
		for(j=i+1;j<length;j++) /*注意循环的上下限*/ 
			if(value[i]<value[j]) 
			{ 
				temp=array[i]; 
				array[i]=array[j]; 
				array[j]=temp;
				temp=value[i]; 
				value[i]=value[j]; 
				value[j]=temp;
			}
}

//以Seed为随机数，对数组array随机排序
inline void  SvrXygRandomSort(int array[],int length,int seed)
{
	srand(seed);
	int* value=new int[length];
	int s=length*1000;
	for(int i=0;i<length;i++)
		value[i]=rand()%s;
	SvrReversalMoreByValue(array,value,length);
	delete []value;
}

