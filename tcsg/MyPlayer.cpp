#include "StdAfx.h"

CMyPlayer::CMyPlayer()
{
	m_digitScore	=	NULL;
	m_plBomb		=	NULL;
	m_bitAnimation	=	NULL;
	m_bitActor		=	NULL;
	m_plShowCloth	=   NULL;
	m_nActorPos		=   0 ;
	m_bitBanker		=	NULL;
	
	m_bBanker		=   FALSE;
}	

CMyPlayer::~CMyPlayer()
{
	
}

void CMyPlayer::FitScrren()//刷新位置
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;

	POINT pt={0,0};
	SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");

	CMyGameFrame* frame=(CMyGameFrame*)(theApp.GetGameFrame());	
	BOOL bTightUI = frame->IsTightUI();
	BOOL bMaxClient = frame->IsMaxClient();
	
	//小窗

	int nOff = 20;
	
	switch(m_nDrawIndex)
	{
	case 0:    //本家
		{
			pt.x=w/2-160;
			if (bTightUI)
			{
				pt.y=h-pic.height/2-nOff;
			}
			else
			{
				if (bMaxClient)
				{
					pt.y=h-pic.height/2-50;
				}
				else
				{
					pt.y=h-pic.height/2-nOff;
				}
			}

			if (m_plShowCloth)
			{
				m_plShowCloth->MoveTo(pt.x+120, pt.y);
				m_plShowCloth->Updata();
			}
		}
		break;
	case 1:    //左
		{
			pt.x=pic.width/2+nOff;
			pt.y=h/2-y;
			
			if (m_plShowCloth)
			{
				m_plShowCloth->MoveTo(pt.x+120, pt.y+60);
				m_plShowCloth->Updata();
			}
		}
		break;
	case 2:    //上
		{
			nOff = 15;
			pt.x=w/2-160;
			pt.y=pic.height/2+nOff;

			if (m_plShowCloth)
			{
				m_plShowCloth->MoveTo(pt.x+120, pt.y+60);
				m_plShowCloth->Updata();
			}
		}
		break;
	case 3:    //右
		{
			pt.x=w-pic.width/2-nOff;
			pt.y=h/2-y;//-nOff;//-pic.height/2;//h/3-nOff;

			if (m_plShowCloth)
			{
				m_plShowCloth->MoveTo(pt.x-120, pt.y+60);
				m_plShowCloth->Updata();
			}
		}
		break;
	default:
		break;
	}

	pt.x+=x;
	pt.y+=y;

 	MoveTo(pt.x,pt.y);
 	Updata();

	FitTalkPosition();
	FitActorPosition();
	FitAnimationPosition();
	FitBankerPos();

	if (m_nChairNO>=0)
		DrawScore();
}

void CMyPlayer::ShowBanker(BOOL bShow)
{
	if (m_bitBanker)
	{
		FitBankerPos();

		if (bShow)
		{
			m_bitBanker->Show();
			m_bitBanker->Updata();
		}
		else
		{
			m_bitBanker->Hide();
			m_bitBanker->Updata();
		}
	}
}

void CMyPlayer::FreshBomb(int nBombCount)
{
	if (nBombCount==0)
	{
		m_plBomb->Hide();
		m_plBomb->Updata();
		return;
	}

	m_plBomb->Erasure();
	m_plBomb->DrawPic(Res_GetPICStruct("炸弹图标"),0,0,50,30);

	CXyDigit* pen=(CXyDigit*)GetObject("DigitPen");
	CString str;
	str.Format("%d",nBombCount);
	pen->WriteString(m_plBomb,45,7,str);
	m_plBomb->Show();
//	m_plBomb->ShowBorderLine(RGB_GREEN);
	m_plBomb->Updata();
}

void CMyPlayer::DrawScore()
{

}

void  CMyPlayer::Restart()
{
	m_bnCurrentChair=FALSE;
	m_digitScore->SetDigit(0);
	m_digitScore->Hide();
	m_digitScore->Updata();
	m_restCard->Hide();
	m_restCard->Updata();
	DrawPlayerBoard();
	HideReady();
	FreshBomb(0);
	m_nActorPos=0;
	HideActor();
	HideAnimation();
}

