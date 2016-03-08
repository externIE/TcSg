#include "StdAfx.h"
#include "game.h"
#include "MyGame.h"
#include "MyFileSystem.h"
//#include <MATH.H>
/*using namespace ShinyNova;*/
CGameTable g_GameTable;

TCHAR* g_szShape[6]={_T("方块"),_T("梅花"),_T("红桃"),_T("黑桃"),_T("小王"),_T("大王")};
TCHAR* g_szValue[14]={_T("2"),_T("3"),_T("4"),_T("5"),_T("6"),_T("7"),_T("8"),_T("9"),_T("10"),_T("J"),_T("Q"),_T("K"),_T("A"),_T("")};

#define DEFAULT_LUCKYBAGCLOSENUM 10

CMyGame::CMyGame()
{
	m_nTotalPlayer = TOTAL_CHAIRS;
	m_btnCardMaster = NULL;
						
	//设置框
	m_GameSetBoard	=	NULL;
				
	//记牌器
	m_CardMaster	=	NULL;
	memset(m_nBottomCard,0,sizeof(m_nBottomCard));
	memset(m_bitActor,0,sizeof(m_bitActor));
	m_BombFanScore	=	NULL;
	m_AuctionScore	=	NULL;
	m_btnSort		=	NULL;
	m_digitBaseBig	=	NULL;
	m_AuctionScoreBig	= NULL;
	m_BombFanScoreBig	= NULL;
	m_btnQZ = NULL;
	m_btnNOQZ = NULL;

	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		m_bitPaiXing[i] = NULL;
		m_bitWaitNextBout[i] = NULL;
		m_bitQZFlag[i] = NULL;
	}

	for ( i = 0; i < TOTAL_CHAIRS; i++)
	{
		m_digitYZYinzi[i]=NULL;
		m_objYinzi[i]=NULL;
	}	

	m_btnSureZuPai = NULL;
	m_bitWaitBet = NULL;
	m_btnHuanZhuo = NULL;

	m_bitCountDown = NULL;
	m_digitCntDwn = NULL;
	m_hEventCountDownTimer = NULL;
	m_nZeroCountDown = 0;

	m_dlgAutoSetDeposit = NULL;
	m_pMyKeyResult = NULL;
	m_btnAutoSetDeposit=NULL;
	
	m_bitClockBase=NULL;
	m_nNewTableCountDown=0;

	//二维码充值start
    m_btnCharge = NULL;
    //二维码充值end

	m_bitGetWelfare	= NULL;
	m_eventId = -1;
	m_nActivityID = -1;
	m_welfDialog = NULL;
	m_welfLight = NULL;
	m_plQZRemaind = NULL;
	m_welfareSurplusNum = -1;
	m_plWelfRush = NULL;
	m_obWelfFrame = NULL;
	m_digtWelfRush = NULL;
	m_dwSoundId = 0;
	m_bTongBi = FALSE;
	m_btnCareType = NULL;
	m_bitCardType = NULL;
	m_bitBaseDeposit = NULL;
	for(i=0;i<TOTAL_CHAIRS-1;i++)
	{
		m_digitBaseDeposit[i]=NULL;
	}
}

CMyGame::~CMyGame()
{
	
}

void CMyGame::FitPaiXing(int nIndex/*=-1*/)
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	SNG_PIC pic=Res_GetPICStruct("牌型");
	int picH=pic.height;

	for (int idx=0;idx<TOTAL_CHAIRS;idx++)
	{
		if (-1 != nIndex && idx != nIndex)
			continue;
		
		POINT pt={0,0};
		GetThrowCardPosCE(idx,pt);
		int nCardHeight=GetThrowCardHeight(idx);

		if (m_bitPaiXing[idx])
		{
			if (idx!=MYDRAWINDEX)
				pt.y=pt.y+nCardHeight/2+picH/2;				
			else
				pt.y=pt.y-nCardHeight/2-picH/2;

			if (0 == idx || 2 == idx)
			{
				pt.x=x+w/2;//始终居中
			}

			TG_MOVETO_OBJECT(m_bitPaiXing[idx], pt.x,pt.y);
		}
	}
}

void CMyGame::FitStartBtn()
{
	if (m_btnStart)
	{
		RECT rect=GetGameRect();
		int x=rect.left;
		int y=rect.top;
		int w=rect.right-rect.left;
		int h=rect.bottom-rect.top;
		
		SNG_PIC picBtn=Res_GetPICStruct(_T("开始"));
		
		POINT pt={0,0};
		POINT ptFit={0,0};
		CMyGameFrame* frame=(CMyGameFrame*)(theApp.GetGameFrame());	
		BOOL bTightUI = frame->IsTightUI();
		if (bTightUI)
		{
			ptFit.x=292;
			ptFit.y=22;
		}
		else
		{
			ptFit.x=310;
			ptFit.y=38;
		}

		pt.x=m_ptrPlayer[0]->GetLeft()+ptFit.x+picBtn.width/2;
		pt.y=m_ptrPlayer[0]->GetBottom()-ptFit.y-picBtn.height/2;

		m_btnStart->MoveTo(pt.x,pt.y);
		m_btnStart->Updata();
	}
	else
	{
		return;
	}
}

void CMyGame::OnGameSize()
{
	OPE_FreshUI();
    RECT rect=GetGameRect();
	int x=rect.left;
	int y=rect.top;
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;

	RUL_CalcGameStatus(w,h);

	if (m_bitBkgPattern)
	{
		m_bitBkgPattern->MoveTo(x+w/2,y+h/2);
		m_bitBkgPattern->Updata();
	}
	if (m_GameSetBoard)
	{
		m_GameSetBoard->MoveTo(x+w/2,y+h/2);
		m_GameSetBoard->Updata();
	}

	{
		for(int i=0;i<m_nTotalPlayer;i++)
		{
			m_ptrPlayer[RUL_GetDrawIndexByChairNO(i)]->FitScrren();
		}
	}
	
	{//重新
		for(int i=0;i<TOTAL_CARDS;i++)
		{
			m_Card[i]->CalcSize();
			m_Card[i]->Fresh();
		}

		for(i=0;i<BOTTOM_CARD;i++)
		{
			m_nBottomCard[i]->CalcSize();
			m_nBottomCard[i]->Fresh();
		}
	}
	FitPaiXing(-1);

	FitStartBtn();

//	int nCardHeight=m_Card[0]->GetCardHeight();
//	Sys_SetIntStatus(GAME_STATUS_CARD_HEIGHT,nCardHeight);
	int nCardHeight=Sys_GetIntStatus(GAME_STATUS_CARD_HEIGHT);
	int btn_y=y+h-nCardHeight-30-Sys_GetIntStatus(GAME_STATUS_PLAYER_HEIGHT)-m_btnPass->GetHeight()/2;

	if (m_btnPass)
	{
		FitButtonPosition();
		
		m_btnAutoPlay->MoveTo(x+w-30,y+h-15);
		m_btnAutoPlay->Updata();
		m_btnSort->MoveTo(x+w-84,y+h-15);
		m_btnSort->Updata();
	}

	FitCountDownPostion();
	FitClockPos();

	int xOff=15;
	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		int nDrawIndex = RUL_GetDrawIndexByChairNO(i);
		POINT ptP={0,0};
		GetPlayerPanePos(nDrawIndex,ptP);
		if (nDrawIndex==MYDRAWINDEX)
		{
			TG_MOVETO_OBJECT(m_bitWaitNextBout[nDrawIndex], w/2+xOff, ptP.y);
		}
		else if(nDrawIndex==1||nDrawIndex==2)
		{
			TG_MOVETO_OBJECT(m_bitWaitNextBout[nDrawIndex], ptP.x+160, ptP.y);
		}
		else
		{
			TG_MOVETO_OBJECT(m_bitWaitNextBout[nDrawIndex], ptP.x-160,ptP.y);
		}
	}

	POINT pt;
	int nDrawIndex=RUL_GetDrawIndexByChairNO(GetGameInfo()->GetCurrentChairNO());
	if (IS_BIT_SET(GetGameInfo()->GetStatus(),TS_WAITING_AUCTION))
	{
		pt=m_ptrPlayer[nDrawIndex]->GetClockPosition(CLOCK_POS_AUCTION);
	}
	else
	{
		pt=m_ptrPlayer[nDrawIndex]->GetClockPosition(GetPublicInfo()->nWaitChair==-1);
	}
	if (m_btnCardMaster)
	{
		m_btnCardMaster->MoveTo(x+w-177,y+h-17);
		m_btnCardMaster->Updata();
	}
	if (m_btnAskKanPai)
	{
		m_btnAskKanPai->MoveTo(x+41,y+h-18);
		m_btnAskKanPai->Updata();
	}
	if (m_bitReMindPass)
	{
		m_bitReMindPass->MoveTo(x+w/2,m_btnThrow->GetY()+30);
		m_bitReMindPass->Updata();
	}
	if (m_plGameInfo)
	{
		if (theApp.IsUseBigUI())
			m_plGameInfo->MoveTo(x+m_plGameInfo->GetWidth()/2+10,y+m_plGameInfo->GetHeight()/2+10);
		else
			m_plGameInfo->MoveTo(x+m_plGameInfo->GetWidth()/2,y+m_plGameInfo->GetHeight()/2);
		m_plGameInfo->Updata();
	}
	if (m_plResultDlg)
	{
		m_plResultDlg->MoveTo(x+w/2,y+h/2);
		m_plResultDlg->Updata();
	}
	if (m_CardMaster)
	{
		m_CardMaster->OnSize();
	}
	if (m_ShowWaitArrage)
	{
		BITObject* pbo = (BITObject*)m_ShowWaitArrage;
		pbo->MoveTo(x+w/2,y+h/2);
		pbo->Updata();
	}

	int nY = m_ptrPlayer[MYDRAWINDEX]->GetTop() - 40;
	
	if (m_btnQZ)
	{
		m_btnQZ->MoveTo(x+w/2+100, nY);
		m_btnQZ->Updata();
	}	
	if(m_btnNOQZ)
	{
		m_btnNOQZ->MoveTo(x+w/2-100, nY);
		m_btnNOQZ->Updata();
	}

	FitZupaiBtnPos();

	if (m_btnAutoSetDeposit)
	{
		m_btnAutoSetDeposit->MoveTo(x+w-55, rect.bottom-25);
		m_btnAutoSetDeposit->Updata();
	}
	if (m_btnHuanZhuo)
	{
		m_btnHuanZhuo->MoveTo(m_btnAutoSetDeposit->GetX(), m_btnAutoSetDeposit->GetY()-50);
		m_btnHuanZhuo->Updata();		
	}

	FitWaitBetPostion();

	if(m_plYaZhuFrame)
	{
		m_plYaZhuFrame->MoveTo(x+w/2, nY+2);
        m_plYaZhuFrame->Updata();
	}
	
	FixIndicatorPosition();

	//二维码充值start
    if (m_btnCharge)
    {
		if (GetGameWinApp()->IsLookOn())
		{
			m_btnCharge->Hide();
		}
		else
		{
			m_btnCharge->Show();
		}
        m_btnCharge->MoveTo(x+w-50, 70);
        m_btnCharge->Updata();
    }
    //二维码充值end

	if(m_bitGetWelfare != NULL)
	{
		m_bitGetWelfare->MoveTo(x+w-150, 70);
		m_bitGetWelfare->Updata();
	}

	FitQZFlag();
	OPE_FitQZReamind();

	if (m_welfLight)
	{	
		TG_MOVETO_OBJECT(m_welfLight,x+w-150,70);
		m_welfLight->Updata();
	}

	OPE_FixYZYinzi();
	FitYZYinZiDigitPos();

	GAME_WIN_RESULT *pGameWin= ((CGameInfoEx*)GetGameInfo())->GetGameWin();
	for(i=0;i<TOTAL_CHAIRS;i++)
	{
		POINT pt={0,0};
		GetRushDigitPos(i, GetIntNum(abs(pGameWin->nGainsWin[i])), pt);
		POINT ptDst={pt.x,pt.y-49};
		int nDrawIndex = RUL_GetDrawIndexByChairNO(i);
		if(nDrawIndex!=MYDRAWINDEX)
			ptDst.y=pt.y+49;
		m_objRushFrame[i]->MoveTo(ptDst.x, ptDst.y);
		m_objRushFrame[i]->Updata();
	}

	if(m_btnCareType)
	{
		m_btnCareType->MoveTo(x+45,y+40);
		m_btnCareType->Updata();
	}

	if(m_bitCardType)
	{
		m_bitCardType->MoveTo(x+55,y+180);
		m_btnCareType->Updata();
	}

	if(m_bitBaseDeposit)
	{
		m_bitBaseDeposit->MoveTo(x+80,y+h-61);
		m_bitBaseDeposit->Updata();
	}

	for(i=0;i<TOTAL_CHAIRS-1;i++)
	{
		if(i==0)
			m_digitBaseDeposit[i]->MoveTo(x+110,y+h-78);
		else if(i==1)
			m_digitBaseDeposit[i]->MoveTo(x+110,y+h-47);
		else if(i==2)
			m_digitBaseDeposit[i]->MoveTo(x+110,y+h-16);
		m_digitBaseDeposit[i]->Updata();
	}
	
	SCR_InvalidateScreen();
}

void CMyGame::CreatePaiXing()
{
	for (int i=0; i<TOTAL_CHAIRS; i++)
	{	
		m_bitPaiXing[i]=CreateBitMap(Res_GetPICStruct("牌型"));
		m_bitPaiXing[i]->SetFloor(FLOOR_RESULT+500);
	}	
}

void CMyGame::OnIniData()
{
	CreateClock();
	CreateInterface();
	CreateMouseObject();
	CreatePlayers(TOTAL_CHAIRS); 
	CreateCard();
	CreateBtn();

	CreatePaiXing();	

	if (theApp.IsAllowCardMaster())
	{
		CreateCardMaster();//记牌器
	}
	
	CreateReplay();

	if (!GetGameInfo()->IsGamePlaying())
	{
		OPE_InitData();
	}

	m_hCursorHand=LoadCursor(NULL,IDC_HAND);
 	m_hCursorArrow=LoadCursor(NULL,IDC_ARROW);

	m_nSortType=SORT_FLAGS_COMMON;
	m_nOldSortType=m_nSortType;
	OnFreshGameSet();

	PGL_MSG_CLASS(m_dlgAutoSetDeposit,CAutoSetDepositDlg);
	m_dlgAutoSetDeposit->CreateSetDepositDlg();
	m_dlgAutoSetDeposit->SetFloor(FLOOR_SYSTEMWARNING-5);
	m_dlgAutoSetDeposit->SetMoveAble(TRUE);

	PGL_MSG_CLASS(m_CEventPipe,CShinyNovaPipeline);

	CreateIndicatorAni();
	CreateQZFlag();
}

void  CMyGame::GameInstance()
{
	m_ptrPlayer[MYDRAWINDEX]->LoadPlayer(GetGameWinApp()->GetChairNO());
	
	xyFmtDbgStr("id=%d", theApp.GetUserID());
	
#ifdef _AUTOPLAY_
	APPLY_WaitFewTimes(1000,&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_AUTOSTART));
#endif
}

void  CMyGame::CreatePlayers(int nTotalChairs)
{
	m_nTotalPlayer=nTotalChairs;
	int nDrawIndex = 0;
	for(int i=0;i<nTotalChairs;i++)
	{
		nDrawIndex = RUL_GetDrawIndexByChairNO(i);
		PGL_MSG_CLASS(m_ptrPlayer[nDrawIndex],CMyPlayer);
		m_ptrPlayer[nDrawIndex]->CreateGUI(nDrawIndex);
	}
}

void  CMyGame::CreateInterface()
{
	m_bitBkgPattern=CreateBitMap(Res_GetPICStruct("bit_BkgPattern"));
	m_bitBkgPattern->SetFloor(MAX_LOW_Z);
	m_bitBkgPattern->Show();
	m_bitBkgPattern->Updata();

	{
		CXyDigit* pen;
		PGL_MSG_CLASS(pen,CXyDigit);
		pen->CreateDigit("digit_Pen",DIGIT_FLAGS_MID_JUSITIFY);
		pen->LoadWord("+",Res_GetPICStruct("digit_Pen",10));
		pen->LoadWord("-",Res_GetPICStruct("digit_Pen",11));
		pen->SetFloor(FLOOR_CLOCK);
		pen->Updata();
		pen->ReName("DigitPen");
	}
	
	{
		CXyDigit* pen2;
		PGL_MSG_CLASS(pen2,CXyDigit);
		pen2->CreateDigit("digit_Pen2",DIGIT_FLAGS_MID_JUSITIFY);
		pen2->SetFloor(FLOOR_CLOCK);
		pen2->Updata();
		pen2->ReName("DigitPen2");
	}
	
	//过牌提示
	m_bitReMindPass=CreateBitMap(Res_GetPICStruct("过牌提示"));

	//设置框
	{
		PGL_MSG_CLASS(m_GameSetBoard,CGameSetBoard);
	}
	
	m_bitWaitBet = CreateBitMap(Res_GetPICStruct(_T("等待闲家下注")));
	m_bitWaitBet->TransGUI();
	m_bitWaitBet->Hide();
	m_bitWaitBet->SetFloor(FLOOR_BTN+1);

	CreateCountDown();

	CreateYinzi();

	for (int i=0; i<TOTAL_CHAIRS; i++)
	{
		int idx=RUL_GetDrawIndexByChairNO(i);
		m_bitWaitNextBout[idx] = CreateBitMap(Res_GetPICStruct(_T("等待下一局")));
		m_bitWaitNextBout[idx]->SetFloor(FLOOR_RESULT);
		PGL_HIDE_OBJECT(m_bitWaitNextBout[idx]);
	}

	//结算时赢得的银两显示
	for (i=0;i<TOTAL_CHAIRS;i++)
	{
		m_objRushFrame[i]=CreateVirtual();
		m_objRushFrame[i]->ClearChild();
		m_objRushFrame[i]->MoveTo(-100,-100);
		m_objRushFrame[i]->SetFloor(FLOOR_RESULT+100);
		
		for (int j=0;j<2;j++)
		{
			PGL_MSG_CLASS(m_digtRush[i][j],CXyDigit);
			if (j==0)
			{
				m_digtRush[i][j]->CreateDigit("rush_digit_plus",DIGIT_FLAGS_LEFT_JUSITIFY|DIGIT_FLAGS_USE_PIC_SIZE);
			}
			else if (j==1)
			{
				m_digtRush[i][j]->CreateDigit("rush_digit_minus",DIGIT_FLAGS_LEFT_JUSITIFY|DIGIT_FLAGS_USE_PIC_SIZE);
			}
			m_digtRush[i][j]->SetFloor(FLOOR_RESULT+1000); 
			
			m_digtRush[i][j]->MoveTo(m_objRushFrame[i]->GetX()+5, m_objRushFrame[i]->GetY());
			m_objRushFrame[i]->AddChild(m_digtRush[i][j],OBJECT_LOCK_POSITION|OBJECT_LOCK_UPDATA|OBJECT_LOCK_ALPHA);
		}
		m_plRush[i]=CreatePlane(14,20);
		m_plRush[i]->SetFloor(FLOOR_RESULT+1000);
		m_plRush[i]->MoveTo(m_objRushFrame[i]->GetX()-5, m_objRushFrame[i]->GetY());
		m_objRushFrame[i]->AddChild(m_plRush[i],OBJECT_LOCK_POSITION|OBJECT_LOCK_UPDATA|OBJECT_LOCK_ALPHA|OBJECT_LOCK_SHOW);
	}
    PGL_MSG_CLASS(m_welfDialog, CWelfareDialog);

	CreateQZRemaind();
    CreateYaZhuDlg();

	m_bitCardType = CreateBitMap(Res_GetPICStruct(_T("牌型提示")));
	m_bitCardType->TransGUI();
	m_bitCardType->Hide();
	m_bitCardType->SetFloor(FLOOR_BTN+1);

	m_bitBaseDeposit = CreateBitMap(Res_GetPICStruct(_T("通比基础银框")));

	for(i=0;i<TOTAL_CHAIRS-1;i++)
	{
		PGL_MSG_CLASS(m_digitBaseDeposit[i],CXyDigit);
		m_digitBaseDeposit[i]->CreateDigit("基础银框数字",DIGIT_FLAGS_MID_JUSITIFY|DIGIT_FLAGS_USE_PIC_SIZE);
	}
}

void  CMyGame::CreateCardMaster()
{
	//记牌器
//	m_btnCardMaster=FastCreateStandButton("btn_Cardmaster",CreateMsg(GetHandle(),TCSG_CALLBACKMSG_BTN_CARDMASTER),"记牌器");
//	PGL_MSG_CLASS(m_CardMaster,CXyCardMaster);	
}

void  CMyGame::CreateCard()
{
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		PGL_MSG_CLASS(m_Card[i],CMyCard);
		m_Card[i]->Hide();
		m_Card[i]->Updata();
	}

	for(i=0;i<BOTTOM_CARD;i++)
	{
		PGL_MSG_CLASS(m_nBottomCard[i],CMyCard);
		m_nBottomCard[i]->Hide();
		m_nBottomCard[i]->Updata();
	}
	
}

void  CMyGame::CreateBtn()
{
	m_btnStart=FastCreateStandButton("开始",CreateMsg(GetHandle(),GAME_MSG_START_GAME),NULL,TRUE);
	m_btnSort=FastCreateStandButton("排序24_49",CreateMsg(GetHandle(),TCSG_CALLBACKMSG_BTN_SORT),NULL,TRUE);
	
	m_btnAutoPlay=FastCreateStandButton("btn_AutoPlay",CreateMsg(GetHandle(),BTN_CALLBACKMSG_AUTOPLAY),NULL,TRUE);
	m_btnAskKanPai=FastCreateStandButton("btn_AskCardView",CreateMsg(GetHandle(),BTN_CALLBACKMSG_ASKKANPAI));
	PGL_HIDE_OBJECT(m_btnAskKanPai);
	/*
	if (theApp.IsLookOn())
	{
		PGL_SHOW_OBJECT(m_btnAskKanPai);
	}
	else
	{
		PGL_HIDE_OBJECT(m_btnAskKanPai);
	}*/


	int w=GetScreenWidth();

	if (w>1600)
	{
		m_btnThrow=FastCreateStandButton("放大版_按钮_出牌",CreateMsg(GetHandle(),BTN_CALLBACKMSG_THROW),NULL,TRUE);
		m_btnPass=FastCreateStandButton("放大版_按钮__不出",CreateMsg(GetHandle(),BTN_CALLBACKMSG_PASS),NULL,TRUE);
		m_btnReMaind=FastCreateStandButton("放大版_按钮__提示",CreateMsg(GetHandle(),BTN_CALLBACKMSG_REMAIN),NULL,TRUE);
	}
	else
	{
		m_btnThrow=FastCreateStandButton("btnThrow",CreateMsg(GetHandle(),BTN_CALLBACKMSG_THROW),NULL,TRUE);
		m_btnPass=FastCreateStandButton("btnPass",CreateMsg(GetHandle(),BTN_CALLBACKMSG_PASS),NULL,TRUE);
		m_btnReMaind=FastCreateStandButton("btnRemaind",CreateMsg(GetHandle(),BTN_CALLBACKMSG_REMAIN),NULL,TRUE);
	}


	m_btnQZ=FastCreateStandButton("抢庄",CreateMsg(GetHandle(),BTN_CALLBACKSMG_AUCTION_1),NULL,TRUE);
	m_btnNOQZ=FastCreateStandButton("不抢",CreateMsg(GetHandle(),BTN_CALLBACKSMG_AUCTION_NO),NULL,TRUE);
	m_btnSureZuPai=FastCreateStandButton("摊牌",CreateMsg(GetHandle(),TCSG_CALLBACKMSG_BTN_SUREZUPAI),NULL,TRUE);

	m_btnHuanZhuo=FastCreateStandButton("换桌",CreateMsg(GetHandle(),GAME_MSG_ASK_NEW_TABLE),NULL,TRUE);
	if (!theApp.IsLookOn())
	{
		m_btnHuanZhuo->Show();
		m_btnHuanZhuo->Updata();
	}

	m_btnAutoSetDeposit=FastCreateStandButton("自动补银",CreateMsg(GetHandle(),TCSG_CALLBACKMSG_AUTO_SET_DEPOSIT),NULL,TRUE);
	if (!theApp.IsLookOn())
	{
		m_btnAutoSetDeposit->Show();
		m_btnAutoSetDeposit->Updata();
	}

	//二维码充值start
    if (m_btnCharge == NULL)
    {
        m_btnCharge = FastCreateStandButton("btn_charge", CreateMsg(GetHandle(), TCSG_CALLBACK_BUTTON_CHARGE), NULL, TRUE);
    }
    //二维码充值end

	if(m_bitGetWelfare == NULL)
	{
		m_bitGetWelfare = FastCreateStandButton("btn_welfare", CreateMsg(GetHandle(), TCSG_CALLBACKMSG_BTN_GET_WELFARE), NULL, TRUE, 0, 0);
	}
	
	if(m_welfLight == NULL)
	{
		m_welfLight = CreateBitMap(Res_GetPICStruct(_T("低保跑光")));
		m_welfLight->TransGUI();
		m_welfLight->Hide();
	    m_welfLight->SetFloor(FLOOR_BTN+10);
	}

	m_btnCareType=FastCreateStandButton("牌型介绍按钮",CreateMsg(GetHandle(),TCSG_CALLBACKMSG_CARDTYPEBTN));
	if(m_btnCareType)
	{
		m_btnCareType->Show();
		m_btnCareType->Updata();
	}
}

void CMyGame::CreateCountDown()
{
	PGL_MSG_CLASS(m_digitCntDwn,CXyDigit);
	m_digitCntDwn->CreateDigit("digiti_Clock",DIGIT_FLAGS_MID_JUSITIFY);
	m_digitCntDwn->SetFloor(FLOOR_CLOCK);

	m_digitCntDwn->SetWordSpace(-10);
	m_digitCntDwn->Hide();
	m_digitCntDwn->TransGUI();
	
	m_bitCountDown = CreateBitMap(Res_GetPICStruct(_T("clock_base")));
	m_bitCountDown->SetFloor(m_digitCntDwn->GetZ()-1);
	m_bitCountDown->Hide();
	m_bitCountDown->TransGUI();	
}

void CMyGame::CreateClock()
{
	PGL_MSG_CLASS(m_Clock,CXyDigit);
	int w=GetScreenWidth();
	m_Clock->CreateDigit("digiti_Clock",DIGIT_FLAGS_MID_JUSITIFY);
	m_Clock->SetFloor(FLOOR_CLOCK);
	m_Clock->SetWordSpace(-10);
	m_Clock->Updata();
	
	m_bitClockBase = CreateBitMap(Res_GetPICStruct(_T("clock_base")));
	m_bitClockBase->SetFloor(m_Clock->GetZ()-1);
	m_bitClockBase->Updata();
}

void  CMyGame::CreateMatchTable()
{
	CSKMainGame::CreateMatchTable();

	if (m_CardMaster)
	{
		m_CardMaster->ReLoad();
	}
}

