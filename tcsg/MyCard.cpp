#include "StdAfx.h"

CMyCard::CMyCard()
{
	
	m_nCardWidth		=	0;
	m_nCardHeight		=	0;
	m_nCardSmallWidth	=	0;
	m_nCardSmallHeight	=   0;
	m_nUnitIndex		=	-1;
	m_nSortIndex		=	-1;
	m_nUnitCount		=	0;
	m_Position.x		=   -10000;
	m_Position.y		=   -10000;
	m_nCardID			=	-1;
	m_nStatus			=	0;
	m_nChairNO			=	-1;
	m_nDrawIndex		=   -1;
	m_nDestChair		=   -1;
	bnMask				=   FALSE;            //是否处于掩码状态 
	bnMoved				=   FALSE;           //是否移动中
	bnSelect			=   TRUE;
	bnChoose			=   FALSE;
	bnJoker				=   FALSE;
	bnFastSelect		=   FALSE;
	bnFace				=   FALSE;
	
	m_bitCard			=	NULL;
	m_bitJoker			=	NULL;
	m_dwLastFastSelect	=   0;
	m_dwLastLeftUp		=	0;
	m_nCardSize			=	0;

	m_bBankerCard		=	FALSE;

	for (int i=0;i<CHAIR_CARDS;i++)
	{
		m_plCardMask[i]	=	NULL;
	}
	
}

CMyCard::~CMyCard()
{
	
	
}

void CMyCard::OnIniData()
{
	SNG_PIC pic_card=Res_GetPICStruct("牌背面50x68");
	SNG_PIC pic_cardl=Res_GetPICStruct("牌背面71x96");

	m_nCardWidth=pic_cardl.width;
	m_nCardHeight=pic_cardl.height;
	m_nCardSmallWidth=pic_card.width;
	m_nCardSmallHeight=pic_card.height;

	m_bitCard=CreateBitMap(pic_cardl);
	SetFrame(m_bitCard,OBJECT_LOCK_ALL);

	CMyGame* pMainGame=(CMyGame*)GetObject(OBJECTNAME_MAIN_GAME);
//	m_bitCard->SetAutoMsg(MSG_INPUT_MOUSE_DOUBLEHIT,pMainGame->GetHandle(),CARD_CALLBACKMSG_LEFT_DOWN,(WPARAM)this);
	m_bitCard->SetAutoMsg(MSG_INPUT_MOUSE_LEFT_DOWN,pMainGame->GetHandle(),CARD_CALLBACKMSG_LEFT_DOWN,(WPARAM)this);
	m_bitCard->SetAutoMsg(MSG_INPUT_MOUSE_LEFT_UP,pMainGame->GetHandle(),CARD_CALLBACKMSG_LEFT_UP,(WPARAM)this);
	m_bitCard->SetAutoMsg(MSG_INPUT_MOUSE_RIGHT_DOWN,pMainGame->GetHandle(),CARD_CALLBACKMSG_RIGHT_DOWN,(WPARAM)this);
	m_bitCard->SetAutoMsg(MSG_INPUT_MOUSE_LHIT_MOVE,pMainGame->GetHandle(),CARD_CALLBACKMSG_LHIT_MOVE,(WPARAM)this);
	m_bitCard->SetAutoMsg(MSG_INPUT_TOUCH,pMainGame->GetHandle(),CARD_CALLBACKMSG_TOUCH,(WPARAM)this);
	m_bitCard->SetAutoMsg(MSG_INPUT_UNTOUCH,pMainGame->GetHandle(),CARD_CALLBACKMSG_UNTOUCH,(WPARAM)this);

	for (int i=0;i<CHAIR_CARDS;i++)
	{
		m_plCardMask[i] = CreatePlane(98,132);
		m_plCardMask[i]->SetFloor(CARD_FLOOR_THROWDOWN+3000);
		m_plCardMask[i]->Updata();
	}
}

