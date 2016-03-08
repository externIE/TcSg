#pragma once


class CMyGameFrame:public CXyGameFrame
{
public:
	CMyGameFrame();
	virtual ~CMyGameFrame();
	virtual void IniGameStatus();
	virtual void OnIniData();
	virtual void GameInstance();
	virtual void LoadRes();
	virtual void CreateFrame();
	virtual void AdjustFrame();
	virtual void OnTest();
	virtual void DrawFrameTop();
	virtual void OnShowPlayerDetail(int nUserID,POINT pt);//在玩家姓名版附近，显示玩家详细面板
	virtual void DrawBackGround();
	virtual void OPE_DrawUserDetail(GAMEPLAYER* pGamePlayer);
			BOOL IsTightUI();//紧凑UI排布
};
