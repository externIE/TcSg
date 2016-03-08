//GameChargeDlg.h : interface for the CGameChargeDlg class
//��Ϸ��ֵ�Ի���
///////////////////////////////////////////////////////////////
//add by fangxj

#if !defined(AFX_GAMECHARGEDLG_H_FHSKDFKJJFKSDJFK_F5678231_INCLUDE_)
#define AFX_GAMECHARGEDLG_H_FHSKDFKJJFKSDJFK_F5678231_INCLUDE_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MONEY_RECT_WIDTH        56//��ʾ������
#define MONEY_RECT_HEIGHT       24//��ʾ����߶�

#define TOTAL_DENOMINATION      8//��ֵ��ѡ�������������8������ֵ

enum MONEY_STATUS
{
    MONEY_STATUS_NORMAL = 0,  //��ͨ״̬
    MONEY_STATUS_TOUCH,       //����״̬
    MONEY_STATUS_SEL,         //ѡ��״̬
};

typedef struct _tagMONEY_INFO
{
    int             nMoney;//���
    MONEY_STATUS    enStatus;//�ý��״̬
    int             nExChangeTongbao;//������ܶһ�����ͨ��
    int             nExChangeSilver;//������ܶһ���������
    int             nGivePercent;//��ֵ�ý����Ͱٷ�֮����
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
    void                ShowChargeDlg();//��ʾ�Ի���
    void                HideChargeDlg();//���ضԻ���
    void                OnCloseDlg();   //�رմ���
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
	int         m_chargeStat;       //0��״̬,1webcharge,2orchare,-1�ر�״̬
	CHARGEURL   m_chargeUrl;        //��ֵ��صĵ�ַ
};

#endif