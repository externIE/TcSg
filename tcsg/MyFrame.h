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
	virtual void OnShowPlayerDetail(int nUserID,POINT pt);//����������渽������ʾ�����ϸ���
	virtual void DrawBackGround();
	virtual void OPE_DrawUserDetail(GAMEPLAYER* pGamePlayer);
			BOOL IsTightUI();//����UI�Ų�
};
