#pragma once
//同城内置记牌器,必须在同城棋牌工作环境内才能正确运作，部分参数引用服务器端定义的宏

using namespace ShinyNova;
#define  CARDID_UNKOWN     99999999

class CXyCardMaster:public CShinyNovaGui
{
public:
	CXyCardMaster();
	virtual ~CXyCardMaster();
	virtual  void OnIniData();
	virtual  void RePaint();
	virtual  void RePaint(int nChairNO);
	virtual  void ReLoad();//重新加载用户信息，每次收到ArrageTable的时候处理
	virtual  void ClearAllCard();
	virtual  void PushCard(int nCardID); //增加一张牌
	virtual  void PopCard(int nCardID);  //去掉一张牌
	virtual  void ReCordChairCard(int nChairNO,int nCardID=CARDID_UNKOWN);
	virtual  BOOL OnMsg(SNG_MSG& Msg);
	virtual  void OnPopMenu(POINT pt);
	virtual  void OnCheckPlayer(int chairno);
	virtual  void OnSize();
	virtual  void FitPlayerSize(int nDrawIndex);
	
  	CGameWinApp*  GetGameWinApp();
	virtual  SNG_PIC GetPicByID(int nCardID);
	inline	int GetCardPic(int nCardID)
	{
		return nCardID%54;
	}
	int				RUL_GetDrawIndexByChairNO(int nChairNO);		 
public:
	DWORD			m_dwMode;  //模式
	int             m_nCardID[TOTAL_CARDS];
	int             m_nChairCard[TOTAL_CHAIRS][CHAIR_CARDS];
	int             m_nCardCount;
	CXyPopMenu*     m_PopMenu;
	PlaneObject*    m_PlayerPlane[TOTAL_CHAIRS];//按DrawIndex排列
};