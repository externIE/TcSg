#pragma once
using namespace ShinyNova;

class CWelfareDialog: public CShinyNovaGui
{
public:
	CWelfareDialog();
	virtual ~CWelfareDialog(); 
    
	void   ShowWelfareDlg();//��ʾ�Ի���
    void   HideWelfareDlg();//���ضԻ���
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
	CXyStandButton*      m_btnGetAward;	    //��ȡ������ť
	CXyStandButton*      m_btnClose;
	PlaneObject*	     m_plWelfareDlg;	//�ͱ���ȡ��
	int                  m_realWelfStat;    //0�����ϵͱ����� -1���ͱ����Ч�� -2������������ڵͱ���,�ڻ���ʱ����ť�Ļ�ԭ��״̬
	int			         m_welfareState;	//0�����ϵͱ����� -1���ͱ����Ч�� -2������������ڵͱ���
	                                        //-3����Ϸ�������޷���ȡ�� -4����ұ�����ȡ��¼�����Ѵ�����ȡ����
	//welfare	end  

	int                  m_welfareSurplusNum;   //���ؼ�¼��ȡ�ͱ��Ĵ���
	int                  m_getMonney;           //������ȡ�Ľ�Ǯ��
	int                  m_nScale;              //����ı���
};