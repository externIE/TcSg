// GameRequest.h: interface for the CGameRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEREQUEST_H__93409324_E26B_4A4D_B8C4_9331792292B0__INCLUDED_)
#define AFX_GAMEREQUEST_H__93409324_E26B_4A4D_B8C4_9331792292B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGameRequest : public CSKGameConnect  
{
public:
	CGameRequest();
	virtual ~CGameRequest();

public:

	virtual BOOL  GC_EnterGame(CString& sRet);	 //������ͨ��Ϸ
	virtual void  PR_EnterGameOK(LPVOID pRet,int nDataLen);		//δ����Ϸ�� ����
	virtual void  PR_EnterGameDXXW(LPVOID pRet,int nDataLen);	//�������� ����
	virtual void  PR_EnterGameIDLE(LPVOID pRet,int nDataLen);	//������ҽ�����Ϸ

	virtual	BOOL  GC_AskNewTableChair(CString& sRet);
	virtual BOOL   ProcessRequest(CString& sRet,UINT nRequest, int& nDataLen, VOID *pDataPtr, UINT &nResponse,
		LPVOID &pRet,BOOL bNeedEcho=TRUE);
	virtual	BOOL  GC_SyncStartCountdown(int nCountdown);
public:
	UINT    GC_QiangZhuang(BOOL bQZ);
	UINT    GC_YaZhu(int nDbl);
	UINT	GC_ZuPai(CString &sRet,ZUPAI * pZuPai);
	//��ά���ֵstart
    virtual BOOL  GC_RoomLimitInfo();
    virtual BOOL  GC_ChargeBtnCount(CHARGE_BTN_TYPE enChargeBtnType, int nChargeNumber = 0);
    //��ά���ֵend
};

#endif // !defined(AFX_GAMEREQUEST_H__93409324_E26B_4A4D_B8C4_9331792292B0__INCLUDED_)