void CMyPlayer::AddScore(int nScore)
{
	if (nScore==0)
		return;
	m_digitScore->Show();
	m_digitScore->Updata();
	m_digitScore->StopDigit();
	int n=m_digitScore->GetDigit()+nScore;

	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
	{
		m_digitScore->ScrollDigitTo(n,nScore/5+1,100,&(SNG_MSG)CreateMsg(GetHandle(),MYPLAYER_CALLBACK_FRESHSCORE));	
	}
	DrawScore();

}



void CMyPlayer::CreateGUI(int nDrawIndex) 
{
	m_nDrawIndex=nDrawIndex;
	CGameApp* pApp=(CGameApp*)AfxGetApp();

	//背景板
	SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
	m_plBoard=CreatePlane(pic.width,pic.height);
	m_plBoard->SetFloor(FLOOR_PLAYER_NAME);
	m_plBoard->SetTextColor(RGB_WHITE);

	m_bitBanker = CreateBitMap(Res_GetPICStruct(_T("庄")));
	m_bitBanker->SetFloor(FLOOR_BTN+1);

	m_plBomb=CreatePlane(80,30);
	m_plBomb->SetFloor(m_plBoard->GetZ()+0.1);
	m_plBomb->MoveTo(m_plBoard->GetLeft()+m_plBoard->GetWidth()/2+70+m_plBomb->GetWidth()/2,GetY());
	m_plBoard->AddChild(m_plBomb,OBJECT_LOCK_POSITION|OBJECT_LOCK_UPDATA);

	//服装展示
	m_plShowCloth=CreatePlane(135,270);
	if (nDrawIndex == 1 || nDrawIndex == 4)
	{
		m_plShowCloth->SetFloor(m_plBoard->GetZ()+1);
	}
	else
	{
		m_plShowCloth->SetFloor(m_plBoard->GetZ()+0.1);
	}
	
	m_plShowCloth->RotateToAngle(0);


	{//分数
		PGL_MSG_CLASS(m_digitScore,CXyDigit);

		if(pApp->IsMatch())
		{
			m_digitScore->CreateDigit("ScoreDigitMatch",DIGIT_FLAGS_MID_JUSITIFY);
			
			m_digitScore->LoadWord("分","ScoreUniteMatch");
		}
		else
		{
			m_digitScore->CreateDigit("ScoreDigit",DIGIT_FLAGS_MID_JUSITIFY);
			
			m_digitScore->LoadWord("分","ScoreUnite");
		}

		m_digitScore->ShowUnit("分");
		m_digitScore->SetFloor(m_plBoard->GetZ()+0.1);
		m_plBoard->AddChild(m_digitScore,OBJECT_LOCK_POSITION|OBJECT_LOCK_UPDATA);
	}


	{
		PGL_MSG_CLASS(m_restCard,CXyDigit);

		if (theApp.IsUseBigUI())
		{
			m_restCard->CreateDigit("放大版_数字_牌数",DIGIT_FLAGS_MID_JUSITIFY);	
			m_restCard->LoadWord("张","放大版_word_zhang");
		}
		else
		{
			m_restCard->CreateDigit("digit_restcard",DIGIT_FLAGS_MID_JUSITIFY);	
			m_restCard->LoadWord("张","word_zhang");
		}

		m_restCard->ShowUnit("张");
		m_restCard->SetFloor(m_plBoard->GetZ()+0.1);
	}

	//角色状态
	{
		m_bitStatus=CreateBitMap(Res_GetPICStruct("animation_AutoPlay"));
		m_bitStatus->MoveTo(m_plBoard->GetX(),m_plBoard->GetY());
		m_bitStatus->SetFloor(m_plBoard->GetZ()+10);
		m_plBoard->AddChild(m_bitStatus,OBJECT_LOCK_POSITION|OBJECT_LOCK_UPDATA);
	}

	SetFrame(m_plBoard);
	CString szName;
	szName.Format("玩家姓名板%d",m_nDrawIndex);
	ReName(szName);
}

BOOL CMyPlayer::OnMsg(SNG_MSG& Msg)
{

	if (Msg.msgID==MYPLAYER_CALLBACK_FRESHSCORE)
	{
		DrawScore();
		return TRUE;
	}

	return CGamePlayer::OnMsg(Msg);
}

