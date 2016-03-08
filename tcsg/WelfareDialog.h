#pragma once
using namespace ShinyNova;

class CWelfareDialog: public CShinyNovaGui
{
public:
	CWelfareDialog();
	virtual ~CWelfareDialog(); 
    
	void   ShowWelfareDlg();//显示对话框
    void   HideWelfareDlg();//隐藏对话框
	void   CreateWelfDialog();
	void   OnApplyWelfare(int nSaveTo, int *pGetAward);
	void   OnGetWelfareConfig(int nActivityID);
	int    GetWelfareLine();
	void   SetBtnEnable(bool bIsTrue);
	int    RecordOrReadWelfareNum(bool bIsWrite);
	void   SetWelfNum(int num) {m_welfareSurplusNum = num;}
	int    GetWelfareStat() {return m_welfareState;}
	void   SetWelfareStat(int welfStat) {m_welfareState = welfStat;}
	int    GetCanGetMon() {return m_getMonney;}
	void   SetCanGetMon(int getMonney) {m_getMonney = getMonney;}
	int    GetLimitNum() {return m_welfareCfg.nDailyLimitNum;}
	int    GetWelfRealStat() {return m_realWelfStat;}

protected:
	virtual  void   OnIniData();
	void   OnInitSize();
	void   CheckGetFareWare();
	
	//welfare begin
	GET_WELFARECONFIG_OK m_welfareCfg;
	CXyStandButton*      m_btnGetAward;	    //领取奖励按钮
	CXyStandButton*      m_btnClose;
	PlaneObject*	     m_plWelfareDlg;	//低保领取框
	int                  m_realWelfStat;    //0：符合低保条件 -1：低保活动无效； -2：玩家银两高于低保线,在换桌时将按钮改回原有状态
	int			         m_welfareState;	//0：符合低保条件 -1：低保活动无效； -2：玩家银两高于低保线
	                                        //-3：游戏进行中无法领取； -4：玩家本地领取记录次数已大于领取上限
	//welfare	end  

	int                  m_welfareSurplusNum;   //本地记录领取低保的次数
	int                  m_getMonney;           //可以领取的金钱数
	int                  m_nScale;              //拉伸的倍数
};