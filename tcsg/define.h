#pragma once

///////////////////////////����汾////////////////////////
#define APP_NAME       _T("tcsg")
#define VER_MAJOR       2
#define VER_MINOR       0
#define VER_BUILDNO     20160303

typedef struct _tagGAME_SET{
	int        nSoundEffect;		//�Ƿ����Ч
	int        nMusic;				//�Ƿ������
	int        nRejectKanPaiAsk;	//��������
	int		   nRejectPassSound;	//����������
	int		   nLikeReMaind;		//��ʾ��ť����
	int		   nUnLikeTip;			//ȥ����������������ʾ
}GAME_SET, *LPGAME_SET;

enum{
	TCSG_CALLBACKMSG_BEGIN = SK_CALLBACK_CUSTOMER_BEGIN + 1,
	TCSG_CALLBACKMSG_COUNTDOWNTIMER,
	TCSG_CALLBACKMSG_COUNTDOWNZERO,
	TCSG_CALLBACKMSG_BTN_CARDMASTER,
	TCSG_GAMEMSG_ACHDATATOUI,				//����ʱ��ʾ�ɾ�����
	TCSG_CALLBACKMSG_AUTOSTART,	
	TCSG_CALLBACKMSG_AUTOCONTINUE,	

	TCSG_CALLBACKMSG_ZUPAI,
	TCSG_CALLBACKMSG_GAMEWIN,
	TCSG_CALLBACKMSG_GAMESTANDOFF,

	TCSG_CALLBACKMSG_BTN_SORT,


	TCSG_CALLBACKMSG_BTN_SUREZUPAI,

	TCSG_CALLBACKMSG_SHOWRESULT,
	TCSG_CALLBACKMSG_MOVIE_HIDEOBJECT,
	TCSG_CALLBACKMSG_MOVIE_FLASHCARD,
	TCSG_CALLBACKMSG_MOVIE_SHOWCARDTYPE,
	TCSG_CALLBACKMSG_MOVIE_STOPOBJECT,
	TCSG_CALLBACKMSG_MOVIE_END_GAME,
	TCSG_CALLBACKMSG_MOVIE_END_MOVE,
	TCSG_CALLBACKMSG_AUTO_SET_DEPOSIT,
	TCSG_CALLBACKMSG_BTN_ACHIEVE_SYSTEM,
	TCSG_MSG_MOVE_ACHSYS,

	TCSG_SAFEBOX_SETDEPOSIT_BTN_SURE,
	TCSG_SAFEBOX_SETDEPOSIT_BTN_CLOSE,
	TCSG_SAFEBOX_SETDEPOSIT_CHECK_SET,
	TCSG_SAFEBOX_TAKEDEPOSIT_BTN_SET,

	TCSG_CALLBACKMSG_DEALCARD,
	TCSG_CALLBACKMSG_DEALCARDOVER,

	TCSG_CALLBACKMSG_NOTHING,
	TCSG_CALLBACKMSG_INDIACTOR,
	TCSG_CALLBACKMSG_FEIZHUANG,
	TCSG_CALLBACKMSG_YAZHUBEGIN,
	TCSG_CALLBACKMSG_FEIZHUANGANI,
	TCSG_CALLBACKMSG_SHOWINDICATORSCALE,
	TCSG_CALLBACKMSG_YAZHUBTN,
	TCSG_CALLBACKMSG_CARDTYPEBTN,

	TCSG_CALLBACK_BUTTON_CHARGE,

	TCSG_CALLBACKMSG_BTN_GET_WELFARE,
	TCSG_CALLBACKMSG_BTN_GET_WElFAREAWARED,
	TCSG_CALLBACKMSG_BTN_CLOSE_WELFARE,
    TCSG_CALLBACKMSG_BTN_SHOW_WELFARE,

	TCSG_CALLBACKMSG_ClOSECHARGE,
	TCSG_CALLBACKMSG_WEBCHARGE,
	TCSG_CALLBACKMSG_QRCODECHARGE,
	TCSG_CALLBACKMSG_REFFESHCHARGE,
    TCSG_CALLBACKMSG_END
};

#define  ZP_BTN_TO_CARD_X 30

#define  FOR_CIRCLE(num, sum) for(int num=0;num<sum;num++)
#define  TG_SHOW_OBJECT(pObject)      if(pObject){pObject->Show();pObject->Updata();}
#define  TG_HIDE_OBJECT(pObject)      if(pObject){pObject->Hide();pObject->Updata();}
#define  TG_MOVETO_OBJECT(pObject,x,y)  if(pObject){pObject->MoveTo(x,y);pObject->Updata();}
#define  TG_MOVE_OBJECT(pObject,x,y)  if(pObject){pObject->Move(x,y);pObject->Updata();}
