#pragma once

#define		PRODUCT_LICENSE	 	 _T("")
#define		PRODUCT_NAME		 _T("TcSgSvr")
#define		PRODUCT_VERSION		 _T("2.00")
#define     STR_SERVICE_NAME     _T("TcSgSvr")
#define     STR_DISPLAY_NAME     _T("ͬ������������")
#define     STR_DISPLAY_NAME_ENU _T("TCY TcSgSvr Service") 

#define		GAME_ID				  687	//��Ҫ�޸�
#define		GAME_PORT			  21687 //��Ҫ�޸�


#define		TOTAL_CHAIRS			4		//�������
#define     TOTAL_CARDS				54		//������
#define     CHAIR_CARDS				3		//ÿ��ӵ���Ƶ������
#define     QZ_WAIT					5		//��ׯ�ȴ�ʱ��
#define     YAZHU_WAIT				8		//Ѻע�ȴ�ʱ��
#define		ZUPAI_WAIT				5		//���Ƶȴ�ʱ��
#define		LAYOUT_MOD				13		//13
#define		LAYOUT_NUM				16
#define		YAZHU_BTN_COUNT			3

#define		THROW_WAIT				25
#define		AUCTION_WAIT			25


#define		GAME_FLAGS			    GF_AUCTION_ONCE \
									| GF_LEVERAGE_ALLOWED

#define		MAX_SCORE_CARD		   12
#define     CARD_PACKS             1 //��������
#define     TOP_CARD               0 //����
#define     BOTTOM_CARD            37 //����0��
#define		MAX_AUCTION_ROUND	   3 //����ׯ3��
#define		MAX_AUTO_COUNT		   3 //�����Զ��������3��
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


//����֧�ֵ�����
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
	BR_NULL_POINTER = -20,		//��ָ��
	BR_NOT_INGAME,				//��û����Ϸ
	BR_IS_BANKER,				//�㲻���м�
	BR_FINISHED,				//���Ѿ�����Ѻע
	BR_ONE_OBJ_IS_BANKER,			//��ׯ��Ѻע
	BR_ONE_OBJ_BETTED,				//��Ѻע��
	BR_ONE_OBJ_MORE_THAN_ONE,		//����Ѻע(��ȫѺ)Ѻ�����
	BR_ONE_OBJ_ZERO,				//����ѺעûĿ��
	BR_ONE_INVALID_DBL,				//����Ѻע������Ч		
	BR_ALL_INVALID_DBL,			//ȫѺ-������Ч
	BR_ALL_OBJ_BETTED,			//ȫѺ-��Ѻע��
	BR_SURE_INVALID_DBL,			//ȷ��-������Ч
};


#define		TS_WAITING_COUNTDOWN		0x00000004			//����ʱ�׶�

#define     TS_WAITING_QZ				0x00010000			//��ׯ�׶�
#define		TS_WAITING_YAZHU			0x00020000			//Ѻע�׶�
#define		TS_WAITING_ZUPAI			0x00040000			//���ƽ׶�
#define		TS_WAITING_COMPARE			0x00080000			//���ƽ׶�

#define		MAX_GAME_NUM				1024