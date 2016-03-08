 

#include "stdafx.h" 
#include "MySubSystem.h"



 

 

void CMyAdvertBar::RefreshHtmlCompletely()
{
	if (m_htmWWW.m_hWnd)
	{
		m_htmWWW.Refresh2(REFRESH_COMPLETELY);
	}
}

void CMyAdvertBar::ClickHtmlBtn(LPCTSTR strBtn)
{
	if (m_htmWWW.m_hWnd)
	{
		m_htmWWW.ClickLink(strBtn);
	}
}

void CMyAdvertBar::OnInitData()
{
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	if (pGameApp->GetOfflineFlag()==OFFLINEFLAG_OFFLINE)
	{
		m_htmWWW.Navigate("shop.uc108.com",NULL,NULL);
	}
	else
	{
		CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
		char url[MAX_PATH];
		strcpy(url,pApp->GetWWW3());
		m_htmWWW.Navigate(url,NULL,NULL);
	}

//	m_htmWWW.Navigate("shop.uc108.com",NULL,NULL);
}

void CMyAdvertBar::OnIniData()
{
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();

	ReName(GAMEBASE_GAME_ADVERTBAR);

	RECT rc={0,0,10,10};
	CWnd* pWnd=AfxGetMainWnd();
  	m_htmWWW.Create(AfxRegisterWndClass(0),"AdvertBar",WS_CLIPCHILDREN,rc,AfxGetMainWnd(),0);
	m_htmWWW.SetCheckDomain(TRUE);
	m_htmWWW.SetDomainFile(xyGetDomainFileName());

	PGL_MSG_CLASS(m_btnAdvertTab,CXyStandButton);
	m_btnAdvertTab->CreateGUI(Res_GetPICStruct("btnAdvertTab"),&(SNG_MSG)CreateMsg(GetHandle(),MSG_FRAME_SHOWADVERT));
	m_btnAdvertTab->SetAutoMsg(MSG_INPUT_TOUCH,GetHandle(),MSG_FRAME_SHOWADVERT);
	m_btnAdvertTab->Show();
	m_btnAdvertTab->SetLockDown(FALSE);

	PGL_MSG_CLASS(m_btnResultTab,CXyStandButton);
	if (pApp->IsPlayDeposit())
		m_btnResultTab->CreateGUI(Res_GetPICStruct("btnDepositTab"),&(SNG_MSG)CreateMsg(GetHandle(),MSG_FRAME_SHOWRESULT));
	else
		m_btnResultTab->CreateGUI(Res_GetPICStruct("btnScoreTab"),&(SNG_MSG)CreateMsg(GetHandle(),MSG_FRAME_SHOWRESULT));

	m_btnResultTab->SetAutoMsg(MSG_INPUT_TOUCH,GetHandle(),MSG_FRAME_SHOWRESULT);
	m_btnResultTab->Show();


	SNG_PIC pic=Res_GetPICStruct(Sys_GetStringStatus(SNG_STRING_SCROLLBAR_HORZBACK));
	
	PGL_MSG_CLASS(m_HorzScrollBar,CCyScrollBar);
	m_HorzScrollBar->CreateGUI(GetHandle(),100,pic.height,SHINYNOVE_SCROLLBAR_ORIENTEATION_X,0);
	m_HorzScrollBar->DrawBackGround(Res_GetPICStruct(Sys_GetStringStatus(SNG_STRING_SCROLLBAR_HORZBACK)));
	m_HorzScrollBar->DrawScrollButton(Res_GetPICStruct(Sys_GetStringStatus(SNG_STRING_SCROLLBAR_HORZBTN)));
	m_HorzScrollBar->CreateBtnMark(Res_GetPICStruct(Sys_GetStringStatus(SNG_STRING_SCROLLBAR_HORZPATTERN)));
	m_HorzScrollBar->CreateBtnScrollLeft(Res_GetPICStruct(Sys_GetStringStatus(SNG_STRING_SCROLLBAR_HORZLEFTBTN)));
	m_HorzScrollBar->CreateBtnScrollRight(Res_GetPICStruct(Sys_GetStringStatus(SNG_STRING_SCROLLBAR_HORZRIGHTBTN)));

	
	m_plResult=CreatePlane(10,10);
	DrawResult();
	
	m_plResult->AddChild(m_HorzScrollBar,OBJECT_LOCK_SHOW|OBJECT_LOCK_UPDATA|OBJECT_LOCK_DELETE);
	m_plResult->SetAutoMsg(MSG_INPUT_MOUSE_LEFT_DOWN,GetHandle(),MSG_FRAME_CANCELADVERT,0,0);

	//自动切换时间和次数
	CString sConfigFile=pApp->GetGameConfigFile();
	m_nAutoChangeSenond = GetPrivateProfileInt ( _T("GAMESET"), _T("AdvertACSencond"), 10, sConfigFile); 
	m_nMaxAutoChangeCount = GetPrivateProfileInt ( _T("GAMESET"), _T("AdvertACMaxCount"), 3, sConfigFile);

	ShowResult();
}