void CMyCard::Fresh()
{
	SetAlpha(1.0);
	m_bitCard->RotateToAngle(0);
	m_bitCard->SetBackColor(COLOR_TRANSPARENT);

	if (m_nStatus==CARD_STATUS_WAITDEAL)
	{
		Hide();
		Updata();
		return;
	}
	else if (m_nStatus==CARD_STATUS_BOTTOM)
	{
		Hide();
		Updata();
		return;
		
		SNG_PIC picBottom;
		if (bnFace&&m_nCardID>=0&&m_nCardID<TOTAL_CARDS)
		{
			if (m_nCardSize==CARD_SIZE_MAX)
				picBottom=Res_GetPICStruct("牌正面98x132",GetCardPic(m_nCardID));
			else
				picBottom=Res_GetPICStruct("牌正面82x110",GetCardPic(m_nCardID));
		}
		else
		{
			if (m_nCardSize==CARD_SIZE_MAX)
				picBottom=Res_GetPICStruct("牌背面98x132");
			else 
				picBottom=Res_GetPICStruct("牌背面82x110");
		}
		m_bitCard->LoadPic(picBottom);
		m_bitCard->ScaleTo(picBottom.width,picBottom.height);
		m_Position=GetBottomPosition();
		
		MoveTo(m_Position.x,m_Position.y);
		SetFloor(CARD_FLOOR_WAITDEAL+m_nUnitIndex*10);
	}
	else if (m_nStatus==CARD_STATUS_INHAND)
	{//手牌
		if (m_nDrawIndex==0)
		{  //自己
			if (bnFace&&m_nCardID>=0&&m_nCardID<TOTAL_CARDS)
			{
				if (m_nCardSize==CARD_SIZE_MIDDLE)
					m_bitCard->LoadPic(Res_GetPICStruct("牌正面71x96",GetCardPic(m_nCardID)));
				else if (m_nCardSize==CARD_SIZE_BIG)
					m_bitCard->LoadPic(Res_GetPICStruct("牌正面82x110",GetCardPic(m_nCardID)));
				else
					m_bitCard->LoadPic(Res_GetPICStruct("牌正面98x132",GetCardPic(m_nCardID)));

				if (bnFastSelect)
				{
					SetAlpha(0.7);
					m_bitCard->SetBackColor(RGBA(0,0,255,80));
				}
				else if (bnTouch)
				{
					SetAlpha(0.9);
					m_bitCard->SetBackColor(RGBA(0,0,255,80));
				}
			}
			else
			{
				if (m_nCardSize==CARD_SIZE_MIDDLE)
					m_bitCard->LoadPic(Res_GetPICStruct("牌背面71x96"));
				else if (m_nCardSize==CARD_SIZE_BIG)
					m_bitCard->LoadPic(Res_GetPICStruct("牌背面82x110"));
				else
					m_bitCard->LoadPic(Res_GetPICStruct("牌背面98x132"));
			}
			
			m_bitCard->ScaleTo(m_nCardWidth,m_nCardHeight);
			m_Position=GetInHandPosition();
			MoveTo(m_Position.x,m_Position.y);
			SetFloor(CARD_FLOOR_INHAND+m_nUnitIndex*10);
		}
		else
		{//其他玩家		
			if (bnFace&&m_nCardID>=0&&m_nCardID<TOTAL_CARDS)
			{
				if (m_nCardSize==CARD_SIZE_MAX)
					m_bitCard->LoadPic(Res_GetPICStruct("牌正面71x96",GetCardPic(m_nCardID)));
				else
					m_bitCard->LoadPic(Res_GetPICStruct("牌正面50x68",GetCardPic(m_nCardID)));
			}
			else
			{
				if (m_nCardSize==CARD_SIZE_MAX)
					m_bitCard->LoadPic(Res_GetPICStruct("牌背面71x96"));
				else
					m_bitCard->LoadPic(Res_GetPICStruct("牌背面50x68"));
			}
			m_bitCard->ScaleTo(m_nCardSmallWidth,m_nCardSmallHeight);
			m_Position=GetInHandPosition();

			MoveTo(m_Position.x,m_Position.y);
			SetFloor(CARD_FLOOR_INHAND-m_nUnitIndex*10);
		}
	}
	else if (m_nStatus==CARD_STATUS_THROWDOWN)
	{//已组完的牌
		CGameInfoEx* pGameInfoEx=(CGameInfoEx*)(theApp.GetGameInfo());
		GAME_PUBLIC_INFO* pPublic=&(pGameInfoEx->GetGameTableInfo()->m_Public);

		SNG_PIC pic;
		if (IS_BIT_SET(((CGameInfoEx*)(theApp.GetGameInfo()))->GetGameStartInfo()->dwStatus, TS_WAITING_YAZHU))
		{//押注阶段
	
		}
		else if (IS_BIT_SET(((CGameInfoEx*)(theApp.GetGameInfo()))->GetGameStartInfo()->dwStatus, TS_WAITING_ZUPAI))
		{//组牌阶段
			BOOL bTouch = FALSE;
			for (int i=0;i<TOTAL_CARDS;i++)
			{
				if (((CMyGame*)(theApp.GetMainGame()))->m_Card[i]->m_nDrawIndex == m_nDrawIndex)
				{
					if (((CMyGame*)(theApp.GetMainGame()))->m_Card[i]->bnTouch)
					{
						bTouch = TRUE;
					}
				}
			}
			
			if (pPublic->bFinishZuPai[m_nChairNO])
			{
				pic=Res_GetPICStruct("牌正面71x96",GetCardPic(m_nCardID));
				if (m_nCardSize==CARD_SIZE_BIG)
					pic=Res_GetPICStruct("牌正面82x110",GetCardPic(m_nCardID));
				else if (m_nCardSize==CARD_SIZE_MAX)
					pic=Res_GetPICStruct("牌正面98x132",GetCardPic(m_nCardID));						
				if (m_plCardMask)
				{
					if (m_nUnitIndex>=0 && m_nUnitIndex<CHAIR_CARDS)
					{
						m_plCardMask[m_nUnitIndex]->Hide();
						m_plCardMask[m_nUnitIndex]->Updata();
					}
				}
			}			
			else
			{
				if (m_nDrawIndex==MYDRAWINDEX && bTouch)
				{
					pic=Res_GetPICStruct("牌正面71x96",GetCardPic(m_nCardID));
					int nCardWidth = 71;
					int nCardHeight = 96;
					if (m_nCardSize==CARD_SIZE_BIG)
					{
						pic=Res_GetPICStruct("牌正面82x110",GetCardPic(m_nCardID));
						nCardWidth = 82;
						nCardHeight = 110;
					}
					else if (m_nCardSize==CARD_SIZE_MAX)
					{
						pic=Res_GetPICStruct("牌正面98x132",GetCardPic(m_nCardID));
						nCardWidth = 98;
						nCardHeight = 132;
					}
					m_bitCard->SetBackColor(RGBA(0,0,255,255));
					if (m_plCardMask)
					{
						if (m_nUnitIndex>=0 && m_nUnitIndex<CHAIR_CARDS)
						{
							POINT Position=GetThrowPosition();
							m_plCardMask[m_nUnitIndex]->ScalePlane(nCardWidth, nCardHeight);
							m_plCardMask[m_nUnitIndex]->ScaleTo(nCardWidth, nCardHeight);
							m_plCardMask[m_nUnitIndex]->SetFloor(GetSurfaceZ());
							m_plCardMask[m_nUnitIndex]->SetTextColor(RGB(0,0,0));
							m_plCardMask[m_nUnitIndex]->DrawRect(0, 0, nCardWidth, nCardHeight,TRUE);
							m_plCardMask[m_nUnitIndex]->SetAlpha(0.5);
							m_plCardMask[m_nUnitIndex]->MoveTo(Position.x,Position.y);
							m_plCardMask[m_nUnitIndex]->Show();
							m_plCardMask[m_nUnitIndex]->Updata();
						}
					}
					
				}
				else
				{
					pic=Res_GetPICStruct("牌背面71x96",GetCardPic(m_nCardID));
					if (m_nCardSize==CARD_SIZE_BIG)
						pic=Res_GetPICStruct("牌背面82x110",GetCardPic(m_nCardID));
					else if (m_nCardSize==CARD_SIZE_MAX)
						pic=Res_GetPICStruct("牌背面98x132",GetCardPic(m_nCardID));
					
					if (m_plCardMask)
					{
						if (m_nUnitIndex>=0 && m_nUnitIndex<CHAIR_CARDS)
						{
							m_plCardMask[m_nUnitIndex]->Hide();
							m_plCardMask[m_nUnitIndex]->Updata();
						}
					}
					
				}
			}
		}
		else
		{
			//比牌阶段
			pic=Res_GetPICStruct("牌正面71x96",GetCardPic(m_nCardID));
			if (m_nCardSize==CARD_SIZE_BIG)
				pic=Res_GetPICStruct("牌正面82x110",GetCardPic(m_nCardID));
			else if (m_nCardSize==CARD_SIZE_MAX)
				pic=Res_GetPICStruct("牌正面98x132",GetCardPic(m_nCardID));
			if (m_plCardMask)
			{
				if (m_nUnitIndex>=0 && m_nUnitIndex<CHAIR_CARDS)
				{
					m_plCardMask[m_nUnitIndex]->Hide();
					m_plCardMask[m_nUnitIndex]->Updata();
				}
			}
		}

		SetFloor(CARD_FLOOR_THROWDOWN+m_nUnitIndex+(10-m_nDrawIndex)*324);//已知最多6副牌
		m_bitCard->SetAlpha(0.99);
	    m_bitCard->LoadPic(pic);
		m_bitCard->ScaleTo(pic.width,pic.height);
	
		m_Position=GetThrowPosition();
		if (!bnMoved)
		{
			MoveTo(m_Position.x,m_Position.y);
		}
	}
	else if (m_nStatus==CARD_STATUS_COST)
	{
		Hide();
		Updata();
		return;
	}
	else
	{
		Hide();
		Updata();
		return;
	}

	Show();
	Updata();
}