void CMyPlayer::LoadPlayer(int nChairNO)
{
	if (nChairNO==-1)
		return;

	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();

	m_nChairNO=nChairNO;
	DrawPlayerBoard();

	//段位
	int nDuan=(pApp->GetDuan(m_nChairNO));
	if (nDuan>=0)
	{
		m_bitStatus->LoadPic(Res_GetPICStruct("bit_Duan",nDuan));
		m_bitStatus->Show();
		m_bitStatus->Updata();
	}
	else
	{
		m_bitStatus->Hide();
		m_bitStatus->Updata();
	}

	if (pApp->IsMatch())
	{
		//填写分数
		srand(GetTickCount());
		SetMatchScore(pApp->m_MatchPlayer[nChairNO].useronmatch.nScore,pApp->m_MatchTable.nMatchMaxScore);
	}
	//SetMatchScore(100,100);
	
	//统一用一种方法打开,点击
	m_plBoard->StopAutoMsg(MSG_INPUT_TOUCH);
	m_plBoard->StopAutoMsg(MSG_INPUT_UNTOUCH);
	m_plBoard->StopAutoMsg(MSG_INPUT_MOUSE_LEFT_DOWN);
	m_plBoard->StopAutoMsg(MSG_INPUT_MOUSE_RIGHT_DOWN);
	m_plBoard->SetAutoMsg(MSG_INPUT_TOUCH,GetHandle(),PLAYER_CALLBACK_TOUCH_BOARD);
	m_plBoard->SetAutoMsg(MSG_INPUT_UNTOUCH,GetHandle(),PLAYER_CALLBACK_UNTOUCH_BOARD);
	m_plBoard->SetAutoMsg(MSG_INPUT_MOUSE_LEFT_DOWN,pApp->GetGameFrame()->GetHandle(),GAME_MSG_TOUCH_USERBOARD,pApp->GetPlayerIDByChairNO(m_nChairNO));
	m_plBoard->SetAutoMsg(MSG_INPUT_MOUSE_RIGHT_DOWN,pApp->GetGameFrame()->GetHandle(),GAME_MSG_UNTOUCH_USERBOARD,pApp->GetPlayerIDByChairNO(m_nChairNO));

	//初始化数据
	m_digitScore->StopDigit();
	m_digitScore->SetDigit(0);
	m_digitScore->Hide();
	m_digitScore->Updata();
	m_plBomb->Hide();
	m_plBomb->Updata();


	Show();
	Updata();

	
	
// 	ShowPlayerCloth();
}

void CMyPlayer::UnLoadPlayer()
{
	m_plBoard->StopAutoMsg(MSG_INPUT_TOUCH);

	m_digitScore->Hide();
	m_digitScore->Updata();
	m_plBomb->Hide();
	m_plBomb->Updata();
	m_restCard->Hide();
	m_restCard->Updata();
	HideReady();
	HideTalk();

	m_nChairNO=-1;
	m_bnCurrentChair=FALSE;

	AutoPlay(FALSE);
	Hide();
	Updata();

	HidePlayerCloth();
}

POINT CMyPlayer::GetClockPosition(int nStatus)
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;

	POINT pt={GetX(),GetY()};
	SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
	int nOff = (h/2-pic.width/2)/3;
	if (m_nDrawIndex==1)//左边
	{
		pt.x=180;
		pt.y=h/2-nOff;
	}
	else if (m_nDrawIndex==2)
	{
		pt.x=w-180;
		pt.y=h/2-nOff;
	}
	else //自己
	{
		CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
		if (pApp->IsLookOn())
		{
			pt.x=w/2;
			pt.y=h-18-30-Sys_GetIntStatus(GAME_STATUS_CARD_HEIGHT)-Sys_GetIntStatus(GAME_STATUS_PLAYER_HEIGHT);
		}
		else
		{
			if (1==nStatus)	//一手出牌
			{
				pt.x=w/2-80;
				pt.y=h-18-30-Sys_GetIntStatus(GAME_STATUS_CARD_HEIGHT)-Sys_GetIntStatus(GAME_STATUS_PLAYER_HEIGHT);		
			}
			else if (0==nStatus)	//跟牌
			{
				pt.x=w/2-160;
				pt.y=h-18-30-Sys_GetIntStatus(GAME_STATUS_CARD_HEIGHT)-Sys_GetIntStatus(GAME_STATUS_PLAYER_HEIGHT);
			}
			else					//叫庄时
			{
				pt.x=w/2-200;
				pt.y=h-18-30-Sys_GetIntStatus(GAME_STATUS_CARD_HEIGHT)-Sys_GetIntStatus(GAME_STATUS_PLAYER_HEIGHT);
			}
		}
	}

	pt.x+=x;
	pt.y+=y;
	
	return pt;
}

