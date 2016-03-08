#include "stdafx.h"
using namespace ShinyNova;

CGameSetBoard::CGameSetBoard()
{

}

CGameSetBoard::~CGameSetBoard()
{

}

void CGameSetBoard::OnIniData()
{
	BITObject* bit=CreateBitMap(Res_GetPICStruct("bit_gamesetboard"));
	bit->TransGUI();
	bit->SetMoveAble(TRUE);
	bit->Updata();
	SetFrame(bit);

	SNG_PIC pic = Res_GetPICStruct("复选框");
	int nX = bit->GetLeft()+45;
	int nY = bit->GetTop()+45;
	int nOffX = 18;
	int nOffY = 23;
	int nPlX = 20;

	//音乐开关
	PGL_MSG_CLASS(m_btnMusic,CXyCheckButton);
	m_btnMusic->CreateGUI(pic,theApp.m_GameSet.nMusic);
	m_btnMusic->MoveTo(nX,nY+30);
	m_btnMusic->SetFloor(bit->GetZ()+0.01);
	m_btnMusic->Updata();
	PlaneObject* pl=CreatePlane(170,pic.height);
	pl->SetTextColor(0);
	pl->SetTextSize(12);
	pl->MoveTo(bit->GetX(),nY+30);
	pl->SetAutoMsg(MSG_INPUT_MOUSE_LEFT_DOWN,GetHandle(),SET_CALLBACK_CHECK_MUSIC);
	pl->TextOut(nPlX,4,_T("打开背景音乐"));
	bit->AddChild(m_btnMusic,OBJECT_LOCK_ALL);
	bit->AddChild(pl,OBJECT_LOCK_ALL);
	nY+=nOffY;

	//音效
	PGL_MSG_CLASS(m_btnSound,CXyCheckButton);
	m_btnSound->CreateGUI(pic,theApp.m_GameSet.nSoundEffect);
	m_btnSound->MoveTo(nX,nY+60);
	m_btnSound->SetFloor(bit->GetZ()+0.01);
	m_btnSound->Updata();
	pl=CreatePlane(170,pic.height);
	pl->SetTextColor(0);
	pl->SetTextSize(12);
	pl->MoveTo(bit->GetX(),nY+60);
	pl->SetAutoMsg(MSG_INPUT_MOUSE_LEFT_DOWN,GetHandle(),SET_CALLBACK_CHECK_SOUND);
	pl->TextOut(nPlX,4,_T("打开游戏音效"));
	bit->AddChild(m_btnSound,OBJECT_LOCK_ALL);
	bit->AddChild(pl,OBJECT_LOCK_ALL);
	nY+=nOffY;

	//屏蔽放弃敲桌声
	PGL_MSG_CLASS(m_btnRejectPassSound,CXyCheckButton);
	m_btnRejectPassSound->CreateGUI(pic,theApp.m_GameSet.nRejectPassSound);
	m_btnRejectPassSound->MoveTo(nX,nY);
	m_btnRejectPassSound->SetFloor(bit->GetZ()+0.01);
	m_btnRejectPassSound->Updata();
	pl=CreatePlane(170,pic.height);
	pl->SetTextColor(0);
	pl->SetTextSize(12);
	pl->MoveTo(bit->GetX(),nY);
	pl->SetAutoMsg(MSG_INPUT_MOUSE_LEFT_DOWN,GetHandle(),SET_CALLBACK_CHECK_REJECTPASSSOUND);
	pl->TextOut(nPlX,4,_T("屏蔽放弃敲桌声"));
//	bit->AddChild(m_btnRejectPassSound,OBJECT_LOCK_ALL);
//	bit->AddChild(pl,OBJECT_LOCK_ALL);
	m_btnRejectPassSound->Hide();
	m_btnRejectPassSound->Updata();
	pl->Hide();
	pl->Updata();
	nY+=nOffY;
	
	//不接受看牌请求
	PGL_MSG_CLASS(m_btnRejectKanPai,CXyCheckButton);
	m_btnRejectKanPai->CreateGUI(pic,theApp.m_GameSet.nRejectKanPaiAsk);
	m_btnRejectKanPai->MoveTo(nX,nY);
	m_btnRejectKanPai->SetFloor(bit->GetZ()+0.01);
	m_btnRejectKanPai->Updata();
	pl=CreatePlane(170,pic.height);
	pl->SetTextColor(0);
	pl->SetTextSize(12);
	pl->MoveTo(bit->GetX(),nY);
	pl->SetAutoMsg(MSG_INPUT_MOUSE_LEFT_DOWN,GetHandle(),SET_CALLBACK_CHECK_REJECTKANPAI);
	pl->TextOut(nPlX,4,_T("不接受看牌请求"));
//	bit->AddChild(m_btnRejectKanPai,OBJECT_LOCK_ALL);
//	bit->AddChild(pl,OBJECT_LOCK_ALL);
	m_btnRejectKanPai->Hide();
	m_btnRejectKanPai->Updata();
	pl->Hide();
	pl->Updata();
	nY+=nOffY;

	//将提示按钮显示在手牌上方
	PGL_MSG_CLASS(m_btnRemaind,CXyCheckButton);
	m_btnRemaind->CreateGUI(pic,theApp.m_GameSet.nLikeReMaind);
	m_btnRemaind->MoveTo(nX,nY);
	m_btnRemaind->SetFloor(bit->GetZ()+0.01);
	m_btnRemaind->Updata();
	pl=CreatePlane(170,pic.height);
	pl->SetTextColor(0);
	pl->SetTextSize(12);
	pl->MoveTo(bit->GetX(),nY);
	pl->SetAutoMsg(MSG_INPUT_MOUSE_LEFT_DOWN,GetHandle(),SET_CALLBACK_CHECK_REMAIND);
	pl->TextOut(nPlX,4,_T("将提示按钮显示在手牌上方"));
//	bit->AddChild(m_btnRemaind,OBJECT_LOCK_ALL);
//	bit->AddChild(pl,OBJECT_LOCK_ALL);
	m_btnRemaind->Hide();
	m_btnRemaind->Updata();
	pl->Hide();
	pl->Updata();
	nY+=nOffY;

	//屏蔽出牌时牌型声音动画
	PGL_MSG_CLASS(m_btnNoTip,CXyCheckButton);
	m_btnNoTip->CreateGUI(pic,theApp.m_GameSet.nUnLikeTip);
	m_btnNoTip->MoveTo(nX,nY);
	m_btnNoTip->SetFloor(bit->GetZ()+0.01);
	m_btnNoTip->Updata();
	pl=CreatePlane(170,pic.height);
	pl->SetTextColor(0);
	pl->SetTextSize(12);
	pl->MoveTo(bit->GetX(),nY);
	pl->SetAutoMsg(MSG_INPUT_MOUSE_LEFT_DOWN,GetHandle(),SET_CALLBACK_CHECK_NO_TIP);
	pl->TextOut(nPlX,4,_T("屏蔽出牌时牌型声音动画"));
//	bit->AddChild(m_btnNoTip,OBJECT_LOCK_ALL);
//	bit->AddChild(pl,OBJECT_LOCK_ALL);
	m_btnNoTip->Hide();
	m_btnNoTip->Updata();
	pl->Hide();
	pl->Updata();
 	nY+=nOffY;

	//确定按钮
	CXyStandButton* btnOK;
	PGL_MSG_CLASS(btnOK,CXyStandButton);
	btnOK->CreateGUI(Res_GetPICStruct("btn_gameset_ok"),&(SNG_MSG)CreateMsg(GetHandle(),MSG_PUBLIC_OK));
	btnOK->SetButtonSound(BUTTON_STATUS_TOUCH,"SndTouchButton");
	btnOK->SetButtonSound(BUTTON_STATUS_LEFT_DOWN,"SndHitButton");
	btnOK->MoveTo(bit->GetLeft()+84, (m_btnSound->GetY()+bit->GetBottom())/ 2);
	btnOK->SetFloor(bit->GetZ()+0.1);
	btnOK->Updata();
	bit->AddChild(btnOK,OBJECT_LOCK_ALL);

	//取消按钮
	CXyStandButton* btnCancel;
	PGL_MSG_CLASS(btnCancel,CXyStandButton);
	btnCancel->CreateGUI(Res_GetPICStruct("btn_gameset_cancel"),&(SNG_MSG)CreateMsg(GetHandle(),MSG_PUBLIC_CANCEL));
	btnCancel->SetButtonSound(BUTTON_STATUS_TOUCH,"SndTouchButton");
	btnCancel->SetButtonSound(BUTTON_STATUS_LEFT_DOWN,"SndHitButton");
	btnCancel->MoveTo(bit->GetLeft()+163, btnOK->GetY());
	btnCancel->SetFloor(bit->GetZ()+0.1);
	btnCancel->Updata();
	bit->AddChild(btnCancel,OBJECT_LOCK_ALL);
	bit->SetFloor(FLOOR_GAMSET);
	bit->Updata();
}