BOOL CMyGame::OnMsg(SNG_MSG& msg)
{
	switch (msg.msgID)
	{
	case GAME_MSG_START_GAME:
		{
			OPE_OnStart();
		}
		break;
	case CARD_CALLBACKMSG_UNSELECT:
		{
			CSKMainGame::OnMsg(msg);
		}
		break;
	case TCSG_CALLBACKMSG_ZUPAI:
		{		
			ZUPAI* pZuPai = (ZUPAI*)msg.param1;
			NTF_ZuPai(pZuPai);
		}
		break;
	case TCSG_CALLBACKMSG_GAMEWIN:
		NTF_GameWin();
		break;
	case TCSG_CALLBACKMSG_GAMESTANDOFF:
		NTF_GameStandOff();
		break;
	case TCSG_CALLBACKMSG_COUNTDOWNTIMER:
		{
			if (m_digitCntDwn->IsShow())
			{
				if (m_digitCntDwn->GetDigit()>0)
				{ 
					m_digitCntDwn->StepDigit();
				}
				else
				{
					m_nZeroCountDown++;
				}
				if (!theApp.IsGameRunning() && !theApp.IsLookOn() && TS_WAITING_COUNTDOWN == GetGameInfoEx()->GetGameStartInfo()->dwStatus)
				{//倒计时玩家
					if ((m_digitCntDwn->GetDigit()%5==0 &&!m_nZeroCountDown))
					{
						if (m_digitCntDwn->GetDigit()==0)
						{//倒计时时钟到0
							APPLY_WaitFewTimes(100*theApp.GetChairNO()+10,&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_COUNTDOWNZERO));
						}
						else
						{
							CGameRequest* pGameConnect=(CGameRequest* )GetGameWinApp()->GetGameConnect();
							pGameConnect->GC_SyncStartCountdown(m_digitCntDwn->GetDigit());
						}
					}
				}
			}
		}
		break;
	case TCSG_CALLBACKMSG_COUNTDOWNZERO:
		{
			if (TS_WAITING_COUNTDOWN == GetGameInfoEx()->GetGameStartInfo()->dwStatus)
			{
				CGameRequest* pGameConnect=(CGameRequest* )GetGameWinApp()->GetGameConnect();
				pGameConnect->GC_SyncStartCountdown(m_digitCntDwn->GetDigit());
			}
		}
		break;
	case TCSG_CALLBACKMSG_MOVIE_FLASHCARD:
		{
			if (1)
			{
				CGameCard* card = (CGameCard*)msg.param1;
				if (card)
				{
					card->Fresh();
				}
			}
		}
		break;
	case TCSG_CALLBACKMSG_MOVIE_SHOWCARDTYPE:
		{
			int chairno = (int)msg.param1;
			int cardtype = (int)msg.param2;
			if (ValidateChair(chairno))
				OPE_ShowSanGongType(chairno, cardtype, TRUE);
		}
		break;
	case TCSG_CALLBACKMSG_AUTOSTART:
		{
#ifdef _AUTOPLAY_
			OnStart();
#endif
		}
		break;
	case TCSG_CALLBACKMSG_SHOWRESULT:
		{
			HideWaitNextBoutAll();
			
			OPE_ShowResult();
			
#ifdef _AUTOPLAY_
			APPLY_WaitFewTimes(1000,&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_AUTOCONTINUE));
#endif
		}
		break;
	case TCSG_CALLBACKMSG_AUTOCONTINUE:
		{
#ifdef _AUTOPLAY_
			OPE_ResultContinue();
#endif
		}
		break;
	case TCSG_CALLBACKMSG_MOVIE_END_GAME:
		{
			OPE_WhileEndGame();
		}
		break;
	case GAME_MSG_SHOW_GAMESET:
		{
			if (m_GameSetBoard)
			{
				if (!m_GameSetBoard->IsShow())
				{
					m_GameSetBoard->SetFloor(FLOOR_GAMSET);
					m_GameSetBoard->Show();
					m_GameSetBoard->Updata();
				}
				else
				{
					m_GameSetBoard->Hide();
					m_GameSetBoard->Updata();
				}
			}
		}
		break;

	case TCSG_CALLBACKMSG_BTN_CARDMASTER:
		{
			OnCardMaster();
		}
		break;
	
	case TCSG_CALLBACKMSG_BTN_SUREZUPAI:
		{
			OnSureZuPai();
		}
		break;
	case TCSG_SAFEBOX_TAKEDEPOSIT_BTN_SET:
		{
			int nKeyResult = (int)msg.param1;
			int nAutoFixDeposit=GetAutoSetDeposit();
			TakeDepositToGame(nAutoFixDeposit,nKeyResult);
		}
		break;
	case TCSG_CALLBACKMSG_AUTO_SET_DEPOSIT:
		{
			m_dlgAutoSetDeposit->MoveTo(GetScreenWidth()/2,GetScreenHeight()/2);		
			m_dlgAutoSetDeposit->Show();
			m_dlgAutoSetDeposit->Updata();
		}
		break;
	case TCSG_CALLBACKMSG_MOVIE_HIDEOBJECT:
		{
			PObject* pObj=(PObject*)msg.param1;
			pObj->Hide();
			pObj->Updata();
		}
		break;
	case TCSG_CALLBACKMSG_MOVIE_STOPOBJECT:
		{
			PObject* pObj=(PObject*)msg.param1;
			StopObject(pObj->GetHandle());
			
			if (m_bitClockBase == pObj)
			{
				HideClock();
			}
		}
		break;
	case BTN_CALLBACKSMG_AUCTION_1:
		{
			OnQiangZhuang(TRUE);
		}
		break;
	case BTN_CALLBACKSMG_AUCTION_NO:
		{
			OnQiangZhuang(FALSE);
		}
		break;
	case TCSG_CALLBACKMSG_BTN_SORT:
		{
			OnSortHandCard();
		}
		break;

	case GAME_CALLBACKMSG_HITSCREEN:
		{
			break;
			theApp.ShowMsgInChatView("hit\r\n");

			if (m_bnAutoPlay)
			{
				MOUSEDATA* data=(MOUSEDATA*)msg.param2;
				RECT RC=GetMainGameRect();
				if (PtInRect(&RC,data->MS_CurrectPoint))
				{
					OnAutoPlay();
				}
			}
		}
		break;
	case CARD_CALLBACKMSG_UNTOUCH:
		{
			//break;
			CGameCard* card=(CGameCard*)msg.param1;
			if (card->m_nDrawIndex!=MYDRAWINDEX || theApp.IsLookOn())
				return FALSE;
			if (card)
			{
				if (card->m_nStatus==CARD_STATUS_INHAND
				||(card->m_nStatus==CARD_STATUS_THROWDOWN && IS_BIT_SET(GetGameInfoEx()->GetGameStartInfo()->dwStatus,TS_WAITING_ZUPAI)))
				{
					if (IS_BIT_SET(GetGameInfo()->GetStatus(),TS_WAITING_ZUPAI) && GetPublicInfoEx()->bFinishZuPai[theApp.GetChairNO()])
						return FALSE;
					SetSystemCursor(m_hCursorArrow);
					card->bnTouch=FALSE;
					card->Fresh();

					if (card->m_nStatus==CARD_STATUS_THROWDOWN 
						&& IS_BIT_SET(GetGameInfoEx()->GetGameStartInfo()->dwStatus,TS_WAITING_ZUPAI))
					{
						for (int i=0;i<TOTAL_CARDS;i++)
						{
							if (m_Card[i]->m_nDrawIndex == card->m_nDrawIndex)
							{
								m_Card[i]->Fresh();
							}
						}
					}
				}
			}
		}
		break;
	case CARD_CALLBACKMSG_TOUCH:
		{
			CGameCard* card=(CGameCard*)msg.param1;
			if (card->m_nDrawIndex!=MYDRAWINDEX || theApp.IsLookOn())
				return FALSE;
			if (card->m_nStatus==CARD_STATUS_INHAND
				||(card->m_nStatus==CARD_STATUS_THROWDOWN && IS_BIT_SET(GetGameInfoEx()->GetGameStartInfo()->dwStatus,TS_WAITING_ZUPAI)))
			{//手牌或者组牌过程中的打出的牌
				if (IS_BIT_SET(GetGameInfo()->GetStatus(),TS_WAITING_ZUPAI) && GetPublicInfoEx()->bFinishZuPai[theApp.GetChairNO()])
					return FALSE;
				SetSystemCursor(m_hCursorHand);
				if (card->m_nChairNO!=GetGameWinApp()->GetChairNO())
					return FALSE;
				if (card->bnTouch==TRUE)
					return FALSE;
					
				for(int i=0;i<GetTotalCardsCount();i++)
				{
					if (m_Card[i]->m_nChairNO==GetGameWinApp()->GetChairNO()
						&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND
						&&m_Card[i]!=card
						&&m_Card[i]->bnTouch==TRUE)
					{
						m_Card[i]->bnTouch=FALSE;
						m_Card[i]->Fresh();
					}
				}
								
				if (GetGameRunTime()-card->m_dwLastFastSelect>300)
					card->bnTouch=TRUE;
							
				card->Fresh();

				if (card->m_nStatus==CARD_STATUS_THROWDOWN 
					&& IS_BIT_SET(GetGameInfoEx()->GetGameStartInfo()->dwStatus,TS_WAITING_ZUPAI))
				{
					for (int i=0;i<TOTAL_CARDS;i++)
					{
						if (m_Card[i]->m_nDrawIndex == card->m_nDrawIndex)
						{
							m_Card[i]->Fresh();
						}
					}
				}
			}
		}
		break;
	case GAME_MSG_ASK_NEW_TABLE:
		{
			GUI_HideCountDownClock();
			m_welfDialog->HideWelfareDlg();
			int realWelfStat = m_welfDialog->GetWelfRealStat();
			if(m_welfareSurplusNum >= m_nDailyNum)
			{
				realWelfStat = -4;
			}
			m_welfDialog->SetWelfareStat(realWelfStat);
			CSKMainGame::OnMsg(msg);
		}
		break;
	case TCSG_CALLBACKMSG_DEALCARD:
		{
			MOVIE_DealSanGongCard();
		}
		break;
	case TCSG_CALLBACKMSG_DEALCARDOVER:
		{
			BeginZuPai();
		}
		break;
	case TCSG_CALLBACKMSG_NOTHING:
		break;
	case TCSG_CALLBACKMSG_INDIACTOR:
		{
			int nBanker = (int)msg.param1;
			int nCount = (int)msg.param2;
			int nChairNO;
			if(m_nDealCount == 0)
			{
				for(int i = 0; i < TOTAL_CHAIRS; ++i)
				{
					nChairNO = (nBanker + i) % TOTAL_CHAIRS;
					if(!IsInGame(nChairNO))
						continue;
					BOOL bShowIndicator=FALSE;
					if(GetIsAllGiveUpQZ())
						bShowIndicator=TRUE;
					else
					{
						if(GetQZFlag(nChairNO)==1)
							bShowIndicator=TRUE;
					}
					if(bShowIndicator)
					{
						int nDrawIndex = RUL_GetDrawIndexByChairNO(nChairNO);
						m_indicator[nDrawIndex]->Show();
						m_indicator[nDrawIndex]->Updata();
					}
				}
			}
			
			UpdataIndicatorIightAni(nBanker, nCount);
		}
		break;
	case TCSG_CALLBACKMSG_FEIZHUANG:
		{
            int nBanker = (int)msg.param1;
		    PlayZJAni(nBanker);	
			CGameWinApp* pGameApp=GetGameWinApp();
			CString str;
			GAMEPLAYER player=pGameApp->m_GamePlayer[GetGameInfo()->GetBanker()];
			str.Format("<size=12> 本局庄家：<c=255>%s\r\n",player.szUsername);
			pGameApp->ShowMsgInChatView(str);
		}
		break;
	case TCSG_CALLBACKMSG_YAZHUBEGIN:
		{
			BeginYaZhu();
		}
		break;
	case TCSG_CALLBACK_BUTTON_CHARGE:
		{
			theApp.ShowGameCharge();
			m_welfDialog->HideWelfareDlg();
		}
		break;
	case TCSG_CALLBACKMSG_BTN_GET_WELFARE:
		{
			int welfStat = m_welfDialog->GetWelfareStat();
			if(welfStat != -3)
			{
				OnGetWelfareConfig(m_nActivityID);
			}
			
			m_welfDialog->CreateWelfDialog();
			m_welfDialog->ShowWelfareDlg();
			int nMoney = m_welfDialog->GetWelfareLine();
			if(welfStat != 0)
			{
                m_welfDialog->SetBtnEnable(false);
			}
			else
			{
				m_welfDialog->SetBtnEnable(true);
			}
		}
		break;
	case TCSG_CALLBACKMSG_BTN_CLOSE_WELFARE:
		{
			m_welfDialog->HideWelfareDlg();
		}
		break;
	case TCSG_CALLBACKMSG_BTN_GET_WElFAREAWARED:
		{
			StopCanGetWelfare();			
			int nGetAward = 0;
			m_welfDialog->OnApplyWelfare(APPLY_WELFARE_TO_GAME, &nGetAward);

			int nChairNo = theApp.GetChairNO();
			GAMEPLAYER *player=&theApp.m_GamePlayer[nChairNo];
			player->nDeposit += nGetAward;
			OPE_ShowWelfRushDigit(nGetAward);
			int index = RUL_GetDrawIndexByChairNO(nChairNo);
            m_ptrPlayer[index]->DrawPlayerBoard();

			m_welfareSurplusNum++;
			m_welfDialog->RecordOrReadWelfareNum(true);
			if(m_welfareSurplusNum >= m_nDailyNum)
			{
				m_welfareSurplusNum = m_nDailyNum;
				m_welfDialog->SetWelfareStat(-4);
			}
			else
			{
				m_welfDialog->SetWelfareStat(-2);
			}
			OnGetWelfareConfig(m_nActivityID);
		}
		break;
	case TCSG_CALLBACKMSG_BTN_SHOW_WELFARE:
		{
			ShowCanGetCallBack();
		}
		break;
	case TCSG_CALLBACKMSG_FEIZHUANGANI:
		{
			GetMusicModule()->PlaySound("定点飞庄字");
			OPE_ShowBankerFlag(TRUE,TRUE);
			APPLY_WaitFewTimes(1000,&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_YAZHUBEGIN));
		}
        break;
	case TCSG_CALLBACKMSG_ClOSECHARGE:
	case TCSG_CALLBACKMSG_WEBCHARGE:
	case TCSG_CALLBACKMSG_QRCODECHARGE:
		{
			m_welfDialog->HideWelfareDlg();		
		}
		break;
	case TCSG_CALLBACKMSG_REFFESHCHARGE:
		{
			m_welfDialog->HideWelfareDlg();
			int welfStat = m_welfDialog->GetWelfareStat();
			if(welfStat != -3)
			{
				OnGetWelfareConfig(m_nActivityID);
			}	
		}
		break;
	case TCSG_CALLBACKMSG_SHOWINDICATORSCALE:
		{
			int nBanker = (int)msg.param1;
			int nWidth = m_indicator[nBanker]->GetWidth();
			int nHeight = m_indicator[nBanker]->GetHeight();
			int nDrawIndex = RUL_GetDrawIndexByChairNO(nBanker);
			BeginGroup();
			APPLY_DirectShow(m_indicator[nDrawIndex], false);
			NextGroup();
			APPLY_WaitFewTimes(100);
			NextGroup();
			APPLY_DirectShow(m_indicator[nDrawIndex], true);
			APPLY_ScaleObject(m_indicator[nDrawIndex], 0, 0, nWidth*1.2, nHeight*1.2, 3, 3);
			NextGroup();
            APPLY_ScaleObject(m_indicator[nDrawIndex], nWidth*1.2, nHeight*1.2, nWidth, nHeight, 3, 3);
			EndGroup();
		}
		break;
	case TCSG_CALLBACKMSG_YAZHUBTN:
		{
			int nDbl = (int)msg.param1;
            OPE_YaZhu(nDbl);
		}
		break;
	case TCSG_CALLBACKMSG_CARDTYPEBTN:
		{
			if(m_bitCardType)
			{
				if(m_bitCardType->IsShow())
				{
					m_bitCardType->Hide();
					m_bitCardType->Updata();
				}
				else
				{
					m_bitCardType->Show();
					m_bitCardType->Updata();
				}

			}
		}
		break;
	default:
		return CSKMainGame::OnMsg(msg);
	}

	return TRUE;
}


void    CMyGame::OPE_CloseResult()
{
	PGL_HIDE_OBJECT(m_plResultDlg);

	if (GetGameWinApp()->IsGameRunning())
		return;

	{
		//是否允许在关闭结果框继续查看
		//OPE_ClearGame();
		if (!GetGameWinApp()->IsLookOn())
			OPE_ShowStartButton();
	}

}

void    CMyGame::OnStart()
{
	if (!theApp.m_bGameAobrt)
	{
		PGL_HIDE_OBJECT(m_btnStart);
		dwAutoWaited=0;
		CString sRet;
		
		if (GetGameInfo()->IsGamePlaying())//已经开始
			return;
		
		OPE_ClearGame();
		
		for(int i=0;i<GetGameWinApp()->GetTableChairCount();i++)
		{
			if (GetGameInfo()->IsChairReady(i))
			{
				int nDrawIndex=RUL_GetDrawIndexByChairNO(i);
				m_ptrPlayer[nDrawIndex]->ShowReady();
			}
		}
		
		// modify by thg, 20130802
		if (GetGameWinApp()->IsSoloRoom() && GetGameWinApp()->IsLeaveAlone())
		{
			//solo独离房 
			BOOL bStart=GetGameWinApp()->GetGameConnect()->GC_StartGameEx(sRet);
			if (!bStart) //等待回应
			{
				OPE_ShowStartButton();
				return;
			}

		}// modify end
		else
		{
			if(!GetGameWinApp()->GetGameConnect()->GC_StartGame(sRet))
			{
				if(GetGameWinApp()->GetOfflineFlag()!=OFFLINEFLAG_OFFLINE)//在线
				{
					AfxMessageBox(sRet,MB_ICONINFORMATION);
					CWnd* pWnd=AfxGetMainWnd();
					if(pWnd) pWnd->PostMessage(WM_CLOSE);
					return ;
				}
			}	 
		}
	}
	else
	{
		PGL_HIDE_OBJECT(m_btnStart);
	}
}

void CMyGame::OnFreshGameSet()
{
	if (GetGameWinApp()->IsSilence())
	{
		GetMusicModule()->Silence(FALSE);
	}
	else
	{
		GetMusicModule()->Silence(TRUE);
	}

	if (theApp.m_GameSet.nMusic)
	{
		if (GetGameWinApp()->IsSilence())
		{
			if (!GetMusicModule()->IsPlaying(0))
			{
				DWORD dwSoundChannel = GetMusicModule()->PlaySound("default",10000);
				GetMusicModule()->BASSChannelSetAttribute(dwSoundChannel,2,1);
			}
		}
		else
		{
			if (!GetMusicModule()->IsPlaying(0))
			{
				GetMusicModule()->Silence(FALSE);
				DWORD dwSoundChannel = GetMusicModule()->PlaySound("default",10000);
				GetMusicModule()->BASSChannelSetAttribute(dwSoundChannel,2,1);
				GetMusicModule()->Silence(TRUE);
			}
		}
	}
	else
	{
		if (GetGameWinApp()->IsSilence())
		{
			GetMusicModule()->FreeSound("default");
		}
		else
		{
			GetMusicModule()->Silence(FALSE);
			GetMusicModule()->FreeSound("default");
			GetMusicModule()->Silence(TRUE);
		}
	}

	if (!m_btnAutoPlay
		|| !m_btnAutoPlay->IsShow())
		return;
	if (theApp.IsLikeRemaind())
	{
		if (m_btnPass->IsShow())
		{
			m_btnReMaind->Show();
			FitButtonPosition();
		}
	}
	else
	{
		if (m_btnReMaind->IsShow())
		{
			FitButtonPosition();
		}
	}
}

void GetCharFormatFromFontTag(CString sTag,TCY_CHARFORMAT& tcf)
{
	// sTag:	<s=180,c=1234456,f=宋体,e=> 
 	BOOL bUnderLine=0;
	BOOL bBold=0;
	BOOL bItalic=0;
	BOOL bLink=0;
	BOOL bPassword=0;
	LONG nSize=180;
	CString sFaceName=_T("宋体");
	CString sLink;
	COLORREF   crColor=RGB(0,0,0);
	
	int nFind;

	/////////////size //////////////////
	nFind=sTag.Find(_T("s="),0);
	if(nFind!=-1)
	{
		TCHAR head=sTag.GetAt(nFind-1);
		if(head=='<' || head==',' || head==' ')
		{
			nSize=atoi(sTag.Mid(nFind+2));
			if(nSize>12)
				nSize=12;
		}
	}


	///////////color///////////////
	nFind=sTag.Find(_T("c="),0);
	if(nFind!=-1)
	{
		TCHAR head=sTag.GetAt(nFind-1);
		if(head=='<' || head==',' || head==' ')
		{
			crColor=atoi(sTag.Mid(nFind+2));
		}
	}

	////////////face/////////////////
	nFind=sTag.Find(_T("f="),0);
	if(nFind!=-1)
	{
		TCHAR head=sTag.GetAt(nFind-1);
		if(head=='<' || head==',' || head==' ')
		{
  			int nEnd=sTag.Find(',',nFind);
			if(nEnd==-1)
				nEnd=sTag.Find('>',nFind);
			sFaceName=sTag.Mid(nFind+2,nEnd-nFind-2);
		}
	}
	
	////////////link/////////////////
	nFind=sTag.Find(_T("l="),0);
	if(nFind!=-1)
	{
		TCHAR head=sTag.GetAt(nFind-1);
		if(head=='<' || head==',' || head==' ')
		{
 			int nEnd=sTag.Find(',',nFind);
			if(nEnd==-1)
				nEnd=sTag.Find('>',nFind);
			sLink=sTag.Mid(nFind+2,nEnd-nFind-2);
		}
	}
	//////////////effects//////////////
	nFind=sTag.Find(_T("e="),0);
	if(nFind!=-1)
	{
		TCHAR head=sTag.GetAt(nFind-1);
		if(head=='<' || head==',' || head==' ')
		{
			 int nEnd=sTag.Find(',',nFind);
			 if(nEnd==-1)
				 nEnd=sTag.Find('>',nFind);
			 
			 CString sTmp=sTag.Mid(nFind+2,nEnd-nFind-2);

			 if(sTmp.Find('u')!=-1)
					bUnderLine=TRUE;
 			 if(sTmp.Find('b')!=-1)
					bBold=TRUE;
 			 if(sTmp.Find('i')!=-1)
					bItalic=TRUE;
 			 if(sTmp.Find('l')!=-1)
					bLink=TRUE;
	 		 if(sTmp.Find('p')!=-1)
					bPassword=TRUE;
		}
	}
	/////////////////////////////////////////////////////////////////////////////
//	bLink  &=m_bEnableLink;
	//////////////////////////////////////////////////////////////////////////////
 	tcf.cf.crTextColor	= crColor; 
	tcf.cf.yHeight     = nSize  ;
	tcf.bPassword =bPassword;
	lstrcpyn(tcf.cf.szFaceName,sFaceName,LF_FACESIZE);
	lstrcpyn(tcf.szURL ,sLink,256);

	if(bUnderLine)
		tcf.cf.dwEffects |= CFE_UNDERLINE;
	if(bBold)
		tcf.cf.dwEffects |= CFE_BOLD;
	if(bItalic)
		tcf.cf.dwEffects |= CFE_ITALIC;
	if(bLink)
  		tcf.cf.dwEffects |= CFE_LINK;
 	//////////////bmp//////////////
	nFind=sTag.Find(_T("z="),0);
	if(nFind!=-1)
	{	
		TCHAR head=sTag.GetAt(nFind-1);
		if(head=='<' || head==',' || head==' ')
		{
 			int nID=atoi(sTag.Mid(nFind+2,3));
//			InsertImage(nID);
		}
	}
 
}

void    CMyGame::OnThrow()
{
	//For test
	CSKMainGame::OnThrow();
}

void     CMyGame::OnPass()
{
	CSKMainGame::OnPass();
}

void    CMyGame::OnReMind()
{
	if (GetPublicInfo()->nWaitChair==-1)//首家出牌不考虑提示
		return;
	
	OPE_UnSelectCard();
	
	if (OnBestReMind())
		return;
	
	int nRemind[SK_CHAIR_CARD];
	int nInHand[SK_CHAIR_CARD];
	XygInitChairCards(nRemind,SK_CHAIR_CARD);
	XygInitChairCards(nInHand,SK_CHAIR_CARD);
	
	RUL_GetInHandCards(nInHand);
	if (m_ReMindUniteType.dwCardType!=0)
	{//使用上次提示的组合作为判断基础，以形成提示链
		if (OPE_BulidCard(nInHand,SK_CHAIR_CARD,nRemind,SK_CHAIR_CARD,m_ReMindUniteType,m_ReMindUniteType))
		{
			OPE_SelectCard(nRemind,SK_CHAIR_CARD);
			OPE_CheckSelect();
			PlaySound("Snd_HitCard");
		}
		else 
		{
			memset(&m_BestReMindUniteType,0,sizeof(m_BestReMindUniteType));
			if (OnBestReMind())
			{
				memset(&m_ReMindUniteType,0,sizeof(m_ReMindUniteType));
				return;
			}
			
			if (OPE_BulidCard(nInHand,SK_CHAIR_CARD,nRemind,SK_CHAIR_CARD,GetPublicInfo()->WaitCardUnite,m_ReMindUniteType))
			{
				OPE_SelectCard(nRemind,SK_CHAIR_CARD);
				OPE_CheckSelect();
				PlaySound("Snd_HitCard");
			}
			else
			{
				OnPass();
			}
		}
	}
	else
	{
		if (OPE_BulidCard(nInHand,SK_CHAIR_CARD,nRemind,SK_CHAIR_CARD,GetPublicInfo()->WaitCardUnite,m_ReMindUniteType))
		{
			OPE_SelectCard(nRemind,SK_CHAIR_CARD);		
			OPE_CheckSelect();
			PlaySound("Snd_HitCard");
		}
		else
		{
			OnPass();
		}
	}
}

BOOL CMyGame::OnBestReMind()
{
//	return CSKMainGame::OnBestReMind();
	int nRemind[SK_CHAIR_CARD];
	int nInHand[SK_CHAIR_CARD];
	int nInHandLay[SK_CHAIR_CARD];
	XygInitChairCards(nRemind,SK_CHAIR_CARD);
	XygInitChairCards(nInHand,SK_CHAIR_CARD);
	memset(nInHandLay,0,sizeof(nInHandLay));
	
	RUL_GetInHandCards(nInHand);
	SK_LayCards(nInHand,SK_CHAIR_CARD,nInHandLay,0);
	
	//首先检索不产生散牌的组合
	UNITE_TYPE perfect;
	memset(&perfect,0,sizeof(perfect));
	
	if (m_BestReMindUniteType.dwCardType==0)
	{
		memcpy(&perfect,&GetPublicInfo()->WaitCardUnite,sizeof(UNITE_TYPE));
	}
	else
	{
		memcpy(&perfect,&m_BestReMindUniteType,sizeof(UNITE_TYPE));
	}
	
	int nRemaindLay[SK_LAYOUT_NUM];
	while(OPE_BulidCard(nInHand,SK_CHAIR_CARD,nRemind,SK_CHAIR_CARD,perfect,perfect))
	{
		memset(nRemaindLay,0,sizeof(nRemaindLay));
		SK_LayCards(nRemind,SK_CHAIR_CARD,nRemaindLay,0);
		BOOL bnMatch=TRUE;
		for(int j=0;j<SK_LAYOUT_NUM;j++)
		{
			if (nRemaindLay[j]!=0
				&&nRemaindLay[j]!=nInHandLay[j])
			{
				bnMatch=FALSE;
				break;
			}
		}
		
		if (bnMatch)
		{
			memcpy(&m_BestReMindUniteType,&perfect,sizeof(UNITE_TYPE));
			
			OPE_SelectCard(nRemind,SK_CHAIR_CARD);
			OPE_CheckSelect();
			PlaySound("Snd_HitCard");
			
			return TRUE;	
		}
		else
		{
			XygInitChairCards(nRemind,SK_CHAIR_CARD);
		}	
	}
	
	return FALSE;
}

void CMyGame::OnReMind2()
{
	if (GetPublicInfo()->nWaitChair==-1)//首家出牌不考虑提示
		return;
	
	OPE_UnSelectCard();
	int nRemind[SK_CHAIR_CARD];
	int nInHand[SK_CHAIR_CARD];
	int lay[SK_LAYOUT_NUM];
	
	XygInitChairCards(nRemind,SK_CHAIR_CARD);
	XygInitChairCards(nInHand,SK_CHAIR_CARD);
	RUL_GetInHandCards(nInHand);
	memset(lay,0,sizeof(lay));
	SK_LayCards(nInHand,SK_CHAIR_CARD,lay,0);
	
	UNITE_TYPE temp;
	memset(&temp,0,sizeof(UNITE_TYPE));
	memcpy(&temp,&(GetPublicInfo()->WaitCardUnite),sizeof(UNITE_TYPE));
	
	int nRemindLay[SK_LAYOUT_NUM];
	memset(nRemindLay,0,sizeof(nRemindLay));
	
	BOOL bnFind=FALSE;
	while(OPE_BulidCard(nInHand,SK_CHAIR_CARD,nRemind,SK_CHAIR_CARD,temp,temp,CARD_UNITE_TYPE_TOTAL_EX,FALSE)
		||OPE_BulidCard(nInHand,SK_CHAIR_CARD,nRemind,SK_CHAIR_CARD,temp,temp,CARD_UNITE_TYPE_TOTAL_EX))
	{
		//检查有没有拆炸弹
		SK_LayCards(nRemind,SK_CHAIR_CARD,nRemindLay,0);
		
		BOOL bnCheckOK=TRUE;
		for(int i=0;i<temp.nCardCount;i++)
		{
			int nCardID=temp.nCardIDs[i];
			int nCardIndex=SK_GetCardIndexEx(nCardID);
			if (nRemindLay[nCardIndex]!=lay[nCardIndex])
			{
				bnCheckOK=FALSE;
				break;
			}
		}
		
		if (bnCheckOK)
		{
			bnFind=TRUE;
			break;
		}
		else
		{
			memset(nRemindLay,0,sizeof(nRemindLay));
			XygInitChairCards(nRemind,SK_CHAIR_CARD);
		}
	}
	
	if (bnFind)
	{
		OPE_SelectCard(nRemind,SK_CHAIR_CARD);
		OPE_CheckSelect();
	}
	else
	{
		XygInitChairCards(nRemind,SK_CHAIR_CARD);
		
		if (OPE_BulidCard(nInHand,SK_CHAIR_CARD,nRemind,SK_CHAIR_CARD,GetPublicInfo()->WaitCardUnite,m_ReMindUniteType,CARD_UNITE_TYPE_TOTAL_EX,FALSE)
			||OPE_BulidCard(nInHand,SK_CHAIR_CARD,nRemind,SK_CHAIR_CARD,GetPublicInfo()->WaitCardUnite,m_ReMindUniteType,CARD_UNITE_TYPE_TOTAL_EX))
		{
			OPE_SelectCard(nRemind,SK_CHAIR_CARD);
			OPE_CheckSelect();
		}
	}
}