void CMyAdvertBar::OnSize()
{
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();

	int nTabHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_TABHEIGHT);
	int nRowHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_ROWHEIGHT);
	int nTitleHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_TITLEHEIGHT);
	int nScrollBarHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_SCROLLBARHEIGHT);
	int nChairCount=pApp->GetTableChairCount();

	//至少显示4个玩家
	if (nChairCount<4)
		nChairCount=4;
    nChairCount=4;
	int nAdvertHeight=nTitleHeight+nRowHeight*nChairCount+nScrollBarHeight;
	int nResultHeight=nTitleHeight+nRowHeight*nChairCount;

	int x=GetLeft();
	if (m_btnAdvertTab)
	{
		int off = (nResultHeight%2==1) ? 1:0;
		m_btnAdvertTab->MoveTo(GetLeft()+m_btnAdvertTab->GetWidth()/2,GetTop()+nTabHeight-m_btnAdvertTab->GetHeight()/2+off);
		m_btnAdvertTab->Updata();

		x+=m_btnAdvertTab->GetWidth()+1;
	}

	if (m_btnResultTab)
	{
		int off = (nResultHeight%2==1) ? 1:0;
		m_btnResultTab->MoveTo(x+m_btnResultTab->GetWidth()/2,GetTop()+nTabHeight-m_btnResultTab->GetHeight()/2+off);
		m_btnResultTab->Updata();

		x+=m_btnResultTab->GetWidth()+1;
	}


	if (m_htmWWW.m_hWnd)
	{
 		m_htmWWW.MoveWindow(GetLeft(),GetTop()+nTabHeight,GetWidth(),nAdvertHeight);
	}

	if (m_plResult)
	{
		RECT rc={GetLeft(),GetTop()+nTabHeight,GetRight(),GetBottom()};
		m_plResult->SetClipRect(rc);
//		m_plResult->MoveTo(GetX(),GetTop()+nResultHeight/2+nTabHeight);
		m_plResult->MoveTo(GetLeft()+m_plResult->GetWidth()/2,GetTop()+nResultHeight/2+nTabHeight);
		m_plResult->Updata();

		
		m_HorzScrollBar->ScaleTo(GetWidth(),m_HorzScrollBar->GetHeight());
		m_HorzScrollBar->MoveTo(GetX(),m_plResult->GetBottom()+m_HorzScrollBar->GetHeight()/2);
		m_HorzScrollBar->Updata();
		m_HorzScrollBar->FreshBar(m_HorzScrollBar->GetCurPosiion(),GetWidth(),m_plResult->GetWidth());
		m_HorzScrollBar->SendFreshInfo();
		
		//Add on 20130219
		DrawResult();
	}


}

