#include "StdAfx.h"
using namespace ShinyNova;

#define MSG_CARD_MASTER_LOOK_CHAIR   MSG_CUSTOMER_BEGIN+1
#define MSG_CARD_MASTER_SHOW_POPMENU MSG_CUSTOMER_BEGIN+2
#define MSG_CARD_MASTER_SHOW_ALL     MSG_CUSTOMER_BEGIN+3   //展开全部玩家
#define MSG_CARD_MASTER_HIDE_ALL     MSG_CUSTOMER_BEGIN+4   //关闭全部玩家


CXyCardMaster::CXyCardMaster():m_dwMode(2),m_nCardCount(0),m_PopMenu(NULL)
{
	//XygInitChairCards(m_nCardID,TOTAL_CARDS);
	memset(m_PlayerPlane,0,sizeof(m_PlayerPlane));
	//m_nCardCount=0;
	ClearAllCard();
}

CXyCardMaster::~CXyCardMaster()
{

}

CGameWinApp*  CXyCardMaster::GetGameWinApp()
{
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
	return pApp;
}

void CXyCardMaster::OnPopMenu(POINT pt)
{
	if (!m_PopMenu)
	{
		PGL_MSG_CLASS(m_PopMenu,CXyPopMenu);
		m_PopMenu->CreateGUI(100,30,12);
		for(int i=0;i<TOTAL_CHAIRS;i++)
		{
			if (i!=GetGameWinApp()->GetChairNO())
				m_PopMenu->PushButton(GetGameWinApp()->GetUserNameByChairNO(i),CreateMsg(GetHandle(),MSG_CARD_MASTER_LOOK_CHAIR,(int)i));
		}
		m_PopMenu->PushButton("全部展开",CreateMsg(GetHandle(),MSG_CARD_MASTER_SHOW_ALL));
		m_PopMenu->PushButton("全部关闭",CreateMsg(GetHandle(),MSG_CARD_MASTER_HIDE_ALL));
		m_PopMenu->PushButton("取消",CreateMsg(m_PopMenu->GetHandle(),MSG_PUBLIC_HIDE));
		
		m_PopMenu->SetFloor(GetZ()+100);
		AddChild(m_PopMenu,OBJECT_LOCK_ONLY_HIDE|OBJECT_LOCK_COORDINATE|OBJECT_LOCK_FLOOR|OBJECT_LOCK_UPDATA);
		m_PopMenu->SetAutoMsg(MSG_INPUT_LOST_FOCUS,m_PopMenu->GetHandle(),MSG_PUBLIC_HIDE);
		m_PopMenu->EnableGetFocus(TRUE);
	}
	SetKeyBoardFocus(m_PopMenu->GetHandle());

	pt.x+=m_PopMenu->GetWidth()/2;
	pt.y-=m_PopMenu->GetHeight()/2;

	if (pt.x+m_PopMenu->GetWidth()/2>GetScreenWidth())
		pt.x=GetScreenWidth()-m_PopMenu->GetWidth()/2;
	if (pt.y-m_PopMenu->GetHeight()/2<0)
		pt.y=m_PopMenu->GetHeight();


	m_PopMenu->MoveTo(pt.x,pt.y);
	m_PopMenu->Show();
	m_PopMenu->Updata();
}