BOOL CMyGame::OPE_ReMaindEx(int nInHand[],int nInLen,DWORD dwUniteType,BOOL bnSelect)
{
	OPE_UnSelectCard();
	int nRemind[SK_CHAIR_CARD];
	XygInitChairCards(nRemind,SK_CHAIR_CARD);
	
	UNITE_TYPE temp={0};
	
	if (!IS_BIT_SET(dwUniteType,temp.dwCardType))
	{
		memset(&temp,0,sizeof(temp));
	}
	
	if (!OPE_BulidCard(nInHand,nInLen,nRemind,SK_CHAIR_CARD,temp,temp,dwUniteType))
	{
		if (temp.dwCardType)
		{
			memset(&temp,0,sizeof(temp));
			if (!OPE_BulidCard(nInHand,nInLen,nRemind,SK_CHAIR_CARD,temp,temp,dwUniteType))
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}
	
	if (bnSelect)
	{
		for(int i=0;i<SK_CHAIR_CARD;i++)
		{
			if (nRemind[i]==-1) break;
			CGameCard* card=GetCard(GetGameWinApp()->GetChairNO(),nRemind[i]);
			card->bnSelect=TRUE;
			card->Fresh();
		}
		OPE_CheckSelect();
	}
	
	return TRUE;
}

void   CMyGame::OnCardMaster()
{
	if (m_CardMaster
		&&m_CardMaster->IsShow())
	{
		m_CardMaster->Hide();
		m_CardMaster->Updata();
		return;
	}

	
	RECT rect=GetGameRect();
	int x=rect.left;
	int y=rect.top;
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;

	m_CardMaster->MoveTo(x+w-m_CardMaster->GetWidth()/2,y+h-m_btnCardMaster->GetHeight()-m_CardMaster->GetHeight()/2);
	m_CardMaster->RePaint();
	m_CardMaster->Show();
	m_CardMaster->Updata();
}

void    CMyGame::OnTimer()
{
	if (m_Clock->GetDigit()>0)
	{
		m_nGameZeroCount=0;
		m_Clock->StepDigit();
	}
	else if (m_Clock->GetDigit()==0)
	{
		if (m_nGameZeroCount==0)
			OPE_GameClockZero();

		m_nGameZeroCount++;
		if (m_nGameZeroCount>0&&m_nGameZeroCount%10==3)
		{
			//时钟连续为0，检查当前玩家是否掉线
			if (!theApp.IsLookOn())
			{
				//theApp.ShowMsgInChatView("是否掉线");
				REQ_SendMsgToServer(SYSMSG_GAME_CLOCK_STOP,0,NULL,FALSE);
			}
		}
	}
}

void  CMyGame::OnGetNewPosition()
{
	CSKMainGame::OnGetNewPosition();

	int nReadyCount=0;
	for(int i=0;i<GetGameWinApp()->GetTableChairCount();i++)
	{
		if (GetGameWinApp()->IsPlayerEnter(i))
		{
			int nDrawIndex=RUL_GetDrawIndexByChairNO(i);
			if (GetGameInfo()->IsChairReady(i)
				&&!GetGameInfoEx()->IsGamePlaying())
			{
				m_ptrPlayer[nDrawIndex]->ShowReady();
				nReadyCount++;
			}
			else
				m_ptrPlayer[nDrawIndex]->HideReady();
		}
	}

	if (GetGameInfoEx()->IsGamePlaying())
	{
		OPE_DXXW();
	}
	//换桌进来如果处于TS_WAITING_COUNTDOWN阶段 那么显示时钟
	else if(nReadyCount>=2)
	{
		GetGameInfoEx()->GetGameStartInfo()->dwStatus=TS_WAITING_COUNTDOWN;
		OPE_StartCountDownTimer();
		GUI_ShowCountDownClock(m_nNewTableCountDown);
	}
}

void  CMyGame::NTF_GameStart()
{
	//关闭开始倒计时
	OPE_StopCountDownTimer();
	GUI_HideCountDownClock();
	m_nZeroCountDown=0;

	if (!theApp.IsLookOn())
		xyAppCallAttention();//提醒用户操作

	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		if (GetGameWinApp()->IsGameRunning()&&!GetGameInfoEx()->GetGameStartInfo()->bInGame[i]&& GetGameWinApp()->IsPlayerEnter(i))
		{
			PGL_SHOW_OBJECT(m_bitWaitNextBout[RUL_GetDrawIndexByChairNO(i)]);
		}
		else
		{
			PGL_HIDE_OBJECT(m_bitWaitNextBout[RUL_GetDrawIndexByChairNO(i)]);
		}
		
		memcpy(m_szUsername[i], GetGameWinApp()->m_GamePlayer[i].szUsername, sizeof(m_szUsername[i]));
	}
	if (theApp.IsLookOn() || !GetGameInfoEx()->GetGameStartInfo()->bInGame[theApp.GetChairNO()])
	{
		OPE_ClearGame();
		
		PGL_HIDE_OBJECT(m_btnStart);
	}
	m_bnAutoPlay = FALSE;
	for ( i = 0; i < TOTAL_CHAIRS; i++)
	{
		((CMyPlayer*)m_ptrPlayer[RUL_GetDrawIndexByChairNO(i)])->HidePlayerCloth();
		m_ptrPlayer[RUL_GetDrawIndexByChairNO(i)]->AutoPlay(FALSE);
	}
	CGameWinApp* pGameApp=GetGameWinApp();
		
	OPE_EndTimer();						//关闭时钟
	OPE_HideButtons();					//隐藏所有的按钮
	OPE_ShowAssistantButton(FALSE);		//隐藏辅助按钮
	OPE_InitData();						//初始化数据
	OPE_ShowGameInfo();					//显示游戏信息面板
	HideWaitArrageTable();				//隐藏“等待系统配桌”
	ShowHuanZhuoBtn(FALSE);
    OPE_StartTimer();

	CString strMsg;
	strMsg.Format("\r\n<size=12>【第%d局】\r\n",GetGameInfoEx()->GetBoutCount());
	pGameApp->ShowMsgInChatView(strMsg);

	//抢庄(坐庄场)
	if (IS_BIT_SET(GetGameInfo()->GetStatus(), TS_WAITING_QZ))
	{
		OPE_QZ();
	}
	//直接押注(坐庄场)
	else if(IS_BIT_SET(GetGameInfo()->GetStatus(), TS_WAITING_YAZHU))
	{
		GAMEPLAYER player=pGameApp->m_GamePlayer[GetGameInfo()->GetBanker()];
		strMsg.Format("<size=12> 本局庄家：<c=255>%s\r\n",player.szUsername);
		pGameApp->ShowMsgInChatView(strMsg);
		OPE_ShowBankerFlag();
		BeginYaZhu();
	}
	//直接组牌(通比场)
	else if(IS_BIT_SET(GetGameInfo()->GetStatus(), TS_WAITING_ZUPAI))
	{
		GUI_HideCountDownClock();
		HideClock();
		OPE_DealSanGongCard();
		OPE_ShowBaseDeposit();
		m_bTongBi=TRUE;
	}

	if (strlen(pGameApp->m_szMatchWWW)>0)	//刷新比赛排行榜
	{
		CAdvertBar* bar=(CAdvertBar*)GetObject(GAMEBASE_GAME_ADVERTBAR);
		if (bar)
		{
			bar->ClickHtmlBtn(_T("刷新排名"));
		}
	}
}

void CMyGame::OPE_EndTimer()
{
	CSKMainGame::OPE_EndTimer();
	HideClock();
}

void   CMyGame::NTF_GameWin()
{
	GetPublicInfoEx()->bAllGiveUpQZ=FALSE;
	memset(GetPublicInfoEx()->nQZFlag, 0, sizeof(int)*(TOTAL_CHAIRS));
	memset(GetPublicInfoEx()->bFinishQZ,0,sizeof(BOOL)*TOTAL_CHAIRS);
	memset(GetPublicInfoEx()->nYaZhuDbl, 0, sizeof(int)*(TOTAL_CHAIRS));
	memset(GetPublicInfoEx()->bFinishYaZhu,0,sizeof(BOOL)*TOTAL_CHAIRS);
	memset(GetPublicInfoEx()->bFinishZuPai,0,sizeof(BOOL)*TOTAL_CHAIRS);
	for(int k=0;k<TOTAL_CHAIRS;k++)
	{
		memset(&(GetPlayerInfoEx(k)->stDetail),0,sizeof(SANGONG_DETAIL));
	}

	CGameWinApp* pGameApp=GetGameWinApp();
	if (!pGameApp->IsLookOn())
		xyAppCallAttention();//提醒用户操作

	GetGameInfoEx()->GetGameStartInfo()->dwStatus=TS_WAITING_COMPARE|TS_PLAYING_GAME;		//状态
	
	OPE_EndTimer();
	OPE_HideButtons();
	OPE_ShowAssistantButton(FALSE);

	APPLY_WaitFewTimes(500,&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_SHOWRESULT));	//显示结果

	//刷新比赛排行榜
	if (strlen(pGameApp->m_szMatchWWW)>0)
	{
		CAdvertBar* bar=(CAdvertBar*)GetObject(GAMEBASE_GAME_ADVERTBAR);
		if (bar)
		{
			bar->ClickHtmlBtn(_T("刷新排名"));
		}
	}
	m_CEventPipe->ReleaseEvent();
	
}

CGameCard* CMyGame::GetThrowCard(int chairno, int nCardID)
{
	for(int i=0;i<GetTotalCardsCount();i++)
	{
		if (m_Card[i]->m_nChairNO==chairno
			&&m_Card[i]->m_nCardID==nCardID
			&&m_Card[i]->m_nStatus==CARD_STATUS_THROWDOWN)
			return m_Card[i];
	}
	
	for(i=0;i<GetTotalCardsCount();i++)
	{
		if (m_Card[i]->m_nChairNO==chairno
			&&m_Card[i]->m_nStatus==CARD_STATUS_THROWDOWN)
			return m_Card[i];
	}
	
	return NULL;
}

void   CMyGame::NTF_GameStandOff()
{
	CGameWinApp* pGameApp=GetGameWinApp();
	if (!pGameApp->IsLookOn())
		xyAppCallAttention();//提醒用户操作
	
	OPE_EndTimer();
	OPE_HideButtons();
	OPE_ShowAssistantButton(FALSE);
	
	int chairno = GetGameInfoEx()->GetGameWin()->nReserved[1];
	if (chairno>=0&&chairno<TOTAL_CHAIRS)
	{
		OPE_HideAllPlayerAnmition();
// 		CString str;
// 		str.Format(_T(" <z=tc系统通知><c=%d> %s<>不叫\r\n"), RGB(255,0,0), theApp.GetUserNameByChairNO(chairno));
// 		theApp.ShowMsgInChatView(str);
// 		OPE_PlaySoundForAuction(chairno,TRUE,0);
	}

//	theApp.ShowMsgInChatView(" <z=tc系统通知> 连续3次无人叫分，可以退出游戏，或点击开始继续游戏！\r\n");

	//玩家手牌
	for(int i=0;i<m_nTotalPlayer;i++)
	{
		int nDrawIndex=RUL_GetDrawIndexByChairNO(i);
		
		OPE_ShowPlayerCard(i,GetGameInfoEx()->GetGameWin()->nCardID[i],CHAIR_CARDS);
		OPE_FreshPlayerCard(i);
		if (m_ptrPlayer[nDrawIndex])
			m_ptrPlayer[nDrawIndex]->ClearStatus();
	}
	//
	for (i=0;i<BOTTOM_CARD;i++)
	{
		m_nBottomCard[i]->m_nCardID=GetGameInfoEx()->GetGameWin()->nBottomIDs[i];
		m_nBottomCard[i]->bnFace=TRUE;
		m_nBottomCard[i]->Fresh();
	}
	
	PlaySound("Snd_standoff");
	
	//刷新比赛排行榜
	if (strlen(pGameApp->m_szMatchWWW)>0)
	{
		CAdvertBar* bar=(CAdvertBar*)GetObject(GAMEBASE_GAME_ADVERTBAR);
		if (bar)
		{
			bar->ClickHtmlBtn(_T("刷新排名"));
		}
	}

	if (!GetGameWinApp()->IsLookOn())
		OPE_ShowStartButton();

	m_CEventPipe->ReleaseEvent();
}

void CMyGame::NTF_PlayerAbort(GAME_ABORT * pGameAbort)
{
	GetGameInfoEx()->ResetChairStatus(pGameAbort->nChairNO);

	if (!theApp.IsGameRunning() && pGameAbort->nChairNO == GetGameInfo()->GetBanker())
	{
		OPE_ShowBankerFlag(FALSE);
	}

	//还剩多少人
	int nEnterCount=0;
	int nEnterChairNO=-1;
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		if(theApp.IsPlayerEnter(i) && i!=pGameAbort->nChairNO)
		{
			nEnterCount++;
			nEnterChairNO=i;
		}
	}
	if(nEnterCount==1 && nEnterChairNO==GetGameInfo()->GetBanker())
	{
		OPE_ShowBankerFlag(FALSE);
	}
}

void CMyGame::NTF_CardsThrow(CARDS_THROW *pCardsThrow)
{
	if (pCardsThrow->unite.dwCardType==CARD_UNITE_TYPE_ABT_SINGLE
		||pCardsThrow->unite.dwCardType==CARD_UNITE_TYPE_ABT_COUPLE
		||pCardsThrow->unite.dwCardType==CARD_UNITE_TYPE_ABT_THREE
		||pCardsThrow->unite.dwCardType==CARD_UNITE_TYPE_ABT_THREE_COUPLE)
	{
		if (m_nSortType!=SORT_FLAGS_COUNT_MORE
			&&m_nSortType!=SORT_FLAGS_BIG)
		{
			int nCardPri[CHAIR_CARDS];
			memset(nCardPri,0,sizeof(nCardPri));
			for(int i=0;i<pCardsThrow->unite.nCardCount;i++)
			{
				nCardPri[i]=(SK_GetCardPRIEx(pCardsThrow->unite.nCardIDs[i],0))*10+SK_GetCardShapeEx(pCardsThrow->unite.nCardIDs[i]);
			}
			
			XygReversalMoreByValue(pCardsThrow->unite.nCardIDs,nCardPri,pCardsThrow->unite.nCardCount);
		}

	}

	BOOL bFirstHand = GetPublicInfoEx()->nWaitChair==-1;
	GetPublicInfoEx()->nWaitChair=pCardsThrow->nChairNO;
//	memcpy(&(GetPublicInfoEx()->WaitCardUnite),&pCardsThrow->unite,sizeof(UNITE_TYPE)-sizeof(int)*(MAX_CARDS_PER_CHAIR-pCardsThrow->unite.nCardCount));
	memcpy(&(GetPublicInfoEx()->WaitCardUnite),&pCardsThrow->unite,sizeof(UNITE_TYPE));
	OPE_HideButtons();

	//庄家去掉底牌标记
	{
		if (pCardsThrow->nChairNO==GetGameInfoEx()->GetBanker())
		{
			if (theApp.IsLookOn()&&!GetGameInfoEx()->m_bnAlowLook)
			{
				
			}
			else
			{
				if (RUL_GetCardCount(pCardsThrow->nChairNO, CARD_STATUS_INHAND)==20)
				{
					for (int i=0;i<TOTAL_CARDS;i++)
					{
						if ((m_Card[i]->m_nChairNO==pCardsThrow->nChairNO)
							&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND
							&& ((CMyCard*)m_Card[i])->m_bBankerCard)
						{
							((CMyCard*)m_Card[i])->m_bBankerCard = FALSE;
							m_Card[i]->Fresh();
						}
					}
				}
			}
		}
	}

//	int move_time=130/(GetMinApplySpace());//牌飞行的时间
	for(int i=0;i<pCardsThrow->unite.nCardCount;i++)
	{
		if (pCardsThrow->unite.nCardIDs[i]!=-1)
		{
			CGameCard* card=GetCard(pCardsThrow->nChairNO,pCardsThrow->unite.nCardIDs[i]);
			if (card)
			{
				card->m_nCardID=pCardsThrow->unite.nCardIDs[i];
				card->m_nDrawIndex=RUL_GetDrawIndexByChairNO(pCardsThrow->nChairNO);
				card->m_nChairNO=pCardsThrow->nChairNO;
				card->m_nUnitCount=pCardsThrow->unite.nCardCount;
				card->m_nUnitIndex=i;
				card->m_nStatus=CARD_STATUS_THROWDOWN;
//				card->bnMoved=TRUE;
				card->Fresh();

//				POINT ptDest=card->GetThrowPosition();
//				double l=sqrt((card->GetX()-ptDest.x)*(card->GetX()-ptDest.x)+(card->GetY()-ptDest.y)*(card->GetY()-ptDest.y));
//				double speed=l/move_time;
// 
// 				BeginGroup();
// 
// 					APPLY_WaitFewTimes(GetMinApplySpace()*i);
// 				
// 				NextGroup();
// 				
// 					APPLY_MoveObject(card,card->GetThrowPosition(),speed,0,speed,&(SNG_MSG)CreateMsg(card->GetHandle(),CARD_CALLBACKMSG_FRESH));
// 				
// 				EndGroup();
			}
		}
	}

	OPE_AddBomb(pCardsThrow);
	OPE_FreshPlayerCard(pCardsThrow->nChairNO);
	if (theApp.IsUnlikeTip())
	{
		OPE_PlaySoundByOperation(pCardsThrow->nChairNO,pCardsThrow->unite.dwCardType,pCardsThrow->unite.nCardCount);
	}
	else
	{
		OPE_PlaySoundByOperation(bFirstHand, pCardsThrow);
		OPE_PlayAnimationByThrow(pCardsThrow);
	}

	
//  //显示名次
// 	if (pCardsThrow->nWinPlayce!=0)
// 	{
//  		m_Player[pCardsThrow->chairno]->m_bitEndRank->GoToFrame(pCardsThrow->nWinPlayce-1);
// 		m_Player[pCardsThrow->chairno]->m_bitEndRank->Show();
// 	 	m_Player[pCardsThrow->chairno]->m_bitEndRank->Updata();
// 	}
	
	
	if (pCardsThrow->nChairNO==theApp.GetChairNO())
	{
		//OPE_SortHand(SORT_FLAGS_NOSORT);
		OPE_SortHand((m_nSortType));
	}
	else if (m_CardMaster)
	{
		//记牌器
		for(int i=0;i<pCardsThrow->unite.nCardCount;i++)
			m_CardMaster->PopCard(pCardsThrow->unite.nCardIDs[i]);
		
		if (pCardsThrow->nChairNO!=theApp.GetChairNO())
		{
			for(int i=0;i<pCardsThrow->unite.nCardCount;i++)
			{
				m_CardMaster->ReCordChairCard(pCardsThrow->nChairNO,pCardsThrow->unite.nCardIDs[i]);
			}
			int nDrawIndex=RUL_GetDrawIndexByChairNO(pCardsThrow->nChairNO);
			if (m_CardMaster->m_PlayerPlane[nDrawIndex]
				&&m_CardMaster->m_PlayerPlane[nDrawIndex]->IsShow())
				m_CardMaster->RePaint(pCardsThrow->nChairNO);
 		}
	}

	GUI_MoveClock(pCardsThrow->Next_Chair,pCardsThrow->nWaitTime);
    OPE_ShowButtons();
}

