#include "StdAfx.h"


CMyGameFrame::CMyGameFrame()
{
	
}

CMyGameFrame::~CMyGameFrame()
{
	
}

void	CMyGameFrame::LoadRes()
{
	Res_LoadPack(GetGamePath("skins\\frame.xyr"));
	Res_LoadPack(GetGamePath("skins\\common.xyr"));
	if (theApp.IsAllowCardMaster())
	{
		Res_LoadPack(GetGamePath("skins\\cardmaster.xyr"));
	}
	Res_LoadPack(GetGamePath("skins\\聊天框资源包.xyr"));
	Res_LoadPack(GetGamePath("skins\\xyface_tangtang.xyr"));
	Res_LoadPack(GetGamePath("skins\\xyfaces_tangtang.xyr"));
	Res_LoadPack(GetGamePath("skins\\大分辨率对应.xyr"));
	Res_LoadPack(GetGamePath("skins\\welfare.xyr"));
}

void    CMyGameFrame::CreateFrame()
{
	CXyGameFrame::CreateFrame();
}

void CMyGameFrame::AdjustFrame()
{
	SetSubSystem(new CMySubSystem);
	
	SetSubWidth(GetSubSystemWidth());
	SetSubDisToRight(GetSubSystemWidth());
	
	SetMainGame(new CMyGame);
}

void CMyGameFrame::IniGameStatus()
{
	CXyGameFrame::IniGameStatus();

	//...
}

void CMyGameFrame::OnIniData()
{
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();

//	pApp->SetGameFlags(GAME_FLAGS_PLAYERSIMPLE);

	CXyGameFrame::OnIniData();
}

void CMyGameFrame::GameInstance()
{
	CXyGameFrame::GameInstance();
}

void CMyGameFrame::OnTest()
{
	CString str;
	str.Format("定局淘汰第%d轮",1);
	m_boardWashTurn->BulidString(str);
	m_boardWashTurn->Show();
	m_boardWashTurn->Updata();
}

