#pragma once

class CMySockClit : public CSockClient
{
public:
	CMySockClit(CBaseServer* pMainServer, 
				const BYTE key[] = 0, const ULONG key_len = 0, 
				DWORD flagEncrypt = 0, DWORD flagCompress = 0) 
		: CSockClient(pMainServer, key, key_len, flagEncrypt, flagCompress)
	{
	}

	CMySockClit(CBaseServer* pMainServer, int nKeyType,  DWORD flagEncrypt = 0, DWORD flagCompress = 0) 
		: CSockClient(pMainServer, nKeyType, flagEncrypt, flagCompress)
	{
	}

protected:
	virtual BOOL OnRequest(void* lpParam1, void* lpParam2);	
	virtual BOOL OnResponse(LPCONTEXT_HEAD lpContext, LPREQUEST lpReqToClient, LPREQUEST lpReqFromServer);
	virtual BOOL OnNotifyClient(LPREQUEST lpRequest);
	virtual BOOL OnSendPulse(LPREQUEST lpRequest);
};