void	CMyGame::OPE_ShowCardType(int chairno,DWORD type,int nCount)
{
	int nDrawIndex=RUL_GetDrawIndexByChairNO(chairno);
	POINT pt = ((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->GetTalkPosition();
	if (m_btnThrow&&nDrawIndex==MYDRAWINDEX)
		pt.y = m_btnThrow->GetY()-50;
	else if (nDrawIndex==1)
		pt.x += 50;
	else if (nDrawIndex==2)
		pt.x -= 50;
	switch(type)
	{
	case CARD_UNITE_TYPE_PASS:
		{
			if (theApp.IsUseBigUI())
				((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(_T("放大版_不出"), pt.x, pt.y);
			else
				((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(_T("文字_不出"), pt.x, pt.y);
		}
		
		break;
	}
	
}

void CMyGame::NTF_CardsPass(CARDS_PASS* pCardsPass)
{
	if (!theApp.IsRejectPassSound())
		PlaySound("Snd_Pass");
	OPE_HideButtons();
	OPE_ShowCardType(pCardsPass->nChairNO,CARD_UNITE_TYPE_PASS,0);
	
	if (pCardsPass->bNextFirst)
	{
		GetPublicInfoEx()->nWaitChair=-1;//第一手牌
		OPE_ClearTable();
	}

	GUI_MoveClock(pCardsPass->Next_Chair,pCardsPass->nWaitTime);
	OPE_ShowButtons();
}

void    CMyGame::OPE_InitData()
{
	m_nTotalPlayer=TOTAL_CHAIRS;
	m_nFastStartIndex=-1;
	m_nDealCount=0;
	m_nLastSelectCount=0;
	m_nRemindType=0;
	m_tLastCloseAutoPlay=0;
	memset(&m_ReMindUniteType,0,sizeof(m_ReMindUniteType));//重置回等前等待的牌型
	memset(&m_BestReMindUniteType,0,sizeof(m_BestReMindUniteType));
	
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		m_Card[i]->Reset();
		m_Card[i]->m_nUnitIndex=i;
		((CMyCard*)m_Card[i])->m_bBankerCard=FALSE;
		m_Card[i]->Fresh();
	}

	for(i=0;i<BOTTOM_CARD;i++)
	{
		m_nBottomCard[i]->Reset();
		m_nBottomCard[i]->m_nUnitIndex=i;
		m_nBottomCard[i]->Fresh();
	}

	for(i=0;i<m_nTotalPlayer;i++)
	{
		m_ptrPlayer[RUL_GetDrawIndexByChairNO(i)]->Restart();
	}
	
	//结束后重置
	OPE_ResetPublicInfo();

	StopApply(m_hEventTimer);

	GetGameInfoEx()->m_bnAlowLook = FALSE;
}

void CMyGame::OPE_ResetPublicInfo()
{
	for(int i=0;i<m_nTotalPlayer;i++)
	{
		GetPlayerInfoEx(i)->nInHandCount=0;
		GetPlayerInfoEx(i)->nBombCount=0;
	}
	
	GetPublicInfoEx()->nWaitChair=-1;
	GetPublicInfoEx()->nCurrentRank=1;	//定死打1
	GetPublicInfoEx()->nBombFan=0;
}

void      CMyGame::OPE_ClearGame()
{
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		m_Card[i]->m_nStatus=CARD_STATUS_HIDE;
		m_Card[i]->Fresh();
	}

	for(i=0;i<BOTTOM_CARD;i++)
	{
		m_nBottomCard[i]->m_nStatus=CARD_STATUS_HIDE;
		m_nBottomCard[i]->Fresh();
	}

	for(i=0;i<m_nTotalPlayer;i++)
	{
		int nIdx=RUL_GetDrawIndexByChairNO(i);

		m_ptrPlayer[nIdx]->HideTalk();
		m_ptrPlayer[nIdx]->Restart();
		//((CMyPlayer*)m_ptrPlayer[nIdx])->SetBanker(FALSE);

		m_bitPaiXing[nIdx]->Hide();
		m_bitPaiXing[nIdx]->Updata();

		m_bitQZFlag[nIdx]->Hide();
		m_bitQZFlag[nIdx]->Updata();
	}

	OPE_HideYZYinzi();
	OPE_HideYZYinziDigit();

	//隐藏结果框
	PGL_HIDE_OBJECT(m_plResultDlg);
	PGL_HIDE_OBJECT(m_plGameInfo);
	
	OPE_HideButtons();
	OPE_ShowAssistantButton(FALSE);

	GetGameFrame()->HideBoard();

	GetGameFrame()->OPE_HideMatchResult();

	//记牌器
	if (m_CardMaster)
	{
		m_CardMaster->ClearAllCard();
		m_CardMaster->RePaint();
		m_CardMaster->Hide();
		m_CardMaster->Updata();
	}

	for(i=0;i<TOTAL_CHAIRS;i++)
	{
		m_plRush[i]->Hide();
		m_plRush[i]->Updata();
		for(int j=0;j<2;j++)
		{
			m_digtRush[i][j]->Hide();
			m_digtRush[i][j]->Updata();
		}
	}

	for(i=0;i<TOTAL_CHAIRS-1;i++)
	{
		m_digitBaseDeposit[i]->Hide();
		m_digitBaseDeposit[i]->Updata();
	}
}

void CMyGame::OPE_SortHand(int dwFlags)
{
   int CardID[CHAIR_CARDS];
   int nCardIndex[TOTAL_CARDS];
   int Pri[CHAIR_CARDS];

   XygInitChairCards(CardID,CHAIR_CARDS);
   XygInitChairCards(nCardIndex,TOTAL_CARDS);
   memset(Pri,0,sizeof(Pri));
   int index=0;
   POINT st={10000,0};
  
   if (dwFlags==SORT_FLAGS_COMMON)
   {
	   for(int i=0;i<TOTAL_CARDS;i++)
	   {
		   if (theApp.GetChairNO()==m_Card[i]->m_nChairNO
			   &&m_Card[i]->m_nStatus==CARD_STATUS_INHAND)
		   {
			   CardID[index]=m_Card[i]->m_nCardID;
			   nCardIndex[index]=i;
			   if (CardID[index]!=-1)
				   Pri[index]=SK_GetCardPRIEx(CardID[index],GetGameInfoEx()->GetCurrentRank())*10+SK_GetCardShapeEx(CardID[index]);
			   else
				   Pri[index]=0;
			   
			   if (m_Card[i]->m_Position.x<st.x)
			   {
				   st.x=m_Card[i]->m_Position.x;
				   st.y=m_Card[i]->m_Position.y;
			   }
			   index++;
		   }
	   }
	   XygReversalMoreByValue(nCardIndex,Pri,index);

	   
	   for(i=0;i<index;i++)
	   {
		   if (nCardIndex[i]!=-1)
		   {
			   CGameCard* card=m_Card[nCardIndex[i]];
			   if (!card) continue;
			   card->m_nUnitIndex=i;
			   card->m_Position.x=st.x+i*card->GetCardSpace();
			   card->m_Position.y=st.y;
			   card->Fresh();
		   }
		}
   }
   else if (dwFlags==SORT_FLAGS_BIG)
   {
	   for(int i=0;i<TOTAL_CARDS;i++)
	   {
		   if (theApp.GetChairNO()==m_Card[i]->m_nChairNO
			   &&m_Card[i]->m_nStatus==CARD_STATUS_INHAND)
		   {
			   CardID[index]=m_Card[i]->m_nCardID;
			   nCardIndex[index]=i;
			   if (CardID[index]!=-1)
				   Pri[index]=SK_GetCardPRIEx(CardID[index],GetGameInfoEx()->GetCurrentRank())*10+SK_GetCardShapeEx(CardID[index]);
			   else
				   Pri[index]=0;
			   
			   if (m_Card[i]->m_Position.x<st.x)
			   {
				   st.x=m_Card[i]->m_Position.x;
				   st.y=m_Card[i]->m_Position.y;
			   }
			   index++;
		   }
	   }
	   XygReversalByValue(nCardIndex,Pri,index);
	   
	   
	   for(i=0;i<index;i++)
	   {
		   if (nCardIndex[i]!=-1)
		   {
			   CGameCard* card=m_Card[nCardIndex[i]];
			   if (!card) continue;
			   card->m_nUnitIndex=i;
			   card->m_Position.x=st.x+i*card->GetCardSpace();
			   card->m_Position.y=st.y;
			   card->Fresh();
		   }
	   }
   }
   else if (dwFlags==SORT_FLAGS_SHPAE)
   {
	   for(int i=0;i<TOTAL_CARDS;i++)
	   {
		   if (theApp.GetChairNO()==m_Card[i]->m_nChairNO
			   &&m_Card[i]->m_nStatus==CARD_STATUS_INHAND)
		   {
			   CardID[index]=m_Card[i]->m_nCardID;
			   nCardIndex[index]=i;
			   if (CardID[index]!=-1)
				   Pri[index]=SK_GetCardShapeEx(CardID[index])*1000
							  +SK_GetCardPRIEx(CardID[index],GetGameInfoEx()->GetCurrentRank());
			   else
				   Pri[index]=0;
			   
			   if (m_Card[i]->m_Position.x<st.x)
			   {
				   st.x=m_Card[i]->m_Position.x;
				   st.y=m_Card[i]->m_Position.y;
			   }
			   index++;
		   }
	   }
	   XygReversalMoreByValue(nCardIndex,Pri,index);
	   	   
	   for(i=0;i<index;i++)
	   {
		   if (nCardIndex[i]!=-1)
		   {
			   CGameCard* card=m_Card[nCardIndex[i]];
			   if (!card) continue;
			   card->m_nUnitIndex=i;
			   card->m_Position.x=st.x+i*card->GetCardSpace();
			   card->m_Position.y=st.y;
			   card->Fresh();
		   }
	   }
   }
   else if (dwFlags==SORT_FLAGS_COUNT
	   ||dwFlags==SORT_FLAGS_COUNT_MORE)
   {
	   int lay[LAYOUT_NUM];
	   memset(lay,0,sizeof(lay));
	   RUL_GetInHandLays(lay,LAYOUT_NUM);

	   for(int i=0;i<TOTAL_CARDS;i++)
	   {
		   if (theApp.GetChairNO()==m_Card[i]->m_nChairNO
			   &&m_Card[i]->m_nStatus==CARD_STATUS_INHAND)
		   {
			   CardID[index]=m_Card[i]->m_nCardID;
			   nCardIndex[index]=i;
			   int nCI=SK_GetCardIndexEx(m_Card[i]->m_nCardID);
			   Pri[index]=lay[nCI]*100000+SK_GetCardPRIEx(CardID[index],GetGameInfoEx()->GetCurrentRank())*10+SK_GetCardShapeEx(CardID[index]);
			   
			   if (m_Card[i]->m_Position.x<st.x)
			   {
				   st.x=m_Card[i]->m_Position.x;
				   st.y=m_Card[i]->m_Position.y;
			   }
			   index++;
		   }
	   }
	   if (dwFlags==SORT_FLAGS_COUNT_MORE)
			XygReversalMoreByValue(nCardIndex,Pri,index);
	   else
		    XygReversalByValue(nCardIndex,Pri,index);
	   
	   for(i=0;i<index;i++)
	   {
		   if (nCardIndex[i]!=-1)
		   {
			   CGameCard* card=m_Card[nCardIndex[i]];
			   if (!card) continue;
			   card->m_nUnitIndex=i;
			   card->m_Position.x=st.x+i*card->GetCardSpace();
			   card->m_Position.y=st.y;
			   card->Fresh();
		   }
	   }
   }
   else
   {
	   for(int i=0;i<TOTAL_CARDS;i++)
	   {
		   if (theApp.GetChairNO()==m_Card[i]->m_nChairNO
			   &&m_Card[i]->m_nStatus==CARD_STATUS_INHAND)
		   {
			   CardID[index]=m_Card[i]->m_nCardID;
			   nCardIndex[index]=i;
			   Pri[index]=m_Card[i]->m_nUnitIndex;
			   
			   if (m_Card[i]->m_Position.x<st.x)
			   {
				   st.x=m_Card[i]->m_Position.x;
				   st.y=m_Card[i]->m_Position.y;
			   }
			   index++;
		   }
	   }
	   XygReversalByValue(nCardIndex,Pri,index);


	   for(i=0;i<index;i++)
	   {
		   if (nCardIndex[i]!=-1)
		   {
			   CGameCard* card=m_Card[nCardIndex[i]];
			   if (!card) continue;
			   card->m_nUnitIndex=i;
			   card->m_Position.x=st.x+i*card->GetCardSpace();
			   card->m_Position.y=st.y;
			   card->Fresh();
		   }
		}
   }
}

void  CMyGame::OPE_LeftUp()
{		
	int nUnSelectCount=0;
	int nSelectCount=0;
	
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		if (m_Card[i]->m_nChairNO==theApp.GetChairNO()
			&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND
			&&m_Card[i]->bnFastSelect==TRUE)
		{
			if (m_Card[i]->bnSelect)
				nSelectCount++;
			else
				nUnSelectCount++;
		}
	}

	if (nSelectCount>0&&nUnSelectCount==0)//选中的都是选起来的牌，那么放下
	{
		for(int i=0;i<TOTAL_CARDS;i++)
		{
			if (m_Card[i]->m_nChairNO==theApp.GetChairNO()
				&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND
				&&m_Card[i]->bnFastSelect==TRUE)
			{
				m_Card[i]->UnSelect();
			}
		}
	}
	else
	{
		int nFastCount=0;
		int nTotalSelectCount=0;
		for(int i=0;i<TOTAL_CARDS;i++)
		{
			if (m_Card[i]->m_nChairNO==theApp.GetChairNO()
				&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND)
			{
				if (m_Card[i]->bnFastSelect==TRUE)
				{
					m_Card[i]->FastSelect(); //点起
					nFastCount++;
				}
				
				if (m_Card[i]->bnSelect==TRUE)
				{
					nTotalSelectCount++;
				}
			}
		}
		
		BOOL bnAssit=OPE_AssitSelect();
		//检查快速选牌中有没有顺子
		if (nFastCount>1&&!bnAssit&&nTotalSelectCount==nFastCount)
		{//只有所有弹起的牌皆为快速拖动的牌时做智能提示
			int nSelectCardID[SK_CHAIR_CARD];
			XygInitChairCards(nSelectCardID,SK_CHAIR_CARD);
			int nCount=RUL_GetSeletCards(nSelectCardID);
						
			if (nCount>=5)
			{
				UNITE_TYPE temp,best;
				int nOutCard[SK_CHAIR_CARD];
				memset(&temp,0,sizeof(UNITE_TYPE));
				memset(&best,0,sizeof(UNITE_TYPE));
				XygInitChairCards(nOutCard,SK_CHAIR_CARD);

				int nIndexCount=0;
				if (GetPublicInfoEx()->nWaitChair==-1)
				{
					CGameTable table;
					CARD_UNITE details;
					ZeroMemory(&details,sizeof(CARD_UNITE));
					if (!table.GetUniteDetails(theApp.GetChairNO(),nSelectCardID,SK_CHAIR_CARD,details,CARD_UNITE_TYPE_TOTAL_EX))
					{
						DWORD dwCardType[6]={CARD_UNITE_TYPE_ABT_THREE_COUPLE,CARD_UNITE_TYPE_ABT_THREE_1,
							CARD_UNITE_TYPE_ABT_THREE,CARD_UNITE_TYPE_THREE_COUPLE,
							CARD_UNITE_TYPE_ABT_COUPLE,CARD_UNITE_TYPE_ABT_SINGLE};
						
						for(int i=0;i<6;i++)
						{
							XygInitChairCards(nOutCard,SK_CHAIR_CARD);
							memset(&temp,0,sizeof(UNITE_TYPE));
							if (OPE_BulidCardEx(nSelectCardID,nCount,nOutCard,SK_CHAIR_CARD,temp,temp,dwCardType[i],FALSE))
							{
								int c=RUL_GetIndexCount(nOutCard, SK_CHAIR_CARD)*1000+temp.nCardCount;
								if (c>nIndexCount)
								{
									nIndexCount=c;
									memcpy(&best,&temp,sizeof(UNITE_TYPE));
								}
							}
						}
						if (best.dwCardType!=0)
						{
							OPE_UnSelectCard();
							OPE_SelectCard(best.nCardIDs,best.nCardCount);
						}
					}
				}
				else if (GetPublicInfoEx()->WaitCardUnite.nCardCount>=4)
				{
					if (OPE_BulidCard(nSelectCardID,nCount,nOutCard,SK_CHAIR_CARD,GetPublicInfoEx()->WaitCardUnite,temp,GetPublicInfoEx()->WaitCardUnite.dwCardType,FALSE))
					{
						OPE_UnSelectCard();
						OPE_SelectCard(temp.nCardIDs,temp.nCardCount);
					}
				}
			}
		}
	}
	PlaySound("Snd_HitCard");
	OPE_CheckSelect();
}

void    CMyGame::OPE_ShowGameInfo()
{
	if (!m_plGameInfo)
	{
		m_plGameInfo=CreatePlane(250,150);
		m_plGameInfo->TransGUI();
		m_plGameInfo->Updata();

		PGL_MSG_CLASS(m_digitBase,CXyDigit);
		m_digitBase->CreateDigit("digit_basescore",DIGIT_FLAGS_USE_PIC_SIZE);
		if (GetGameWinApp()->IsPlayDeposit())
			m_digitBase->LoadWord("两",Res_GetPICStruct("基础银_两"));
		else
			m_digitBase->LoadWord("分",Res_GetPICStruct("str_分"));
		m_digitBase->Updata();

		PGL_MSG_CLASS(m_AuctionScore,CXyDigit);
		m_AuctionScore->CreateDigit("digit_basescore",DIGIT_FLAGS_USE_PIC_SIZE);
		m_AuctionScore->LoadWord("分",Res_GetPICStruct("str_分"));
		m_AuctionScore->Updata();

		PGL_MSG_CLASS(m_BombFanScore,CXyDigit);
		m_BombFanScore->CreateDigit("digit_basescore",DIGIT_FLAGS_USE_PIC_SIZE);
		m_BombFanScore->LoadWord("倍",Res_GetPICStruct("倍数_倍"));
		m_BombFanScore->Updata();

		// Big UI
		PGL_MSG_CLASS(m_digitBaseBig,CXyDigit);
		m_digitBaseBig->CreateDigit("放大版_基础分数",DIGIT_FLAGS_USE_PIC_SIZE);
		if (GetGameWinApp()->IsPlayDeposit())
			m_digitBaseBig->LoadWord("两",Res_GetPICStruct("放大版_基础银_两"));
		else
			m_digitBaseBig->LoadWord("分",Res_GetPICStruct("放大版_文字_分"));
		m_digitBaseBig->Updata();
		
		PGL_MSG_CLASS(m_AuctionScoreBig,CXyDigit);
		m_AuctionScoreBig->CreateDigit("放大版_基础分数",DIGIT_FLAGS_USE_PIC_SIZE);
		m_AuctionScoreBig->LoadWord("分",Res_GetPICStruct("放大版_文字_分"));
		m_AuctionScoreBig->Updata();
		
		PGL_MSG_CLASS(m_BombFanScoreBig,CXyDigit);
		m_BombFanScoreBig->CreateDigit("放大版_基础分数",DIGIT_FLAGS_USE_PIC_SIZE);
		m_BombFanScoreBig->LoadWord("倍",Res_GetPICStruct("放大版_倍数_倍"));
		m_BombFanScoreBig->Updata();
	}

	/////////////////////////////////////////////////////////////
	m_plGameInfo->Erasure();
	SNG_PIC picBasescore;
	CString strBaseScore;
	int nX=10;
	int nY=10;
	int nH = 25;
	if (!theApp.IsUseBigUI())
		nH = 20;
		
	//底分
	if (GetGameInfoEx()->GetGameStartInfo()->nObjectGains>0)
	{
		if (theApp.IsUseBigUI())
		{
			picBasescore=Res_GetPICStruct("放大版_底分");
			m_plGameInfo->DrawPic(picBasescore,nX,nY+nH,picBasescore.width,picBasescore.height);
			strBaseScore.Format("%d分",GetGameInfoEx()->GetGameStartInfo()->nObjectGains);
			m_AuctionScoreBig->WriteString(m_plGameInfo,nX+78,nY+nH+2,strBaseScore,DIGIT_FLAGS_LEFT_JUSITIFY);
		}
		else
		{
			picBasescore=Res_GetPICStruct("底分");
			m_plGameInfo->DrawPic(picBasescore,nX,nY+nH,picBasescore.width,picBasescore.height);
			strBaseScore.Format("%d分",GetGameInfoEx()->GetGameStartInfo()->nObjectGains);
			m_AuctionScore->WriteString(m_plGameInfo,nX+65,nY+nH+2,strBaseScore,DIGIT_FLAGS_LEFT_JUSITIFY);
		}
	}

	//倍数
	int nBombFan = GetPublicInfoEx()->nBombFan;
	if (nBombFan>0)
	{
		if (theApp.IsUseBigUI())
		{
			picBasescore=Res_GetPICStruct("放大版_文字_倍数");
			m_plGameInfo->DrawPic(picBasescore,nX,nY+nH*2,picBasescore.width,picBasescore.height);
			strBaseScore.Format("%d倍",UwlPow2(nBombFan));
			m_BombFanScoreBig->WriteString(m_plGameInfo,nX+78,nY+nH*2+2,strBaseScore,DIGIT_FLAGS_LEFT_JUSITIFY);
		}
		else
		{
			picBasescore=Res_GetPICStruct("倍数");
			m_plGameInfo->DrawPic(picBasescore,nX,nY+nH*2,picBasescore.width,picBasescore.height);
			strBaseScore.Format("%d倍",UwlPow2(nBombFan));
			m_BombFanScore->WriteString(m_plGameInfo,nX+65,nY+nH*2+2,strBaseScore,DIGIT_FLAGS_LEFT_JUSITIFY);
		}
	}

	//////////////////////////////////////////////////////////////
	RECT rect=GetGameRect();
	int x=rect.left;
	int y=rect.top;
	if (theApp.IsUseBigUI())
		m_plGameInfo->MoveTo(x+m_plGameInfo->GetWidth()/2+10,y+m_plGameInfo->GetHeight()/2+10);
	else
		m_plGameInfo->MoveTo(x+m_plGameInfo->GetWidth()/2,y+m_plGameInfo->GetHeight()/2);
	
	m_plGameInfo->Updata();

	{
		m_plGameInfo->Show();
		m_plGameInfo->Updata();
	}
}

void   CMyGame::OPE_ShowAssistantButton(BOOL bn)
{
	//旁观下，只显示请求看牌
	if (GetGameWinApp()->IsLookOn())
	{
		//PGL_SHOW_OBJECT(m_btnAskKanPai);
		return;
	}

	if (bn)
	{
		m_btnAutoPlay->Show();
		m_btnAutoPlay->Updata();
		m_btnSort->Show();
		m_btnSort->Updata();	

		if (m_btnCardMaster)
		{
			m_btnCardMaster->Show();
			m_btnCardMaster->Updata();
		}
	}
	else
	{
		m_btnSort->Hide();
		m_btnSort->Updata();	
		m_btnAutoPlay->Hide();
		m_btnAutoPlay->Updata();
		if (m_btnCardMaster)
		{
			m_btnCardMaster->Hide();
			m_btnCardMaster->Updata();
		}
	}
}

void CMyGame::OPE_ShowBankerFlag(BOOL bShow,BOOL bAnimate)
{
	if (bShow && !m_bTongBi)
	{
		int nBanker=GetGameInfo()->GetBanker();		
		if (!IsInGame(nBanker))
			return;
		for(int i=0;i< TOTAL_CHAIRS;i++)
		{
			if (IsInGame(i))
			{		
				CMyPlayer* pPlayer = (CMyPlayer*)m_ptrPlayer[RUL_GetDrawIndexByChairNO(i)];
				if(pPlayer)
					pPlayer->SetBanker(i==nBanker,bAnimate);
			}
			else
			{
				CMyPlayer* pPlayer = (CMyPlayer*)m_ptrPlayer[RUL_GetDrawIndexByChairNO(i)];
				if(pPlayer)
					pPlayer->SetBanker(FALSE);
			}
		}
	}
	else
	{
		for(int i =0;i<TOTAL_CHAIRS;i++)
		{
			CMyPlayer* pPlayer = (CMyPlayer*)m_ptrPlayer[i];
			if(pPlayer)
				pPlayer->SetBanker(FALSE);
		} 
	}
}

void CMyGame::OPE_ShowWaitBet()
{
#ifdef _OFFLINE_
	if (m_bitWaitBet)
	{
		FitWaitBetPostion();
		StopObject(m_bitWaitBet->GetHandle());

		BeginGroup();
		APPLY_DirectShow(m_bitWaitBet,TRUE);
		NextGroup();
		APPLY_PlayBitObject(m_bitWaitBet);
		EndGroup();
	}
	return;
#endif

	if (!IsBanker(theApp.GetChairNO()))
		return;
	
	if (IS_BIT_SET(GetGameInfo()->GetStatus(), TS_WAITING_YAZHU))
	{
		if (m_bitWaitBet)
		{
			m_bitWaitBet->Show();
			FitWaitBetPostion();
			StopObject(m_bitWaitBet->GetHandle());
			APPLY_PlayBitObject(m_bitWaitBet);
		}
	}
}

void CMyGame::OPE_HideWaitBet()
{
	if (m_bitWaitBet)
	{
		StopObject(m_bitWaitBet->GetHandle());
		m_bitWaitBet->Hide();
		m_bitWaitBet->Updata();
	}
}

void CMyGame::FitWaitBetPostion()
{
	if (m_bitWaitBet)
	{
		if (IsBanker(theApp.GetChairNO()))
		{
			RECT rect=GetGameRect();
			int x=rect.left;
			int y=rect.top;
			int w=rect.right-rect.left;
			int h=rect.bottom-rect.top;
			
			CMyPlayer* player=(CMyPlayer*)m_ptrPlayer[0];
			POINT pt=GetClockPos();
			SNG_PIC picC=Res_GetPICStruct("clock_base");
			pt.y+=picC.height/2+m_bitWaitBet->GetHeight()/2+50;
			m_bitWaitBet->MoveTo(x+w/2+15, pt.y);
			m_bitWaitBet->Updata();
		}
		else
		{
			m_bitWaitBet->MoveTo(0,0);
			m_bitWaitBet->Updata();
		}
	}
}

void CMyGame::OPE_ShowSanGongType(int nChairNO, int nCardType, BOOL bSound)
{
	int nIndex = RUL_GetDrawIndexByChairNO(nChairNO);
	m_bitPaiXing[nIndex]->LoadPic(Res_GetPICStruct("牌型", nCardType));
	m_bitPaiXing[nIndex]->Show();
	m_bitPaiXing[nIndex]->Updata();
	FitPaiXing(nIndex);

	CGameWinApp* pGameApp=GetGameWinApp();
	GAMEPLAYER player=pGameApp->m_GamePlayer[nChairNO];
	if(bSound)
	{
		CString strSoundFileName;
		strSoundFileName.Format("Point_%d_%d",player.nNickSex,nCardType);
		GetMusicModule()->PlaySound(strSoundFileName);
	}	
}


void   CMyGame::OPE_AddBomb(CARDS_THROW* pCardsThrow)
{
	if (pCardsThrow->unite.dwCardType==CARD_UNITE_TYPE_BOMB
		|| pCardsThrow->unite.dwCardType==CARD_UNITE_TYPE_2KING)
	{
		GetPlayerInfoEx(pCardsThrow->nChairNO)->nBombCount++;

		//reserved
//		PlaySound("Snd_flower");
		int nDrawIndex=RUL_GetDrawIndexByChairNO(pCardsThrow->nChairNO);
		((CMyPlayer*)m_ptrPlayer[nDrawIndex])->FreshBomb(GetPlayerInfoEx(pCardsThrow->nChairNO)->nBombCount);

		//更新倍数
		GetPublicInfoEx()->nBombFan++;
		OPE_ShowGameInfo();
	}
}

void  CMyGame::OPE_DXXW()
{
	int nChariNO = theApp.GetChairNO();

	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		if (GetGameInfoEx()->IsGamePlaying()&&!GetGameInfoEx()->GetGameStartInfo()->bInGame[i]&& GetGameWinApp()->IsPlayerEnter(i))
		{
			PGL_SHOW_OBJECT(m_bitWaitNextBout[RUL_GetDrawIndexByChairNO(i)]);
		}
		else
		{
			PGL_HIDE_OBJECT(m_bitWaitNextBout[RUL_GetDrawIndexByChairNO(i)]);
		}
		memcpy(m_szUsername[i], GetGameWinApp()->m_GamePlayer[i].szUsername, sizeof(m_szUsername[i]));
	}
	
	for ( i = 0; i < TOTAL_CHAIRS; i++)
	{
		int nDrawIndex = RUL_GetDrawIndexByChairNO(i);
		if (m_ptrPlayer[nDrawIndex])
		{
			((CMyPlayer*)m_ptrPlayer[nDrawIndex])->HidePlayerCloth();
		}
	}

	if (!theApp.IsLookOn())
	{
		REQ_SendMsgToServer(SYSMSG_PLAYER_ONLINE,0,NULL,FALSE);
		theApp.GetGameInfo()->m_nAskExitCount = GetPlayerInfoEx(theApp.GetChairNO())->nAskExitCount;
	}
	
	int count = 0;
	int myindex = 0;

	for(i=0;i<TOTAL_CARDS;i++)
	{
		m_Card[i]->Reset();
		m_Card[i]->m_nChairNO=GetPublicInfoEx()->GameCard[i].nChairNO;
		m_Card[i]->m_nDrawIndex=RUL_GetDrawIndexByChairNO(m_Card[i]->m_nChairNO);
		m_Card[i]->m_nCardID=GetPublicInfoEx()->GameCard[i].nCardID;
		if (m_Card[i]->m_nCardID!=-1)
		{
			m_Card[i]->bnFace=TRUE;
		}
		m_Card[i]->m_nStatus=GetPublicInfoEx()->GameCard[i].nCardStatus;
		m_Card[i]->m_nUnitIndex=GetPublicInfoEx()->GameCard[i].nPositionIndex;
		m_Card[i]->m_nUnitCount=GetPublicInfoEx()->GameCard[i].nUniteCount;	

		if(nChariNO == m_Card[i]->m_nChairNO)
		{
			if(m_Card[i]->m_nCardID != -1 && myindex < CHAIR_CARDS)
			{
				GetGameInfoEx()->GetGameStartInfo()->nHandID[myindex] = m_Card[i]->m_nCardID;
			    myindex++;
			}		
		}
	} 
				
	for(i=0;i<TOTAL_CHAIRS;i++)
	{
		if (!GetGameInfoEx()->GetGameStartInfo()->bInGame[theApp.GetChairNO()]) 
		{
			continue;
		}
		OPE_FreshPlayerCard(i);
		int nDrawIndex=RUL_GetDrawIndexByChairNO(i);
		((CMyPlayer*)m_ptrPlayer[nDrawIndex])->FreshBomb(GetPlayerInfoEx(i)->nBombCount);
	}



	PGL_HIDE_OBJECT(m_btnStart);

	OPE_StartTimer();

	OPE_HideButtons();
	OPE_HideWaitBet();
	OPE_HideQZButton();
	OPE_HideQZRemaind();
	OPE_HideYaZhuDlg();
	OPE_HideYZYinzi();
	OPE_HideYZYinziDigit();

	if (IS_BIT_SET(GetGameInfoEx()->GetStatus(), TS_PLAYING_GAME))
	{
		GetGameInfoEx()->ResetStatusWhenStart();
	}

	if (IS_BIT_SET(GetGameInfoEx()->GetStatus(), TS_WAITING_COUNTDOWN))
	{
		
	}

	if (IS_BIT_SET(GetGameInfoEx()->GetStatus(), TS_WAITING_QZ))
	{
		ShowHuanZhuoBtn(FALSE);
		int nLeft = GetGameInfoEx()->GetGameStartInfo()->nYaZhuWait - GetGameInfoEx()->GetGameStartInfo()->nReserved[0];
		GUI_MoveClock(nLeft);

		for(i = 0; i < TOTAL_CHAIRS; i++)
		{
			if(IsFinishQZ(i))
			{
				OPE_ShowQZ(i, GetQZFlag(i));
			}
		}

		if (IsInGame(theApp.GetChairNO()) && !GetGameWinApp()->IsLookOn())
		{
			if(!IsFinishQZ(theApp.GetChairNO()))
			{
				OPE_ShowQZButton();
			}		
		}
	}

	if (IS_BIT_SET(GetGameInfoEx()->GetStatus(), TS_WAITING_YAZHU))
	{
		ShowHuanZhuoBtn(FALSE);
		int nLeft = GetGameInfoEx()->GetGameStartInfo()->nYaZhuWait - GetGameInfoEx()->GetGameStartInfo()->nReserved[0];
		OPE_ShowBankerFlag();
		GUI_MoveClock(nLeft);

		if (!IsInGame(theApp.GetChairNO()))
		{

		}
		else if (IsBanker(theApp.GetChairNO()))
		{
			OPE_ShowWaitBet();
		}
		else
		{
			if (GetPublicInfoEx()->bFinishYaZhu[theApp.GetChairNO()])
			{
				
			}
			else
			{
				if(!GetGameWinApp()->IsLookOn())
				{
					OPE_ShowYaZhuDlg();
				}
			}
		}
		for(int i=0;i<TOTAL_CHAIRS;i++)
		{
			if(!IsInGame(i) || IsBanker(i))
				continue;
			if(GetPublicInfoEx()->bFinishYaZhu[i])
			{
				int nDbl = GetPublicInfoEx()->nYaZhuDbl[i];
				OPE_ShowYZYinzi(i, nDbl);
			}
		}
	}

	for(i=0;i<TOTAL_CARDS;i++)
	{
		m_Card[i]->Fresh();
	}

	if (IS_BIT_SET(GetGameInfoEx()->GetGameStartInfo()->dwStatus,TS_WAITING_ZUPAI))
	{//如果是组牌阶段
		int nLeft = GetGameInfoEx()->GetGameStartInfo()->nThrowWait - GetGameInfoEx()->GetGameStartInfo()->nReserved[0];
		
		ShowHuanZhuoBtn(FALSE);
		OPE_ShowBankerFlag();
		OPE_SortHand();
		GUI_MoveClock(nLeft);
		
		for(int i = 0; i < TOTAL_CHAIRS; ++i)
		{
			if(!IsInGame(i) || IsBanker(i))
				continue;
			if(GetPublicInfoEx()->bFinishYaZhu[i])
			{
				int nDbl = GetPublicInfoEx()->nYaZhuDbl[i];
				OPE_ShowYZYinzi(i, nDbl);
			}
		}
		
		for (i=0;i<TOTAL_CHAIRS;i++)
		{
			if (!GetGameInfoEx()->GetGameStartInfo()->bInGame[i]) 
				continue;
			
			if (GetPublicInfoEx()->bFinishZuPai[i])
			{
				OPE_ShowSanGongType(i, GetPlayerInfoEx(i)->stDetail.nCardType, FALSE);
				
				if (i == theApp.GetChairNO())
				{//如果我完成
					m_btnSureZuPai->Hide();
					m_btnSureZuPai->Updata();
				}
			}
			else
			{
				if (i == theApp.GetChairNO()&&!theApp.IsLookOn())
				{//如果我没完成
					FitZupaiBtnPos();
					m_btnSureZuPai->Show();
					m_btnSureZuPai->Enable(TRUE);
					m_btnSureZuPai->Updata();
				}
			}
		}
		//显示基础银框
		OPE_ShowBaseDeposit();
	}

	for(i=0;i<TOTAL_CHAIRS;i++)
	{
		if (!GetGameInfoEx()->GetGameStartInfo()->bInGame[i]) 
		{
			continue;
		}
		int nDrawIndex=RUL_GetDrawIndexByChairNO(i);
		//断线状态
		if (i!=theApp.GetChairNO()||theApp.IsLookOn())
		{
			DWORD dwPlayerStatus = GetGameInfoEx()->GetGameTableInfo()->m_Public.dwUserStatus[i];
			if (IS_BIT_SET(dwPlayerStatus, US_USER_OFFLINE)|IS_BIT_SET(dwPlayerStatus,US_USER_QUIT))
			{
				m_ptrPlayer[nDrawIndex]->Offline(TRUE);
			}
		}
	}


	//加载积分榜
	memcpy(theApp.m_nResultDiff, GetPublicInfoEx()->nResultDiff, sizeof(GetPublicInfoEx()->nResultDiff));
	memcpy(theApp.m_nResultTotal,GetPublicInfoEx()->nTotalResult,sizeof(GetPublicInfoEx()->nTotalResult));
	theApp.m_nResultCount = GetGameInfoEx()->GetBoutCount()-1;
	theApp.UpdataResultList();

	//二维码充值start
    CGameRequest* pGameConnect = (CGameRequest*)theApp.GetGameConnect();
    if (pGameConnect)
    {
        pGameConnect->GC_RoomLimitInfo();//获取房间额度信息
    }
    //二维码充值end
}

void CMyGame::OPE_ShowButtons()
{
	if (GetGameInfo()->GetCurrentChairNO()==theApp.GetChairNO()
		&&!GetGameWinApp()->IsLookOn())
	{
		xyAppCallAttention();//提醒用户操作

		m_btnThrow->Show();
		m_btnThrow->Enable(FALSE);
		m_btnThrow->Updata();
		if (GetPublicInfoEx()->nWaitChair!=-1)
		{
			m_btnPass->Show();
			m_btnReMaind->Show();
		}
		else
		{
			m_btnPass->Hide();
			m_btnReMaind->Hide();
		}
		OPE_CheckSelect();

		if (GetPublicInfoEx()->nWaitChair==-1)
		{
			OPE_ClearTable();
			m_btnPass->Enable(FALSE);
			m_btnReMaind->Enable(FALSE);
		}
		else
		{
			int nRemind[CHAIR_CARDS];
			int nInHand[CHAIR_CARDS];
			XygInitChairCards(nRemind,CHAIR_CARDS);
			XygInitChairCards(nInHand,CHAIR_CARDS);
			int count=RUL_GetInHandCards(nInHand);
			if (count>0
				&&!OPE_BulidCard(nInHand,count,nRemind,CHAIR_CARDS,GetPublicInfoEx()->WaitCardUnite,m_ReMindUniteType,CARD_UNITE_TYPE_TOTAL_EX))
			{
				if (count==1
					&&count<GetPublicInfoEx()->WaitCardUnite.nCardCount)
				{
					APPLY_WaitFewTimes(1000,&(SNG_MSG)CreateMsg(GetHandle(),OPE_CALLBACKMSG_AUTOPASS));
				}
				else if (count==1
					&&GetPublicInfoEx()->WaitCardUnite.dwCardType==CARD_UNITE_TYPE_SINGLE
					&&SK_GetCardIndexEx(GetPublicInfoEx()->WaitCardUnite.nCardIDs[0])==15)
				{
					//手牌小于3张，并且首家出了大牌
					APPLY_WaitFewTimes(1000,&(SNG_MSG)CreateMsg(GetHandle(),OPE_CALLBACKMSG_AUTOPASS));
				}
				else if (GetPublicInfoEx()->WaitCardUnite.dwCardType==CARD_UNITE_TYPE_2KING)
				{
					//大小王
					APPLY_WaitFewTimes(2000,&(SNG_MSG)CreateMsg(GetHandle(),OPE_CALLBACKMSG_AUTOPASS));
				}
				else
					MOVIE_CanNotThrow();
			}
			
			memset(&m_ReMindUniteType,0,sizeof(m_ReMindUniteType));
			memset(&m_BestReMindUniteType,0,sizeof(m_BestReMindUniteType));
			m_btnPass->Enable(TRUE);
			m_btnReMaind->Enable(TRUE);
		}
		
//		m_btnPass->Show();
//		m_btnReMaind->Show();
		FitButtonPosition();
	}

	//检查当前弹起来的牌
	
}

BOOL   CMyGame::OPE_AssitSelect()
{
	int nInHand[CHAIR_CARDS];
	int nInHandLay[LAYOUT_NUM];
	int nSelectCardID[CHAIR_CARDS];
	int Selectlay[LAYOUT_NUM];
	memset(nInHandLay,0,sizeof(nInHandLay));
	memset(Selectlay,0,sizeof(Selectlay));
	XygInitChairCards(nSelectCardID,CHAIR_CARDS);
	XygInitChairCards(nInHand,CHAIR_CARDS);
	int nInHandCount=RUL_GetInHandCards(nInHand);
	SK_LayCards(nInHand,CHAIR_CARDS,nInHandLay,0);
	
	int nCount=RUL_GetSeletCards(nSelectCardID);
	SK_LayCards(nSelectCardID,CHAIR_CARDS,Selectlay,0);
	
	if (nCount==0)
		return FALSE;

	if (GetPublicInfoEx()->nWaitChair!=-1&&GetPublicInfoEx()->nWaitChair!=theApp.GetChairNO())
	{
		UNITE_TYPE single;
		memset(&single,0,sizeof(UNITE_TYPE));
		
		int tempLay[LAYOUT_NUM];
		memset(tempLay,0,sizeof(tempLay));
		
		int nOutCard[CHAIR_CARDS];
		XygInitChairCards(nOutCard,CHAIR_CARDS);

		//跟牌
		DWORD flags=0;
		DWORD dwSupport=CARD_UNITE_TYPE_TOTAL_EX;
		int nTypeCount=0;
		UNITE_TYPE find_unite;
		memset(&find_unite,0,sizeof(find_unite));

		for(int n=0;n<4;n++)
			for(int m=0;m<8;m++)
			{
				flags=pow(2,n)*(pow(16,m));
				if (IS_BIT_SET(dwSupport,flags))
				{
					memcpy(&single,&(GetPublicInfoEx()->WaitCardUnite),sizeof(UNITE_TYPE));
					while(OPE_BulidCard(nInHand,nInHandCount,nOutCard,CHAIR_CARDS,single,single,flags,FALSE))
					{
						memset(tempLay,0,sizeof(tempLay));
						SK_LayCards(nOutCard,CHAIR_CARDS,tempLay,0);
						if (XygContainCards(tempLay,Selectlay,LAYOUT_NUM))
						{
							nTypeCount++;
							memcpy(&find_unite,&single,sizeof(UNITE_TYPE));
						}
					}
				}

			}


			if (nTypeCount==1)
			{
				
				for(int i=0;i<nCount;i++)
				{
					//检查选中的牌是否有在输出列中
					BOOL bnMatch=FALSE;
					for(int j=0;j<find_unite.nCardCount;j++)
					{
						if (find_unite.nCardIDs[j]==nSelectCardID[i])
						{
							bnMatch=TRUE;
							break;
						}
					}
					
					if (!bnMatch)
					{
						//未能必配成功
						for(int k=0;k<find_unite.nCardCount;k++)
						{
							if (SK_GetCardIndexEx(nSelectCardID[i])==SK_GetCardIndexEx(find_unite.nCardIDs[k])
								&&!XygHaveCard(nSelectCardID,nCount,find_unite.nCardIDs[k]))
							{
								find_unite.nCardIDs[k]=nSelectCardID[i];
								break;
							}
						}
					}
				}
				OPE_SelectCard(find_unite.nCardIDs,find_unite.nCardCount);
				OPE_CheckSelect();
				return TRUE;
			}

	}

	return FALSE;

}


void  CMyGame::OPE_CheckSelect()
{
	if (theApp.IsLookOn()) 
		return;
	
	int nSelectCard[CHAIR_CARDS];
	XygInitChairCards(nSelectCard,CHAIR_CARDS);
	int nCount=0;
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		if (m_Card[i]->m_nStatus==CARD_STATUS_INHAND
			&&m_Card[i]->m_nChairNO==theApp.GetChairNO()
			&&m_Card[i]->bnSelect
			&&m_Card[i]->m_nCardID!=-1)
		{
			nSelectCard[nCount++]=m_Card[i]->m_nCardID;
		}
	}
	
	m_nLastSelectCount=nCount;
	//////////////////////////////////
	m_btnThrow->Enable(FALSE);
    /////////////////////////////////
	
	if (nCount==0)
	{
		m_nRemindType=0;
		return;
	}
	
	CGameTable table;
	CARD_UNITE details;
	ZeroMemory(&details,sizeof(CARD_UNITE));
	
	if (!table.GetUniteDetails(theApp.GetChairNO(),nSelectCard,CHAIR_CARDS,details,CARD_UNITE_TYPE_TOTAL_EX))
		return;
	
	if (GetPublicInfoEx()->nWaitChair==-1)
	{
		table.GetBestUnitType(details);
		m_btnThrow->Enable(TRUE);
	}
	else
	{
		if (table.GetBestUnitType(GetPublicInfoEx()->WaitCardUnite,details))
		{
			m_btnThrow->Enable(TRUE);
		}
		else
		{
			
		}
	}
	
}

void CMyGame::OPE_HideButtons()
{
	m_btnPass->Hide();
	m_btnPass->Updata();
	m_btnThrow->Hide();
	m_btnThrow->Updata();
	m_btnReMaind->Hide();
	m_btnReMaind->Updata();

	m_btnSureZuPai->Hide();
	m_btnSureZuPai->Updata();
}

BOOL CMyGame::OPE_BulidCard(int nInCards[],int nInCardLen,int nOutCards[],int nOutCardLen,UNITE_TYPE in_type,UNITE_TYPE& out_type,DWORD dwUniteSupport,BOOL bnUseJoker)
{
	CGameTable table;
	CARD_UNITE details;
	ZeroMemory(&details,sizeof(CARD_UNITE));
//	table.m_nCurrentRank=GetSKGameApp()->GetCurrentRank();

	if (!bnUseJoker)
        table.m_dwGameFlags&=~GAME_FLAGS_USE_JOKER;

	int lay[SK_LAYOUT_NUM];
	memset(lay,0,sizeof(lay));
	int nJokerCount=0;
	int nCardCount=table.PreDealCards(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount);

	DWORD flags=0;
	memcpy(&out_type,&in_type,sizeof(UNITE_TYPE));

	flags=COMPARE_UNITE_TYPE_SINGLE;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_SINGLE)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Single(nInCards,nInCardLen,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////

	flags=COMPARE_UNITE_TYPE_COUPLE;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_COUPLE)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Couple(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////

	flags=COMPARE_UNITE_TYPE_THREE;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_THREE)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Three(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////
	
	flags=COMPARE_UNITE_TYPE_THREE_1;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_THREE_1)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Three_1(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////

	flags=COMPARE_UNITE_TYPE_THREE_COUPLE;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_THREE_COUPLE)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Three_Couple(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////

	flags=COMPARE_UNITE_TYPE_ABT_SINGLE;
	for(int i=5;i<=12;i++)
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_ABT_SINGLE)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_ABT_Single(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}

////////////////////////////////////////////////////////////////////////////////////////

	flags=COMPARE_UNITE_TYPE_ABT_COUPLE;
	for(i=3;i<=12;i++)//最小2队，最多12连队
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_ABT_COUPLE)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_ABT_Couple(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}