void CMyAdvertBar::DrawResult()
{
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
	
	int nTabHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_TABHEIGHT);
	int nRowHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_ROWHEIGHT);
	int nLineWidth=Sys_GetIntStatus(GAME_STATUS_ADVERT_LINEWIDTH);
	int nTitleHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_TITLEHEIGHT);
	int nScrollBarHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_SCROLLBARHEIGHT);

	int nChairCount=pApp->GetTableChairCount();

	//至少显示4个玩家
	if (nChairCount<4)
		nChairCount=4;

	int nTableCount=pApp->m_nResultCount>=1?pApp->m_nResultCount+1:2;
	if (nTableCount>MAX_RESULT_COUNT+1)
	{
		nTableCount=MAX_RESULT_COUNT+1;
	}
	int nNameWidth=110;

	int h=nChairCount*nRowHeight+nTitleHeight;
	int w=nNameWidth+nLineWidth*nTableCount;

	//Add on 20130219
	//最小宽度不得低于区域宽度
	int nSubWidth = GetWidth();
	if (w<nSubWidth)
		w=nSubWidth;

	m_plResult->ScalePlane(w,h);
	m_plResult->ScaleTo(w,h);
	m_plResult->SetGroundColor(pApp->GetAdvBkCol());
	m_plResult->Erasure();

	m_plResult->DrawPic(Res_GetPICStruct("bitHistoryTitle"),0,0,w,nTitleHeight);

	//画格子线
	m_plResult->SetTextColor(pApp->GetAdvGridCol());
	m_plResult->DrawLine(nNameWidth,0,nNameWidth,h);

	for(int i=0;i<nTableCount;i++)
	{
		m_plResult->DrawLine(nNameWidth+i*nLineWidth,0,nNameWidth+i*nLineWidth,h);
	}

	m_plResult->DrawLine(0,nTitleHeight,w,nTitleHeight);
	for(i=1;i<=nChairCount;i++)
	{
		m_plResult->DrawLine(0,nTitleHeight+i*nRowHeight,w,nTitleHeight+i*nRowHeight);
	}

	//标题栏
	//画列表头
	SNG_PIC pic = Res_GetPICStruct("jf_line");
	m_plResult->SetTextColor(pApp->GetAdvHeaderCol());
	m_plResult->SetTextSize(12);

	m_plResult->TextOut(9,5,"用户名");
	//表头分割线
	int nHPic = (nTitleHeight-pic.height)/2+1;
	if (nHPic<0)
		nHPic=0;
	m_plResult->DrawPic(pic,nNameWidth,nHPic,pic.width,pic.height);
	for(i=0;i<nTableCount;i++)
	{
		m_plResult->DrawPic(pic,nNameWidth+i*nLineWidth,nHPic,pic.width,pic.height);
	}

	if (pApp->IsPlayDeposit())
		m_plResult->TextOutEx(nNameWidth+nLineWidth/2,nRowHeight/2-1,"总银子");
	else
		m_plResult->TextOutEx(nNameWidth+nLineWidth/2,nRowHeight/2-1,"总积分");
	CString str;
	int nBout=pApp->m_nResultCount;
	for(i=0;i<pApp->m_nResultCount
		&&i<MAX_RESULT_COUNT;i++)
	{
		str.Format("第%d局",nBout);
		m_plResult->TextOutEx(nNameWidth+(i+1)*nLineWidth+nLineWidth/2,nRowHeight/2-1,str);
		nBout--;
	}

	//画成绩
	m_plResult->SetTextColor(pApp->GetAdvTextCol());
	int nIndex = 0;
	for(i=0;i<nChairCount;i++)
	{
		if (pApp->IsPlayerEnter(i))
		{
			m_plResult->TextOut(9,nTitleHeight+nIndex*nRowHeight+7,pApp->m_GamePlayer[i].szUsername);
			nIndex++;
		}
	}

	nIndex = 0;
	for(i=0;i<nChairCount;i++)
	{
		if (pApp->IsPlayerEnter(i))
		{
			str.Format("%d",pApp->m_nResultTotal[i]);
			m_plResult->TextOutEx(nNameWidth+nLineWidth/2,nTitleHeight+nIndex*nRowHeight+nRowHeight/2,str);
			nIndex++;
		}
	}
	nBout=pApp->m_nResultCount;
	for(i=0;i<pApp->m_nResultCount
		&&i<MAX_RESULT_COUNT;i++)
	{
		nIndex = 0;
		for(int j=0;j<nChairCount;j++)
		{
			if (pApp->IsPlayerEnter(j))
			{
				str.Format("%d",pApp->m_nResultDiff[j][(nBout-1)%MAX_RESULT_COUNT]);
				m_plResult->TextOutEx(nNameWidth+nLineWidth+i*nLineWidth+nLineWidth/2,nTitleHeight+nIndex*nRowHeight+nRowHeight/2,str);
				nIndex++;
			}
		}

		nBout--;
	}

	m_HorzScrollBar->FreshBar(0,nSubWidth,m_plResult->GetWidth());
	m_HorzScrollBar->SendFreshInfo();
}