int   CMyPlayer::GetRestCardWidth()
{
	if (m_restCard==NULL)
		 return 0;

	return m_restCard->GetWidth();
}

int   CMyPlayer::GetRestCardHeight()
{
	if (m_restCard==NULL)
		return 0;
	
	return m_restCard->GetHeight();
}

void  CMyPlayer::SetRestCard(int nRestCount)//在指定位置显示牌的剩余张数
{
	if (nRestCount<=0)
	{
		m_restCard->Hide();
		m_restCard->Updata();
		return;
	}
	
	m_restCard->SetDigit(nRestCount);
	m_restCard->Updata();
}

void  CMyPlayer::ShowRestCard(int x,int y)
{
	m_restCard->MoveTo(x,y);
	m_restCard->Show();
	m_restCard->Updata();
}

void CMyPlayer::ShowPlayerCloth()
{
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
	//服装展示
	CXyGameFrame* frame = pApp->GetGameFrame();
	int nClothID = pApp->m_GamePlayer[m_nChairNO].nClothingID;
	if (nClothID!=0 && nClothID!=1 && frame && m_plShowCloth)
	{
		CString szCharacter=GetUserCharacter(nClothID,CHARACTER_TYPE_BIG);
		SNG_PIC headpic=Res_GetPICStruct(szCharacter);
		headpic.x=0;
		headpic.y=0;
		headpic.width=CHARACTER_WIDTH;
		headpic.height=CHARACTER_HEIGHT;
		m_plShowCloth->Erasure();
		m_plShowCloth->DrawPic(headpic);
		m_plShowCloth->SetTextColor(255);
		m_plShowCloth->Show();
		m_plShowCloth->Updata();
	}
}

void CMyPlayer::HidePlayerCloth()
{
	if (m_plShowCloth)
	{
		m_plShowCloth->Hide();
		m_plShowCloth->Updata();
	}
}

BOOL CMyPlayer::IsClothShow()
{
	if (m_plShowCloth
		&& m_plShowCloth->IsShow())
		return TRUE;

	return FALSE;
}

POINT CMyPlayer::GetTalkPosition()
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
	int nOff = (h/2-pic.width/2)/3;
	POINT pt;
	
	{
		if (m_nDrawIndex==0)
		{
			pt.x=x+w/2;
			pt.y=y+h-200;
		}
		else if (m_nDrawIndex==1)
		{
			pt.x=x+200;
			pt.y=y+h/2-nOff;
		}
		else if (m_nDrawIndex==2)
		{
			pt.x=x+w-200;
			pt.y=y+h/2-nOff;
		}
	}

	return pt;
}

void  CMyPlayer::FitTalkPosition()
{
	if (!m_bitTalk)
		return;
	
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
	int nOff = (h/2-pic.width/2)/3;
	
	int nOff2 = 200;
	if (theApp.IsUseBigUI())
		nOff2+=20;
	if (MYDRAWINDEX == m_nDrawIndex)
	{
		m_bitTalk->MoveTo(x+w/2,h*3/4);
	}
	else if (1 == m_nDrawIndex)
	{
		m_bitTalk->MoveTo(GetX()+160,GetY());
	}
	else if (2 == m_nDrawIndex)
	{
		m_bitTalk->MoveTo(x+w/2,GetY());
	}
	else if (3 == m_nDrawIndex)
	{
		m_bitTalk->MoveTo(GetX()-160,GetY());
	}

	m_bitTalk->Updata();
}

void  CMyPlayer::ShowAnimation(LPCTSTR szObject, int nX, int nY)
{
	
	if (!m_bitAnimation)
	{
		m_bitAnimation=CreateBitMap(Res_GetPICStruct(szObject));
		m_bitAnimation->TransGUI();
		m_bitAnimation->SetFloor(FLOOR_ANIMATION);
	}
	else
	{
		StopObject(m_bitAnimation->GetHandle());
		m_bitAnimation->LoadPic(Res_GetPICStruct(szObject));
	}

	FitAnimationPosition(nX,nY);
	m_bitAnimation->Show();
	m_bitAnimation->Updata();
	BeginGroup();
//	APPLY_DirectShow(m_bitAnimation,TRUE);
	APPLY_PlayBitObject(m_bitAnimation,MODE_PLAYBIT_BY_CIRCLE,1);
	NextGroup();
	APPLY_DirectShow(m_bitAnimation,FALSE);
	EndGroup();

}