POINT CMyCard::GetWaitDealPositon()
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int width=rc.right-rc.left;
	int height=rc.bottom-rc.top;
	
	POINT pt={0,0};
	if (m_nUnitIndex>=54)
	{
		pt.x=width/2-m_nCardSmallWidth/2-2;
		pt.y=height/2+TOTAL_CARDS-(TOTAL_CARDS-m_nUnitIndex%54/2)*2;
	}
	else
	{
		pt.x=width/2+m_nCardSmallWidth/2+2;
		pt.y=height/2+TOTAL_CARDS-(TOTAL_CARDS-m_nUnitIndex%54/2)*2;
	}
	
	pt.x=pt.x+x;
	pt.y=pt.y+y;
	
	return pt;
}


POINT CMyCard::GetInHandPosition()
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int width=rc.right-rc.left;
	int height=rc.bottom-rc.top;

	POINT pt={0,0};
	int nFrameHeight=50;
	int nFrameHeight2=60;
	int nSpace=bnFace?GetCardSpace():12;
	int nPlayerSpace=0;//和姓名板的距离
	SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
	int nOff = (height/2-pic.width/2)/3;
	CMyGame* pGame=(CMyGame*)(theApp.GetMainGame());

	int total=(m_nUnitCount-1)*nSpace+m_nCardWidth;

	int nInterval = 110;
	if (MYDRAWINDEX==m_nDrawIndex)
	{
		pt.x=x+width/2-total/2+m_nCardWidth/2+m_nUnitIndex*GetCardSpace();
		pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetBottom()-m_nCardHeight/2;
		if (bnSelect)
			pt.y-=GetPopLength();
	}
	else if (m_nDrawIndex==1)
	{
		pt.x=pGame->m_ptrPlayer[m_nDrawIndex]->GetX()+nInterval+m_nUnitIndex*nSpace;
		pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetY();
	}
	else if (m_nDrawIndex==2)
	{
		pt.x=x+width/2-total/2+m_nCardWidth/2+m_nUnitIndex*nSpace;
		pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetY();
	}
	else if (m_nDrawIndex==3)
	{
		pt.x=pGame->m_ptrPlayer[m_nDrawIndex]->GetX()-nInterval-m_nUnitIndex*nSpace;
		pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetY();
	}	
	return pt;
}