void CMyAdvertBar::OnOpenHtml(LPCTSTR sURL)
{
    m_htmWWW.Navigate(sURL,NULL,NULL);
}

BOOL CMyAdvertBar::OnMsg(SNG_MSG& Msg)
{
	switch(Msg.msgID)
	{
		case GAME_MSG_UPDATARESULT:
			{
				UpdataResult();
			}
			break;
		case MSG_GUI_FRESH_SCROLLBAR:
			{
				SROLLBAR_FRESH* fresh=(SROLLBAR_FRESH*)Msg.param1;
				OnFreshScrollBar(fresh);
			}
			break;
		case MSG_PUBLIC_MOVE_LEFT:
			{
				OnScrollBarLeftBtn();
			}
			break;
		case MSG_PUBLIC_MOVE_RIGHT:
			{
				OnScrollBarRightBtn();
			}
			break;
		case MSG_FRAME_SHOWADVERT:
			{
				ShowAdvert();
			}
			break;
		case MSG_FRAME_SHOWRESULT:
			{
				ShowResult();
			}
			break;
		case MSG_FRAME_AUTOADVERT:
			{
				OnAutoChangeAdvert();
			}
			break;
		case MSG_FRAME_CANCELADVERT:
			{
				m_nMaxAutoChangeCount=0;
			}
			break;

	}

	return CShinyNovaGui::OnMsg(Msg);
}

void CMyAdvertBar::ShowAdvert()
{
	m_btnAdvertTab->Show();
	m_btnAdvertTab->SetLockDown(TRUE);
	m_btnAdvertTab->Updata();
				
	m_btnResultTab->Show();
	m_btnResultTab->SetLockDown(FALSE);
	m_btnResultTab->Updata();

	m_htmWWW.ShowWindow(SW_SHOW);
 	m_htmWWW.UpdateWindow();

	m_plResult->Hide();
	m_plResult->Updata();

	m_nTabCurSel	=	0;
}

void CMyAdvertBar::ShowResult()
{
	m_btnAdvertTab->Show();
	m_btnAdvertTab->SetLockDown(FALSE);
	m_btnAdvertTab->Updata();
				
	m_btnResultTab->Show();
	m_btnResultTab->SetLockDown(TRUE);
	m_btnResultTab->Updata();

	m_htmWWW.ShowWindow(SW_HIDE);
	m_htmWWW.UpdateWindow();

	m_plResult->Show();
	m_plResult->Updata();

	m_nTabCurSel	=	1;

	if (m_nMaxAutoChangeCount>0)
	{
		m_nMaxAutoChangeCount--;
		APPLY_WaitFewTimes(m_nAutoChangeSenond*1000,&(SNG_MSG)CreateMsg(GetHandle(),MSG_FRAME_AUTOADVERT));
	}
}

void CMyAdvertBar::OnAutoChangeAdvert()
{
	if (m_nTabCurSel==0)
		return;

//	if (!m_htmWWW.m_bComplete)
//		return;


	if (IsPointIn(GetMouseData().MS_CurrectPoint))
	{
		APPLY_WaitFewTimes(m_nAutoChangeSenond*1000,&(SNG_MSG)CreateMsg(GetHandle(),MSG_FRAME_AUTOADVERT));
	}
	else
	{
		ShowAdvert();
	}


}

void CMyAdvertBar::UpdataResult()
{
	DrawResult();
	m_HorzScrollBar->FreshBar(0,GetWidth(),m_plResult->GetWidth());
	m_HorzScrollBar->SendFreshInfo();
}

void CMyAdvertBar::OnFreshScrollBar(SROLLBAR_FRESH* pFresh)
{
	m_plResult->MoveTo(GetLeft()-pFresh->nPosition+pFresh->nTotalLength/2,m_plResult->GetY());
	m_plResult->Updata();
}