////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_ABT_THREE;
	for(i=2;i<=12;i++)//最小2队，最多12连队
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_ABT_THREE)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_ABT_Three(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}
////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_ABT_THREE_1;
	for(i=2;i<=12;i++)//最小2队，最多12连队
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_ABT_THREE_1)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_ABT_Three_1(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}
////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_ABT_THREE_COUPLE;
	for(i=2;i<=12;i++)//最小2队，最多12连队
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_ABT_THREE_COUPLE)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_ABT_Three_Couple(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}
/////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_FOUR_2;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_FOUR_2)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Four_2(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}
/////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_FOUR_2_COUPLE;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_FOUR_2_COUPLE)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Four_2_Couple(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}
/////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_BOMB_EX;
	for(i=4;i<=8;i++)
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_BOMB)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_Bomb(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}
/////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_2KING;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_2KING)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_BOMB_2King(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////
    return FALSE;
}

BOOL CMyGame::OPE_BulidCardEx(int nInCards[],int nInCardLen,int nOutCards[],int nOutCardLen,UNITE_TYPE in_type,UNITE_TYPE& out_type,DWORD dwUniteSupport,BOOL bnUseJoker)
{
	CGameTable table;
	CARD_UNITE details;
	ZeroMemory(&details,sizeof(CARD_UNITE));
//	table.m_nCurrentRank=GetSKGameApp()->GetCurrentRank();

	if (!bnUseJoker)
        table.m_dwGameFlags&=~GAME_FLAGS_USE_JOKER;

	int lay[SK_LAYOUT_NUM];
	memset(lay,0,sizeof(lay));
	int nJokerCount=0;
	int nCardCount=table.PreDealCards(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount);

	DWORD flags=0;
	memcpy(&out_type,&in_type,sizeof(UNITE_TYPE));

///////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_SINGLE;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_SINGLE)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Single(nInCards,nInCardLen,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_COUPLE;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_COUPLE)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Couple(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_THREE;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_THREE)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Three(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}
////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_THREE_1;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_THREE_1)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Three_1(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}
////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_THREE_COUPLE;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_THREE_COUPLE)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Three_Couple(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_ABT_SINGLE;
	for(int i=12;i>=5;i--)
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_ABT_SINGLE)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_ABT_Single(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}

////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_ABT_COUPLE;
	for(i=12;i>=3;i--)//最小2队，最多12连队
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_ABT_COUPLE)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_ABT_Couple(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}

////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_ABT_THREE;
	for(i=12;i>=2;i--)//最小2队，最多12连队
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_ABT_THREE)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_ABT_Three(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}
////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_ABT_THREE_1;
	for(i=12;i>=2;i--)//最小2队，最多12连队
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_ABT_THREE_1)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_ABT_Three_1(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}
////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_ABT_THREE_COUPLE;
	for(i=12;i>=2;i--)//最小2队，最多12连队
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_ABT_THREE_COUPLE)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_ABT_Three_Couple(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}
////////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_FOUR_2;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_FOUR_2)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Four_2(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}
/////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_FOUR_2_COUPLE;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_FOUR_2_COUPLE)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_Four_2_Couple(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}
/////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_BOMB_EX;
	for(i=8;i>=4;i--)
	{
		if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_BOMB)
			&&IS_BIT_SET(flags,in_type.dwCardType)
			&&table.GetCardType_Bomb(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type,i))
		{
			memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
			return TRUE;
		}
	}
/////////////////////////////////////////////////////////////////////////////////////
	flags=COMPARE_UNITE_TYPE_2KING;
	if (IS_BIT_SET(dwUniteSupport,CARD_UNITE_TYPE_2KING)
		&&IS_BIT_SET(flags,in_type.dwCardType)
		&&table.GetCardType_BOMB_2King(nInCards,nInCardLen,lay,SK_LAYOUT_NUM,nJokerCount,out_type))
	{
		memcpy(nOutCards,out_type.nCardIDs,out_type.nCardCount*sizeof(int));
		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////
    return FALSE;
}
void   CMyGame::OPE_BulidResult()
{
	if (!m_plResultDlg)
	{
		SNG_PIC pic=Res_GetPICStruct("游戏结果框");
		
		m_plResultDlg=CreatePlane(pic.width,pic.height);
		m_plResultDlg->TransGUI();
		m_plResultDlg->SetFloor(FLOOR_RESULT);
		m_plResultDlg->Updata();
		m_plResultDlg->SetAutoMsg(MSG_INPUT_MOUSE_LHIT_MOVE,CreateMsg(GetHandle(),GAME_CALLBACKMSG_MOVERESULT));
//		m_plResultDlg->SetAutoMsg(MSG_INPUT_MOUSE_RIGHT_DOWN,CreateMsg(GetHandle(),BTN_CALLBACKMSG_CLOSERESULT));

// 		for(int i=0;i<TOTAL_CHAIRS;i++)
// 		{
// 			m_plResultHead[i]=CreatePlane(32,32);
// 			m_plResultHead[i]->SetFloor(m_plResultDlg->GetSurfaceZ());
// 			m_plResultHead[i]->SetAutoMsg(MSG_INPUT_TOUCH,CreateMsg(GetHandle(),GAME_CALLBACKMSG_TOUCH_RESULTHEAD));
// 			m_plResultHead[i]->SetAutoMsg(MSG_INPUT_UNTOUCH,CreateMsg(GetHandle(),GAME_CALLBACKMSG_UNTOUCH_RESULTHEAD));
// 			m_plResultDlg->AddChild(m_plResultHead[i],OBJECT_LOCK_ALL);
// 		}

		for (int i=0;i<TOTAL_CHAIRS;i++)
		{
			m_bitActor[i]=CreateBitMap(Res_GetPICStruct("农民笑"));
			m_bitActor[i]->SetFloor(m_plResultDlg->GetSurfaceZ());
			m_plResultDlg->AddChild(m_bitActor[i],OBJECT_LOCK_ALL);
		}

		if (theApp.IsLookOn())
		{
			CXyStandButton* btnOK=NULL;
			PGL_MSG_CLASS(btnOK,CXyStandButton);
			btnOK->CreateGUI(Res_GetPICStruct("结果框确定"),&(SNG_MSG)CreateMsg(GetHandle(),BTN_CALLBACKMSG_CLOSERESULT));
			btnOK->MoveTo(m_plResultDlg->GetX(),m_plResultDlg->GetTop()+267);
			btnOK->SetFloor(m_plResultDlg->GetSurfaceZ());
			btnOK->SetButtonSound(BUTTON_STATUS_TOUCH,"SndTouchButton");
			btnOK->SetButtonSound(BUTTON_STATUS_LEFT_DOWN,"SndHitButton");
			m_plResultDlg->AddChild(btnOK,OBJECT_LOCK_ALL);			
		}
		else
		{
			PGL_MSG_CLASS(m_btnStartNextBout,CXyStandButton);
			m_btnStartNextBout->CreateGUI(Res_GetPICStruct("结果框继续游戏"),&(SNG_MSG)CreateMsg(GetHandle(),BTN_CALLBACKMSG_STARTNEXTBOUT));
			m_btnStartNextBout->MoveTo(m_plResultDlg->GetX(),m_plResultDlg->GetTop()+267);
			m_btnStartNextBout->SetFloor(m_plResultDlg->GetSurfaceZ());
			m_btnStartNextBout->SetButtonSound(BUTTON_STATUS_TOUCH,"SndTouchButton");
			m_btnStartNextBout->SetButtonSound(BUTTON_STATUS_LEFT_DOWN,"SndHitButton");
			m_btnStartNextBout->Updata();
			m_plResultDlg->AddChild(m_btnStartNextBout,OBJECT_LOCK_ALL);
		}

		CXyStandButton* btnClose=NULL;
		PGL_MSG_CLASS(btnClose,CXyStandButton);
		btnClose->CreateGUI(Res_GetPICStruct("结算框关闭"),&(SNG_MSG)CreateMsg(GetHandle(),BTN_CALLBACKMSG_CLOSERESULT));
		btnClose->MoveTo(m_plResultDlg->GetLeft()+447,m_plResultDlg->GetTop()+19);
		btnClose->SetFloor(m_plResultDlg->GetSurfaceZ());
		btnClose->SetButtonSound(BUTTON_STATUS_TOUCH,"SndTouchButton");
		btnClose->SetButtonSound(BUTTON_STATUS_LEFT_DOWN,"SndHitButton");
		m_plResultDlg->AddChild(btnClose,OBJECT_LOCK_ALL);
	}

	if (!theApp.IsLookOn()&&m_btnStartNextBout)
	{
		m_btnStartNextBout->Enable(!GetGameWinApp()->IsGameRunning());
	}


	GAME_WIN_RESULT* pGameWin=GetGameInfoEx()->GetGameWin();
	
	m_plResultDlg->Erasure();

	if (GetGameWinApp()->IsPlayDeposit())
		m_plResultDlg->DrawPic(Res_GetPICStruct("游戏结果框"));
	else
		m_plResultDlg->DrawPic(Res_GetPICStruct("结算框没银子"));

// 	CXyDigit* pen=(CXyDigit*)GetObject("DigitPen");
	
	int nLineHeight=52;
	int nH = 92;
	CString str;
	int nBanker = GetGameInfoEx()->GetBanker();
	int nMyChair = theApp.GetChairNO();
	for(int i=0;i<m_nTotalPlayer;i++)
	{
//		int chairno=nPlaceChair[i];
// 		int nClothID=GetGameWinApp()->m_GamePlayer[i].nClothingID;
// 		CString szCharacterName=GetUserCharacter(nClothID,CHARACTER_TYPE_SMALL);
// 		SNG_PIC cloth_pic=Res_GetPICStruct(szCharacterName);

// 		m_plResultHead[i]->StopAutoMsg(MSG_INPUT_TOUCH);
// 		m_plResultHead[i]->SetAutoMsg(MSG_INPUT_TOUCH,CreateMsg(GetHandle(),GAME_CALLBACKMSG_TOUCH_RESULTHEAD,nClothID));
// 		m_plResultHead[i]->DrawPic(cloth_pic);
// 		m_plResultHead[i]->MoveTo(m_plResultDlg->GetLeft()+41+16,m_plResultDlg->GetTop()+nLineHeight*i+76+16);

		if (i==nBanker)
		{
			m_plResultDlg->SetTextSize(14,700);
			m_plResultDlg->SetTextColor(RGB(251,207,1));
		}
		else
		{
			m_plResultDlg->SetTextSize(14,400);
			m_plResultDlg->SetTextColor(RGB(224,236,217));
		}
		//姓名
		if (GetGameWinApp()->IsPlayDeposit())
			m_plResultDlg->TextOut(62,nLineHeight*i+86,GetGameWinApp()->GetUserNameByChairNO(i));
		else
			m_plResultDlg->TextOut(70,nLineHeight*i+86,GetGameWinApp()->GetUserNameByChairNO(i));

		int nGain=0;
		//银子、积分
		/////////////////////////////////////

		m_plResultDlg->SetTextSize(16,400);
		if (GetGameWinApp()->IsPlayDeposit())
		{
			nGain=GetGameInfoEx()->GetGameWin()->gamewin.nDepositDiffs[i];
			if (nGain>0)
			{
				str.Format("+%d",nGain);
				m_plResultDlg->TextOutEx(230,nLineHeight*i+nH,str);
			}
			else
			{
				str.Format("%d",nGain);
				m_plResultDlg->TextOutEx(230,nLineHeight*i+nH,str);					
			}
			int nScore=GetGameInfoEx()->GetGameWin()->gamewin.nScoreDiffs[i];
			if (nScore>0)
			{
				str.Format("+%d",nScore);
				m_plResultDlg->TextOutEx(324,nLineHeight*i+nH,str);
			}
			else
			{
				str.Format("%d",nScore);
				m_plResultDlg->TextOutEx(324,nLineHeight*i+nH,str);
			}
		}
		else
		{
			nGain=GetGameInfoEx()->GetGameWin()->gamewin.nScoreDiffs[i];
			if (nGain>0)
			{
				str.Format("+%d",nGain);
				m_plResultDlg->TextOutEx(280,nLineHeight*i+nH,str);
			}
			else
			{
				str.Format("%d",nGain);
				m_plResultDlg->TextOutEx(280,nLineHeight*i+nH,str);
			}
		}

		StopObject(m_bitActor[i]->GetHandle());
		if (nGain>0)
		{
			if (i==nBanker)
				m_bitActor[i]->LoadPic(Res_GetPICStruct("地主笑"));
			else
				m_bitActor[i]->LoadPic(Res_GetPICStruct("农民笑"));
			APPLY_PlayBitObject(m_bitActor[i],MODE_PLAYBIT_BY_CIRCLE);
		}
		else if(nGain<0)
		{
			if (i==nBanker)
				m_bitActor[i]->LoadPic(Res_GetPICStruct("地主哭"));
			else
				m_bitActor[i]->LoadPic(Res_GetPICStruct("农民哭"));
			APPLY_PlayBitObject(m_bitActor[i],MODE_PLAYBIT_BY_CIRCLE);
		}
		else
		{
			if (i==nBanker)
				m_bitActor[i]->LoadPic(Res_GetPICStruct("地主",0));
			else
				m_bitActor[i]->LoadPic(Res_GetPICStruct("农民",0));
		}

		if (GetGameWinApp()->IsPlayDeposit())
			m_bitActor[i]->MoveTo(m_plResultDlg->GetLeft()+36,m_plResultDlg->GetTop()+96+i*nLineHeight);
		else
			m_bitActor[i]->MoveTo(m_plResultDlg->GetLeft()+41,m_plResultDlg->GetTop()+96+i*nLineHeight);

		//炸弹
// 		if (GetGameInfoEx()->GetGameWin()->nBombCount[chairno]>0)
// 		{
// 			m_plResultDlg->DrawPic(Res_GetPICStruct("炸弹图标"),325,nLineHeight*i+80,50,30);
// 			
// 			str.Format("%d",GetGameInfoEx()->GetGameWin()->nBombCount[chairno]);
// 			pen->WriteString(m_plResultDlg,370,nLineHeight*i+87,str);
// 		}
	}

	//本轮基础分，下轮基础分
	CXyDigit* pen2=(CXyDigit*)GetObject("DigitPen2");
	if (GetGameWinApp()->IsPlayDeposit())
	{
		m_plResultDlg->DrawPic(Res_GetPICStruct("下局基础银"),12,230,-1,-1);
		if (m_digitBase)
		{
			str.Format(_T("%d两"), pGameWin->gamewin.nNextBaseDeposit);
			m_digitBase->WriteString(m_plResultDlg,107,231,str,DIGIT_FLAGS_LEFT_JUSITIFY);
		}
		else
		{
			str.Format(_T("%d"), pGameWin->gamewin.nNextBaseDeposit);
			pen2->WriteString(m_plResultDlg,107,231,str);
		}
	}
	else
	{
		m_plResultDlg->DrawPic(Res_GetPICStruct("下局基础分"),20,230,-1,-1);
		if (m_digitBase)
		{
			str.Format(_T("%d分"), pGameWin->gamewin.nNextBaseDeposit);
			m_digitBase->WriteString(m_plResultDlg,110,230,str,DIGIT_FLAGS_LEFT_JUSITIFY);
		}
		else
		{
			str.Format(_T("%d"), pGameWin->nNextBaseScore);
			pen2->WriteString(m_plResultDlg,110,230,str);
		}
	}
	

	//名目
	{
		str = _T("");
		int nList = 1;
		if (pGameWin->nSpring>0)
		{
			nList++;
			str.Format(_T("春天 × %d"), UwlPow2(pGameWin->nSpring));
			GetPublicInfoEx()->nBombFan++;
			OPE_ShowGameInfo();
		}
		if (pGameWin->nBombFan>0)
		{
			nList++;
			str.Format(_T("炸弹 × %d"), UwlPow2(pGameWin->nBombFan));
		}
	
		int nX = 0;
		if (GetGameWinApp()->IsPlayDeposit())
			nX = 385;
		else
			nX = 365;

		m_plResultDlg->SetTextSize(14,400);
		m_plResultDlg->SetTextColor(RGB(251,207,1));
		if (nList==3)
		{
			str.Format(_T("叫分 × %d"), GetGameInfoEx()->GetGameStartInfo()->nObjectGains);
			m_plResultDlg->TextOut(nX,110,str);
			str.Format(_T("春天 × %d"), UwlPow2(pGameWin->nSpring));
			m_plResultDlg->TextOut(nX,140,str);
			str.Format(_T("炸弹 × %d"), UwlPow2(pGameWin->nBombFan));
			m_plResultDlg->TextOut(nX,170,str);
		}
		else if (nList==2)
		{
			str.Format(_T("叫分 × %d"), GetGameInfoEx()->GetGameStartInfo()->nObjectGains);
			m_plResultDlg->TextOut(nX,121,str);
			if (pGameWin->nSpring>0)
				str.Format(_T("春天 × %d"), UwlPow2(pGameWin->nSpring));
			else
				str.Format(_T("炸弹 × %d"), UwlPow2(pGameWin->nBombFan));
			m_plResultDlg->TextOut(nX,151,str);
		}
		else if (nList==1)
		{
			str.Format(_T("叫分 × %d"), GetGameInfoEx()->GetGameStartInfo()->nObjectGains);
			m_plResultDlg->TextOut(nX,140,str);
		}
	}
	
	if (GetGameInfoEx()->GetGameWin()->gamewin.nScoreDiffs[theApp.GetChairNO()]>0)
		PlaySound("Snd_win");
	else
		PlaySound("Snd_lose"); 

	//	APPLY_WaitFewTimes(2000,&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_AUTOSTART));
	//定位
	{
		RECT rect=GetGameRect();
		int x=rect.left;
		int y=rect.top;
		int w=rect.right-rect.left;
		int h=rect.bottom-rect.top;
		m_plResultDlg->MoveTo(x+w/2,y+h/2);
		m_plResultDlg->Updata();
		m_plResultDlg->Show();
		m_plResultDlg->Updata();
	}
}

UINT CMyGame::OPE_ZuPai()
{
	CString sRet;
    ZUPAI stZuPai;
	ZeroMemory(&stZuPai,sizeof(stZuPai));
	
	CGameWinApp* pGameApp=GetGameWinApp();
	stZuPai.nUserID=pGameApp->GetUserID();
	stZuPai.nRoomID=pGameApp->GetRoomID();
	stZuPai.nTableNO=pGameApp->GetTableNO();
	stZuPai.nChairNO=pGameApp->GetChairNO();

	CGameRequest* pGameConnect=(CGameRequest* )GetGameWinApp()->GetGameConnect();
	return pGameConnect->GC_ZuPai(sRet,&stZuPai);
}

UINT CMyGame::OPE_ThrowCard(int nSelectCard[], int nCardLen)
{
	CGameTable table;
	CARD_UNITE details;
	ZeroMemory(&details,sizeof(CARD_UNITE));
	
	if (!table.GetUniteDetails(theApp.GetChairNO(),nSelectCard,nCardLen,details,CARD_UNITE_TYPE_TOTAL_EX))
		return UR_OPERATE_FAILED;
	if (GetPublicInfoEx()->nWaitChair==-1)
	{
		table.GetBestUnitType(details);
	}
	else
	{
		table.GetBestUnitType(GetPublicInfoEx()->WaitCardUnite,details);
	}
	
	CString sRet;
    CARDS_THROW card_throw;
	ZeroMemory(&card_throw, sizeof(card_throw));
	CGameWinApp* pGameApp=GetGameWinApp();
	card_throw.nUserID=pGameApp->GetUserID();
	card_throw.nRoomID=pGameApp->GetRoomID();
	card_throw.nTableNO=pGameApp->GetTableNO();
	card_throw.nChairNO=pGameApp->GetChairNO();

	card_throw.unite=details.uniteType[0];
	card_throw.Next_Chair=RUL_GetNextChairNO(theApp.GetChairNO());
	
#if _OFFLINE_
	OPE_ClearTable();
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		card_throw.nChairNO=i;
		card_throw.Next_Chair=theApp.GetChairNO();
		card_throw.nWaitTime=99;
		NTF_CardsThrow(&card_throw);
	}
	return 0;
#else	
	CGameRequest* pGameConnect=(CGameRequest* )GetGameWinApp()->GetGameConnect();
	return pGameConnect->ThrowCard(sRet,&card_throw);
#endif
	
}

void  CMyGame::GUI_MoveClock(int chairno,int second)
{
	if (second>=0)//当time<0的时候，时间不变化
	{
		m_Clock->SetDigit(second);
	}

	int nDrawIndex=RUL_GetDrawIndexByChairNO(chairno);

	POINT pt = {0,0};
	if (IS_BIT_SET(GetGameInfo()->GetStatus(),TS_WAITING_AUCTION))
	{
		pt=m_ptrPlayer[nDrawIndex]->GetClockPosition(CLOCK_POS_AUCTION);
	}
	else
	{
		pt=m_ptrPlayer[nDrawIndex]->GetClockPosition(GetPublicInfo()->nWaitChair==-1);
	}

	m_Clock->MoveTo(pt.x,pt.y);
	m_Clock->Updata();
	m_bitClockBase->MoveTo(pt.x,pt.y);
	m_bitClockBase->Updata();
	
	
	if (IS_BIT_SET(GetGameInfo()->GetStatus(),TS_WAITING_THROW))
	{
		int current_chair=GetGameInfo()->GetCurrentChairNO();
		while(current_chair!=chairno)
		{
			current_chair=RUL_GetNextChairNO(current_chair);
			OPE_ClearTable(current_chair);//转到某家操作时自动清桌
		}
	}

	GetPlayerInfoEx(chairno)->nWaitTime=second;
	GetGameInfo()->SetCurrentChairNO(chairno);

	
	//for(int i=0;i<GetGameWinApp()->GetPlayerCount();i++)
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		m_ptrPlayer[RUL_GetDrawIndexByChairNO(i)]->SetCurrentChair(i==chairno);
	}
	//    FreshCursor();
}

void CMyGame::GUI_MoveClock(int second)
{
	if (second>=0)//当time<0的时候，时间不变化
	{
		m_Clock->SetDigit(second);
	}

	ShowClock();

	if (IS_BIT_SET(GetGameInfo()->GetStatus(), TS_WAITING_YAZHU))
	{
		int nFrame=240;
		int nTimeSpace=(YAZHU_WAIT)*1000/nFrame;
		m_bitClockBase->GoToFrame(0);
		APPLY_PlayBitObject(m_bitClockBase, MODE_PLAYBIT_STEP_BY_STEP, nFrame, 0, nTimeSpace, 
			&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_MOVIE_STOPOBJECT,(WPARAM)m_bitClockBase));
	}
	else /*if (IS_BIT_SET(GetGameInfo()->GetStatus(), TS_WAITING_ZUPAI))*/
	{
 		APPLY_PlayBitObject(m_bitClockBase,MODE_PLAYBIT_BY_FRAMECOUNT,-1,0,0,
 			&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_MOVIE_STOPOBJECT,(WPARAM)m_bitClockBase));
	}
}

POINT CMyGame::GetClockPos()
{
	RECT rect=GetGameRect();
	int x=rect.left;
	int y=rect.top;
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;
	POINT pt = {x+w/2,h/2};
	return pt;
}

SK_PUBLIC_INFO* CMyGame::GetPublicInfo()
{
	return (SK_PUBLIC_INFO*)&(GetGameInfoEx()->GetGameTableInfo()->m_Public);
}