void CGameSetBoard::CheckSet()
{
	theApp.m_GameSet.nSoundEffect=m_btnSound->CheckButton();
	theApp.m_GameSet.nMusic=m_btnMusic->CheckButton();
	theApp.m_GameSet.nRejectKanPaiAsk=m_btnRejectKanPai->CheckButton();
	theApp.m_GameSet.nRejectPassSound=m_btnRejectPassSound->CheckButton();
	theApp.m_GameSet.nLikeReMaind=m_btnRemaind->CheckButton();
	theApp.m_GameSet.nUnLikeTip=m_btnNoTip->CheckButton();
	PObject* pMainGame=GetObject(OBJECTNAME_MAIN_GAME);
	if (pMainGame)
		pMainGame->OnMsg(CreateMsg(0,GAME_MSG_FRESH_GAMESET));

}

BOOL CGameSetBoard::OnMsg(SNG_MSG& Msg)
{
	switch(Msg.msgID)
	{
	case MSG_PUBLIC_OK:
		{
			CheckSet();
			Hide();
			Updata();
		}
		break;
	case MSG_PUBLIC_CANCEL:
		{
			m_btnSound->SetCheckStatus(theApp.m_GameSet.nSoundEffect);
			m_btnMusic->SetCheckStatus(theApp.m_GameSet.nMusic);
			m_btnRejectPassSound->SetCheckStatus(theApp.m_GameSet.nRejectPassSound);
			m_btnRejectKanPai->SetCheckStatus(theApp.m_GameSet.nRejectKanPaiAsk);
			m_btnRemaind->SetCheckStatus(theApp.m_GameSet.nLikeReMaind);
			m_btnNoTip->SetCheckStatus(theApp.m_GameSet.nUnLikeTip);
			Hide();
			Updata();
		}
		break;

	case SET_CALLBACK_CHECK_SOUND:
		{
			m_btnSound->SetCheckStatus(!m_btnSound->CheckButton());
		}
		break;
	case SET_CALLBACK_CHECK_MUSIC:
		{
			m_btnMusic->SetCheckStatus(!m_btnMusic->CheckButton());
		}
		break;
	case SET_CALLBACK_CHECK_REJECTPASSSOUND:
		{
			m_btnRejectPassSound->SetCheckStatus(!m_btnRejectPassSound->CheckButton());
		}
		break;
	case SET_CALLBACK_CHECK_REJECTKANPAI:
		{
			m_btnRejectKanPai->SetCheckStatus(!m_btnRejectKanPai->CheckButton());
		}
		break;
	case SET_CALLBACK_CHECK_REMAIND:
		{
			m_btnRemaind->SetCheckStatus(!m_btnRemaind->CheckButton());
		}
		break;		
	case SET_CALLBACK_CHECK_NO_TIP:
		{
			m_btnNoTip->SetCheckStatus(!m_btnNoTip->CheckButton());
		}
		break;	
	default:
		return CShinyNovaGui::OnMsg(Msg);
	}


	return TRUE;
}