 
#pragma once
class CMyAdvertBar:public CAdvertBar
{
public:
	
			 virtual  void OnIniData();
			 virtual  void OnInitData();	//房间初始化后收到WM_INIT_DATA消息
			 virtual  void OnSize();
			 virtual  BOOL OnMsg(SNG_MSG& Msg);
public:
			 virtual  void ClickHtmlBtn(LPCTSTR strBtn);
			 virtual  void RefreshHtmlCompletely();
			 virtual  void OnOpenHtml(LPCTSTR sURL);
			 virtual  void DrawResult();
			 virtual  void UpdataResult();
			 virtual  void OnFreshScrollBar(SROLLBAR_FRESH* pFresh);
			 virtual  void OnScrollBarLeftBtn();
			 virtual  void OnScrollBarRightBtn();
public:
			 virtual  void ShowAdvert();
			 virtual  void ShowResult();
			 virtual  void OnAutoChangeAdvert();
			 
			 //防止窗口改变尺寸时，ie控件闪烁
			 virtual  BOOL IsSomeWebShow();
			 virtual  void HideAllWebs();
			 virtual  void RecoverWebs();
};


class CMySubSystem : public CSubSystem
{
 
public:
 
   virtual  void CreateAdvertBar();
   virtual  int  GetAdvertBarHeight();
public:
	virtual  void NTF_PlayerEnter(GAMEPLAYER* pPlayer);//其他玩家进入
	virtual  void NTF_PlayerAbort(GAMEPLAYER* pPlayer);//其他玩家离开
	virtual  void NTF_VisitorEnter(GAMEPLAYER* pPlayer);//旁观者进入
	virtual  void NTF_VisitorAbort(GAMEPLAYER* pPlayer);//旁观者退出
	virtual  void NTF_StartSoloTable();
	virtual  void NTF_UserPosition();
	virtual  void NTF_UserDepositEvent(LPUSER_DEPOSITEVENT pUser_Deposit);

	virtual  void OnGameSize();
	virtual  BOOL OnMsg(SNG_MSG& msg);
};
 

//保护密码验证
class CMyKeyResult:public CNumKeyPad
{
public:
	CMyKeyResult(){};
	virtual ~CMyKeyResult(){}
	virtual void	OnOK();
};