void CMyAdvertBar::OnScrollBarLeftBtn()
{
	int nPosition=GetLeft()-m_plResult->GetLeft();
	int nLineWidth=Sys_GetIntStatus(GAME_STATUS_ADVERT_LINEWIDTH);
	nPosition-=nLineWidth;
	if (nPosition<100) nPosition=0;

	m_HorzScrollBar->FreshBar(nPosition,GetWidth(),m_plResult->GetWidth());
	m_HorzScrollBar->SendFreshInfo();
}

void CMyAdvertBar::OnScrollBarRightBtn()
{
	int nPosition=GetLeft()-m_plResult->GetLeft();
	int nLineWidth=Sys_GetIntStatus(GAME_STATUS_ADVERT_LINEWIDTH);
	nPosition+=nLineWidth;

	if (nPosition>0&&nPosition<100)
		nPosition=100;

	if (nPosition>m_plResult->GetWidth()-GetWidth()) 
		nPosition=m_plResult->GetWidth()-GetWidth();

	m_HorzScrollBar->FreshBar(nPosition,GetWidth(),m_plResult->GetWidth());
	m_HorzScrollBar->SendFreshInfo();
}


BOOL CMyAdvertBar::IsSomeWebShow()
{
	if (m_btnAdvertTab->IsShow()
		&& m_nTabCurSel==0)
	{
		return TRUE;
	}
	
	return FALSE;
}

void CMyAdvertBar::HideAllWebs()
{
	m_htmWWW.ShowWindow(SW_HIDE);
	m_htmWWW.UpdateWindow();
}

void CMyAdvertBar::RecoverWebs()
{
	if (m_btnAdvertTab->IsShow()
		&& m_nTabCurSel==0)
	{
		m_htmWWW.ShowWindow(SW_SHOW);
		m_htmWWW.UpdateWindow();
	}
}


void CMySubSystem::CreateAdvertBar()
{
	CMyAdvertBar* bar=NULL;
	PGL_MSG_CLASS(bar,CMyAdvertBar);
}



int  CMySubSystem::GetAdvertBarHeight()
{
	CGameWinApp* pApp=(CGameWinApp*)AfxGetApp();
	
	int nTabHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_TABHEIGHT);
	int nRowHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_ROWHEIGHT);
	int nTitleHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_TITLEHEIGHT);
	int nScrollBarHeight=Sys_GetIntStatus(GAME_STATUS_ADVERT_SCROLLBARHEIGHT);
	int nChairCount=pApp->GetTableChairCount();
	
	//至少显示4个玩家
	if (nChairCount<4)
		nChairCount=4;
    nChairCount=4;
	
	int h=nChairCount*nRowHeight+nTabHeight+nTitleHeight+nScrollBarHeight+1;
	
	return h;
}

void  CMySubSystem::NTF_StartSoloTable()
{
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	
	CString  sMsg;
	sMsg.Format(_T(" <c=255>您已经被随机分配到第%d桌\r\n"),pGameApp->GetTableNO()+1);
 //	pGameApp->ShowMsgInChatView(sMsg);

	CAdvertBar* bar=(CAdvertBar*)GetObject(GAMEBASE_GAME_ADVERTBAR);
	if (bar)
	{
		pGameApp->ClearResult();
		bar->UpdataResult();
	}
}

void  CMySubSystem::NTF_UserPosition()
{
	CSubSystem::NTF_UserPosition();
	//换桌后立即发送开始游戏消息
	/*
	CGameApp* pGameApp=(CGameApp*)AfxGetApp();
	if (pGameApp)
	{
		CMyGame* pGame=(CMyGame*)(pGameApp->GetMainGame());
		if(pGame)
			pGame->OPE_OnStart();
	}*/
	return;
}

void  CMySubSystem::NTF_PlayerEnter(GAMEPLAYER* pGamePlayer)
{
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();

	CString sMsg;
	sMsg.Format(_T(" <c=%d>%s进入第%d桌 \r\n"),	RGB(255,0,0),
		pGameApp->GetPlayerNameByUserID(pGamePlayer->nUserID),
		pGamePlayer->nTableNO+1);
//	pGameApp->ShowMsgInChatView(sMsg);

	CAdvertBar* bar=(CAdvertBar*)GetObject(GAMEBASE_GAME_ADVERTBAR);
	if (bar)
	{
		bar->UpdataResult();
	}
}