void  CMyPlayer::ShowAnimation(LPCTSTR szObject, int nStatus, POINT pt)
{
	if (!m_bitAnimation)
	{
		m_bitAnimation=CreateBitMap(Res_GetPICStruct(szObject, nStatus));
		m_bitAnimation->TransGUI();
		m_bitAnimation->SetFloor(FLOOR_ANIMATION);
	}
	else
	{
		StopObject(m_bitAnimation->GetHandle());
		m_bitAnimation->LoadPic(Res_GetPICStruct(szObject, nStatus));
	}
	
	FitAnimationPosition(pt.x,pt.y);
	m_bitAnimation->Show();
	m_bitAnimation->Updata();
}

void CMyPlayer::FitAnimationPosition(int nX, int nY)
{
	if (!m_bitAnimation)
		return;
	
	//动画在玩家信息居中
	if (0==nX && 0==nY)
	{
		POINT pt = GetTalkPosition();
		if (m_nDrawIndex==MYDRAWINDEX)
			pt.y -=50;
		else if (m_nDrawIndex==1)
			pt.x += 50;
		else if (m_nDrawIndex==2)
			pt.x -= 50;
		m_bitAnimation->MoveTo(pt.x, pt.y);
	}
	else	//动画跟着牌
	{
		m_bitAnimation->MoveTo(nX, nY);
	}

	m_bitAnimation->Updata();
}


void  CMyPlayer::ShowAnimation(LPCTSTR szObject,POINT ptBegin, POINT ptEnd)
{
	if (!m_bitAnimation)
	{
		m_bitAnimation=CreateBitMap(Res_GetPICStruct(szObject));
		m_bitAnimation->TransGUI();
		m_bitAnimation->SetFloor(FLOOR_ANIMATION);
	}
	else
	{
		StopObject(m_bitAnimation->GetHandle());
		m_bitAnimation->LoadPic(Res_GetPICStruct(szObject));
	}
	
	m_bitAnimation->MoveTo(ptBegin.x,ptBegin.y);
	m_bitAnimation->Show();
	m_bitAnimation->Updata();
	int move_time=800/(GetMinApplySpace());
	double l=sqrt((ptBegin.x-ptEnd.x)*(ptBegin.x-ptEnd.x)+(ptBegin.y-ptEnd.y)*(ptBegin.y-ptEnd.y));
	double speed=l/move_time;

	APPLY_PlayBitObject(m_bitAnimation,MODE_PLAYBIT_BY_CIRCLE);
	BeginGroup();
//	APPLY_DirectShow(m_bitAnimation,TRUE);
	APPLY_MoveObject(m_bitAnimation,ptEnd,speed);
	NextGroup();
	APPLY_DirectShow(m_bitAnimation,FALSE);
	EndGroup();
}

void CMyPlayer::HideAnimation()
{
	if (m_bitAnimation)
	{
		StopObject(m_bitAnimation->GetHandle());
		m_bitAnimation->Hide();
		m_bitAnimation->Updata();
	}
}

POINT CMyPlayer::GetBombPosition()
{
	POINT pt;
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
	int nOff = (h/2-pic.width/2)/3;
	
	if (m_nDrawIndex==0)
	{
		SNG_PIC pic_cardl=Res_GetPICStruct("牌背面71x96");
		int nCardHeight=pic_cardl.height;
		pt.x = w/2;
		pt.y = h-40-nCardHeight-20-nCardHeight/2-PLAYER_HEIGHT;
	}
	else if (m_nDrawIndex==1)
	{
		pt.x=180;
		pt.y=h/2-nOff;
	}
	else if (m_nDrawIndex==2)
	{
		pt.x=w-180;
		pt.y=h/2-nOff;
	}

	pt.x+=x;
	pt.y+=y;
	return pt;
}