void   CMyGameFrame::DrawFrameTop()
{
	SNG_PIC pic_top=Res_GetPICStruct("Frame_Top");
	SNG_PIC pic_icon=Res_GetPICStruct("GameIcon");
	SNG_PIC pic_pattern_left=Res_GetPICStruct("FramePatternLeft");
	SNG_PIC pic_pattern_right=Res_GetPICStruct("FramePatternRight");
	SNG_PIC pic_pattern_middle=Res_GetPICStruct("FramePatternMiddle");
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();

	int w=GetScreenWidth();
	int h=GetScreenHeight();

	int top_height=pic_top.height;
	if (top_height==0)
		top_height=m_nTopHeight;


	m_pFrameTop->ScalePlane(w,top_height);
	m_pFrameTop->ScaleTo(w,top_height);
	m_pFrameTop->Erasure();
	for(int i=0;i<m_pFrameTop->GetWidth();i+=pic_top.width)
	{
		m_pFrameTop->DrawPic(pic_top,i,0,pic_top.width,pic_top.height);
	}

	m_pFrameTop->DrawPic(pic_pattern_left,0,0,pic_pattern_left.width,pic_pattern_left.height);
	m_pFrameTop->DrawPic(pic_pattern_right,w-pic_pattern_right.width,0,pic_pattern_right.width,pic_pattern_right.height);
	m_pFrameTop->DrawPic(pic_pattern_middle,w/2-pic_pattern_middle.width/2,0,pic_pattern_middle.width,pic_pattern_middle.height);

	//Modify by chenyang on 20120907 
	//top_height为奇数时，上面少一像素
	//top_height+1 
	//按钮整体左移4像素
	int nDrawHeight = (top_height+1)/2;
	m_pFrameTop->SetFloor(FLOOR_BORDER+100);
	m_pFrameTop->MoveTo(w/2,nDrawHeight);		
	m_pFrameTop->Show();
	m_pFrameTop->Updata();

	int nInterval = 4;
	int x=w-m_btnFrameClose->GetWidth()/2-nInterval;

	m_btnFrameClose->MoveTo(x,nDrawHeight);
	m_btnFrameClose->Show();
	m_btnFrameClose->Updata();
	
	
	x=w-m_btnFrameClose->GetWidth()-m_btnFrameFit->GetWidth()/2-nInterval;
	m_btnFrameFit->MoveTo(x,nDrawHeight);
	m_btnFrameFit->Show();
	m_btnFrameFit->Updata();
	
	x=w-m_btnFrameClose->GetWidth()-m_btnFrameFit->GetWidth()-m_btnFrameMin->GetWidth()/2-nInterval;
	m_btnFrameMin->MoveTo(x,nDrawHeight);
	m_btnFrameMin->Show();
	m_btnFrameMin->Updata();


	x=w-m_btnFrameClose->GetWidth()-m_btnFrameFit->GetWidth()-m_btnFrameMin->GetWidth()-nInterval;
	if (m_btnHelp)
	{
		m_btnHelp->MoveTo(x-m_btnHelp->GetWidth()/2,nDrawHeight);
		m_btnHelp->Show();
		m_btnHelp->Updata();

		x-=m_btnHelp->GetWidth();
	}

	if (m_btnSet)
	{
		m_btnSet->MoveTo(x-m_btnSet->GetWidth()/2,nDrawHeight);
		m_btnSet->Show();
		m_btnSet->Updata();
		
		x-=m_btnSet->GetWidth();
	}

	
	if (m_btnShop)
	{
		m_btnShop->MoveTo(x-m_btnShop->GetWidth()/2,nDrawHeight);
		m_btnShop->Show();
		m_btnShop->Updata();

		x-=m_btnShop->GetWidth();
	}

	if (!m_pFrameIcon)
	{
		//老老实实12号黑体才方便看
		m_pFrameIcon=CreatePlane(200,top_height);
		CString szGameName=pApp->GetGameName();
		m_pFrameIcon->SetTextSize(12);
		m_pFrameIcon->SetTextColor(pApp->GetTitleTextCol());
		m_pFrameIcon->DrawPic(pic_icon,10,(top_height+1)/2-pic_icon.height/2,pic_icon.width,pic_icon.height);
		m_pFrameIcon->TextOut(10+pic_icon.width+5,nDrawHeight-5,pApp->GetGameName());
		m_pFrameIcon->MoveTo(m_pFrameTop->GetLeft()+m_pFrameIcon->GetWidth()/2,m_pFrameTop->GetY());
		m_pFrameIcon->Show();
		m_pFrameIcon->SetFloor(m_pFrameTop->GetZ()+FLOOR_DIF_MIN);
		m_pFrameTop->AddChild(m_pFrameIcon,OBJECT_LOCK_POSITION|OBJECT_LOCK_DELETE|OBJECT_LOCK_UPDATA|OBJECT_LOCK_SHOW);

//		CreateShadowPlane(m_pFrameIcon);
	}
	else
	{
		m_pFrameIcon->MoveTo(m_pFrameTop->GetLeft()+m_pFrameIcon->GetWidth()/2,m_pFrameTop->GetY());
		m_pFrameIcon->Updata();
	}

}

