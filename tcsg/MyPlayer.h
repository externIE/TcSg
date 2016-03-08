#pragma once
//#include "PglGui.h"
using namespace ShinyNova;

enum{
	MYPLAYER_CALLBACK_FRESHSCORE=PLAYER_CALLBACK_CUSTOMER+1,
};


class CMyPlayer:public CGamePlayer
{
	public:
				 CMyPlayer();
		virtual ~CMyPlayer();
		virtual BOOL  OnMsg(SNG_MSG& Msg);

		virtual void  CreateGUI(int nDrawIndex);  //创建玩家，从CGameWinApp数据
		virtual void  LoadPlayer(int nChairNO);
		virtual void  UnLoadPlayer();
		virtual void  DrawScore();

		virtual POINT GetClockPosition(int nStatus);
		virtual void  AddScore(int nScore);
		virtual void  FreshBomb(int nBombCount);
		virtual	void  ShowBanker(BOOL bShow);
				void  ShowTitle();
		
		virtual void  FitScrren();//刷新位置
		
		virtual void  Restart();

				int   GetRestCardWidth();
				int   GetRestCardHeight();
		virtual void  SetRestCard(int nRestCount);//在指定位置显示牌的剩余张数
		virtual void  ShowRestCard(int x,int y);
				void  ShowPlayerCloth();
				void  HidePlayerCloth();
				BOOL  IsClothShow();
		virtual void  FitTalkPosition();
	
				void  ShowAnimation(LPCTSTR szObject, int nX=0, int nY=0);
				void  ShowAnimation(LPCTSTR szObject,POINT ptBegin, POINT ptEnd);
				void  ShowAnimation(LPCTSTR szObject, int nStatus, POINT pt);
				void  HideAnimation();
				void  FitAnimationPosition(int nX=0, int nY=0);
				POINT GetBombPosition();
				POINT GetRocketPosition();
				void  GetAirPlanePosition(POINT& ptBegin, POINT& ptEnd);
				void  FitActorPosition();
				void  FitBankerPos();
				void  ShowActor(BOOL bAnimation, int nStatus);
				void  HideActor();
		virtual void  SetCurrentChair(BOOL bn);
				POINT GetTalkPosition();
		virtual void  AutoPlay(BOOL bnAutoPlay);
		virtual void  Offline(BOOL bnOffline);
		virtual void  DrawPlayerBoard();
		virtual void  ShowReady();
		virtual void  SetBanker(BOOL bn, BOOL bAnimate=FALSE);
				
	public:
				CXyDigit*     m_digitScore;
				PlaneObject*  m_plBomb;
				PlaneObject*  m_plShowCloth;
				BITObject*    m_bitAnimation;	//玩家活动状态，包括牌型，不出等，一般都是动画
				BITObject*	  m_bitActor;		//角色
				int			  m_nActorPos;

				BOOL		  m_bBanker;
				BITObject*	  m_bitBanker;
};			