GAME_PUBLIC_INFO* CMyGame::GetPublicInfoEx()
{
	return &(GetGameInfoEx()->GetGameTableInfo()->m_Public);
}

SK_PLAYER_INFO* CMyGame::GetPlayerInfo(int chairno)
{
	return (SK_PLAYER_INFO*)&GetGameInfoEx()->GetGameTableInfo()->m_Player[chairno];
}

GAME_PLAYER_INFO* CMyGame::GetPlayerInfoEx(int chairno)
{
	return &(GetGameInfoEx()->GetGameTableInfo()->m_Player[chairno]);
}

CGameInfoEx* CMyGame::GetGameInfoEx()
{
	CGameApp* pApp=(CGameApp*)GetGameWinApp();
	return (CGameInfoEx*)pApp->GetGameInfo();
}

void  CMyGame::OPE_ShowPlayerRest(int chairno,int nRestCount)
{
	int nDrawIndex=RUL_GetDrawIndexByChairNO(chairno);
	if (nDrawIndex<=0) return;
	if (!m_ptrPlayer[nDrawIndex]) return;
	CGamePlayer* pPlyaer=	m_ptrPlayer[nDrawIndex];
	pPlyaer->SetRestCard(nRestCount);
	
	int x=0;
	int y=0;
	if (nDrawIndex==0)
	{
		return;		
	}
	else if (nDrawIndex==1)
	{
		CGameCard* pCard=NULL;
		for(int i=0;i<GetTotalCardsCount();i++)
		{
			if (m_Card[i]->m_nChairNO==chairno
				&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND
				&&m_Card[i]->m_nUnitIndex==0)
			{
				pCard=m_Card[i];
				break;
			}
		}
		
		if (!pCard) return;
		x=pCard->GetX();
		y=pCard->GetY()+pCard->GetWidth()/2+pPlyaer->GetRestCardHeight()/2+5;
		
	}
	else if (nDrawIndex==2)
	{
		CGameCard* pCard=NULL;
		for(int i=0;i<GetTotalCardsCount();i++)
		{
			if (m_Card[i]->m_nChairNO==chairno
				&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND
				&&m_Card[i]->m_nUnitIndex==nRestCount-1)
			{
				pCard=m_Card[i];
				break;
			}
		}
		
		if (!pCard) return;
		
		x=pCard->GetX();
		y=pCard->GetY()+pCard->GetWidth()/2+pPlyaer->GetRestCardHeight()/2+5;	
	}
	else
	{
		return;
	}
}

BOOL CMyGame::RUL_IsFriend(int chairno)
{
	int nBanker = GetGameInfoEx()->GetBanker();
	int nMyChair = GetGameWinApp()->GetChairNO();

	if (nMyChair==nBanker||chairno==nBanker)
		return FALSE;
	else
		return TRUE;
}

void CMyGame::OPE_PlaySoundByOperation(int chairno,DWORD dwOpeFlags,int nCount)
{
	switch (dwOpeFlags)
	{
	   case CARD_UNITE_TYPE_BOMB:
		   {
			   PlaySound("Snd_Bomb");
		   }
		   break;
	   case CARD_UNITE_TYPE_2KING:
		   {
			   PlaySound("Snd_Bomb");
		   }
		   break;
	   default:
			   PlaySound("Snd_Throw");
		   break;
	}
}

char g_szCardName[16][8]={"", "2", "3", "4", "5", "6", "7", 
						   "8", "9", "10", "J", "Q", "K", "A", "Joke_s", "Joke"};

void CMyGame::OPE_PlaySoundByOperation(BOOL bFirstHand, CARDS_THROW *pCardsThrow)
{
	if (!pCardsThrow)
		return ;
	int chairno = pCardsThrow->nChairNO;
	if (chairno<0||chairno>=TOTAL_CHAIRS)
		return;

	//reserved
// 	if (RUL_GetCardCount(chairno,CARD_STATUS_INHAND)==0
// 		&& pCardsThrow->unite.dwCardType != CARD_UNITE_TYPE_2KING)
// 	{
// 		PlaySound("Snd_Bomb");
// 		return;
// 	}

	
	CString str;
	switch (pCardsThrow->unite.dwCardType)
	{
	case CARD_UNITE_TYPE_2KING:
		{
			str = _T("Huojian");
			
		}
		break;
	case CARD_UNITE_TYPE_BOMB:
		{
			str = _T("Boom");
			
		}
		break;
	case CARD_UNITE_TYPE_FOUR_2:
	case CARD_UNITE_TYPE_FOUR_2_COUPLE:
		{
			if(!bFirstHand)
				str = _T("Da");
			else
				str = _T("D42");

		}
		break;
	case CARD_UNITE_TYPE_THREE:
		{
			if(!bFirstHand)
				str = _T("Da");
			else
				str = _T("D30");
			
		}
		break;
	case CARD_UNITE_TYPE_THREE_1:
		{
			if(!bFirstHand)
				str = _T("Da");
			else
				str = _T("D31");
			
		}
		break;	
	case CARD_UNITE_TYPE_THREE_COUPLE:
		{
			if(!bFirstHand)
				str = _T("Da");
			else
				str = _T("D32");
			
		}
		break;	
	case CARD_UNITE_TYPE_ABT_THREE:
	case CARD_UNITE_TYPE_ABT_THREE_1:
	case CARD_UNITE_TYPE_ABT_THREE_COUPLE:
		{
			if(!bFirstHand)
				str = _T("Da");
			else
				str = _T("feiji");
			
		}
		break;
	case CARD_UNITE_TYPE_ABT_COUPLE:
		{
			if(!bFirstHand)
				str = _T("Da");
			else
				str = _T("liandui");
			
		}
		break;	
	case CARD_UNITE_TYPE_ABT_SINGLE:
		{
			if(!bFirstHand)
				str = _T("Da");
			else
				str = _T("Shunzi");
			
		}
		break;
	case CARD_UNITE_TYPE_SINGLE:
	case CARD_UNITE_TYPE_COUPLE:
		{
			if (pCardsThrow->unite.nCardIDs[0]>=0&&pCardsThrow->unite.nCardIDs[0]<TOTAL_CARDS)
			{
				str.Format(_T("%s"), g_szCardName[SK_GetCardIndexEx(pCardsThrow->unite.nCardIDs[0])]);
				if (pCardsThrow->unite.nCardCount==2)
				{
					str+=str;
				}
			}
		}
		break;

	   default:
		   {

			    PlaySound("Snd_Throw");
		   }
		   return;
	}

	if (theApp.m_GamePlayer[chairno].nNickSex)
		str = _T("female_") + str;

	PlaySound(str);
}

void    CMyGame::OPE_GameClockZero()
{
	if (GetGameWinApp()->IsLookOn()) return;
	if (!IsInGame(theApp.GetChairNO())) return;
	int nChairNO=GetGameWinApp()->GetChairNO();



	if(IS_BIT_SET(GetGameInfo()->GetStatus(), TS_WAITING_QZ))
	{
		if (!IsFinishQZ(nChairNO))
		{
			OnQiangZhuang(FALSE);
		}

		return;
	}
	else if(IS_BIT_SET(GetGameInfo()->GetStatus(),TS_WAITING_YAZHU))
	{
		if (!IsBanker(nChairNO) &&!GetPublicInfoEx()->bFinishYaZhu[nChairNO])
		{
			//最低5倍
			OPE_YaZhu(5);
		}

		return;
	}
	else if (IS_BIT_SET(GetGameInfo()->GetStatus(),TS_WAITING_ZUPAI))
	{
		if (!GetPublicInfoEx()->bFinishZuPai[theApp.GetChairNO()])
		{
			if (m_btnSureZuPai->IsShow() && m_btnSureZuPai->IsEnable())
			{
				OnSureZuPai();
			}
		}
		return;
	}
	return;
}


void  CMyGame::OnEnterGameOK()
{
	for(int i=0;i<GetGameWinApp()->GetTableChairCount();i++)
	{
		if (GetGameWinApp()->IsPlayerEnter(i))
		{
			int nDrawIndex=RUL_GetDrawIndexByChairNO(i);
			m_ptrPlayer[nDrawIndex]->LoadPlayer(i);
			if (GetGameInfo()->IsChairReady(i))
			{
				m_ptrPlayer[nDrawIndex]->ShowReady();
			}
		}
	}
	
	if(GetGameWinApp()->IsRandomRoom()
		&&!GetGameWinApp()->IsReEnter())
	{
		if (!GetGameWinApp()->IsLookOn()
			&&!GetGameWinApp()->IsGameRunning())
		{
			//显示提示:等待分桌
			ShowWaitArrageTable();
		}
	}
	else if (!GetGameWinApp()->IsLookOn())
	{   
		OPE_ShowStartButton();
	}

	//加载积分榜
	memcpy(theApp.m_nResultDiff, ((GAME_ENTER_INFO*)theApp.GetEnterInfo())->nResultDiff, sizeof(GetPublicInfoEx()->nResultDiff));
	memcpy(theApp.m_nResultTotal,((GAME_ENTER_INFO*)theApp.GetEnterInfo())->nTotalResult,sizeof(GetPublicInfoEx()->nTotalResult));
	theApp.m_nResultCount = ((GAME_ENTER_INFO*)theApp.GetEnterInfo())->ei.nBout;
	theApp.UpdataResultList();

	//二维码充值start
    CGameRequest* pGameConnect = (CGameRequest*)theApp.GetGameConnect();
    if (pGameConnect)
    {
        pGameConnect->GC_RoomLimitInfo();//获取房间额度信息
    }
    //二维码充值end
}

void CMyGame::OPE_ShowStartButton()
{
	FitStartBtn();

	TG_SHOW_OBJECT(m_btnStart);

	//TG_SHOW_OBJECT(m_bitAdvice);

	// solo独离房长时间不操作则T玩家	
	OPE_BeginEventKickOff(); // add on 20130807
}


void CMyGame::ShowWaitArrageTable()
{
	//游戏已经开始
	if (GetGameWinApp()->IsGameRunning())
		return;

	RECT rect=GetGameRect();
	int x=rect.left;
	int y=rect.top;
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;
	
	BITObject* pbo=NULL;
	if (!m_ShowWaitArrage)
	{
		pbo = CreateBitMap(Res_GetPICStruct("waitsolo"));
		pbo->SetFloor(FLOOR_ANIMATION);
		pbo->MoveTo(x+w/2,y+h/2);
		m_ShowWaitArrage = pbo;
	}
	else
	{
		pbo = (BITObject*)m_ShowWaitArrage;
	}
	APPLY_PlayBitObject(pbo);
	pbo->Show();
	pbo->Updata();
}