POINT CMyPlayer::GetRocketPosition()
{
	POINT pt;
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
	int nOff = (h/2-pic.width/2)/3;
	
	if (m_nDrawIndex==0)
	{
		SNG_PIC pic_cardl=Res_GetPICStruct("牌背面71x96");
		int nCardHeight=pic_cardl.height;
		pt.x = w/2;
		pt.y = h-40-nCardHeight-20-nCardHeight/2-PLAYER_HEIGHT-120;
	}
	else if (m_nDrawIndex==1)
	{
		pt.x=180;
		pt.y=h/2-nOff;
	}
	else if (m_nDrawIndex==2)
	{
		pt.x=w-180;
		pt.y=h/2-nOff;
	}
	
	pt.x+=x;
	pt.y+=y;
	return pt;
}

void CMyPlayer::GetAirPlanePosition(POINT& ptBegin, POINT& ptEnd)
{
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;

	SNG_PIC pic=Res_GetPICStruct("feiji");

	ptBegin.x = x+w/*-pic.width/2*/;
	ptBegin.y = y+h/2;
	ptEnd.x = x+pic.width/2;
	ptEnd.y = y+h/2;
}

void CMyPlayer::FitBankerPos()
{
	if (m_bitBanker)
	{		
		if(m_nDrawIndex==3)
		{
			m_bitBanker->MoveTo(GetX()-40, GetY()-53);
			m_bitBanker->Updata();
		}
		else
		{
			m_bitBanker->MoveTo(GetX()+40, GetY()-53);
			m_bitBanker->Updata();
		}
	}	
}

void CMyPlayer::FitActorPosition()
{
	if (!m_bitActor)
		return;

	POINT pt;
	RECT rc=GetMainGameRect();
	int x=rc.left;
	int y=rc.top;
	int w=rc.right-rc.left;
	int h=rc.bottom-rc.top;
	SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
	int nOff = (h/2-pic.width/2)/3;
	
	if (m_nDrawIndex==0)
	{
		int nCardHeight = Sys_GetIntStatus(GAME_STATUS_CARD_HEIGHT);
		int nOff2 = 100;
		if (theApp.IsUseBigUI())
			nOff2 += 30;
		pt.x = w/2 - pic.width/2 - nOff2;
		pt.y = h-nCardHeight/2-PLAYER_HEIGHT;
	}
	else if (m_nDrawIndex==1)
	{
		pt.x=pic.height+Sys_GetIntStatus(GAME_STATUS_CARDSMALL_HEIGHT)+50;
		int nOff2 = 80;
		if (theApp.IsUseBigUI())
			nOff2 -= 20;
		pt.y=h/2-nOff-pic.width/2+nOff2;
	}
	else if (m_nDrawIndex==2)
	{
		pt.x=w-pic.height-Sys_GetIntStatus(GAME_STATUS_CARDSMALL_HEIGHT)-48;
		int nOff2 = 80;
		if (theApp.IsUseBigUI())
			nOff2 -= 20;
		pt.y=h/2-nOff-pic.width/2+nOff2;
	}
	pt.x+=x;
	pt.y+=y;

	m_bitActor->MoveTo(pt.x + 100,pt.y+10);
	m_bitActor->Updata();
}

void CMyPlayer::ShowActor(BOOL bAnimation, int nStatus)
{
	SNG_PIC pic;
	BOOL bBanker = (m_nChairNO==theApp.GetGameInfo()->GetBanker());
	if (bAnimation)
	{
		if (bBanker)
		{
			if (nStatus==0)
				pic = Res_GetPICStruct("地主哭");
			else
				pic = Res_GetPICStruct("地主笑");
		}
		else
		{
			if (nStatus==0)
				pic = Res_GetPICStruct("农民哭");
			else
				pic = Res_GetPICStruct("农民笑");
		}
	}
	else
	{
		if (bBanker)
			pic = Res_GetPICStruct("地主", nStatus);
		else
			pic = Res_GetPICStruct("农民", nStatus);
	}

	if (!m_bitActor)
	{
		m_bitActor=CreateBitMap(pic);
		m_bitActor->TransGUI();
		m_bitActor->SetFloor(FLOOR_ANIMATION);
	}
	else
	{
		StopObject(m_bitActor->GetHandle());
		m_bitActor->LoadPic(pic);
	}
	
	FitActorPosition();
	m_bitActor->Show();
	m_bitActor->Updata();
	
	if (bAnimation)
	{
		APPLY_PlayBitObject(m_bitActor,MODE_PLAYBIT_BY_CIRCLE);
	}
}


void CMyPlayer::HideActor()
{
	if (m_bitActor)
	{
		m_bitActor->Hide();
		m_bitActor->Updata();
	}
}

