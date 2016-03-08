// GameRequest.cpp: implementation of the CGameRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game.h"
#include "GameRequest.h"
#include "resource.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameRequest::CGameRequest()
{

}

CGameRequest::~CGameRequest()
{

}

BOOL CGameRequest::ProcessRequest(CString& sRet,UINT nRequest, int& nDataLen, VOID *pDataPtr, UINT &nResponse,
								  LPVOID &pRet,BOOL bNeedEcho)
{
	//系统消息不进入重启动之列
	if (nRequest<=GAME_REQ_BASE_EX+20000)
		return CGameConnect::ProcessRequest(sRet,nRequest,nDataLen,pDataPtr,nResponse,pRet,bNeedEcho);
	
	BOOL bn=CGameConnect::ProcessRequest(sRet,nRequest,nDataLen,pDataPtr,nResponse,pRet,bNeedEcho);
	if (bNeedEcho&&!bn)//需要回应的时候
	{
	//	if (nResponse==UR_OPERATE_FAILED||!bn)//返回UR_OPERATE_FAILED或者链接失败
	//	{
		UwlLogFile("ProcoessRequest Faild,ReQuestid:%ld,nResponse:%ld",nRequest,nResponse);
		//((CGameApp*)GetGameWinApp())->UpdateTableInfo();
			return FALSE;//返回False,避免重复执行
	//	}
	}
	return TRUE;
}

BOOL CGameRequest::GC_EnterGame(CString& sRet)
{
	BOOL bRet=CSKGameConnect::GC_EnterGame(sRet);
	if (bRet)
	{
		CGameApp* pApp=(CGameApp*)GetGameWinApp();
		if (pApp)
		{
			pApp->AchToUI();
		}
	}
	return bRet;
}

void CGameRequest::PR_EnterGameOK(LPVOID pRet,int nDataLen)
{
	if(nDataLen>0)
	{
		int nInfoLen=theApp.GetEnterInfoSize();
		BYTE* pData=(BYTE*)pRet;
		if (nDataLen == nInfoLen)
		{
			memcpy(GetGameWinApp()->GetEnterInfo(), pData, nInfoLen);
		}
	}
}

void  CGameRequest::PR_EnterGameDXXW(LPVOID pRet,int nDataLen)
{
	if(nDataLen>0)
	{
		if (GetGameWinApp()->IsCloakingRoom() && !GetGameWinApp()->IsLookOn())
		{
			CGameConnect::PR_EnterGameDXXW(pRet,nDataLen);
		}
		else
		{
			int nInfoLen=theApp.GetTableInfoSize();
			LPBYTE pData=(LPBYTE)pRet;
			if (nDataLen == nInfoLen)
			{
				memcpy(GetGameWinApp()->GetTableInfo(), pData ,nInfoLen);
			}
		}
	}
}

//空闲玩家进入游戏
void  CGameRequest::PR_EnterGameIDLE(LPVOID pRet,int nDataLen)
{
	if(nDataLen>0)
	{
		int nInfoLen=theApp.GetTableInfoSize();
		BYTE* pData=(BYTE*)pRet;
		if (nDataLen == nInfoLen)
		{
			memcpy(GetGameWinApp()->GetTableInfo(), pData, nInfoLen);
		}
	}
}

BOOL CGameRequest::GC_AskNewTableChair(CString& sRet)
{
	BOOL bR=CSKGameConnect::GC_AskNewTableChair(sRet);
	if (bR)
	{
		theApp.ClearChairStatus();
	}
	return bR;
}

UINT  CGameRequest::GC_ZuPai(CString &sRet,ZUPAI * pZuPai)
{
	sRet.Empty();

	///////////     Process  Request   ///////////////////////////////////////
	UINT nResponse = UR_OPERATE_SUCCEEDED;
	LPVOID pRet=NULL;
	int nDataLen=sizeof(ZUPAI);
	BOOL bResult = ProcessRequest(sRet,GR_ZU_PAI, nDataLen, pZuPai, nResponse, pRet);
	
	if (bResult==FALSE)
		nResponse=UR_OPERATE_FAILED;
	
	return nResponse;
}

