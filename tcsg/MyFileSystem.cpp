#include "stdafx.h"
#include "MyFileSystem.h"

CMyFileSystem::CMyFileSystem()
{
	m_szFileName.Empty();
	memset(m_readItem, 0, 32);
	memset(m_recordKey, 0, 32);
}

CMyFileSystem::~CMyFileSystem()
{
	
}

BOOL CMyFileSystem::InitFilePath(TCHAR *fileName)
{
	if(fileName == NULL)
	{
		return FALSE;
	}
	TCHAR tcExePath[MAX_PATH] = _T("\0");
	GetModuleFileName(NULL, tcExePath, MAX_PATH);
	TCHAR *pFind = _tcsrchr(tcExePath, '\\'); 
	if(pFind == NULL)
	{
		return FALSE;
	}
	
	*pFind = _T('\0');
	
	m_szFileName = tcExePath;
	m_szFileName += "\\"; 
	m_szFileName += fileName;
	
	return TRUE;
}

BOOL CMyFileSystem::SetFileItem(TCHAR *item)
{
	if(item == NULL)
	{
		return FALSE;
	}
	
    strcpy(m_readItem, item);
	
	return TRUE;
}

BOOL CMyFileSystem::SetFileKeyByUserId()
{
	int userId = theApp.GetUserID();
	if(userId <= 0)
	{
		return FALSE;
	}
	sprintf(m_recordKey, _T("%d"), userId);	
	
	return TRUE;
}

int CMyFileSystem::ReadFile()
{
	int stata = 0;
	
	stata = GetPrivateProfileInt(m_readItem, m_recordKey, 0, m_szFileName);

	if(stata <= 0)
	{
		stata = 0;
	}
	else
	{
		stata = 1;
	}
	
	return stata; 	
}

void CMyFileSystem::WriteFile(int nStata)
{
	int nValue = 0;
	if(nStata <= 0)
	{
		nValue = 0;
	}
	else
	{
		nValue = 1;
	}
	TCHAR state[32] = "\0";
	_stprintf(state, _T("%d"), nValue);
	WritePrivateProfileString(m_readItem, m_recordKey, state, m_szFileName);	
}