void CMyPlayer::SetCurrentChair(BOOL bn)
{
	m_bnCurrentChair=bn;
	DrawPlayerBoard();
}

void  CMyPlayer::AutoPlay(BOOL bnAutoPlay)
{
	if (bnAutoPlay)
	{
		StopObject(m_bitStatus->GetHandle());
		m_bitStatus->LoadPic(Res_GetPICStruct("animation_AutoPlay"));
		m_bitStatus->Show();
		m_bitStatus->Updata();
		
		//APPLY_FlashObject(m_bitStatus,0.1,3,200,1,0.3);
	}
	else
	{
		StopObject(m_bitStatus->GetHandle());
		
		CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
		int nDuan=(pApp->GetDuan(m_nChairNO));
		if (nDuan>=0)
		{
			m_bitStatus->LoadPic(Res_GetPICStruct("bit_Duan",nDuan));
			m_bitStatus->SetAlpha(1.0);
			m_bitStatus->Show();
			m_bitStatus->Updata();
		}
		else
		{
			m_bitStatus->SetAlpha(1.0);
			m_bitStatus->Hide();
			m_bitStatus->Updata();
		}
		
	}
}

void  CMyPlayer::Offline(BOOL bnOffline)
{
	m_bOffline = bnOffline;
	if (bnOffline)
	{
		StopObject(m_bitStatus->GetHandle());
		
		m_bitStatus->LoadPic(Res_GetPICStruct("animation_Offline"));
		m_bitStatus->Show();
		m_bitStatus->Updata();
		
		//APPLY_FlashObject(m_bitStatus,0.1,3,200,1,0.3);
	}
	else
	{
		StopObject(m_bitStatus->GetHandle());
		
		CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
		int nDuan=(pApp->GetDuan(m_nChairNO));
		if (nDuan>=0)
		{
			m_bitStatus->LoadPic(Res_GetPICStruct("bit_Duan",nDuan));
			m_bitStatus->SetAlpha(1.0);
			m_bitStatus->Show();
			m_bitStatus->Updata();
		}
		else
		{
			m_bitStatus->SetAlpha(1.0);
			m_bitStatus->Hide();
			m_bitStatus->Updata();
		}
	}
}


void CMyPlayer::DrawPlayerBoard()
{
	if (m_nChairNO<0) 
		return;
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
	GAMEPLAYER* player=&pApp->m_GamePlayer[m_nChairNO];
	
	m_plBoard->Erasure();

	SNG_PIC pic=Res_GetPICStruct("NewPlayerBoardSmall");
		
	int nClothID=pApp->m_GamePlayer[m_nChairNO].nClothingID;
	CString szCharacterName=GetUserCharacter(nClothID,CHARACTER_TYPE_MIDDLE);
	SNG_PIC cloth_pic=Res_GetPICStruct(szCharacterName);

	m_plBoard->DrawPic(pic);
	m_plBoard->DrawPic(cloth_pic,27,27,48,48);
	m_plBoard->SetTextColor(RGB(250,220,40));
	
	COLORREF txtClr=RGB(192,197,138);
	int nFontSize=12;
	m_plBoard->SetTextColor(txtClr);
 	m_plBoard->SetTextSize(nFontSize,400);

	m_plBoard->TextOutEx(GetWidth()/2, 85, player->szUsername);
	CString str;
 	str.Format(_T("%d两"),player->nDeposit);
	m_plBoard->TextOutEx(GetWidth()/2, 100, str);

	ShowBanker(m_bBanker);
}

void  CMyPlayer::ShowReady()
{
	ShowTalk("准备");
}

void CMyPlayer::SetBanker(BOOL bn, BOOL bAnimate/*=FALSE*/)
{
	m_bBanker=bn;
	DrawPlayerBoard();

	if (bn&&bAnimate)
	{
		int nWidth=m_bitBanker->GetWidth();
		int nHeight=m_bitBanker->GetHeight();
		int nScaleWidth=nWidth*1.2;
		int nScaleHeight=nHeight*1.2;
		BeginGroup();
		APPLY_ScaleObject(m_bitBanker,nWidth,nHeight,
			nScaleWidth,nScaleHeight,3,3);
		NextGroup();
		APPLY_ScaleObject(m_bitBanker,nScaleWidth,nScaleHeight,
			nWidth,nHeight,3,3);
		EndGroup();
	}
}