BOOL CXyCardMaster::OnMsg(SNG_MSG& Msg)
{
	switch(Msg.msgID)
	{
		case MSG_INPUT_MOUSE_RIGHT_DOWN:
		{
			MOUSEDATA* data=(MOUSEDATA*)Msg.param2;
			OnPopMenu(data->MS_CurrectPoint);
		}
		break;
		case MSG_CARD_MASTER_SHOW_POPMENU:
		{
			POINT pt={GetX(),GetTop()};
			if (m_PopMenu&&m_PopMenu->IsShow())
			{
				m_PopMenu->Hide();
				m_PopMenu->Updata();
			}
			else
				OnPopMenu(pt);
		}
		break;
		case MSG_CARD_MASTER_LOOK_CHAIR:
		{
			int chairno=(int)Msg.param1;
			OnCheckPlayer(chairno);
			m_PopMenu->Hide();
			m_PopMenu->Updata();
			
		}
		break;
		case MSG_CARD_MASTER_SHOW_ALL:
		{
			for(int i=0;i<TOTAL_CHAIRS;i++)
			{
				if (i!=GetGameWinApp()->GetChairNO())
					OnCheckPlayer(i);
			}
			m_PopMenu->Hide();
			m_PopMenu->Updata();

		}
		break;
		case MSG_CARD_MASTER_HIDE_ALL:
			{
				int myIndex=RUL_GetDrawIndexByChairNO(GetGameWinApp()->GetChairNO());
				for(int i=0;i<TOTAL_CHAIRS;i++)
				{
					if (i==myIndex)
						continue;

					if (m_PlayerPlane[i]&&m_PlayerPlane[i]->IsShow())
					{
						m_PlayerPlane[i]->Hide();
						m_PlayerPlane[i]->Updata();
					}
				}
				m_PopMenu->Hide();
				m_PopMenu->Updata();

			}
		break;
		default:
			return CShinyNovaGui::OnMsg(Msg);
	}
	return TRUE;
}

void CXyCardMaster::OnCheckPlayer(int chairno)
{
	int nDrawIndex=RUL_GetDrawIndexByChairNO(chairno);
	
	if (m_PlayerPlane[nDrawIndex]==NULL)
	{
		SNG_PIC pic=Res_GetPICStruct("bit_cardmaster_player");
		m_PlayerPlane[nDrawIndex]=CreatePlane(pic.width,pic.height);
		m_PlayerPlane[nDrawIndex]->TransGUI();
		m_PlayerPlane[nDrawIndex]->SetFloor(FLOOR_CARDMASTER+chairno);
		m_PlayerPlane[nDrawIndex]->SetMoveAble(TRUE);
	
		CXyStandButton* btn_close;
		PGL_MSG_CLASS(btn_close,CXyStandButton);
		btn_close->CreateGUI(Res_GetPICStruct("btn_redcross"),&(SNG_MSG)CreateMsg(m_PlayerPlane[nDrawIndex]->GetHandle(),MSG_PUBLIC_HIDE));
		btn_close->SetFloor(m_PlayerPlane[nDrawIndex]->GetZ()+0.1);
		btn_close->MoveTo(m_PlayerPlane[nDrawIndex]->GetRight()-btn_close->GetWidth()/2-7,m_PlayerPlane[nDrawIndex]->GetTop()+btn_close->GetHeight()/2+3);
		m_PlayerPlane[nDrawIndex]->AddChild(btn_close,OBJECT_LOCK_ALL);

	}

	//绘制

	RePaint(chairno);


	//移动到位置
	FitPlayerSize(nDrawIndex);
}

void CXyCardMaster::OnSize()
{
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		if (m_PlayerPlane[i]
			&&m_PlayerPlane[i]->IsShow())
		{
			FitPlayerSize(i);
		}
	}

	MoveTo(GetScreenWidth()-GetWidth()/2,GetScreenHeight()-30-GetHeight()/2);
	Updata();
}

void CXyCardMaster::FitPlayerSize(int nDrawIndex)
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int wScreen=rc.right-rc.left;
	int hScreen=rc.bottom-rc.top;

	CString str;
	str.Format("玩家姓名板%d",nDrawIndex);
	PObject* pPlayer=GetObject(str);
	if (!pPlayer) return;

	POINT pt={pPlayer->GetX(),pPlayer->GetY()};


	int w=m_PlayerPlane[nDrawIndex]->GetWidth();
	int h=m_PlayerPlane[nDrawIndex]->GetHeight();
	if (pt.x-w/2<x)
		pt.x=w/2+x;

	if (pt.x+w/2>x+wScreen)
		pt.x=x+wScreen-w/2;

	if (pt.y-h/2<y)
		pt.y=h/2+y;

	if (pt.y+h/2>hScreen+y)
		pt.y=y+hScreen-h/2;
	
	m_PlayerPlane[nDrawIndex]->MoveTo(pt.x,pt.y);
	m_PlayerPlane[nDrawIndex]->Show();
	m_PlayerPlane[nDrawIndex]->Updata();
}

