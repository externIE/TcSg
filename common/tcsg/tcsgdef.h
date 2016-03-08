#pragma once

#define		PRODUCT_LICENSE	 	 _T("")
#define		PRODUCT_NAME		 _T("TcSgSvr")
#define		PRODUCT_VERSION		 _T("2.00")
#define     STR_SERVICE_NAME     _T("TcSgSvr")
#define     STR_DISPLAY_NAME     _T("同城游三公服务")
#define     STR_DISPLAY_NAME_ENU _T("TCY TcSgSvr Service") 

#define		GAME_ID				  687	//需要修改
#define		GAME_PORT			  21687 //需要修改


#define		TOTAL_CHAIRS			4		//多少玩家
#define     TOTAL_CARDS				54		//总牌数
#define     CHAIR_CARDS				3		//每人拥有牌的最大数
#define     QZ_WAIT					5		//抢庄等待时长
#define     YAZHU_WAIT				8		//押注等待时长
#define		ZUPAI_WAIT				5		//组牌等待时长
#define		LAYOUT_MOD				13		//13
#define		LAYOUT_NUM				16
#define		YAZHU_BTN_COUNT			3

#define		THROW_WAIT				25
#define		AUCTION_WAIT			25


#define		GAME_FLAGS			    GF_AUCTION_ONCE \
									| GF_LEVERAGE_ALLOWED

#define		MAX_SCORE_CARD		   12
#define     CARD_PACKS             1 //打两副牌
#define     TOP_CARD               0 //进张
#define     BOTTOM_CARD            37 //底牌0张
#define		MAX_AUCTION_ROUND	   3 //最多叫庄3轮
#define		MAX_AUTO_COUNT		   3 //断线自动出牌最多3次
#define		MAKEBOMB_CHANCE		   0

#define     CARD_UNITE_TYPE_ABT_THREE_1         0x00100000  
#define     CARD_UNITE_TYPE_FOUR_2				0x00200000  
#define     CARD_UNITE_TYPE_FOUR_2_COUPLE		0x00400000  
#define		CARD_UNITE_TYPE_2KING				0x00800000	

#define  COMPARE_UNITE_TYPE_BOMB_EX	            CARD_UNITE_TYPE_BOMB\
											  | CARD_UNITE_TYPE_SINGLE\
											  | CARD_UNITE_TYPE_COUPLE\
		  									  | CARD_UNITE_TYPE_THREE\
											  |	CARD_UNITE_TYPE_THREE_1 \
											  | CARD_UNITE_TYPE_THREE_COUPLE\
											  | CARD_UNITE_TYPE_ABT_SINGLE\
											  | CARD_UNITE_TYPE_ABT_COUPLE\
											  | CARD_UNITE_TYPE_ABT_THREE_COUPLE\
											  | CARD_UNITE_TYPE_ABT_THREE\
											  | CARD_UNITE_TYPE_ABT_THREE_1\
											  | CARD_UNITE_TYPE_FOUR_2\
											  | CARD_UNITE_TYPE_FOUR_2_COUPLE

#define	COMPARE_UNITE_TYPE_THREE_1			  CARD_UNITE_TYPE_THREE_1
#define COMPARE_UNITE_TYPE_ABT_THREE_1		  CARD_UNITE_TYPE_ABT_THREE_1
#define COMPARE_UNITE_TYPE_FOUR_2			  CARD_UNITE_TYPE_FOUR_2
#define COMPARE_UNITE_TYPE_FOUR_2_COUPLE	  CARD_UNITE_TYPE_FOUR_2_COUPLE
#define COMPARE_UNITE_TYPE_2KING                CARD_UNITE_TYPE_2KING\
											  | CARD_UNITE_TYPE_BOMB\
											  | CARD_UNITE_TYPE_SINGLE\
											  | CARD_UNITE_TYPE_COUPLE\
		  									  | CARD_UNITE_TYPE_THREE\
											  |	CARD_UNITE_TYPE_THREE_1 \
											  | CARD_UNITE_TYPE_THREE_COUPLE\
											  | CARD_UNITE_TYPE_ABT_SINGLE\
											  | CARD_UNITE_TYPE_ABT_COUPLE\
											  | CARD_UNITE_TYPE_ABT_THREE_COUPLE\
											  | CARD_UNITE_TYPE_ABT_THREE\
											  | CARD_UNITE_TYPE_ABT_THREE_1\
											  | CARD_UNITE_TYPE_FOUR_2\
											  | CARD_UNITE_TYPE_FOUR_2_COUPLE


//所有支持的牌型
#define     CARD_UNITE_TYPE_TOTAL_EX               CARD_UNITE_TYPE_SINGLE \
											  | CARD_UNITE_TYPE_COUPLE \
											  | CARD_UNITE_TYPE_THREE \
											  |	CARD_UNITE_TYPE_THREE_1 \
											  | CARD_UNITE_TYPE_THREE_COUPLE \
											  | CARD_UNITE_TYPE_ABT_SINGLE \
											  | CARD_UNITE_TYPE_ABT_COUPLE \
											  | CARD_UNITE_TYPE_ABT_THREE \
											  | CARD_UNITE_TYPE_ABT_THREE_1 \
											  | CARD_UNITE_TYPE_ABT_THREE_COUPLE\
											  | CARD_UNITE_TYPE_BOMB \
											  | CARD_UNITE_TYPE_FOUR_2 \
											  | CARD_UNITE_TYPE_2KING \
											  | CARD_UNITE_TYPE_FOUR_2_COUPLE


enum    CARD_TYPE{
		CARD_TYPE_POINT0 = 0,
		CARD_TYPE_POINT1,
		CARD_TYPE_POINT2,
		CARD_TYPE_POINT3,
		CARD_TYPE_POINT4,
		CARD_TYPE_POINT5,
		CARD_TYPE_POINT6,
		CARD_TYPE_POINT7,
		CARD_TYPE_POINT8,
		CARD_TYPE_POINT9,
		CARD_TYPE_SANGONG,
		CARD_TYPE_ZHADAN,
		CARD_TYPE_BAOJIU
};

enum REQ_BET_ERROR{
	BR_NULL_POINTER = -20,		//空指针
	BR_NOT_INGAME,				//你没在游戏
	BR_IS_BANKER,				//你不是闲家
	BR_FINISHED,				//你已经结束押注
	BR_ONE_OBJ_IS_BANKER,			//向庄家押注
	BR_ONE_OBJ_BETTED,				//已押注过
	BR_ONE_OBJ_MORE_THAN_ONE,		//单次押注(非全押)押多个人
	BR_ONE_OBJ_ZERO,				//单次押注没目标
	BR_ONE_INVALID_DBL,				//单次押注倍数无效		
	BR_ALL_INVALID_DBL,			//全押-倍数无效
	BR_ALL_OBJ_BETTED,			//全押-已押注过
	BR_SURE_INVALID_DBL,			//确定-倍数无效
};


#define		TS_WAITING_COUNTDOWN		0x00000004			//倒计时阶段

#define     TS_WAITING_QZ				0x00010000			//抢庄阶段
#define		TS_WAITING_YAZHU			0x00020000			//押注阶段
#define		TS_WAITING_ZUPAI			0x00040000			//组牌阶段
#define		TS_WAITING_COMPARE			0x00080000			//比牌阶段

#define		MAX_GAME_NUM				1024