void  CMyGameFrame::OnShowPlayerDetail(int nUserID,POINT pt)
{
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();

	//
	{
		GAMEPLAYER* pPlayer=pApp->GetGamePlayerByUserID(nUserID);
		if (!pPlayer)
		{
			m_nCurrentDetailUserID=-1;
			m_nCureentDetailClothID=-1;
			m_plPlayerDetail->Hide();
			m_plPlayerDetail->Updata();
			return;	
		}

		OPE_DrawUserDetail(pPlayer);
	}


// 	BOOL bnSameID=FALSE;
// 	if (nUserID!=m_nCurrentDetailUserID)
// 	{
// 		bnSameID=TRUE;
// 	}

	{
		RECT rc=GetMainGame()->GetGameRect();
		int x=rc.left;
		int y=rc.top;
		int w=rc.right-rc.left;
		int h=rc.bottom-rc.top;

//		if (!m_plPlayerDetail->IsShow()||bnSameID)
		{
			pt.x+=m_plPlayerDetail->GetWidth()/2;
			pt.y+=m_plPlayerDetail->GetHeight()/2;

			//Modify on 20121123
			//美工意见，展开框不要遮着姓名栏
			SNG_PIC pic = Res_GetPICStruct("NewPlayerBoardSmall");
			int nMaxY = y+h-m_plPlayerDetail->GetHeight()/2-pic.height+10;
			int nMinLeft = x+m_plPlayerDetail->GetWidth()/2+pic.height-6;
			int nMinTop = y+m_plPlayerDetail->GetHeight()/2+pic.height;
			int nMaxRight = x+w-m_plPlayerDetail->GetWidth()/2-pic.height+8;
			if (pt.y>nMaxY)
				pt.y=nMaxY;
			if (pt.y<nMinTop)
				pt.y=nMinTop;
			if (pt.x<nMinLeft)
				pt.x=nMinLeft;
			if (pt.x>nMaxRight)
				pt.x=nMaxRight;
			//Modify end
			
			m_plPlayerDetail->MoveTo(pt.x,pt.y);
		}


		
		m_plPlayerDetail->SetFloor(FLOOR_PLAYERDETAIL);
		m_plPlayerDetail->SetAlpha(0);
		m_plPlayerDetail->Show();
		m_plPlayerDetail->Updata();
	}

	StopObject(m_plPlayerDetail->GetHandle());
	APPLY_ShowObject(m_plPlayerDetail,0.2,1.0);
}

void    CMyGameFrame::DrawBackGround()
{
	SNG_PIC patten=Res_GetPICStruct("NewTablePatten");
	
	//底纹
	for(int i=0;i<GetScreenWidth();i+=patten.width)
	{
		for(int j=0;j<GetScreenHeight();j+=patten.height)
		{
			SCR_SetBackGround(patten,i,j,patten.width,patten.height);
		}
	}
	
	if (GetMainGame())
	{
		RECT rect=GetMainGame()->GetGameRect();
		int x=rect.left;
		int y=rect.top;
		int w=rect.right-rect.left;
		int h=rect.bottom-rect.top;
		
		SNG_PIC Halo=Res_GetPICStruct("NewTableHalo");
		
		int minsize=w>h?h:w;
		int nSize=minsize;
		int nHaloX=x+w/2-nSize/2;
		int nHaloY=y+h/2-nSize/2;
		
		SCR_SetBackGround(Halo,nHaloX,nHaloY,nSize,nSize);
		
// 		SNG_PIC lt=Res_GetPICStruct("TableCornerLT");
// 		SNG_PIC rt=Res_GetPICStruct("TableCornerRT");
// 		SNG_PIC lb=Res_GetPICStruct("TableCornerLB");
// 		SNG_PIC rb=Res_GetPICStruct("TableCornerRB");
// 		SCR_SetBackGround(lt,x,y,lt.width,lt.height,0);
// 		SCR_SetBackGround(rt,x+w-rt.width,y,rt.width,rt.height);
// 		SCR_SetBackGround(lb,x,y+h-lb.height,lb.width,lb.height);
// 		SCR_SetBackGround(rb,x+w-rb.width,y+h-rb.height,rb.width,rb.height);
		
		//边角		
	}
	
	DrawFrameTop();
	DrawFrameLeft();
	DrawFrameRight();
	DrawFrameBottom();
	DrawSplitterBar();
}

BOOL CMyGameFrame::IsTightUI()
{
	if (!IsMaxClient()
		&& m_SplitterWnd->IsShowSubSystem())
	{
		return TRUE;
	}

	return FALSE;
}

