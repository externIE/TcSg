#pragma once
extern CGameTable g_GameTable;
namespace ShinyNova{

//��Ϸ���߼�ͼ����0��10000֮��
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
	virtual void		HideCardMask();	//������Ƥ
			int			GetPopLength();
			void        GiveCardAni();

public:
	BOOL		  m_bBankerCard;


	PlaneObject*  m_plCardMask[CHAIR_CARDS];				// ���ƺ������Ƥ
};


}