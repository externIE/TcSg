//GameChargeDlg.h : interface for the CGameChargeDlg class
//游戏充值对话框
///////////////////////////////////////////////////////////////
//add by fangxj

#if !defined(AFX_GAMECHARGEDLG_H_FHSKDFKJJFKSDJFK_F5678231_INCLUDE_)
#define AFX_GAMECHARGEDLG_H_FHSKDFKJJFKSDJFK_F5678231_INCLUDE_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MONEY_RECT_WIDTH        56//显示金额框宽度
#define MONEY_RECT_HEIGHT       24//显示金额框高度

#define TOTAL_DENOMINATION      8//充值可选的面额总数，有8种面额供充值

enum MONEY_STATUS
{
    MONEY_STATUS_NORMAL = 0,  //普通状态
    MONEY_STATUS_TOUCH,       //触碰状态
    MONEY_STATUS_SEL,         //选择状态
};

typedef struct _tagMONEY_INFO
{
    int             nMoney;//面额
    MONEY_STATUS    enStatus;//该金额状态
    int             nExChangeTongbao;//该面额能兑换多少通宝
    int             nExChangeSilver;//该面额能兑换多少银子
    int             nGivePercent;//充值该金额，赠送百分之多少
}MONEY_INFO, *LPMONEY_INFO;

class CGameChargeDlg : public CDialog
{
//data
private:
    MONEY_INFO          m_stMoneyInfo[TOTAL_DENOMINATION];

    CxImage             m_imgMoneyNoraml[TOTAL_DENOMINATION];
    CxImage             m_imgMoneyTouch[TOTAL_DENOMINATION];
    CxImage             m_imgMoneySel[TOTAL_DENOMINATION];

    CxImage             m_imgBK;

    CFont               m_Font1;
    CFont               m_Font2;

    BOOL                m_bTransDeposit;
public:
    enum {IDD = IDD_GAMECHARGE};
    
//functions
public:
    CGameChargeDlg(CWnd* pParent = NULL);
    void                ShowChargeDlg();//显示对话框
    void                HideChargeDlg();//隐藏对话框
    void                OnCloseDlg();   //关闭窗口
    void                OnWebCharge();
    void                OnQrcodeCharge();
	void                OnQrcodeCharge_WeChat();
    void                SetMoneyInfo(MONEY_INFO* pMoneyInfo, LPCHARGEURL pChargeUrl);

    void                GetMoneyRect(int nIndex, RECT& rc);
    void                DrawMoneyRect(CPaintDC& dc);
    void                DrawChargeText(CPaintDC& dc);
	int                 GetChargeStat() {return m_chargeStat;}
	void                SetChargeStat(int chargeStat) {m_chargeStat = chargeStat;}

protected:
    virtual     void    DoDataExchange(CDataExchange* pDX);
    virtual     BOOL    OnInitDialog();
    afx_msg     void    OnPaint();
    afx_msg     UINT    OnNcHitTest(CPoint point);
    afx_msg     BOOL    OnEraseBkgnd(CDC* pDC);
    afx_msg     void    OnSize(UINT nType, int cx, int cy);
    afx_msg     void    OnMouseMove(UINT nFlags, CPoint point);
    afx_msg     void    OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg     HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg     BOOL    OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

    DECLARE_MESSAGE_MAP()

protected:
	int         m_chargeStat;       //0打开状态,1webcharge,2orchare,-1关闭状态
	CHARGEURL   m_chargeUrl;        //充值相关的地址
};

#endif