void  CMyGameFrame::OPE_DrawUserDetail(GAMEPLAYER* pGamePlayer)
{
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
	
	int nUserID=pGamePlayer->nUserID;
	int chairno=pApp->GetChairNOByUserID(nUserID);
	
	m_nCurrentDetailUserID=nUserID;
	m_nCureentDetailClothID=pGamePlayer->nClothingID;

	SNG_PIC pic=Res_GetPICStruct("newbit_playerdetail");
	if (!m_plPlayerDetail)
	{
		m_plPlayerDetail=CreatePlane(pic.width,pic.height);
		m_plPlayerDetail->TransGUI();
		m_plPlayerDetail->SetFloor(FLOOR_PLAYERDETAIL);
		m_plPlayerDetail->SetMoveAble(TRUE);
		m_plPlayerDetail->SetAutoMsg(MSG_INPUT_MOUSE_RIGHT_HIT,GetHandle(),GAME_MSG_CLOSE_USERDETAIL);
		
		CXyStandButton* smallredcross=NULL;
		PGL_MSG_CLASS(smallredcross,CXyStandButton);
		smallredcross->CreateGUI(Res_GetPICStruct("btn_redcrossEx"),&(SNG_MSG)CreateMsg(GetHandle(),GAME_MSG_CLOSE_USERDETAIL));
		smallredcross->SetButtonSound(BUTTON_STATUS_TOUCH,"SndTouchButton");
		smallredcross->SetButtonSound(BUTTON_STATUS_LEFT_DOWN,"SndHitButton");
		smallredcross->SetFloor(m_plPlayerDetail->GetZ()+0.1);
		smallredcross->MoveTo(m_plPlayerDetail->GetRight()-smallredcross->GetWidth()/2-10,m_plPlayerDetail->GetTop()+19);
		m_plPlayerDetail->AddChild(smallredcross,OBJECT_LOCK_ALL);

		CXyStandButton* btn_buy=NULL;
		PGL_MSG_CLASS(btn_buy,CXyStandButton);
		btn_buy->CreateGUI(Res_GetPICStruct("newbtn_buycharacter"),&(SNG_MSG)CreateMsg(GetHandle(),GAME_MSG_BUY_CLOTH));
		btn_buy->SetButtonSound(BUTTON_STATUS_TOUCH,"SndTouchButton");
		btn_buy->SetButtonSound(BUTTON_STATUS_LEFT_DOWN,"SndHitButton");
		btn_buy->MoveTo(m_plPlayerDetail->GetX(),m_plPlayerDetail->GetTop()+290);
		btn_buy->SetFloor(m_plPlayerDetail->GetZ()+0.1);
		m_plPlayerDetail->AddChild(btn_buy,OBJECT_LOCK_ALL);

// 		CXyStandButton* btn_friend=NULL;
// 		PGL_MSG_CLASS(btn_friend,CXyStandButton);
// 		btn_friend->CreateGUI(Res_GetPICStruct("btn_makefriend"),&(SNG_MSG)CreateMsg(GetHandle(),GAME_MSG_ADD_KOWNPLAYER));
// 		btn_friend->MoveTo(m_plPlayerDetail->GetLeft()+105,m_plPlayerDetail->GetTop()+290);
// 		btn_friend->SetFloor(m_plPlayerDetail->GetZ()+0.1);
// 		m_plPlayerDetail->AddChild(btn_friend,OBJECT_LOCK_ALL);
// 		m_btnAddFriend=btn_friend;
	}
	
	m_plPlayerDetail->Erasure();
	m_plPlayerDetail->DrawPic(pic);
	
	CString szCharacter=GetUserCharacter(pGamePlayer->nClothingID,CHARACTER_TYPE_BIG);
	SNG_PIC headpic=Res_GetPICStruct(szCharacter);
	headpic.x=0;
	headpic.y=0;
	headpic.width=CHARACTER_WIDTH;
	headpic.height=CHARACTER_HEIGHT;
	
	CString str;
	int xResult=145;
	
	//写名字
	m_plPlayerDetail->SetTextSize(14,700);
	m_plPlayerDetail->SetTextColor(RGB(230,216,56));
	m_plPlayerDetail->TextOutEx(195,46,pGamePlayer->szUsername);

	//写信息
	m_plPlayerDetail->SetTextSize(12);
	m_plPlayerDetail->SetTextColor(RGB(214,232,91));
	char szText[128];
	int nInterval = 18, nH = 70;
	if (pApp->IsPlayDeposit())
	{
		//银子
		str.Format("   银 子 ： %d 两",pGamePlayer->nDeposit);
		m_plPlayerDetail->TextOut(xResult,nH,str);
		nH+=nInterval;

		//财富等级
		memset(szText, 0, sizeof(szText));
		xyGetDepositLevel(pGamePlayer->nDeposit,szText);
		str.Format("财富等级 ： %s",szText);
		m_plPlayerDetail->TextOut(xResult,nH,str);
		nH+=nInterval;
	
		m_plPlayerDetail->SetTextColor(RGB(209,236,205));

		//积分
		str.Format("   积 分 ： %d",pGamePlayer->nScore);
		m_plPlayerDetail->TextOut(xResult,nH,str);
		nH+=nInterval;
		
		//级别
		str.Format("   级 别 ： %s",pGamePlayer->szLevelName);
		m_plPlayerDetail->TextOut(xResult,nH,str);
		nH+=nInterval;
	}
	else
	{
		//积分
		str.Format("   积 分 ： %d",pGamePlayer->nScore);
		m_plPlayerDetail->TextOut(xResult,nH,str);
		nH+=nInterval;
		
		//级别
		str.Format("   级 别 ： %s",pGamePlayer->szLevelName);
		m_plPlayerDetail->TextOut(xResult,nH,str);
		nH+=nInterval;

		m_plPlayerDetail->SetTextColor(RGB(209,236,205));

		//银子
		str.Format("   银 子 ： %d 两",pGamePlayer->nDeposit);
		m_plPlayerDetail->TextOut(xResult,nH,str);
		nH+=nInterval;
		
		//财富等级
		memset(szText, 0, sizeof(szText));
		xyGetDepositLevel(pGamePlayer->nDeposit,szText);
		str.Format("财富等级 ： %s",szText);
		m_plPlayerDetail->TextOut(xResult,nH,str);
		nH+=nInterval;
	}

	//经验
	str.Format("   经 验 ： %d",pGamePlayer->nExperience);
	m_plPlayerDetail->TextOut(xResult,nH,str);
	nH+=nInterval;

	int nBout = pGamePlayer->nWin+pGamePlayer->nLoss+pGamePlayer->nStandOff;
	//断线率
	memset(szText, 0, sizeof(szText));
	if(nBout==0)
		lstrcpy(szText,_T("0.00%"));
	else
	{
		float fBreakOff=(float)pGamePlayer->nBreakOff /(float)nBout;
		sprintf(szText,_T("%.2f%%"),fBreakOff*100);
	}
	str.Format("断 线 率 ： %s",szText);
	m_plPlayerDetail->TextOut(xResult,nH,str);
	nH+=nInterval;

	//网速
	str.Format("   网 速 ： %d",pGamePlayer->nNetSpeed);
	m_plPlayerDetail->TextOut(xResult,nH,str);
	nH+=nInterval;

	//赢
	str.Format("   赢 局 ： %d",pGamePlayer->nWin);
	m_plPlayerDetail->TextOut(xResult,nH,str);
	nH+=nInterval;

	//输
	str.Format("   输 局 ： %d",pGamePlayer->nLoss);
	m_plPlayerDetail->TextOut(xResult,nH,str);
	nH+=nInterval;

	//平
	str.Format("   和 局 ： %d",pGamePlayer->nStandOff);
	m_plPlayerDetail->TextOut(xResult,nH,str);
	nH+=nInterval;

	//胜率
	if(nBout==0)
		lstrcpy(szText,_T("100%"));
	else
	{
		float fWinPercent=(float)pGamePlayer->nWin / (float)nBout;
		sprintf(szText,_T("%.2f%%"),fWinPercent*100);
	}
	str.Format("   胜 率 ： %s",szText);
	m_plPlayerDetail->TextOut(xResult,nH,str);

	//画头像
	m_plPlayerDetail->DrawPic(headpic,10,10,CHARACTER_WIDTH,CHARACTER_HEIGHT);
}