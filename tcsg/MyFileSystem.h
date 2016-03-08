#pragma once

class CMyFileSystem
{
	public:
	CMyFileSystem();
	virtual ~CMyFileSystem();
	
	BOOL InitFilePath(TCHAR *filename);
	BOOL SetFileItem(TCHAR *item);
	BOOL SetFileKeyByUserId();
	
	int ReadFile();
	void WriteFile(int stata);
	
	private:
	CString m_szFileName;
	TCHAR m_readItem[32];
	TCHAR m_recordKey[32];	
};