POINT CMyCard::GetThrowPosition()
{
	POINT pt={0,0};
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	CMyGame* pGame=(CMyGame*)(theApp.GetMainGame());
	SNG_PIC picP=Res_GetPICStruct("NewPlayerBoardSmall");
	int cardW=m_nCardWidth;

	int off=10;
	int xSrc=0;

	int totalWidth= (m_nUnitCount-1)*GetCardSpace()+m_nCardWidth;

	if (((CSKGameWinApp*)AfxGetApp())->GetTableChairCount() == 4)
	{
		if (MYDRAWINDEX == m_nDrawIndex)//自己
		{
			pt.x=x+w/2-totalWidth/2+m_nCardWidth/2+m_nUnitIndex*GetCardSpace();
			pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetBottom()-m_nCardHeight/2;
		}
		else if (m_nDrawIndex==1)//左边
		{
			xSrc=pGame->m_ptrPlayer[m_nDrawIndex]->GetX()+picP.width/2+off+cardW/2;
			pt.x=xSrc+m_nUnitIndex*GetCardSpace()+30;
			pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetY();
		}
		else if (m_nDrawIndex==2)
		{
			pt.x=x+w/2-totalWidth/2+m_nCardWidth/2+m_nUnitIndex*GetCardSpace();
			pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetTop()+m_nCardHeight/2;
		}
		else if (m_nDrawIndex==3)
		{
			xSrc=pGame->m_ptrPlayer[m_nDrawIndex]->GetX()-picP.width/2-off-cardW/2;
			pt.x=xSrc-(3-1-m_nUnitIndex)*GetCardSpace()-30;
			pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetY();
		}
	}
	else if (((CSKGameWinApp*)AfxGetApp())->GetTableChairCount() == 5)
	{//5个人
		if (m_nDrawIndex==1)//左边
		{
			pt.x=200+m_nUnitIndex*GetCardSpace()+(m_nUnitIndex>=3?GetCardSpace():0);
			pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetY();
		}
		else if (m_nDrawIndex==2)
		{
			pt.x=200+m_nUnitIndex*GetCardSpace()+(m_nUnitIndex>=3?GetCardSpace():0);
			pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetY();
		}
		else if (m_nDrawIndex==3)
		{
			pt.x=rc.right-180-6*GetCardSpace()+m_nUnitIndex*GetCardSpace()+(m_nUnitIndex>=3?GetCardSpace():0);
			pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetY();
		}
		else if (m_nDrawIndex==4)
		{
			pt.x=rc.right-180-6*GetCardSpace()+m_nUnitIndex*GetCardSpace()+(m_nUnitIndex>=3?GetCardSpace():0);
			pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetY();
		}
		else //自己
		{
			pt.x=w/2-m_nUnitCount/2*GetCardSpace()+m_nUnitIndex*GetCardSpace()+(m_nUnitIndex>=3?GetCardSpace():0);
			pt.y=pGame->m_ptrPlayer[m_nDrawIndex]->GetY()+10;
			int a = 1;
			a++;
		}
		pt.x=pt.x+x;
		pt.y=pt.y+y;	
 	}
	
	return pt;
}