void CMyGame::OPE_PlaySoundForAuction(int chairno, BOOL bPass, int nGains, BOOL bAutoHide)
{
	if (chairno<0||chairno>=TOTAL_CHAIRS)
		return;
	if (!bPass && (nGains<1||nGains>3))
		return;
	
	int nDrawIndex = RUL_GetDrawIndexByChairNO(chairno);
	POINT pt = ((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->GetTalkPosition();
	if (m_btnThrow&&nDrawIndex==MYDRAWINDEX)
		pt.y = m_btnThrow->GetY()-50;
	else if (nDrawIndex==1)
		pt.x += 50;
	else if (nDrawIndex==2)
		pt.x -= 50;
	CString str,strPic;
	if (bPass)
	{
		str = _T("BuJiao");
		if (theApp.m_bUseBigUI)
			strPic = _T("放大版_不叫");
		else
			strPic = _T("文字_不叫");
		if (!bAutoHide)
			((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(strPic,0, pt);
		else
			((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(strPic, pt.x, pt.y);
	}
	else
	{
		if (nGains==1)
		{
			str = _T("F1");
			if (theApp.m_bUseBigUI)
				strPic = _T("放大版_文字_1分");
			else
				strPic = _T("文字_1分");
			if (!bAutoHide)
				((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(strPic,0, pt);
			else
				((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(strPic, pt.x, pt.y);
		}
		else if (nGains==2)
		{
			str = _T("F2");
			if (theApp.m_bUseBigUI)
				strPic = _T("放大版_2分");
			else
				strPic = _T("文字_2分");
			if (!bAutoHide)
				((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(strPic,0, pt);
			else
				((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(strPic, pt.x, pt.y);
		}
		else
		{
			str = _T("F3");
			if (theApp.m_bUseBigUI)
				strPic = _T("放大版_3分");
			else
				strPic = _T("文字_3分");
			if (!bAutoHide)
				((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(strPic,0, pt);
			else
				((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(strPic, pt.x, pt.y);
		}
	}

	if (theApp.m_GamePlayer[chairno].nNickSex)
		str = _T("female_") + str;
	
	PlaySound(str);
}


void CMyGame::OPE_PlayAnimationByThrow(CARDS_THROW *pCardsThrow)
{
	if (!pCardsThrow)
		return ;
	int chairno = pCardsThrow->nChairNO;
	if (chairno<0||chairno>=TOTAL_CHAIRS)
		return;

	int nDrawIndex = RUL_GetDrawIndexByChairNO(chairno);
	switch (pCardsThrow->unite.dwCardType)
	{
	case CARD_UNITE_TYPE_2KING:
		{
			POINT pt = ((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->GetRocketPosition();
			((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(_T("huojian"), pt.x, pt.y);
			PlaySound("Snd_rocket");
		}
		break;
	case CARD_UNITE_TYPE_BOMB:
		{
			POINT pt = ((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->GetBombPosition();
			((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(_T("bomb"), pt.x, pt.y);
			PlaySound("Snd_Bomb");
		}
		break;
	case CARD_UNITE_TYPE_ABT_THREE:
	case CARD_UNITE_TYPE_ABT_THREE_1:
	case CARD_UNITE_TYPE_ABT_THREE_COUPLE:
		{
			POINT pt1,pt2;
			((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->GetAirPlanePosition(pt1,pt2);
			((CMyPlayer*)(m_ptrPlayer[nDrawIndex]))->ShowAnimation(_T("feiji"), pt1,pt2);
			PlaySound("Snd_feiji");
		}
		break;
	   default:
		   PlaySound("Snd_Throw");
		   break;
	}
}

void CMyGame::FitButtonPosition()
{
	if (!m_Card[0]
		|| !m_btnPass)
		return;

	RECT rect=GetGameRect();
	int x=rect.left;
	int y=rect.top;
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;

// 	int nCardHeight=m_Card[0]->GetCardHeight();
// 	Sys_SetIntStatus(GAME_STATUS_CARD_HEIGHT,nCardHeight);
	int nCardHeight=Sys_GetIntStatus(GAME_STATUS_CARD_HEIGHT);
	int btn_y=y+h-nCardHeight-30-Sys_GetIntStatus(GAME_STATUS_PLAYER_HEIGHT)-m_btnPass->GetHeight()/2;

	if (theApp.IsLikeRemaind())
	{
		if (m_btnPass->IsShow())
		{
			m_btnPass->MoveTo(x+w/2-80,btn_y);
			m_btnPass->Updata();
			m_btnReMaind->MoveTo(x+w/2,btn_y);
			m_btnReMaind->Updata();
			m_btnThrow->MoveTo(x+w/2+80,btn_y);
			m_btnThrow->Updata();
		}
		else
		{
			m_btnThrow->MoveTo(x+w/2,btn_y);
			m_btnThrow->Updata();
		}
	}
	else
	{
		if (m_btnPass->IsShow())
		{
			m_btnPass->MoveTo(x+w/2-41,btn_y);
			m_btnPass->Updata();
			m_btnThrow->MoveTo(x+w/2+41,btn_y);
			m_btnThrow->Updata();
		}
		else
		{
			m_btnThrow->MoveTo(x+w/2,btn_y);
			m_btnThrow->Updata();
		}

		m_btnReMaind->Hide();
		m_btnReMaind->Updata();
	}
}


void	CMyGame::OnSortHandCard()   //按张数多少排序
{
	m_nRemindType=0;

	m_nOldSortType=m_nSortType;
	if (m_nSortType==SORT_FLAGS_COMMON)
	{
		m_nOldSortType=m_nSortType;
		m_nSortType=SORT_FLAGS_COUNT_MORE;
	}
	else
	{
		m_nSortType=SORT_FLAGS_COMMON;
	}
	
	OPE_SortHand(m_nSortType);
	PlaySound("snd_sort");
}

void    CMyGame::NTF_ChairCard(CARDS_INFO* pCardInfo)
{
	CARDS_INFO cardinfo;
	memset(&cardinfo,0,sizeof(cardinfo));
	XygInitChairCards(cardinfo.nCardIDs,MAX_CARDS_PER_CHAIR);
	
	cardinfo.nUserID	=	pCardInfo->nUserID;
	cardinfo.nChairNO	=	pCardInfo->nChairNO;
	cardinfo.nCardsCount	=	pCardInfo->nCardsCount;
	
	memcpy(cardinfo.nCardIDs,pCardInfo->nCardIDs,sizeof(int)*pCardInfo->nCardsCount);
	
	
	OPE_ShowPlayerCard(cardinfo.nChairNO,cardinfo.nCardIDs,pCardInfo->nCardsCount);
	OPE_FreshPlayerCard(cardinfo.nChairNO);
	
	//看到自己旁观这家的牌，就隐藏掉请求看牌按钮
	if (GetGameWinApp()->IsLookOn()
		&&pCardInfo->nChairNO==GetGameWinApp()->GetChairNO())
	{
		PGL_HIDE_OBJECT(m_btnAskKanPai);
		GetGameInfoEx()->m_bnAlowLook = TRUE;
	}

	PlaySound("Snd_kanpai");
}

POINT CMyGame::GetActorPosition(int chairno,int &nActorPos)
{
	POINT pt={0,0};

	int nDrawIndex=RUL_GetDrawIndexByChairNO(chairno);
	if (nDrawIndex<0) return pt;
	if (!m_ptrPlayer[nDrawIndex]) return pt;

	CGamePlayer* pPlyaer=	m_ptrPlayer[nDrawIndex];

	int nCardCount = RUL_GetCardCount(chairno, CARD_STATUS_INHAND);
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;

	if (nDrawIndex==0)
	{
		if (nCardCount==0)
		{
			pt.x = x+w/2;
			pt.y = y+h-200;		//改一下
		}
		else
		{
			pt.x = x+w/4;
			pt.y = y+h-200;
			CGameCard* pCard=NULL;
			for(int i=0;i<GetTotalCardsCount();i++)
			{
				if (m_Card[i]->m_nChairNO==chairno
					&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND
					&&m_Card[i]->m_nUnitIndex==0)
				{
					pCard=m_Card[i];
					break;
				}
			}			
			if (!pCard) return pt;
			pt.x=pCard->GetX()-pCard->GetWidth()/2-45;
			pt.y=pCard->GetY();
			nActorPos = pt.y;
		}	
	}
	else if (nDrawIndex==1)
	{
		if (nCardCount==0)
		{
			SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
			int nOff = (h/2-pic.width/2)/3;
			pt.x = x+200;		//改一下
			pt.y = y+h/2-nOff;
		}
		else
		{
			pt.x = x+200;		//改一下
			pt.y = y+h/4;
			CGameCard* pCard=NULL;
			for(int i=0;i<GetTotalCardsCount();i++)
			{
				if (m_Card[i]->m_nChairNO==chairno
					&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND
					&&m_Card[i]->m_nUnitIndex==nCardCount-1)
				{
					pCard=m_Card[i];
					break;
				}
			}			
			if (!pCard) return pt;
			pt.x=pCard->GetX();
			pt.y=pCard->GetY()-pCard->GetWidth()/2-50;
			nActorPos = pt.x;
		}	
	}
	else if (nDrawIndex==2)
	{
		if (nCardCount==0)
		{
			SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
			int nOff = (h/2-pic.width/2)/3;
			pt.x = x+w-200;		//改一下
			pt.y = y+h/2-nOff;
		}
		else
		{
			pt.x = x+w-200;		//改一下
			pt.y = y+h/4;
			CGameCard* pCard=NULL;
			for(int i=0;i<GetTotalCardsCount();i++)
			{
				if (m_Card[i]->m_nChairNO==chairno
					&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND
					&&m_Card[i]->m_nUnitIndex==0)
				{
					pCard=m_Card[i];
					break;
				}
			}			
			if (!pCard) return pt;
			pt.x=pCard->GetX();
			pt.y=pCard->GetY()-pCard->GetWidth()/2-50;
			nActorPos = pt.x;
		}	
	}

	
	return pt;
}

void CMyGame::OPE_HideAllPlayerAnmition()
{
	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		((CMyPlayer*)(m_ptrPlayer[RUL_GetDrawIndexByChairNO(i)]))->HideAnimation();
	}
}

void CMyGame::OPE_FreshUI()
{
	if (m_plGameInfo&&m_plGameInfo->IsShow())
	{
		OPE_ShowGameInfo();
	}

	if (m_bitReMindPass)
	{
		StopObject(m_bitReMindPass->GetHandle());
		if (theApp.IsUseBigUI())
			m_bitReMindPass->LoadPic(Res_GetPICStruct("放大版_没有牌能大过上家"));
		else
			m_bitReMindPass->LoadPic(Res_GetPICStruct("过牌提示"));
		m_bitReMindPass->Hide();
		m_bitReMindPass->Updata();
	}
}

void CMyGame::MoveGameRect(RECT& Rect)
{
	int width = Rect.right-Rect.left;
	if (width<=1100)
		theApp.SetUseBigUI(FALSE);
	else
		theApp.SetUseBigUI(TRUE);

	CGameBase::MoveGameRect(Rect);
}

void  CMyGame::RUL_CalcGameStatus(int w,int h)
{
	SNG_PIC pic_player=Res_GetPICStruct("NewPlayerBoardSmall");
	Sys_SetIntStatus(GAME_STATUS_PLAYER_HEIGHT,pic_player.height);
	
	Sys_SetIntStatus(GAME_STATUS_RGB_ME,RGB(0,128,64));
	Sys_SetIntStatus(GAME_STATUS_RGB_PLAYER,RGB(48,113,246));
	Sys_SetIntStatus(GAME_STATUS_RGB_SYSTEM,RGB(64,64,64));

	RECT rc=GetMainGameRect();
	int width=rc.right-rc.left;

	SNG_PIC picCardSelf,picCardOthers;
	if (width<=1000)
	{
		picCardSelf=Res_GetPICStruct("牌背面71x96");
		picCardOthers=Res_GetPICStruct("牌背面50x68");
	}
	else if (width<=1100)
	{
		picCardSelf=Res_GetPICStruct("牌背面82x110");
		picCardOthers=Res_GetPICStruct("牌背面50x68");
	}
	else
	{
		picCardSelf=Res_GetPICStruct("牌背面98x132");
		picCardOthers=Res_GetPICStruct("牌背面71x96");
	}

	Sys_SetIntStatus(GAME_STATUS_CARD_WIDTH,picCardSelf.width);
	Sys_SetIntStatus(GAME_STATUS_CARD_HEIGHT,picCardSelf.height);
	Sys_SetIntStatus(GAME_STATUS_CARDSMALL_WIDTH,picCardOthers.width);
	Sys_SetIntStatus(GAME_STATUS_CARDSMALL_HEIGHT,picCardOthers.height);
}

void  CMyGame::OnSureZuPai()
{
	if (GetGameWinApp()->IsLookOn()) 
		return;
	if (!m_btnSureZuPai->IsShow()) 
		return;
	if (!m_btnSureZuPai->IsEnable()) 
		return;
		
	OPE_HideButtons();						//隐藏按钮
	OPE_ZuPai();
}

void CMyGame::NTF_ZuPai(ZUPAI *pZuPai)
{
	if (!ValidateChair(pZuPai->nChairNO) || !IsInGame(pZuPai->nChairNO)) return;

	int nDrawIndex = RUL_GetDrawIndexByChairNO(pZuPai->nChairNO);

	for(int i=0;i<CHAIR_CARDS;i++)
	{
		if (pZuPai->nCardID[i]!=-1)
		{
			CGameCard* card=GetCard(pZuPai->nChairNO,pZuPai->nCardID[i]);
			if (card)
			{
				card->m_nCardID=pZuPai->nCardID[i];
				card->m_nDrawIndex=nDrawIndex;
				card->m_nChairNO=pZuPai->nChairNO;
				card->m_nUnitIndex=i;
				card->m_nStatus=CARD_STATUS_THROWDOWN;
				card->Fresh();
			}
		}
	}

	GetPublicInfoEx()->bFinishZuPai[pZuPai->nChairNO] = TRUE;
	BeginGroup();

	for (int j=0;j<CHAIR_CARDS;j++)
	{
		CGameCard* card=GetThrowCard(pZuPai->nChairNO,pZuPai->nCardID[j]);
		SNG_PIC pic;
		if (card->m_nCardSize==CARD_SIZE_MAX)
			pic=Res_GetPICStruct("牌正面98x132",card->GetCardPic(card->m_nCardID));
		else
			pic=Res_GetPICStruct("牌正面82x110",card->GetCardPic(card->m_nCardID));
		
		APPLY_ReverseObject(card->m_bitCard, 15,180,pic,0,
			&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_MOVIE_FLASHCARD,(WPARAM)(card)));
		if (i == theApp.GetChairNO())
		{
			((CMyCard*)(card))->HideCardMask();
		}
	}	
  	NextGroup(); 
 	APPLY_WaitFewTimes(0,&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_MOVIE_SHOWCARDTYPE,(WPARAM)(pZuPai->nChairNO), (LPARAM)(pZuPai->nCardType)));
	EndGroup();

	m_CEventPipe->ReleaseEvent();

#ifdef _AUTOPLAY_
	UwlLogFile("[End]NTF_ZuPai--Time:%u,nUserID:%d,nRoomID:%d,nTableNO:%d,nChairNO:%d",
		GetTickCount(), pZuPai->nUserID, pZuPai->nRoomID, pZuPai->nTableNO, pZuPai->nChairNO);
#endif
	
}

void CMyGame::OPE_ShowResult()
{
	RushResultDigit();

	APPLY_WaitFewTimes(900,&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_MOVIE_END_GAME));
}

void CMyGame::OPE_WhileEndGame()
{
	GAME_WIN_RESULT* pGWR=GetGameInfoEx()->GetGameWin();
	theApp.NotifyGameWin((GAME_WIN*)pGWR); //战绩榜等的刷新
	
	GetGameInfoEx()->GetGameStartInfo()->dwStatus=0;
	
	CGameWinApp* pGameApp=GetGameWinApp();
	theApp.ShowMsgInChatView("\r\n");
	CString str,strTmp;
	str.Format(" <c=%d><size=14>===本局结束===\r\n",RGB(0,0,0));
	pGameApp->ShowMsgInChatView(str);		
	
	for(int i=0;i<m_nTotalPlayer;i++)
	{
		if (!GetGameInfoEx()->GetGameStartInfo()->bInGame[i]) continue;
		
		int nDrawIndex=RUL_GetDrawIndexByChairNO(i);
		
		OPE_ShowPlayerCard(i,pGWR->nCardID[i],CHAIR_CARDS);
		OPE_FreshPlayerCard(i);
		if (m_ptrPlayer[nDrawIndex])
			m_ptrPlayer[nDrawIndex]->ClearStatus();
		
		int nScore=0;
		CString strTmp;
		if (GetGameWinApp()->IsPlayDeposit())
		{
			nScore=pGWR->gamewin.nDepositDiffs[i];
			strTmp=_T("两");
		}
		else
		{
			nScore=pGWR->gamewin.nScoreDiffs[i];
			strTmp=_T("分");
		}
		
		UINT nTextColor=0;
		if (nScore>0)
			nTextColor=RGB(255,0,0);
		else
			nTextColor=RGB(35,35,35);
		
		int length = strlen(m_szUsername[i]);
		CString str;
		str.Format(" <c=%d>[%s]",nTextColor,m_szUsername[i]);
		pGameApp->ShowMsgInChatView(str);
		for(int s=0;s<=20-length;s++)
			pGameApp->ShowMsgInChatView(" ");
		
		if (nScore>=0)
			str.Format("<c=%d>+%d %s\r\n",nTextColor,nScore,strTmp);
		else
			str.Format("<c=%d>%d %s\r\n",nTextColor,nScore,strTmp);
		
		pGameApp->ShowMsgInChatView(str);
	}		

	if (!theApp.IsLookOn())
	{
		OPE_ShowStartButton();
		ShowHuanZhuoBtn(TRUE);
	}
	
	if (m_dlgAutoSetDeposit)
	{
		if (m_dlgAutoSetDeposit->IsAutoSetDeposit())
		{
			AutoSetDeposit();
		}
	}
	for( i=0;i<m_nTotalPlayer;i++)
	{
		m_ptrPlayer[RUL_GetDrawIndexByChairNO(i)]->DrawPlayerBoard();
	}
	
	int nRoomMoney = GetGameWinApp()->m_GameRoom.nMinDeposit;
	bool bIsEnough = LeftMoneyEnough(nRoomMoney);

	if(!bIsEnough)
	{
		if(!GetGameWinApp()->IsLookOn())
		{
			theApp.ShowGameCharge();
		}
		
	}

	if(m_welfDialog != NULL)
	{
		m_welfDialog->HideWelfareDlg();
		m_welfareSurplusNum = m_welfDialog->RecordOrReadWelfareNum(false);
        if(m_welfareSurplusNum < m_nDailyNum)
		{
			OnGetWelfareConfig(m_nActivityID);
		}
		else
		{
			m_welfareSurplusNum = m_nDailyNum;
			m_welfDialog->SetWelfareStat(-4);
		}
	}
	
}

UINT CMyGame::GetIntNum(UINT nInteger)
{
	int num=0;

	if (0 == nInteger)
		return num;
	else 
	{
		while (1)
		{
			nInteger /= 10;
			if (nInteger > 0)			
				num++;			
			else
				break;
		}
		num++;		
	}

	return num;
}

void CMyGame::GetRushDigitPos(int chairno, UINT digitnum, POINT& pt)
{
	pt.x=0;
	pt.y=0;

	if (!ValidateChair(chairno))
	{
		return;
	}

	SNG_PIC pic=Res_GetPICStruct(_T("rush_digit_plus"));
	int totalwidth=digitnum*pic.width;

	int nDrawIndex = RUL_GetDrawIndexByChairNO(chairno);
	pt.x=m_ptrPlayer[nDrawIndex]->GetX()-totalwidth/2;
	if(nDrawIndex!=MYDRAWINDEX)
	{
		pt.y=m_ptrPlayer[nDrawIndex]->GetY()+33;
	}
	else
	{
		pt.y=m_ptrPlayer[nDrawIndex]->GetY()-33;
	}
	
}

void CMyGame::RushResultDigit()
{
	GAME_WIN_RESULT *pGameWin= ((CGameInfoEx*)GetGameInfo())->GetGameWin();
	
	BeginGroup();
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		int nChairNo = i;
		
		if (!IsInGame(nChairNo)) 
			continue;
		
		m_plRush[nChairNo]->Erasure();
		
		CXyDigit* pShowDigit=NULL;
		
		if (pGameWin->nGainsWin[nChairNo]<0)
		{
			m_plRush[nChairNo]->DrawPic(Res_GetPICStruct("rush_bit_minus"));
			m_digtRush[nChairNo][0]->Hide();
			m_digtRush[nChairNo][0]->Updata();
			pShowDigit=m_digtRush[nChairNo][1];
			pShowDigit->SetDigit(abs(pGameWin->nGainsWin[nChairNo]));
		}
		else
		{
			m_plRush[nChairNo]->DrawPic(Res_GetPICStruct("rush_bit_plus"));
			m_digtRush[nChairNo][1]->Hide();
			m_digtRush[nChairNo][1]->Updata();
			pShowDigit=m_digtRush[nChairNo][0];
			pShowDigit->SetDigit(pGameWin->nGainsWin[nChairNo]);
		}
		m_plRush[nChairNo]->Hide();
		m_plRush[nChairNo]->SetAlpha(1);
		m_plRush[nChairNo]->Updata();
		pShowDigit->Hide();
		pShowDigit->Updata();
		
		POINT pt={0,0};
		GetRushDigitPos(nChairNo, GetIntNum(abs(pGameWin->nGainsWin[nChairNo])), pt);
		
		m_objRushFrame[nChairNo]->MoveTo(pt.x, pt.y);
		m_objRushFrame[nChairNo]->SetAlpha(1);
		m_objRushFrame[nChairNo]->Updata();
		
		POINT ptDst={pt.x,pt.y-49};
		int nDrawIndex = RUL_GetDrawIndexByChairNO(nChairNo);
		if(nDrawIndex!=MYDRAWINDEX)
			ptDst.y=pt.y+49;
		
		BeginGroup();
		GetMusicModule()->PlaySound("Snd_win");
		NextGroup();
		APPLY_DirectShow(m_digitYZYinzi[nDrawIndex],FALSE);
		APPLY_DirectShow(m_objYinzi[nDrawIndex],FALSE);
		APPLY_DirectShow(m_plRush[nChairNo],TRUE);
		APPLY_DirectShow(pShowDigit,TRUE);
		NextGroup();
		APPLY_MoveObject(m_objRushFrame[nChairNo],ptDst,6);
		APPLY_ShowObject(m_objRushFrame[nChairNo],0,0);
		NextGroup();
		EndGroup();
		
	}
	EndGroup();
}

void CMyGame::NTF_GameMsg(GAME_MSG* pGameMsg)
{
	DWORD key=pGameMsg->nMsgID;
	void* data=((PBYTE)pGameMsg)+sizeof(GAME_MSG);
	DWORD len=pGameMsg->nDatalen;
	
    if (key>TCSG_GAME_MSG_BEGIN && key<TCSG_GAME_MSG_END)
	{
		
	}
	else
	{
		CSKMainGame::NTF_GameMsg(pGameMsg);
	}
}

void CMyGame::NTF_StartCountdown(START_COUNTDOWN* pCountdown)
{
	if (theApp.IsLookOn())
		return;
	
	if (pCountdown->bStartorStop)
	{
// 		CString str;
// 		str.Format(_T("准备人数已达到下限，游戏将在%d秒后开始！\r\n"), pCountdown->nCountDown);
// 		theApp.ShowMsgInChatView(str);

		GetGameInfoEx()->GetGameStartInfo()->dwStatus=TS_WAITING_COUNTDOWN;
		OPE_StartCountDownTimer();							//启动倒计时
		GUI_ShowCountDownClock(pCountdown->nCountDown);		//移动倒计时时钟
	}
	else
	{
		//theApp.ShowMsgInChatView("2\r\n");
		//theApp.ShowMsgInChatView(_T("人数不足，停止倒计时。\r\n"));
		OPE_StopCountDownTimer();
		GUI_HideCountDownClock();
		m_nZeroCountDown=0;
		GetGameInfoEx()->GetGameStartInfo()->dwStatus = 0;
	}
}

void    CMyGame::OPE_StartCountDownTimer()
{
	if (m_hEventCountDownTimer)
	{
		StopApply(m_hEventCountDownTimer);
	}
	m_hEventCountDownTimer=APPLY_CallBackPerTime(1000,-1,&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_COUNTDOWNTIMER));
}

void  CMyGame::GUI_ShowCountDownClock(int second)
{//移动倒计时时钟
	if (second>=0)//当time<0的时候，时间不变化
	{
		m_digitCntDwn->SetDigit(second);
	}

	m_nZeroCountDown=0;

	m_digitCntDwn->Show();
	m_digitCntDwn->Updata(); 
	m_bitCountDown->Show();
	m_bitCountDown->Updata();
}

void CMyGame::GUI_HideCountDownClock()
{//移动倒计时时钟
	m_digitCntDwn->Hide();
	m_digitCntDwn->Updata(); 
	m_bitCountDown->Hide();
	m_bitCountDown->Updata();
}

void CMyGame::OPE_StopCountDownTimer()
{
	if (m_hEventCountDownTimer)
	{
		StopApply(m_hEventCountDownTimer);
	}
	m_hEventCountDownTimer = NULL;
}

void CMyGame::NTF_SyncStartCountdown(SYNC_COUNTDOWN* pSyncCountdown)
{
	if (theApp.IsLookOn())
		return;
	
	if (m_digitCntDwn->IsShow() && TS_WAITING_COUNTDOWN == GetGameInfoEx()->GetGameStartInfo()->dwStatus)///?
	{
		m_digitCntDwn->SetDigit(pSyncCountdown->nCountDown);
		m_digitCntDwn->Updata();
	}
}

void   CMyGame::OnPlayerEnter(GAMEPLAYER* pGamePlayer)
{
	CSKMainGame::OnPlayerEnter(pGamePlayer);
	if (GetGameWinApp()->IsGameRunning()&&!GetGameInfoEx()->GetGameStartInfo()->bInGame[pGamePlayer->nChairNO]&& GetGameWinApp()->IsPlayerEnter(pGamePlayer->nChairNO))
	{
		PGL_SHOW_OBJECT(m_bitWaitNextBout[RUL_GetDrawIndexByChairNO(pGamePlayer->nChairNO)]);
	}
}

void   CMyGame::OnPlayerAbort(GAMEPLAYER* pGamePlayer)
{
	CSKMainGame::OnPlayerAbort(pGamePlayer);

	PGL_HIDE_OBJECT(m_bitWaitNextBout[RUL_GetDrawIndexByChairNO(pGamePlayer->nChairNO)]);
}


////////////////////////////////自动划银////////////////////////////////////
int CMyGame::GetAutoSetDeposit()
{
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	TCHAR szValue[32];	
	memset(szValue,0,32);
	CString strKeyName;
	strKeyName.Format(_T("AutoFix"));
	GetPrivateProfileString ( _T("FIXDEPOSIT"),strKeyName,_T(""),szValue,32,pGameApp->GetDepositSetFile());
	return atoi(szValue);
}

void CMyGame::AutoSetDeposit()
{
	if (m_dlgAutoSetDeposit)
	{
		if (m_dlgAutoSetDeposit->IsAutoSetDeposit())
		{
			int nAutoFixDeposit=GetAutoSetDeposit();
			if (nAutoFixDeposit > GetGameWinApp()->m_GamePlayer[theApp.GetChairNO()].nDeposit)
			{
				CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
				CString sRet;
				UINT nResponse = 0;
				SAFE_RNDKEY sr;
				memset(&sr,0,sizeof(SAFE_RNDKEY));
				sr.nUserID = pGameApp->GetUserID();
// 				pGameApp->ShowWaitBar(SAFEBOX_QUERYING_SECUREPWD,TRUE);
				BOOL bRet = pGameApp->GetGameConnect()->GC_TakeRndKey(sRet,&sr,nResponse);
// 				pGameApp->ShowWaitBar(SAFEBOX_QUERYING_SECUREPWD,FALSE);
				
// 				if(bRet && !m_bPassSecurityPWD)
				if(bRet)
				{//有密码且没输入过密码	
					ShowKeyResult(TRUE, sr.nRndKey, 1);
				}
				else
				{//包括保护密码为空和其他情况
					TakeDepositToGame(nAutoFixDeposit,0);		
				}
			}
		}
	}
}

void CMyGame::ShowKeyResult(BOOL bShow,int nRandKey, int nFlag)
{
	////////////保护密码验证框/////////////////
	if (bShow)
	{
		if (NULL==m_pMyKeyResult)
		{
			PGL_MSG_CLASS(m_pMyKeyResult,CMyKeyResult);
			m_pMyKeyResult->CreateNumKeyPad(nRandKey);
			m_pMyKeyResult->SetMoveAble(TRUE);			
			m_pMyKeyResult->SetFloor(FLOOR_SYSTEMWARNING-20);
		}
		
		if (m_pMyKeyResult)
		{
			m_pMyKeyResult->SetRandNumber(nRandKey);
			m_pMyKeyResult->ClearData();
			m_pMyKeyResult->SetFlag(nFlag);
			
			m_pMyKeyResult->MoveTo(GetScreenWidth()/2,GetScreenHeight()/2);
			m_pMyKeyResult->Show();
			m_pMyKeyResult->Updata();
		}
	}
	else
	{
		if (m_pMyKeyResult)
		{
			m_pMyKeyResult->Hide();
			m_pMyKeyResult->Updata();
		}
	}
}

void CMyGame::TakeDepositToGame(int nDestDeposit,int nKeyResult) 
{//手中银子	
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();			
	int nGameDeposit = pGameApp->m_GamePlayer[pGameApp->GetChairNO()].nDeposit;
	
	if (nDestDeposit>nGameDeposit)
	{
		CString sRet;
		CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
// 		pGameApp->ShowWaitBar(SAFEBOX_TAKING_DEPOSIT,TRUE);
		BOOL bRet = pGameApp->GetGameConnect()->GC_TakeSafeDeposit(sRet,nDestDeposit-nGameDeposit,nKeyResult);	//取银子
// 		pGameApp->ShowWaitBar(SAFEBOX_TAKING_DEPOSIT,FALSE);
		
		if (bRet)
		{
// 			m_bPassSecurityPWD = TRUE;
//			sRet.Format(_T("设定成功。\r\n\r\n(自动从保险箱中取出 %d 两，%s当前有银子 %d 两)"),nDestDeposit-nGameDeposit,pGameApp->GetGameName(),nDestDeposit);	
//			if(nKeyResult>0)
//				pGameApp->ShowTimerMsgBox(sRet,NULL,NULL,10,TIMERDLG_FLAGS_ICON|TIMERDLG_FLAGS_DANYMIC,"xyf打牌");
//			else
//				pGameApp->ShowTimerMsgBox(sRet,NULL,NULL,10,TIMERDLG_FLAGS_ICON|TIMERDLG_FLAGS_DANYMIC,"xyf打牌");
		}
		else
		{
			if (!theApp.IsGameRunning())
			{
				if (m_dlgAutoSetDeposit->IsAutoSetDeposit())
				{
					m_dlgAutoSetDeposit->OnCheck();
				}
			}
			pGameApp->ShowTimerMsgBox(sRet,NULL,NULL,10,TIMERDLG_FLAGS_ICON|TIMERDLG_FLAGS_DANYMIC,"xyf打牌");
		}
	}
}
////////////////////////////////自动划银////////////////////////////////////


BOOL CMyGame::IsBanker(int nChairNO)
{
	if (!ValidateChair(nChairNO)) return FALSE;
	
	return (nChairNO == GetGameInfo()->GetBanker());
}

BOOL CMyGame::IsInGame(int nChairNO)
{
#ifdef _OFFLINE_
	return TRUE;
#endif

	if (!ValidateChair(nChairNO)) return FALSE;
	
	return GetGameInfoEx()->GetGameStartInfo()->bInGame[nChairNO];
}

BOOL CMyGame::ValidateChair(int nChairNO)
{
	if (nChairNO >= 0 && nChairNO < TOTAL_CHAIRS)
		return TRUE;
	else 
		return FALSE;	
}

int CMyGame::RUL_GetUnSelectCard(int nCardID[])
{
	int count=0;

	for(int i=0;i<GetTotalCardsCount();i++)
	{
		if (m_Card[i]->m_nChairNO==GetGameWinApp()->GetChairNO()
			&&!m_Card[i]->bnSelect
			&&m_Card[i]->m_nStatus==CARD_STATUS_INHAND)
		{
			nCardID[count]=m_Card[i]->m_nCardID;
			count++;
		}
	}
	
	return count;
}

void CMyGame::FitCountDownPostion()
{
	POINT pt = GetClockPos();

	m_digitCntDwn->MoveTo(pt.x,pt.y);
	m_digitCntDwn->Updata();
	m_bitCountDown->MoveTo(pt.x,pt.y);
	m_bitCountDown->Updata();
}
void CMyGame::FitClockPos()
{
	POINT pt = GetClockPos();
	m_Clock->MoveTo(pt.x,pt.y);
	m_Clock->Updata();
	m_bitClockBase->MoveTo(pt.x,pt.y);
	m_bitClockBase->Updata();
}

int CMyGame::GetYinziPos(int nDrawIndex, POINT& pt)
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	
	for (int i = 0; i < TOTAL_CHAIRS; i++)
	{
		if (-1 != nDrawIndex && i != nDrawIndex)
			continue;
		
		if(i == 0)
		{
			pt.x = x + w/2;
			pt.y = y + h/2 + 40;
		}
		else if(i == 1)
		{
			pt.x = x + w/2 - 70;
			pt.y = y + h/2 - 15;
		}
		else if(i == 2)
		{
			pt.x = x + w/2;
			pt.y = y + h/2 - 80 - 20;
		}
		else if(i == 3)
		{
			pt.x = x + w/2 + 70;
			pt.y = y + h/2 - 15;
		}
	}

	return 0;
}

void CMyGame::FitZupaiBtnPos()
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;	

	SNG_PIC picBtn=Res_GetPICStruct(_T("摊牌"));

	POINT pt={0,0};
	GetInHandCardPosCE(MYDRAWINDEX, CHAIR_CARDS-1, pt);
	int nCardWidth=GetInHandCardWidth(MYDRAWINDEX);
	pt.x=pt.x+nCardWidth/2 + ZP_BTN_TO_CARD_X + picBtn.width/2;

	if (m_btnSureZuPai)
	{
		m_btnSureZuPai->MoveTo(pt.x, pt.y);
		m_btnSureZuPai->Updata();
	}
}

void CMyGame::ShowClock()
{
	m_Clock->Show();
	m_Clock->Updata();

	m_bitClockBase->Show();
	m_bitClockBase->Updata();

}
void CMyGame::HideClock()
{
	m_Clock->Hide();
	m_Clock->Updata();

	m_bitClockBase->Hide();
	m_bitClockBase->Updata();
	StopObject(m_bitClockBase->GetHandle());	
}

int CMyGame::GetBaseValue(int nChairNo)
{
	int nBase=GetGameInfoEx()->GetGameStartInfo()->nBaseDeposit[nChairNo];
	if (nBase<1) 
		nBase = 1;
	return nBase;
}

double CMyGame::GetSpeed(int x1, int y1, int x2, int y2, DWORD dwTimes)
{
	if (dwTimes>0)
	{
		double unit=sqrt(pow(x1-x2, 2) + pow(y1-y2, 2)) / dwTimes;  
		return unit*GetMinApplySpace();
	}

	return 30;
}

int CMyGame::GetInHandCardPosCE(int nDrawIndex, int nCardIndex, POINT& pt)
{
	if (nDrawIndex<0 || nDrawIndex>=TOTAL_CHAIRS)
		return 0;
	
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		CGameCard* pC=m_Card[i];
		if (pC && pC->m_nStatus==CARD_STATUS_INHAND
			&& pC->m_nDrawIndex == nDrawIndex && pC->m_nUnitIndex==nCardIndex)
		{
			pt.x=pC->m_Position.x;
			pt.y=pC->m_Position.y;
			return 1;
		}
	}	
	
	return 0;
}

int CMyGame::GetInHandCardWidth(int nIndex)
{
	if (nIndex<0 || nIndex>=TOTAL_CHAIRS)
		return 0;
	
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		CGameCard* pC=m_Card[i];
		if (pC && pC->m_nStatus==CARD_STATUS_INHAND
			&& pC->m_nDrawIndex == nIndex && pC->m_nUnitIndex>=0 && pC->m_nUnitIndex<CHAIR_CARDS)
		{
			return pC->m_nCardWidth;
		}
	}	
	return 0;
}

int CMyGame::GetInHandCardHeight(int nIndex)
{
	if (nIndex<0 || nIndex>=TOTAL_CHAIRS)
		return 0;
	
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		CGameCard* pC=m_Card[i];
		if (pC && pC->m_nStatus==CARD_STATUS_INHAND
			&& pC->m_nDrawIndex == nIndex && pC->m_nUnitIndex>=0 && pC->m_nUnitIndex<CHAIR_CARDS)
		{
			return pC->m_nCardHeight;
		}
	}	
	return 0;
}

int CMyGame::GetThrowCardPosCE(int nIndex, POINT& pt)
{
	if (nIndex<0 || nIndex>=TOTAL_CHAIRS)
		return 0;
	
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		CGameCard* pC=m_Card[i];
		if (pC && pC->m_nStatus==CARD_STATUS_THROWDOWN
			&& pC->m_nDrawIndex == nIndex && pC->m_nUnitIndex==1)
		{
			pt.x=pC->m_Position.x;
			pt.y=pC->m_Position.y;
			return 1;
		}
	}	

	return 0;
}

int CMyGame::GetThrowCardWidth(int nIndex)
{
	if (nIndex<0 || nIndex>=TOTAL_CHAIRS)
		return 0;
	
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		CGameCard* pC=m_Card[i];
		if (pC && pC->m_nStatus==CARD_STATUS_THROWDOWN
			&& pC->m_nDrawIndex == nIndex && pC->m_nUnitIndex>=0 && pC->m_nUnitIndex<CHAIR_CARDS)
		{
			return pC->m_nCardWidth;
		}
	}	
	return 0;
}

int CMyGame::GetThrowCardHeight(int nIndex)
{
	if (nIndex<0 || nIndex>=TOTAL_CHAIRS)
		return 0;
	
	for(int i=0;i<TOTAL_CARDS;i++)
	{
		CGameCard* pC=m_Card[i];
		if (pC && pC->m_nStatus==CARD_STATUS_THROWDOWN
			&& pC->m_nDrawIndex == nIndex && pC->m_nUnitIndex>=0 && pC->m_nUnitIndex<CHAIR_CARDS)
		{
			return pC->m_nCardHeight;
		}
	}	
	return 0;
}

void CMyGame::GetPlayerPanePos(int index, POINT& pt)
{
	if (index < 0 || index >= TOTAL_CHAIRS)
		return;

	pt.x=m_ptrPlayer[index]->GetX();
	pt.y=m_ptrPlayer[index]->GetY();
}

void CMyGame::HideWaitNextBoutAll()
{
	for (int i=0;i<TOTAL_CHAIRS;i++)
	{
		int idx=RUL_GetDrawIndexByChairNO(i);
		PGL_HIDE_OBJECT(m_bitWaitNextBout[idx]);
	}	
}

void CMyGame::ShowHuanZhuoBtn(BOOL bShow)
{
	if (bShow)
		m_btnHuanZhuo->Show();
	else
		m_btnHuanZhuo->Hide();
	m_btnHuanZhuo->Updata();
}

void CMyGame::OnAskNewTable()
{
	CGameWinApp* pApp = GetGameWinApp();
	if (pApp != NULL)
	{
		if (pApp->IsSoloRoom() && IS_BIT_SET(pApp->GetRoomConfigs(), RC_LEAVEALONE))
		{
			if (!pApp->IsRandomRoom())
			{
				CString sRet;
				if (!pApp->GetGameConnect()->GC_AskNewTableChair(sRet))
				{
					
				}
				else
				{
					if (m_ptrPlayer[MYDRAWINDEX])
						m_ptrPlayer[MYDRAWINDEX]->HideReady();
					OPE_ClearGame();
					OPE_ShowStartButton();
					OPE_ShowBankerFlag(FALSE);
				}
			}
		}
	}
}

int CMyGame::GetNextChair(int chairno)
{
	chairno=(chairno + TOTAL_CHAIRS - 1) % TOTAL_CHAIRS;

	return chairno;
}

#define OUT_FILE_PIXEL_PRESCALER	8
void CMyGame::EncodeTextToQRBitmapData(LPCTSTR lpText)
{
	QRcode*	pQRC = NULL;
	if (pQRC = QRcode_encodeString(lpText,0,QR_ECLEVEL_L,QR_MODE_8,1))
	{
		unsigned int unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
		BYTE* pRGBData, *pSourceData, *pDestData;
		
		unWidth = pQRC->width;
		unWidthAdjusted = unWidth*OUT_FILE_PIXEL_PRESCALER*3;
		if (unWidthAdjusted%4)
			unWidthAdjusted = (unWidthAdjusted/4+1)*4;
		unDataBytes = unWidthAdjusted*unWidth*OUT_FILE_PIXEL_PRESCALER;
		
		pRGBData = new BYTE[unDataBytes];
		memset(pRGBData,0xff,unDataBytes);
		
		pSourceData = pQRC->data;
		for(y=0; y<unWidth; y++)
		{
			pDestData = pRGBData+unWidthAdjusted*y*OUT_FILE_PIXEL_PRESCALER;
			for(x=0; x<unWidth; x++)
			{
				if (*pSourceData&1)
				{
					for(l=0; l<OUT_FILE_PIXEL_PRESCALER; l++)
					{
						for(n=0; n<OUT_FILE_PIXEL_PRESCALER; n++)
						{
							*(pDestData+0+n*3+unWidthAdjusted*l) = 0;
							*(pDestData+1+n*3+unWidthAdjusted*l) = 0;
							*(pDestData+2+n*3+unWidthAdjusted*l) = 0;
						}
					}
				}
				pDestData += 3*OUT_FILE_PIXEL_PRESCALER;
				pSourceData++;
			}
		}
		
		BITMAPFILEHEADER kFileHeader;
		ZeroMemory(&kFileHeader,sizeof(kFileHeader));	
		kFileHeader.bfType = 0x4d42;  // "BM"
		kFileHeader.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+unDataBytes;
		kFileHeader.bfReserved1 = 0;
		kFileHeader.bfReserved2 = 0;
		kFileHeader.bfOffBits =	sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
		
		BITMAPINFOHEADER kInfoHeader;
		ZeroMemory(&kInfoHeader,sizeof(kInfoHeader));	
		kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		kInfoHeader.biWidth = unWidth * OUT_FILE_PIXEL_PRESCALER;
		kInfoHeader.biHeight = -((int)unWidth * OUT_FILE_PIXEL_PRESCALER);
		kInfoHeader.biPlanes = 1;
		kInfoHeader.biBitCount = 24;
		kInfoHeader.biCompression = BI_RGB;
		kInfoHeader.biSizeImage = 0;
		kInfoHeader.biXPelsPerMeter = 0;
		kInfoHeader.biYPelsPerMeter = 0;
		kInfoHeader.biClrUsed = 0;
		kInfoHeader.biClrImportant = 0;
		
		CBuffer buff;
		buff.Write((PBYTE)&kFileHeader, sizeof(BITMAPFILEHEADER));
		buff.Write((PBYTE)&kInfoHeader, sizeof(BITMAPINFOHEADER));
		buff.Write(pRGBData,sizeof(unsigned char)*unDataBytes);
		
		BYTE* pFileData=new BYTE[buff.GetBufferLen()];
		memcpy(pFileData,buff.GetBuffer(),buff.GetBufferLen());
		GetResourceModule()->RES_Create(_T("QRCode_Image.bmp"),pFileData,buff.GetBufferLen());
		
		QRcode_free(pQRC);
		SAFE_DELETE_ARRAY(pRGBData);
	}
}

void CMyGame::OPE_QZ()
{
	OPE_ShowBankerFlag(FALSE);
	GUI_MoveClock(GetGameInfoEx()->GetGameStartInfo()->nQZWait);
	
	if(!IsInGame(GetGameWinApp()->GetChairNO()) || GetGameWinApp()->IsLookOn())	
		return;

	OPE_ShowQZButton();
}

void CMyGame::OPE_ShowQZButton()
{
	if(m_btnQZ)
	{
		m_btnQZ->Show();
		m_btnQZ->Enable(TRUE);
		m_btnQZ->Updata();
	}
	if(m_btnNOQZ)
	{
		m_btnNOQZ->Show();
		m_btnNOQZ->Updata();
	}	
	//当大于2个人玩时，需要满足以下任一条件才能抢庄
	//1.携带量>=10W
	//2.携带量>=房间下限的2倍
	//3.携带量>=(四人或三人的总携带量)*0.15时
	int nInGameCount=0;
	int nSelfDeposit=0;
	int nTotalDeposit=0;
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		if(IsInGame(i))
		{
			nInGameCount++;
			GAMEPLAYER* gameplayer = GetGameWinApp()->GetGamePlayerByUserID(GetGameWinApp()->GetPlayerIDByChairNO(i));
			if(gameplayer)
			{
				int nDeposit=gameplayer->nDeposit;
				nTotalDeposit+=nDeposit;
				if(i==GetGameWinApp()->GetChairNO())
					nSelfDeposit=nDeposit;
			}
		}
	}
	if(nInGameCount>2)
	{
		if(!(nSelfDeposit>=100000 || nSelfDeposit>=GetGameWinApp()->m_GameRoom.nMinDeposit*2 ||
			nSelfDeposit>=(int)(nTotalDeposit*0.15)))
		{
			OPE_ShowQZRemaind();
			m_btnQZ->Enable(FALSE);
			m_btnQZ->Updata();
		}
	}
}

void CMyGame::OPE_HideQZButton()
{
	if(m_btnQZ)
	{
		m_btnQZ->Hide();
		m_btnQZ->Updata();
	}
	if(m_btnNOQZ)
	{
		m_btnNOQZ->Hide();
		m_btnNOQZ->Updata();
	}
}

void CMyGame::OnQiangZhuang(BOOL bQZ)
{
	if(!IsInGame(GetGameWinApp()->GetChairNO()))
		return;

    OPE_HideQZRemaind();
	OPE_HideQZButton();

	CGameRequest* pGameConnect=(CGameRequest* )GetGameWinApp()->GetGameConnect();
	if(pGameConnect)
	{
		 pGameConnect->GC_QiangZhuang(bQZ);
	}
}

void CMyGame::NTF_QZOK(QZ *pInfo)
{
	if (IsFinishQZ(pInfo->nChairNO))
		return;

	SetFinishQZ(pInfo->nChairNO);
	SetQZFlag(pInfo->nChairNO,pInfo->nQZFlag);
	if(pInfo->nQZFlag>0)
	{
		GetMusicModule()->PlaySound("随机点");
	}

	OPE_ShowQZ(pInfo->nChairNO, pInfo->nQZFlag);
    
	if(pInfo->nFinish==1)
	{
		QZFinish(pInfo);
	}
}

BOOL CMyGame::IsFinishQZ(int nChairNO)
{
	return GetPublicInfoEx()->bFinishQZ[nChairNO];
}

void CMyGame::SetFinishQZ(int nChairNO)
{
	GetPublicInfoEx()->bFinishQZ[nChairNO] = TRUE;
}

int  CMyGame::GetQZFlag(int nChairNO)
{
	return GetPublicInfoEx()->nQZFlag[nChairNO];
}

void CMyGame::SetQZFlag(int nChairNO,int nFlag)
{
	GetPublicInfoEx()->nQZFlag[nChairNO] = nFlag;
}

BOOL CMyGame::GetIsAllGiveUpQZ()
{
	return GetPublicInfoEx()->bAllGiveUpQZ;
}

void CMyGame::SetIsAllGiveUpQZ(BOOL bAllGiveUpQZ)
{
	GetPublicInfoEx()->bAllGiveUpQZ = bAllGiveUpQZ;
}


void CMyGame::QZFinish(QZ *pInfo)
{
	OPE_HideQZRemaind();
	OPE_HideQZButton();

	GUI_HideCountDownClock();
	GetGameInfoEx()->GetGameStartInfo()->nBanker=pInfo->nBanker;
	for(int i = 0; i < TOTAL_CHAIRS; ++i)
	{
		if(IsInGame(i))
			GetGameInfoEx()->GetGameStartInfo()->nBaseDeposit[i]=pInfo->nBaseDeposit[i];
	}
	SetIsAllGiveUpQZ(pInfo->bAllGiveUpQZ);

	int nQZCount = 0;
	for(i = 0; i < TOTAL_CHAIRS; ++i)
	{
		if(!IsInGame(i))
			continue;
		if(GetIsAllGiveUpQZ())
			nQZCount++;
		else
		{
			if(GetQZFlag(i)==1)
				nQZCount++;
		}
	}
	
	if(nQZCount==1)
	{
		HideClock();
		int index = RUL_GetDrawIndexByChairNO(pInfo->nBanker);
		m_indicator[index]->Enable(true);
		PlayZJAni(pInfo->nBanker);
		CGameWinApp* pGameApp=GetGameWinApp();
		GAMEPLAYER player=pGameApp->m_GamePlayer[GetGameInfo()->GetBanker()];
		CString str;
		str.Format("<size=12> 本局庄家：<c=255>%s\r\n",player.szUsername);
		pGameApp->ShowMsgInChatView(str);
	}
	else
	{
        HideClock();
		BeginGroup();
		APPLY_WaitFewTimes(300);
		NextGroup();
		int nBankerNo = pInfo->nBanker;
		
		m_nDealCount = 0;
		int totalCount =  nQZCount * 2 + 1;
		m_indicator[4] = NULL;
		APPLY_CallBackPerTime(125, totalCount, 
			                  &(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_INDIACTOR,(WPARAM)nBankerNo, (LPARAM)totalCount)); 
		EndGroup();
	}
}

void CMyGame::PlayZJAni(int Banker)
{
	int nBanker = Banker;
	int index =  RUL_GetDrawIndexByChairNO(nBanker);
	BeginGroup();
	APPLY_WaitFewTimes(0, &(SNG_MSG)CreateMsg(GetHandle(), TCSG_CALLBACKMSG_SHOWINDICATORSCALE, WPARAM(nBanker)));
	NextGroup();
	APPLY_WaitFewTimes(400, &(SNG_MSG)CreateMsg(GetHandle(), TCSG_CALLBACKMSG_FEIZHUANGANI));
	EndGroup();
}

void CMyGame::BeginYaZhu()
{
	for(int i = 0; i < TOTAL_CHAIRS; ++i)
	{
		m_bitQZFlag[i]->Hide();
		m_bitQZFlag[i]->Updata();	
	}
	for(i = 0; i < TOTAL_CHAIRS; ++i)
	{
		if(m_indicator[i])
		{
			m_indicator[i]->Enable(false);
			m_indicator[i]->Hide();
			m_indicator[i]->Updata();
		}
	}
	OPE_EndTimer();
	OPE_StartTimer();
	GUI_MoveClock(GetGameInfoEx()->GetGameStartInfo()->nYaZhuWait);
	GetGameInfoEx()->GetGameStartInfo()->dwStatus = TS_WAITING_YAZHU|TS_PLAYING_GAME;
	if(!IsInGame(GetGameWinApp()->GetChairNO()) || theApp.IsLookOn())
		return;

	if(IsBanker(GetGameWinApp()->GetChairNO()))
	{
		if (m_bitWaitBet)
		{
			m_bitWaitBet->Show();
			FitWaitBetPostion();
			StopObject(m_bitWaitBet->GetHandle());
			APPLY_PlayBitObject(m_bitWaitBet);
		}
	}
	else
	{
		OPE_ShowYaZhuDlg();
	}
}

void CMyGame::OPE_YaZhu(int nDbl)
{
	if(!IsInGame(GetGameWinApp()->GetChairNO()))
		return;

	OPE_HideYaZhuDlg();
		
	CGameRequest* pGameConnect=(CGameRequest* )GetGameWinApp()->GetGameConnect();
	if(pGameConnect)
	{
		pGameConnect->GC_YaZhu(nDbl);;
	}
}

void CMyGame::NTF_YaZhuOK(YAZHU *pInfo)
{
	if (GetPublicInfoEx()->bFinishYaZhu[pInfo->nChairNO])
		return;
	GetPublicInfoEx()->bFinishYaZhu[pInfo->nChairNO]=TRUE;
	GetPublicInfoEx()->nYaZhuDbl[pInfo->nChairNO]=pInfo->nDbl;

	OPE_ShowYZYinzi(pInfo->nChairNO, pInfo->nDbl);
	
	if(pInfo->nFinish==1)
	{
		YaZhuFinish(pInfo);
	}
}

void CMyGame::YaZhuFinish(YAZHU *pInfo)
{
	GUI_HideCountDownClock();
	HideClock();
	
	OPE_HideWaitBet();
	
	OPE_HideYaZhuDlg();
	
	memcpy(GetGameInfoEx()->GetGameStartInfo()->nHandID, pInfo->nCardID[theApp.GetChairNO()], sizeof(int)*(CHAIR_CARDS));
	OPE_DealSanGongCard();
}

void CMyGame::OPE_DealSanGongCard()
{
	m_nDealCount=0;
	APPLY_CallBackPerTime(215,CHAIR_CARDS,
		&(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_DEALCARD));
}

void CMyGame::MOVIE_DealSanGongCard()
{
	GetMusicModule()->PlaySound("飞牌");
	if(m_nDealCount>=CHAIR_CARDS)
		return;
	
	int index=m_nDealCount*m_nTotalPlayer;
	for(int j=0;j<m_nTotalPlayer;j++)
	{
		if (!IsInGame(j)) continue;
		
		m_Card[index]->m_nChairNO=j;
		m_Card[index]->m_nDrawIndex=RUL_GetDrawIndexByChairNO(j);
		m_Card[index]->m_nStatus=CARD_STATUS_INHAND;
		m_Card[index]->m_nUnitIndex=m_nDealCount;
		m_Card[index]->bnFace=FALSE;
		m_Card[index]->m_nUnitCount=3;
		if (j==theApp.GetChairNO()&&!GetGameWinApp()->IsLookOn())
		{
			m_Card[index]->bnFace=TRUE;
			m_Card[index]->m_nCardID=GetGameInfoEx()->GetGameStartInfo()->nHandID[m_nDealCount];
		}
		
		if(!GetGameWinApp()->IsLookOn())
		{
			((CMyCard*)m_Card[index])->GiveCardAni();
		}
		else
		{
			m_Card[index]->Fresh();
		}
		
		index++;	
	}
	
	m_nDealCount++;
	if (m_nDealCount==CHAIR_CARDS)
	{
		APPLY_WaitFewTimes(200, &(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_DEALCARDOVER));
	}
}

void CMyGame::BeginZuPai()
{
	OPE_EndTimer();
	OPE_StartTimer();
	GUI_MoveClock(GetGameInfoEx()->GetGameStartInfo()->nZuPaiWait);
	GetGameInfoEx()->GetGameStartInfo()->dwStatus = TS_WAITING_ZUPAI|TS_PLAYING_GAME;
	if(!IsInGame(GetGameWinApp()->GetChairNO()) || theApp.IsLookOn())
		return;
	FitZupaiBtnPos();
	m_btnSureZuPai->Enable(TRUE);
	m_btnSureZuPai->Show();
	m_btnSureZuPai->Updata();
}

void CMyGame::CreateIndicatorAni()
{
	for(int i = 0; i < TOTAL_CHAIRS; i++)
	{	
		m_indicator[i] = FastCreateStandButton("btn_indicator",CreateMsg(GetHandle(),TCSG_CALLBACKMSG_NOTHING),NULL,TRUE);	
		m_indicator[i]->Hide();
		m_indicator[i]->Enable(false);
        m_indicator[i]->Updata();
	}
}

void CMyGame::FixIndicatorPosition()
{
	POINT pt={0,0};
	RECT rc=GetMainGameRect();
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	
	SNG_PIC picFLASH = Res_GetPICStruct("btn_indicator");
	int picFLASHH = picFLASH.height;
	
	int nMyOffX;
	int nMyOffY;
	int nOtherOffX;
	int nOtherOffY;
	
	if(w <= 1000)
	{
		nMyOffX = 35;
		nMyOffY = 48;
		nOtherOffX = 25;
		nOtherOffY = 34;
	}
	else if(w <= 1100)
	{
		nMyOffX = 41;
		nMyOffY = 55;
		nOtherOffX = 25;
		nOtherOffY = 34;
	}
	else
	{
		nMyOffX = 49;
		nMyOffY = 66 + 10;
		nOtherOffX = 35 - 10;
		nOtherOffY = 48;
	}
	
	for (int i = 0; i < TOTAL_CHAIRS; i++)
	{		
		POINT pt={0,0};
		if(i == 0)
		{
			pt.x = w/2;
			pt.y = m_ptrPlayer[i]->GetY()-nMyOffY-picFLASHH/2-10;
		}
		else if(i == 1)
		{
			pt.x = m_ptrPlayer[i]->GetX() + nOtherOffX*6 +32+20;
			pt.y = h/2 + 10;
		}
		else if(i == 2)
		{
			pt.x = w/2;
			pt.y = m_ptrPlayer[i]->GetY()+nOtherOffY+picFLASHH/2+10;
		}
		else if(i == 3)
		{
            pt.x = m_ptrPlayer[i]->GetX()-nOtherOffX*6-32-20;
			pt.y = h/2 + 10;
		}
		m_indicator[i]->MoveTo(pt.x, pt.y);
		m_indicator[i]->Updata();
	}	
}

void CMyGame::UpdataIndicatorIightAni(int nBanker, int nCount)
{   
	int nChairNo;
	int nDrawIndex;
	if(m_dwSoundId != 0)
	{
		GetMusicModule()->Stop(m_dwSoundId);
		GetMusicModule()->FreeSound("随机点");
	}

	m_dwSoundId = GetMusicModule()->PlaySound("随机点");

	for(int i = 0; i < TOTAL_CHAIRS; ++i)
	{
		if(m_indicator[i]->IsEnable())
			break;
	}
	if(i == TOTAL_CHAIRS)
	{
		nChairNo = nBanker;
		nDrawIndex = RUL_GetDrawIndexByChairNO(nChairNo);
	}
	else
	{
		nDrawIndex = (i+1)%TOTAL_CHAIRS;
    	nChairNo = RUL_GetChairNOByDrawIndex(nDrawIndex);
	}
	
    while(!(IsInGame(nChairNo)&&m_indicator[nDrawIndex]->IsVisible()))
	{
		nChairNo = (nChairNo+1) % TOTAL_CHAIRS;
		nDrawIndex = RUL_GetDrawIndexByChairNO(nChairNo);
	}

    if(m_indicator[4] != NULL)
	{
		m_indicator[4]->Enable(false);
		m_indicator[4]->Updata();
	}
	
	m_indicator[nDrawIndex]->Enable(true);
	m_indicator[nDrawIndex]->Updata();

	m_indicator[4] = m_indicator[nDrawIndex];

	m_nDealCount++;

	if(m_nDealCount == nCount)
	{
		int nDrawIndexBanker = RUL_GetDrawIndexByChairNO(nBanker);
		if(nDrawIndex != nDrawIndexBanker)
		{
			m_indicator[nDrawIndex]->Enable(false);
			m_indicator[nDrawIndex]->Updata();
            m_indicator[nDrawIndexBanker]->Enable(true);
			m_indicator[nDrawIndexBanker]->Updata();
		}
		for(i = 0 ; i < TOTAL_CHAIRS; ++i)
		{		
			PGL_HIDE_OBJECT(m_indicator[i]);
		}
		APPLY_WaitFewTimes(100, &(SNG_MSG)CreateMsg(GetHandle(), TCSG_CALLBACKMSG_FEIZHUANG, WPARAM(nBanker)));
	}
}

void CMyGame::CreateQZFlag()
{
	for (int i=0; i<TOTAL_CHAIRS; i++)
	{	
		m_bitQZFlag[i]=CreateBitMap(Res_GetPICStruct("抢庄标记"));
		m_bitQZFlag[i]->SetFloor(FLOOR_RESULT+500);
	}
}

void CMyGame::FitQZFlag()
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	
	SNG_PIC pic = Res_GetPICStruct("抢庄标记");
	
	for (int i = 0; i < TOTAL_CHAIRS; i++)
	{
		POINT pt={0,0};
		
		int ptLeft = m_ptrPlayer[i]->GetX();
		int ptTop = m_ptrPlayer[i]->GetY();
		int picH = pic.height;
		int picW = pic.width;
		
		if(w <= 1000)
		{
			if(i == 0)
			{
				pt.x = ptLeft + 64 + picW/2;
				pt.y = ptTop - 64 + picH/2;
				
			}
			else if(i == 1)
			{
				pt.x = ptLeft + 64 + picW/2;
				pt.y = ptTop + 64 - picH/2;
			}
			else if(i == 2)
			{
				pt.x = ptLeft + 64 + picW/2;
				pt.y = ptTop + 64 - picH/2;
			}
			else if(i == 3)
			{
				pt.x = ptLeft - 64 - picW/2;
				pt.y = ptTop + 64 - picH/2;
			}
		}
		else
		{
			if(i == 0)
			{
				pt.x = ptLeft;
				pt.y = ptTop - 64 - picH/2;
				
			}
			else if(i == 1)
			{
				pt.x = ptLeft;
				pt.y = ptTop - 64 - picH/2;
			}
			else if(i == 2)
			{
				pt.x = ptLeft;
				pt.y = ptTop + 64 + picH/2;
			}
			else if(i == 3)
			{
				pt.x = ptLeft;
				pt.y = ptTop - 64 - picH/2;
			}
			
		}
		
		TG_MOVETO_OBJECT(m_bitQZFlag[i], pt.x,pt.y);
		m_bitQZFlag[i]->Updata();
	}
}

void CMyGame::OPE_ShowQZ(int nChairNO, int nQZFlag)
{
	int nIndex = RUL_GetDrawIndexByChairNO(nChairNO);
	m_bitQZFlag[nIndex]->LoadPic(Res_GetPICStruct("抢庄标记", nQZFlag));
	m_bitQZFlag[nIndex]->Show();
	m_bitQZFlag[nIndex]->Updata();
}

void CMyGame::CreateYinzi()
{
	for (int i = 0; i < TOTAL_CHAIRS; ++i)
	{
		PGL_MSG_CLASS(m_digitYZYinzi[i], CXyDigit);
		m_digitYZYinzi[i]->CreateDigit(_T("押注数字"), DIGIT_FLAGS_MID_JUSITIFY);
		m_digitYZYinzi[i]->SetFloor(FLOOR_BTN);
	}
	
	for (i = 0; i < TOTAL_CHAIRS; ++i)
	{	
    	m_objYinzi[i]=CreateBitMap(Res_GetPICStruct("下注银子"));
		m_objYinzi[i]->SetFloor(FLOOR_BTN);
		m_objYinzi[i]->Hide();
		m_objYinzi[i]->Updata();
	}
}

void CMyGame::OPE_ShowYZYinzi(int nChairNO, int nDbl)
{
	if(!IsInGame(nChairNO)||IsBanker(nChairNO))
		return;

	int nFrame=0;
	switch(nDbl)
	{
	case 5:
		nFrame=0;
		break;
	case 7:
		nFrame=1;
		break;
	case 10:
		nFrame=2;
		break;
	default:
		nFrame=0;
		break;
	}

	int nDrawIndex = RUL_GetDrawIndexByChairNO(nChairNO);
	m_objYinzi[nDrawIndex]->LoadPic(Res_GetPICStruct("下注银子", nFrame));

	POINT pt1, pt2;
	pt1.x=m_ptrPlayer[nDrawIndex]->GetX();
	pt1.y=m_ptrPlayer[nDrawIndex]->GetY();
	m_objYinzi[nDrawIndex]->MoveTo(pt1.x, pt1.y);
	
	GetYinziPos(nDrawIndex,pt2);
	double speed=GetSpeed(pt1.x, pt1.y, pt2.x, pt2.y,200);
	
	GetMusicModule()->PlaySound("筹码");
	BeginGroup();
	APPLY_DirectShow(m_objYinzi[nDrawIndex],TRUE);
	NextGroup();
	APPLY_MoveObject(m_objYinzi[nDrawIndex],pt2,speed,0,0, NULL);
	NextGroup();
	OPE_PreFixYinzi(nChairNO, nDbl);
	APPLY_DirectShow(m_digitYZYinzi[nDrawIndex],TRUE);
	EndGroup();
}

void CMyGame::OPE_HideYZYinzi()
{
	for(int i = 0; i < TOTAL_CHAIRS; ++i)
	{
		PGL_HIDE_OBJECT(m_objYinzi[i])
	}
}

void CMyGame::OPE_PreFixYinzi(int nChairNO, int nDbl)
{
	int nBase = GetBaseValue(theApp.GetChairNO());

	if(!IsBanker(nChairNO))
	{
		int nTotalMoney = GetBaseValue(nChairNO)*nDbl;
		int nDrawIndex = RUL_GetDrawIndexByChairNO(nChairNO);
		m_digitYZYinzi[nDrawIndex]->SetDigit(nTotalMoney);
		m_digitYZYinzi[nDrawIndex]->Updata();
	}
}

void CMyGame::OPE_HideYZYinziDigit()
{
	for(int i = 0; i < TOTAL_CHAIRS; ++i)
	{
		PGL_HIDE_OBJECT(m_digitYZYinzi[i]);
	}
}

bool CMyGame::LeftMoneyEnough(int nMoney)
{
     bool bIsEnough;
     
	 int nChairNO = theApp.GetChairNO();
	 GAMEPLAYER player = GetGameWinApp()->m_GamePlayer[nChairNO];
	 int nSelfMoney = player.nDeposit;

	 CString sRet;
	 SAFE_DEPOSIT_EX SafeDeposit;
	 memset(&SafeDeposit, 0, sizeof(SAFE_DEPOSIT_EX));

	 theApp.GetGameConnect()->GC_LookSafeDeposit(sRet,&SafeDeposit);	

	 int nBoxMoney = SafeDeposit.nDeposit;

	 int nTotalMoney = nSelfMoney + nBoxMoney;

	 bIsEnough = nTotalMoney >= nMoney;

	 return bIsEnough;
}

void CMyGame::OnGetWelfareConfig(int nActivityID)
{   
	if(m_nActivityID == -1)
	{
	    m_nActivityID = nActivityID;
	}
	
	if (!GetGameWinApp()->IsLookOn())
	{
		m_welfDialog->OnGetWelfareConfig(m_nActivityID);
		m_welfareSurplusNum  = m_welfDialog->RecordOrReadWelfareNum(false);
        m_nDailyNum = m_welfDialog->GetLimitNum();
        
		if(m_welfareSurplusNum >= m_nDailyNum)
		{
			m_welfareSurplusNum = m_nDailyNum;
			m_welfDialog->SetWelfareStat(-4);
		}
		m_bitGetWelfare->Show();
		m_bitGetWelfare->Updata();

		if(m_welfDialog->GetWelfareStat() == 0)
		{
			ShowCanGetWelfare();
		}
		else
		{
			StopCanGetWelfare();
		}
	}
	else
	{
		m_bitGetWelfare->Hide();
		m_bitGetWelfare->Updata();
	}
}

void CMyGame::ShowCanGetWelfare()
{
	if(m_eventId == -1)
	{
		m_eventId = APPLY_CallBackPerTime(1200, -1, &(SNG_MSG)CreateMsg(GetHandle(),TCSG_CALLBACKMSG_BTN_SHOW_WELFARE));
	}
}

void CMyGame::ShowCanGetCallBack()
{
	m_welfLight->Show();
	APPLY_PlayBitObject(m_welfLight);
	m_welfLight->Updata();
}

void CMyGame::StopCanGetWelfare()
{
	if(m_welfLight)
	{
		StopObject(m_welfLight->GetHandle());
        m_welfLight->Hide();
		m_welfLight->Updata();
	}
	if(m_eventId != -1)
	{
		StopApply(m_eventId);
        m_eventId = -1;
	}
}

void CMyGame::CreateQZRemaind()
{
	PGL_MSG_CLASS(m_plQZRemaind, PlaneObject);

	SNG_PIC pic=Res_GetPICStruct(_T("不能抢庄提示"));
	m_plQZRemaind=CreatePlane(pic.width, pic.height);
	m_plQZRemaind->SetFloor(FLOOR_CARDMASTER+10);
	m_plQZRemaind->Erasure();
	m_plQZRemaind->DrawPic(pic,0,0,pic.width,pic.height);
	m_plQZRemaind->TransGUI();
	m_plQZRemaind->Hide();
	m_plQZRemaind->Updata();
}

void CMyGame::OPE_FitQZReamind()
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	
	POINT pt= {0, 0};
	pt.x = x + w/2;
	pt.y = m_ptrPlayer[MYDRAWINDEX]->GetTop() - 80;
	m_plQZRemaind->MoveTo(pt.x+100, pt.y);
}

void CMyGame::OPE_ShowQZRemaind()
{
	PGL_SHOW_OBJECT(m_plQZRemaind);
}

void CMyGame::OPE_HideQZRemaind()
{
    PGL_HIDE_OBJECT(m_plQZRemaind);
}

//领取低保后头像是显示增加的金钱数
void CMyGame::OPE_ShowWelfRushDigit(int nDigitNum)
{
	if(m_obWelfFrame == NULL)
	{
		m_obWelfFrame=CreateVirtual();
		m_obWelfFrame->ClearChild();
		m_obWelfFrame->MoveTo(-100,-100);
		m_obWelfFrame->SetFloor(FLOOR_RESULT+100);
	}
	
	if(m_plWelfRush == NULL)
	{
		m_plWelfRush = CreatePlane(14, 20);
		m_plWelfRush->MoveTo(m_obWelfFrame->GetX()-5, m_obWelfFrame->GetY());
		m_plWelfRush->SetFloor(FLOOR_RESULT+1000);
		m_plWelfRush->Erasure();
		m_plWelfRush->DrawPic(Res_GetPICStruct("rush_bit_plus"));
		m_obWelfFrame->AddChild(m_plWelfRush,OBJECT_LOCK_POSITION|OBJECT_LOCK_UPDATA|OBJECT_LOCK_ALPHA|OBJECT_LOCK_SHOW);
	}

	if(m_digtWelfRush == NULL)
	{
		PGL_MSG_CLASS(m_digtWelfRush, CXyDigit);
		m_digtWelfRush->CreateDigit("rush_digit_plus",DIGIT_FLAGS_LEFT_JUSITIFY|DIGIT_FLAGS_USE_PIC_SIZE);
		m_digtWelfRush->MoveTo(m_obWelfFrame->GetX()+5, m_obWelfFrame->GetY());
		m_digtWelfRush->SetFloor(FLOOR_RESULT+1000);
		m_obWelfFrame->AddChild(m_digtWelfRush,OBJECT_LOCK_POSITION|OBJECT_LOCK_UPDATA|OBJECT_LOCK_ALPHA);
	}

    m_digtWelfRush->SetDigit(nDigitNum);
	int nChairNo = theApp.GetChairNO();
	POINT pt = {0, 0};
	GetRushDigitPos(nChairNo, GetIntNum(nDigitNum), pt);

    m_obWelfFrame->MoveTo(pt.x, pt.y+120);
	m_obWelfFrame->SetAlpha(1);
	m_obWelfFrame->Updata();

	POINT ptMid={pt.x,pt.y+30};
	POINT ptDst={pt.x,pt.y-200};
	
	BeginGroup();
	APPLY_WaitFewTimes(100);
	NextGroup();
	APPLY_DirectShow(m_plWelfRush,TRUE);
	APPLY_DirectShow(m_digtWelfRush,TRUE);
	NextGroup();
	APPLY_MoveObject(m_obWelfFrame,ptMid,2);
	NextGroup();
	APPLY_MoveObject(m_obWelfFrame,ptDst,2);
	APPLY_ShowObject(m_obWelfFrame,-0.02,0,0,&(SNG_MSG)CreateMsg(GetHandle(), TCSG_CALLBACKMSG_MOVIE_STOPOBJECT, (WPARAM)m_obWelfFrame));
	EndGroup();
}

void CMyGame::OPE_FixYZYinzi()
{
	POINT pt={0,0};
	for(int i = 0; i < TOTAL_CHAIRS; ++i)
	{
		GetYinziPos(i,pt);
		m_objYinzi[i]->MoveTo(pt.x, pt.y);
		m_objYinzi[i]->Updata();
	}
}

void CMyGame::CreateYaZhuDlg()
{
	SNG_PIC pic = Res_GetPICStruct(_T("押注框"));
	SNG_PIC picb = Res_GetPICStruct(_T("押注按钮"));
	TCHAR szDigit[20]="";
	
	m_plYaZhuFrame = CreatePlane(pic.width,pic.height);
	PlaneObject* pl = m_plYaZhuFrame;
	pl->SetFloor(FLOOR_CLOCK-10);
	pl->Erasure();
	pl->DrawPic(pic);
	pl->Hide();
	pl->Updata();
	pl->TransGUI();
	
	int nDbl=0;
	for (int i = 0; i < YAZHU_BTN_COUNT; ++i)
	{
		switch(i)
		{
		case 0:
			nDbl=5;
			break;
		case 1:
			nDbl=7;
			break;
		case 2:
			nDbl=10;
			break;
		default:
			nDbl=5;
			break;
		}
		m_btnYaZhu[i] = FastCreateStandButton("押注按钮",CreateMsg(GetHandle(), TCSG_CALLBACKMSG_YAZHUBTN, (WPARAM)nDbl),NULL,TRUE);
		m_btnYaZhu[i]->SetFloor(pl->GetSurfaceZ());
		m_btnYaZhu[i]->Hide();
		m_btnYaZhu[i]->Updata();
		pl->AddChild(m_btnYaZhu[i], OBJECT_LOCK_POSITION|OBJECT_LOCK_UPDATA|OBJECT_LOCK_SHOW);
		
		PGL_MSG_CLASS(m_digitYaZhuDbl[i],CXyDigit);
		m_digitYaZhuDbl[i]->CreateDigit(_T("押注数字"),DIGIT_FLAGS_MID_JUSITIFY);
		m_digitYaZhuDbl[i]->SetFloor(pl->GetSurfaceZ()+1);
		m_digitYaZhuDbl[i]->MoveTo(pl->GetLeft(),pl->GetTop());
		pl->AddChild(m_digitYaZhuDbl[i],OBJECT_LOCK_POSITION|OBJECT_LOCK_UPDATA|OBJECT_LOCK_SHOW);
	}
	int xSrc = pl->GetX()-pic.width/2 + picb.width/2 + 10;
	int ySrc = pl->GetY();
	int xOff = picb.width+2;
	for (i = 0; i < YAZHU_BTN_COUNT; i++)
	{
		CXyStandButton* btn = m_btnYaZhu[i];
		btn->MoveTo(xSrc+i*xOff, ySrc+2);
		m_digitYaZhuDbl[i]->MoveTo(btn->GetX(),ySrc);
	}
}

void CMyGame::OPE_ShowYaZhuDlg()
{
	FreshYaZhuDigit();
	TG_SHOW_OBJECT(m_plYaZhuFrame);
}

void CMyGame::OPE_HideYaZhuDlg()
{
	TG_HIDE_OBJECT(m_plYaZhuFrame);
}

void CMyGame::FreshYaZhuDigit()
{
	int nBase = GetBaseValue(theApp.GetChairNO());
	int nDbl=0;
	for(int i=0 ;i<YAZHU_BTN_COUNT; ++i)
	{
		switch(i)
		{
		case 0:
			nDbl=5;
			break;
		case 1:
			nDbl=7;
			break;
		case 2:
			nDbl=10;
			break;
		default:
			nDbl=5;
			break;
		}
		if (m_digitYaZhuDbl[i])
		{
			m_digitYaZhuDbl[i]->SetDigit(nBase*nDbl);
		}
	}
}

void CMyGame::FitYZYinZiDigitPos()
{
	POINT pt={0,0};
	SNG_PIC pic = Res_GetPICStruct(_T("下注银子"));
	for(int i = 0; i < TOTAL_CHAIRS; ++i)
	{
		GetYinziPos(i,pt);
		pt.y += pic.height/2+5;
		m_digitYZYinzi[i]->MoveTo(pt.x, pt.y);
		m_digitYZYinzi[i]->Updata();		
	}
}

void  CMyGame::OnEnterDXXW()
{
	CSKMainGame::OnEnterDXXW();
}

void CMyGame::OnEnterIdle()
{
	CSKMainGame::OnEnterIdle();
}

void CMyGame::OPE_OnStart()
{
	OnStart();
	m_welfDialog->HideWelfareDlg();
	m_welfDialog->SetWelfareStat(-3);
}

void CMyGame::NTF_StartGame(int chairno)
{
	CSKMainGame::NTF_StartGame(chairno);
	PlaySound("Snd_HitCard");
}

void CMyGame::OPE_ShowBaseDeposit()
{
	BOOL bShowBaseDeposit=FALSE;
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		if(i==GetGameWinApp()->GetChairNO())
			continue;
		int nBaseDeposit=GetGameInfoEx()->GetGameStartInfo()->nTongBiBaseDeposit[GetGameWinApp()->GetChairNO()][i];
		if(nBaseDeposit>0)
		{
			int nDrawIndex=RUL_GetDrawIndexByChairNO(i);
			if(m_digitBaseDeposit[nDrawIndex-1])
			{
				m_digitBaseDeposit[nDrawIndex-1]->SetDigit(nBaseDeposit);
				m_digitBaseDeposit[nDrawIndex-1]->Show();
				m_digitBaseDeposit[nDrawIndex-1]->Updata();
				bShowBaseDeposit=TRUE;
			}
		}
	}
	
	if(bShowBaseDeposit)
	{
		m_bitBaseDeposit->Show();
		m_bitBaseDeposit->Updata();
	}
}