void  CMySubSystem::NTF_PlayerAbort(GAMEPLAYER* pGamePlayer)
{
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	CString sMsg;
	sMsg.Format(_T(" <c=%d>%s离开第%d桌 \r\n"),	RGB(255,0,0),
		pGameApp->GetPlayerNameByUserID(pGamePlayer->nUserID),
		pGamePlayer->nTableNO+1);
//	pGameApp->ShowMsgInChatView(sMsg);

	memset(pGamePlayer,0,sizeof(GAMEPLAYER));
	CAdvertBar* bar=(CAdvertBar*)GetObject(GAMEBASE_GAME_ADVERTBAR);
	if (bar)
	{
		pGameApp->ClearResult();
		bar->UpdataResult();
	}
}

void  CMySubSystem::NTF_VisitorEnter(GAMEPLAYER* pGamePlayer)
{
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	
	if(!pGameApp->IsAdmin(pGamePlayer->nUserID))
	{
		CString sMsg;
		sMsg.Format(_T(" <c=%d>%s进入旁观 \r\n"),	RGB(88,88,44),
			pGameApp->GetPlayerNameByUserID(pGamePlayer->nUserID));
	//	pGameApp->ShowMsgInChatView(sMsg);
	}
}

void  CMySubSystem::NTF_VisitorAbort(GAMEPLAYER* pPlayer)
{
	//提示旁观退出
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	int chairno=pPlayer->nChairNO;
	
	if(!pGameApp->IsAdmin(pPlayer->nUserID))
	{
		CString sMsg;
		sMsg.Format(_T(" <c=%d>%s退出旁观 \r\n"),	RGB(88,88,44),
			pGameApp->GetPlayerNameByUserID(pPlayer->nUserID));
		
	//	pGameApp->ShowMsgInChatView(sMsg);
	}

	//清空此人旁观看牌记录
	CCyPlayerListBar* bar=(CCyPlayerListBar*)GetObject(GAMEBASE_GAME_USERLISTBAR);
	if (bar)
	{
		bar->GetUserList()->NTF_LookOnAbort(pPlayer);
	}
}



void  CMySubSystem::NTF_UserDepositEvent(LPUSER_DEPOSITEVENT pUser_Deposit)
{
	CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
	int nMeUserID=theApp.m_GamePlayer[theApp.GetChairNO()].nUserID;
	CString  sMsg;
	if (USER_TAKE_SAFE_DEPOSIT == pUser_Deposit->nEvent)
	{
		sMsg.Format(_T(" <z=tc系统通知> <c=255>%s<>从保险箱中取出<c=255>%d<>两银子, 现有<c=255>%d<>两银子 \r\n"), 
			pGameApp->GetUserNameByID(pUser_Deposit->nUserID), pUser_Deposit->nDepositDiff, pUser_Deposit->nDeposit);
	}
	else if (USER_SAVE_SAFE_DEPOSIT == pUser_Deposit->nEvent)
	{
		sMsg.Format(_T(" <z=tc系统通知> <c=255>%s<>存入<c=255>%d<>两银子到保险箱, 现有<c=255>%d<>两银子 \r\n"), 
			pGameApp->GetUserNameByID(pUser_Deposit->nUserID), pUser_Deposit->nDepositDiff, pUser_Deposit->nDeposit);
	}
	if (nMeUserID==pUser_Deposit->nUserID&&!theApp.IsLookOn())
	{
		//pengsy
	    pGameApp->ShowMsgInChatView(sMsg);
	}

// 	
// 	if (pUser_Deposit->nBaseDeposit>0
// 		&& pGameApp->GetGameInfo()->GetBaseDeposit()!=pUser_Deposit->nBaseDeposit)
// 	{
// 		sMsg.Format(_T(" <z=tc系统通知> 当前基础银为<c=255>%d<>两 \r\n"), pUser_Deposit->nBaseDeposit);
// 		pGameApp->ShowMsgInChatView(sMsg);
// 	}
	
}