POINT CMyCard::GetBottomPosition()
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int width=rc.right-rc.left;
	int height=rc.bottom-rc.top;
	POINT pt={0,0};
	
	SNG_PIC picBottom;
	if (m_nCardSize==CARD_SIZE_MAX)
		picBottom=Res_GetPICStruct("牌背面98x132");
	else 
		picBottom=Res_GetPICStruct("牌背面82x110");
	int nCardWidth = picBottom.width;
	int nH = 100;
	pt.x = rc.left+width/2;
	pt.y = rc.top + nH;
	int nInterval = nCardWidth+10;
	int nSingle = m_nUnitCount%2;
	pt.x=pt.x-nInterval*(m_nUnitCount/2-m_nUnitIndex)+nInterval/2*(1-nSingle);
	
	return pt;	
}

void  CMyCard::CalcSize()
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int width=rc.right-rc.left;
	int height=rc.bottom-rc.top;

	SNG_PIC pic_cardl,pic;
	memset(&pic_cardl,0,sizeof(SNG_PIC));
	if (width<=1000)
	{
		m_nCardSize=CARD_SIZE_MIDDLE;
		pic_cardl=Res_GetPICStruct("牌背面71x96");
		pic=Res_GetPICStruct("牌背面50x68");
		m_nCardSmallWidth=pic.width;
		m_nCardSmallHeight=pic.height;
	}
	else if (width<=1100)
	{
		pic_cardl=Res_GetPICStruct("牌背面82x110");
		m_nCardSize=CARD_SIZE_BIG;
		pic=Res_GetPICStruct("牌背面50x68");
		m_nCardSmallWidth=pic.width;
		m_nCardSmallHeight=pic.height;
	}
	else
	{
		pic_cardl=Res_GetPICStruct("牌背面98x132");
		m_nCardSize=CARD_SIZE_MAX;
		pic=Res_GetPICStruct("牌背面71x96");
		m_nCardSmallWidth=pic.width;
		m_nCardSmallHeight=pic.height;
	}

	m_bitCard->LoadPic(pic_cardl);
	m_nCardWidth=pic_cardl.width;
	m_nCardHeight=pic_cardl.height;
}

void CMyCard::HideCardMask()
{
	for (int i=0; i<CHAIR_CARDS;i++)
	{
		m_plCardMask[i]->Hide();
		m_plCardMask[i]->Updata();
	}
}

int CMyCard::GetPopLength()
{
	return 14;
}