void CXyCardMaster::OnIniData()
{
	SNG_PIC pic=Res_GetPICStruct("bit_cardmaster");
	PlaneObject* pl=CreatePlane(pic.width,pic.height);
	pl->DrawPic(pic);
	pl->TransGUI();
	pl->SetFloor(FLOOR_CARDMASTER);
	pl->SetMoveAble(TRUE);
	SetFrame(pl);

	CXyStandButton* btn_close;
	PGL_MSG_CLASS(btn_close,CXyStandButton);
	btn_close->CreateGUI(Res_GetPICStruct("btn_redcross"),&(SNG_MSG)CreateMsg(GetHandle(),MSG_PUBLIC_HIDE));
	btn_close->SetFloor(pl->GetZ()+0.1);
	btn_close->MoveTo(pl->GetRight()-btn_close->GetWidth()/2-13,pl->GetTop()+btn_close->GetHeight()/2+6);
	pl->AddChild(btn_close,OBJECT_LOCK_ALL);

	CXyStandButton* btn_popmenu;
	PGL_MSG_CLASS(btn_popmenu,CXyStandButton);
	btn_popmenu->CreateGUI(Res_GetPICStruct("btn_popmenu"),&(SNG_MSG)CreateMsg(GetHandle(),MSG_CARD_MASTER_SHOW_POPMENU));
	btn_popmenu->SendMsgWhenLeftDown();
	btn_popmenu->SetFloor(pl->GetZ()+0.1);
	btn_popmenu->MoveTo(pl->GetX(),pl->GetTop()+btn_close->GetHeight()/2+6);
	pl->AddChild(btn_popmenu,OBJECT_LOCK_ALL);


	pl->SetAutoMsg(MSG_INPUT_MOUSE_RIGHT_DOWN,CreateMsg(GetHandle(),MSG_INPUT_MOUSE_RIGHT_DOWN));
}

void CXyCardMaster::RePaint(int nChairNO)
{
	int nDrawIndex=RUL_GetDrawIndexByChairNO(nChairNO);
	SNG_PIC pic=Res_GetPICStruct("bit_cardmaster_player");

	m_PlayerPlane[nDrawIndex]->Erasure();
	m_PlayerPlane[nDrawIndex]->DrawPic(pic);
	m_PlayerPlane[nDrawIndex]->SetTextSize(12);
	m_PlayerPlane[nDrawIndex]->SetTextColor(RGB_WHITE);
	m_PlayerPlane[nDrawIndex]->TextOutEx(m_PlayerPlane[nDrawIndex]->GetWidth()/2,10,GetGameWinApp()->GetUserNameByChairNO(nChairNO));
	int nCount=0;
	int x=0;
	int y=0;
	int nLine=10;
	int nCardWidth=12;
	int nCardHeight=28;
	for(int i=0;i<CHAIR_CARDS;i++)
	{
		x=13+(nCount%nLine)*nCardWidth;
		y=20+(nCount/nLine)*nCardHeight;

		if (m_nChairCard[nDrawIndex][i]==-1)
			break;
		else if (m_nChairCard[nDrawIndex][i]==CARDID_UNKOWN)
		{
			//未知牌
			SNG_PIC card_pic=Res_GetPICStruct("牌背面50x68");
			card_pic.width=nCardWidth;
			card_pic.height=nCardHeight;
			m_PlayerPlane[nDrawIndex]->DrawPic(card_pic,x,y,nCardWidth,nCardHeight);
			nCount++;
		}
		else
		{
			//已知牌
			int nPicIndex=GetCardPic(m_nChairCard[nDrawIndex][i]);
			SNG_PIC card_pic=Res_GetPICStruct("牌正面50x68",nPicIndex);
			card_pic.width=nCardWidth;
			card_pic.height=nCardHeight;
			m_PlayerPlane[nDrawIndex]->DrawPic(card_pic,x,y,nCardWidth,nCardHeight);
			nCount++;
		}
	}

}

void CXyCardMaster::ReLoad()
{
	if (!m_PopMenu)
		return;

	int nIndex=0;
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		if (i!=GetGameWinApp()->GetChairNO())
		{
			CXyPopButton* btn=m_PopMenu->GetButton(nIndex);
			btn->SetText(GetGameWinApp()->GetUserNameByChairNO(i));
			nIndex++;
		}
	}
}