BOOL  CGameRequest::GC_SyncStartCountdown(int nCountdown)
{
	SYNC_COUNTDOWN  sc;
	memset(&sc,0,sizeof(SYNC_COUNTDOWN));
	sc.nUserID=GetGameWinApp()->GetUserID();
	sc.nRoomID=GetGameWinApp()->GetRoomID();
	sc.nTableNO=GetGameWinApp()->GetTableNO();
	sc.nChairNO=GetGameWinApp()->GetChairNO();
	sc.nCountDown = nCountdown;
	///////////     Process  Request   ///////////////////////////////////////
	CString sRet;
	UINT nResponse;
	LPVOID pRet=NULL;
	int nDataLen=sizeof(SYNC_COUNTDOWN);
	BOOL bResult = ProcessRequest(sRet,GR_SYNC_START_COUNTDOWN, nDataLen, &sc, nResponse, pRet,FALSE);
   	return bResult;
}

UINT CGameRequest::GC_QiangZhuang(BOOL bQZ)           
{
	QZ stQZ;
	memset(&stQZ,0,sizeof(stQZ));
	
	CGameWinApp* pGameApp=GetGameWinApp();
	stQZ.nUserID=pGameApp->GetUserID();
	stQZ.nRoomID=pGameApp->GetRoomID();
	stQZ.nTableNO=pGameApp->GetTableNO();
	stQZ.nChairNO=pGameApp->GetChairNO();
	stQZ.nQZFlag=bQZ?1:0;

	CString sRet;
	sRet.Empty();

	UINT nResponse = UR_OPERATE_SUCCEEDED;
	LPVOID pRet=NULL;
	int nDataLen=sizeof(QZ);
	int bResult = ProcessRequest(sRet,GR_QZ, nDataLen, &stQZ, nResponse, pRet);
	
	if (bResult==FALSE)
		nResponse=UR_OPERATE_FAILED;
	
	return nResponse;
}

//看牌抢庄客户端发往服务端压注压了多少倍的请求
UINT CGameRequest::GC_YaZhu(int nDbl)
{
	YAZHU stYZ;
	memset(&stYZ,0,sizeof(stYZ));
	
	CGameWinApp* pGameApp=GetGameWinApp();
	stYZ.nUserID=pGameApp->GetUserID();
	stYZ.nRoomID=pGameApp->GetRoomID();
	stYZ.nTableNO=pGameApp->GetTableNO();
	stYZ.nChairNO=pGameApp->GetChairNO();
	stYZ.nDbl=nDbl;

	CString sRet;
	sRet.Empty();

	UINT nResponse = UR_OPERATE_SUCCEEDED;
	LPVOID pRet = NULL;
	int nDataLen = sizeof(YAZHU);
	int bResult = ProcessRequest(sRet,GR_YAZHU, nDataLen, &stYZ, nResponse, pRet);
	
	if (bResult==FALSE)
		nResponse=UR_OPERATE_FAILED;
	
	return nResponse;
}

//二维码充值start
BOOL CGameRequest::GC_RoomLimitInfo()
{
    ROOMLIMIT   stRoomLimit;
    ZeroMemory(&stRoomLimit, sizeof(stRoomLimit));
    stRoomLimit.nUserID     =   theApp.GetUserID();
    stRoomLimit.nRoomID     =   theApp.GetRoomID();
    stRoomLimit.nTableNO    =   theApp.GetTableNO();
    stRoomLimit.nChairNO    =   theApp.GetChairNO();
	
    CString sRet;
    UINT    nResponse = 0;
    LPVOID  pRet = NULL;
    int     nDataLen = sizeof(ROOMLIMIT);
    BOOL    bResult = ProcessRequest(sRet, GR_ROOMLIMIT, nDataLen, &stRoomLimit, nResponse, pRet, FALSE);
    return bResult;
}

BOOL CGameRequest::GC_ChargeBtnCount(CHARGE_BTN_TYPE enChargeBtnType, int nChargeNumber /* = 0 */)
{
    CHARGEBTN stChargeBtn;
    ZeroMemory(&stChargeBtn, sizeof(stChargeBtn));
    stChargeBtn.nUserID         =   theApp.GetUserID();
    stChargeBtn.nRoomID         =   theApp.GetRoomID();
    stChargeBtn.nTableNO        =   theApp.GetTableNO();
    stChargeBtn.nChairNO        =   theApp.GetChairNO();
    stChargeBtn.nChargeNumber   =   nChargeNumber;
    stChargeBtn.enType          =   enChargeBtnType;
	
    CString sRet;
    UINT    nResponse;
    LPVOID  pRet = NULL;
    int     nDataLen = sizeof(CHARGEBTN);
    BOOL    bResult = ProcessRequest(sRet, GR_CHARGEBTN_LEFTDOWN, nDataLen, &stChargeBtn, nResponse, pRet, FALSE);
    return bResult;
}
//二维码充值end
