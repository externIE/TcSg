#pragma once

////////////////////////////////自动划银////////////////////////////////////
#define SAFEBOX_AUTOSETDEPOSIT_DLG	"AutoSetDepositDlg"
class CAutoSetDepositDlg:public CXyStandDlg
{
public:
	CAutoSetDepositDlg();
	virtual ~CAutoSetDepositDlg(){}
	
	virtual void	CreateSetDepositDlg();
	virtual void    RePaintTitle();						//重绘上标题
	virtual void	CreateSpirit();	
	virtual void	SpiritContext();
	virtual void	CreatInputBox();
	virtual void	CreateBtn();
	virtual void	OnOK();
	virtual void	OnCancel();
	virtual void	OnCheck();
	virtual int		IsAutoSetDeposit();
	virtual BOOL	OnMsg(SNG_MSG& Msg);
	
			void	FillDeposit(int nFixDeposit);
	
protected:
	int				m_nFixDeposit;			// 设定的银两值
	CGameInputBox*  m_pInputDeposit;		// 输入框

	int				m_nCheckAuto;			// 是否选中
	CXyCheckButton* m_pCheckAuto;			// 是否开启自动补银功能
};