void CXyCardMaster::RePaint()
{
	int nCardV[TOTAL_CARDS];
	memset(nCardV,0,sizeof(nCardV));
	for(int i=0;i<m_nCardCount;i++)
	{
		nCardV[i]=SK_GetCardPRIEx(m_nCardID[i],1)*1000+SK_GetCardShapeEx(m_nCardID[i]);
	}

	XygReversalMoreByValue(m_nCardID,nCardV,m_nCardCount);

	int x=5;
	int y=40;

	PlaneObject* pl=(PlaneObject*)GetFrame();
	pl->Erasure();
	pl->DrawPic(Res_GetPICStruct("bit_cardmaster"));
	pl->SetTextSize(14);
	CString str;
	str.Format("剩余%d张",m_nCardCount);
	pl->SetTextColor(RGB_WHITE);
	pl->TextOutEx(50,16,str);

	//花色
	if (m_dwMode==1)
	{
		for(int	j=PK_CS_SPADE;j>=PK_CS_DIAMOND;j--)
			pl->DrawPic(Res_GetPICStruct("bit_cardmaster_shape",j),x,y+(5+20*CARD_PACKS)*j,18,19);
		
		//大王
		int nSortNum[4][15];
		memset(nSortNum,0,sizeof(nSortNum));
		x+=20;
		int nLastCardIndex=15;
		int nUniteIndex=0;
		for(i=0;i<m_nCardCount;i++)
		{
			//	SNG_PIC pic=GetPicByID(m_nCardID[i]);
			int index=SK_GetCardIndexEx(m_nCardID[i]);
			int shape=SK_GetCardShapeEx(m_nCardID[i]);
			
			if (index==14)
			{
				nUniteIndex=0;
				shape=1;
			}
			else if (index==15)
			{
				nUniteIndex=0;
				shape=0;
			}
			else if (index!=nLastCardIndex)
			{
				nLastCardIndex=index;
				nUniteIndex++;
			}
			
			pl->DrawPic(Res_GetPICStruct("bit_cardrank",index-1),x+nUniteIndex*16,y+shape*(20*CARD_PACKS+7)+nSortNum[shape][nUniteIndex]*20,16,13);
			nSortNum[shape][nUniteIndex]++;
		}
	}
	if (m_dwMode==2)
	{
	//	for(int	j=PK_CS_SPADE;j>=PK_CS_DIAMOND;j--)
	//		pl->DrawPic(Res_GetPICStruct("bit_cardmaster_shape",j),x,y+(5+20*CARD_PACKS)*j,18,19);
		
		//大王
		int nSortNum[15];
		memset(nSortNum,0,sizeof(nSortNum));
		x=10;
		y=30;
		int nLastCardIndex=15;
		int nUniteIndex=0;
		for(i=0;i<m_nCardCount;i++)
		{
			//	SNG_PIC pic=GetPicByID(m_nCardID[i]);
			int index=SK_GetCardIndexEx(m_nCardID[i]);
			int shape=SK_GetCardShapeEx(m_nCardID[i]);
			
			if (index==14)
			{
				nUniteIndex=0;
				shape=1;
			}
			else if (index==15)
			{
				nUniteIndex=0;
				shape=0;
			}
			else if (index!=nLastCardIndex)
			{
				nLastCardIndex=index;
				nUniteIndex++;
			}
			
			pl->DrawPic(Res_GetPICStruct("bit_cardrank",index-1),x+nUniteIndex*16,y+nSortNum[nUniteIndex]*14,16,13);
			nSortNum[nUniteIndex]++;
		}
	}
	else if (m_dwMode==3)
	{
		int nSortNum[15];
		memset(nSortNum,0,sizeof(nSortNum));
	
		x=0;
		int nLastCardIndex=15;
		int nUniteIndex=0;
		for(int j=0;j<15;j++)
			pl->DrawPic(Res_GetPICStruct("bit_cardrank",14-j),x+j*16,y,16,13);

		for(i=0;i<m_nCardCount;i++)
		{
			int index=SK_GetCardIndexEx(m_nCardID[i]);
			int shape=SK_GetCardShapeEx(m_nCardID[i]);
			
			if (index==14)
			{
				nUniteIndex=0;
			}
			else if (index==15)
			{
				nUniteIndex=0;
			}
			else if (index!=nLastCardIndex)
			{
				nLastCardIndex=index;
				nUniteIndex++;
			}
			
			
			nSortNum[nUniteIndex]++;
		}
	}
	else
	{
		//大王
		int nSortNum[15];
		memset(nSortNum,0,sizeof(nSortNum));
	//	x+=20;
		x=0;
		int nLastCardIndex=15;
		int nUniteIndex=0;
		for(i=0;i<m_nCardCount;i++)
		{
			SNG_PIC pic=GetPicByID(m_nCardID[i]);
			int index=SK_GetCardIndexEx(m_nCardID[i]);
			int shape=SK_GetCardShapeEx(m_nCardID[i]);
			
			if (index==14)
			{
				nUniteIndex=0;
				//shape=1;
			}
			else if (index==15)
			{
				nUniteIndex=0;
				//shape=0;
			}
			else if (index!=nLastCardIndex)
			{
				nLastCardIndex=index;
				nUniteIndex++;
			}
			
			pl->DrawPic(pic,x+nUniteIndex*pic.width,y+nSortNum[nUniteIndex]*pic.height,pic.width,pic.height);
			nSortNum[nUniteIndex]++;
		}
	}
	
	pl->Updata();
}