void CMyCard::GiveCardAni()
{
	if(m_nStatus != CARD_STATUS_INHAND)
	{
		return;
	}

	RECT rc=GetMainGameRect();
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;

	if(m_nDrawIndex==0)
	{
		//原来牌的设置
		if (m_nCardSize==CARD_SIZE_MIDDLE)
			m_bitCard->LoadPic(Res_GetPICStruct("牌正面71x96",GetCardPic(m_nCardID)));
		else if (m_nCardSize==CARD_SIZE_BIG)
			m_bitCard->LoadPic(Res_GetPICStruct("牌正面82x110",GetCardPic(m_nCardID)));
		else
			m_bitCard->LoadPic(Res_GetPICStruct("牌正面98x132",GetCardPic(m_nCardID)));
		
		if (bnFastSelect)
		{
			SetAlpha(0.7);
			m_bitCard->SetBackColor(RGBA(0,0,255,80));
		}
		else if (bnTouch)
		{
			SetAlpha(0.9);
			m_bitCard->SetBackColor(RGBA(0,0,255,80));
		}

		m_bitCard->ScaleTo(m_nCardWidth,m_nCardHeight);

		//显示动画牌的设置
		CMyCard *aniCard;
		PGL_MSG_CLASS(aniCard, CMyCard);
        if (m_nCardSize==CARD_SIZE_MIDDLE)
			aniCard->m_bitCard->LoadPic(aniCard->Res_GetPICStruct("牌背面71x96"));
		else if (m_nCardSize==CARD_SIZE_BIG)
			aniCard->m_bitCard->LoadPic(aniCard->Res_GetPICStruct("牌背面82x110"));
		else
		    aniCard->m_bitCard->LoadPic(aniCard->Res_GetPICStruct("牌背面98x132"));

		aniCard->m_bitCard->ScaleTo(m_nCardSmallWidth,m_nCardSmallHeight);

		//设置牌动画前后位置及层级
		POINT pt2=GetInHandPosition();
		
		m_Position.x = pt2.x;
		m_Position.y = pt2.y;
		aniCard->m_Position.x = pt2.x;
		aniCard->m_Position.y = pt2.y;
		MoveTo(pt2.x, pt2.y);

		POINT pt1;
		pt1.x = w/2;
		pt1.y = h/2;
		aniCard->MoveTo(pt1.x, pt1.y);

		SetFloor(CARD_FLOOR_INHAND+m_nUnitIndex*10);
        aniCard->SetFloor(FLOOR_RESULT+501+m_nUnitIndex*10);

		//设置动画
		DWORD t = 200;
		
		double speed=GetSpeed(pt1.x, pt1.y, pt2.x, pt2.y, t);
		
		BeginGroup();
	    APPLY_DirectShow(aniCard,TRUE);
		NextGroup();
		APPLY_MoveObject(aniCard,pt2,speed,0,0, &(SNG_MSG)CreateMsg(GetHandle(), TCSG_CALLBACKMSG_MOVIE_STOPOBJECT, (WPARAM)aniCard));
		NextGroup();
        APPLY_DirectShow(aniCard,FALSE);
		APPLY_DirectShow(this, TRUE);
	    EndGroup();
	}
	else
	{	
		if (bnFace&&m_nCardID>=0&&m_nCardID<TOTAL_CARDS)
		{
			if (m_nCardSize==CARD_SIZE_MAX)
				m_bitCard->LoadPic(Res_GetPICStruct("牌正面71x96",GetCardPic(m_nCardID)));
			else
				m_bitCard->LoadPic(Res_GetPICStruct("牌正面50x68",GetCardPic(m_nCardID)));
		}
		else
		{
			if (m_nCardSize==CARD_SIZE_MAX)
				m_bitCard->LoadPic(Res_GetPICStruct("牌背面71x96"));
			else
				m_bitCard->LoadPic(Res_GetPICStruct("牌背面50x68"));
		}
		m_bitCard->ScaleTo(m_nCardSmallWidth,m_nCardSmallHeight);

		POINT pt2=GetInHandPosition();
		
		m_Position.x = pt2.x;
		m_Position.y = pt2.y;

		POINT pt1;
		pt1.x = w/2;
		pt1.y = h/2;
		MoveTo(pt1.x, pt1.y);

		SetFloor(CARD_FLOOR_INHAND-m_nUnitIndex*10);
		DWORD t = 200;
		
		double speed=GetSpeed(pt1.x, pt1.y, pt2.x, pt2.y, t);
		
		BeginGroup();
		APPLY_DirectShow(this,TRUE);
		NextGroup();
		//APPLY_MoveObject(this,pt2,speed,0,0, NULL);
		APPLY_MoveObject(this,pt2,speed,0,0, &(SNG_MSG)CreateMsg(GetHandle(), TCSG_CALLBACKMSG_MOVIE_STOPOBJECT, (WPARAM)this));
	    EndGroup();
	}
}