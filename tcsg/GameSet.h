#pragma once
//”Œœ∑…Ë÷√√Ê∞Â
using namespace ShinyNova;

enum{
	SET_CALLBACK_CHECK_SOUND = GAME_MSG_END+1,
	SET_CALLBACK_CHECK_MUSIC,
	SET_CALLBACK_CHECK_REJECTPASSSOUND,
	SET_CALLBACK_CHECK_REJECTKANPAI,
	SET_CALLBACK_CHECK_REMAIND,
	SET_CALLBACK_CHECK_NO_TIP,
	SET_CALLBACK_MAX
};

class CGameSetBoard:public CShinyNovaGui
{
public:
	CGameSetBoard();
	virtual ~CGameSetBoard();
	virtual void OnIniData();
	virtual BOOL OnMsg(SNG_MSG& Msg);
	virtual void CheckSet();

public:
	CXyCheckButton* m_btnSound;
	CXyCheckButton* m_btnMusic;
	CXyCheckButton* m_btnRejectPassSound;
	CXyCheckButton* m_btnRejectKanPai;
	CXyCheckButton* m_btnRemaind;
	CXyCheckButton* m_btnNoTip;
};