void CXyCardMaster::ClearAllCard()
{
	XygInitChairCards(m_nCardID,TOTAL_CARDS);
	for(int i=0;i<TOTAL_CHAIRS;i++)
	{
		XygInitChairCards((int*)m_nChairCard,CHAIR_CARDS*TOTAL_CHAIRS);
		if (m_PlayerPlane[i])
		{
			m_PlayerPlane[i]->Hide();
			m_PlayerPlane[i]->Updata();
		}
	}
	m_nCardCount=0;
}

void CXyCardMaster::PushCard(int nCardID) //增加一张牌
{
	for(int i=0;i<TOTAL_CARDS;i++)
		if (m_nCardID[i]==-1)
		{
			m_nCardID[i]=nCardID;
			m_nCardCount++;
			return;
		}
		
}

void CXyCardMaster::PopCard(int nCardID)  //去掉一张牌
{
	for(int i=0;i<TOTAL_CARDS;i++)
		if (m_nCardID[i]==nCardID)
		{
			m_nCardID[i]=-1;
			m_nCardCount--;
		}

	for(i=0;i<TOTAL_CARDS-1;i++)
	{
		if (m_nCardID[i]==-1)
		{
			m_nCardID[i]=m_nCardID[i+1];
			m_nCardID[i+1]=-1;
		}
	}

	if (IsShow())
		RePaint();
}

void CXyCardMaster::ReCordChairCard(int nChairNO,int nCardID)
{
	int nDrawIndex=RUL_GetDrawIndexByChairNO(nChairNO);
	for(int i=0;i<CHAIR_CARDS;i++)
	{
		if (m_nChairCard[nDrawIndex][i]==-1)
		{
			m_nChairCard[nDrawIndex][i]=nCardID;
			return;
		}
		else if (m_nChairCard[nDrawIndex][i]==CARDID_UNKOWN
			&&nCardID!=CARDID_UNKOWN)
		{
			m_nChairCard[nDrawIndex][i]=nCardID;
			return;
		}
	}
}


SNG_PIC CXyCardMaster::GetPicByID(int nCardID)
{
	SNG_PIC pic=Res_GetPICStruct("bit_cardmaster_small",GetCardPic(nCardID));
//	pic.width=12;
//	pic.height=30;

	return pic;
}

int  CXyCardMaster::RUL_GetDrawIndexByChairNO(int nChairNO)
{
	int index=0;
	CSKGameWinApp* pGameApp=(CSKGameWinApp*)AfxGetApp();
	int chairno=pGameApp->GetChairNO();
	int nTotalChair = GetGameWinApp()->GetTableChairCount();
	for(int i=0;i<nTotalChair;i++)
	{
		if (chairno==nChairNO)
			return index;
		index++;
		chairno=(chairno+1)% nTotalChair;
	}
	return -1;
}