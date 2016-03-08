#pragma once
extern CGameTable g_GameTable;
namespace ShinyNova{

//游戏的逻辑图层在0～10000之间
#define	 CARD_FLOOR_WAITDEAL       0
#define  CARD_FLOOR_INHAND         1000
#define  CARD_FLOOR_THROWDOWN      2000

class CMyCard : public CGameCard
{
public:
	CMyCard();
	virtual ~CMyCard();
public:
	virtual void		OnIniData();
	virtual void		Fresh();
	virtual POINT		GetWaitDealPositon();
	virtual POINT       GetInHandPosition();
	virtual POINT	    GetThrowPosition();
	virtual POINT	    GetBottomPosition();
	virtual void		CalcSize();
	virtual void		HideCardMask();	//隐藏蒙皮
			int			GetPopLength();
			void        GiveCardAni();

public:
	BOOL		  m_bBankerCard;


	PlaneObject*  m_plCardMask[CHAIR_CARDS];				// 组牌后的牌蒙皮
};


}