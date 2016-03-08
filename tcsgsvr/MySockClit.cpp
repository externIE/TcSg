#include "StdAfx.h"

#define		COMMON_REQ_BASE			20000
#define		GR_SEND_PULSE			(COMMON_REQ_BASE + 20)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CMySockClit::OnRequest(void* lpParam1, void* lpParam2)
{
	LPCONTEXT_HEAD pContext = LPCONTEXT_HEAD(lpParam1);
	LPREQUEST pRequest = LPREQUEST(lpParam2);
	
	// if received msg of socket close. we do reconnect
	if(UR_SOCKET_CLOSE == pRequest->head.nRequest){
		
		UwlTrace(_T("Disconnected from chunk server."));
		UwlLogFile(_T("Disconnected from chunk server."));
		
		PostThreadMessage(((CGameServer*)m_pMainServer)->GetThreadID_ChunkConnect(), 
							UM_DATA_TOSEND, UR_SOCKET_CLOSE, 0);
		return TRUE;
	}
	
	if(0 == pRequest->head.nRepeated)
	{ 
		OnNotifyClient(pRequest);
	}
	else
	{	
		CONTEXT_HEAD context;
		memcpy(&context, pRequest->pDataPtr, sizeof(context));
		context.bNeedEcho = FALSE;
		
		REQUEST request;
		memset(&request, 0, sizeof(request));
		request.head.nRequest = pRequest->head.nRequest;
		request.head.nSubReq  = pRequest->head.nSubReq;
		
		OnResponse(&context, &request, pRequest);
		
		UwlClearRequest(&request);
	}
	UwlClearRequest(pRequest);
	return TRUE;
}

BOOL CMySockClit::OnResponse(LPCONTEXT_HEAD lpContext, LPREQUEST lpReqToClient, LPREQUEST lpReqFromSvr)
{
	switch(lpReqToClient->head.nRequest)
	{
	default:
		CSockClient::OnResponse(lpContext, lpReqToClient, lpReqFromSvr);
		break;
	}
	return TRUE;
}

BOOL CMySockClit::OnNotifyClient(LPREQUEST lpRequest)
{
	BOOL bRet=FALSE;
	switch(lpRequest->head.nRequest)
	{
	case GR_SEND_PULSE:
		bRet=OnSendPulse(lpRequest);
		break;
	default:
		bRet=CSockClient::OnNotifyClient(lpRequest);
		break;
	}
	return bRet;
}

BOOL CMySockClit::OnSendPulse(LPREQUEST lpRequest)
{
	if (!m_pMainServer)
		return FALSE;
	
	return ((CGameServer*)m_pMainServer)->OnChunkSendPulse(lpRequest);
}