////////////////////////////////自动划银////////////////////////////////////
#define MAX_SECUREPWD_LEN	16
#define MIN_SECUREPWD_LEN	8

void CMyKeyResult::OnOK()
{
	int len=lstrlen(m_szInput);
	if (len < MIN_SECUREPWD_LEN || len > MAX_SECUREPWD_LEN)
	{
		CGameWinApp* pGameApp=(CGameWinApp*)AfxGetApp();
		CString sMsg;
		sMsg.Format(GMS_SECUREPWDLEN_LIMIT,MIN_SECUREPWD_LEN,MAX_SECUREPWD_LEN);
		pGameApp->ShowTimerMsgBox(sMsg,NULL,NULL,10,TIMERDLG_FLAGS_ICON|TIMERDLG_FLAGS_DANYMIC,"xyf打牌");		
		return ;
	}

	//////////////////////////////////////////////////////////////////////////
	int SecurePwd[4]={0};
	int RandNumber[2]={0};
	
	TCHAR szResult[20]="";
	TCHAR* p=szResult;
	TCHAR szTmp[5]="";
	lstrcpy(szResult, m_szInput);

	for (int i = 0; i < 4; i++)
	{
		if (!p) break;
		
		if (lstrlen(p) > 4)
		{
			strncpy(szTmp, p, 4);
			szTmp[4]='\0';
			SecurePwd[i] = atoi(szTmp);
			p+=4;
		}
		else if (lstrlen(p) > 0)
		{
			SecurePwd[i] = atoi(p);
			break;
		}
		else 
		{
			break;
		}
	}
	
	RandNumber[0] = m_nRandomNum/10000; //随机数高四位
	RandNumber[1] = m_nRandomNum%10000; //随机数低四位
	
	m_nKeyResult=0;
	for (i=0;i<4;i++)
	{
		m_nKeyResult += SecurePwd[i];
	}
	for (i=0;i<2;i++)
	{
		m_nKeyResult += RandNumber[i];
	}

	CMyGame* pGame = (CMyGame*)GetObject(OBJECTNAME_MAIN_GAME);
	pGame->OnMsg(CreateMsg(0,TCSG_SAFEBOX_TAKEDEPOSIT_BTN_SET,m_nKeyResult));

	OnCancel();
}
////////////////////////////////自动划银////////////////////////////////////

void CMySubSystem::OnGameSize()
{
	CSubSystem::OnGameSize();

	RECT rect=GetGameRect();
	int x=rect.left;
	int y=rect.top;
	int width=rect.right-rect.left;
	int height=rect.bottom-rect.top;

	{
		CGameInputBar* bar=(CGameInputBar*)GetObject(GAMEBASE_GAME_INPUTBAR);
		if (bar)
		{
			bar->ScaleTo(width+1,GetInputHeight());
			bar->MoveTo(x+width/2,rect.bottom-GetInputHeight()/2);
			bar->Updata();
			
			bar->OnSize();
		}
	}
}

BOOL  CMySubSystem::OnMsg(SNG_MSG& msg)
{
	switch(msg.msgID)
	{
	case GAME_MSG_USER_PLAYTOGAME_EVENT:
		{
			LPUSER_PAYTOGAMEEVENT pUser_PayToGame = (LPUSER_PAYTOGAMEEVENT)msg.param1;
			if(IS_BIT_SET(pUser_PayToGame->stPayMsg.dwFlags,FLAG_PAY_ARRIVE))
			{
				CGameApp* pGameApp=(CGameApp*)AfxGetApp();
				if (pGameApp)
				{
					CMyGame* pGame=(CMyGame*)(pGameApp->GetMainGame());
					if (pGame)
					{
						pGame->APPLY_WaitFewTimes(0, &(SNG_MSG)CreateMsg(pGame->GetHandle(),TCSG_CALLBACKMSG_REFFESHCHARGE));
					}
				}
			}
			
			NTF_UserPayToGameEvent((LPUSER_PAYTOGAMEEVENT)msg.param1);
			//CSubSystem::NTF_UserPayToGameEvent((LPUSER_PAYTOGAMEEVENT)msg.param1);
		}
		break;
    default:
		{
			CSubSystem::OnMsg(msg);
		}

	}

	return TRUE;
}