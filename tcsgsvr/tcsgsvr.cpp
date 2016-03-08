// TcSgSvr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TcSgSvr.h"

#include  <dbghelp.h > 
#pragma comment(lib,  "dbghelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

void WriteMiniDMP(struct _EXCEPTION_POINTERS *pExp)
{
	CString   strDumpFile;
	TCHAR szFilePath[MAX_PATH];
	GetModuleFileName(NULL,szFilePath,MAX_PATH);
	*strrchr(szFilePath,'\\')=0;
    strDumpFile.Format("%s\\%d.dmp",szFilePath,CTime::GetCurrentTime().GetTickCount());
    HANDLE   hFile   =   CreateFile(strDumpFile,   GENERIC_WRITE,   FILE_SHARE_WRITE,   NULL,   CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,   NULL   ); 
    if   (hFile!=INVALID_HANDLE_VALUE) 
    { 
        MINIDUMP_EXCEPTION_INFORMATION   ExInfo; 
        ExInfo.ThreadId   =   ::GetCurrentThreadId(); 
        ExInfo.ExceptionPointers   =   pExp; 
        ExInfo.ClientPointers   =   NULL; 
        //   write   the   dump 
        BOOL   bOK   =   MiniDumpWriteDump(GetCurrentProcess(),   GetCurrentProcessId(),   hFile,   MiniDumpNormal,   &ExInfo,   NULL,   NULL   ); 
        CloseHandle(hFile); 
    } 
}


LONG WINAPI ExpFilter(struct _EXCEPTION_POINTERS *pExp)  
{  
	WriteMiniDMP(pExp);
    return EXCEPTION_EXECUTE_HANDLER;  
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{	
	::SetUnhandledExceptionFilter(ExpFilter);
	DWORD dwTraceMode = UWL_TRACE_DATETIME | UWL_TRACE_FILELINE | UWL_TRACE_NOTFULLPATH
						| UWL_TRACE_FORCERETURN | UWL_TRACE_CONSOLE;
	//UwlBeginTrace(PRODUCT_NAME, dwTraceMode);
	if( !XygInitNoRes(PRODUCT_NAME, dwTraceMode))
		return 0; 
#ifdef UWL_SERVICE
	CString sDisplayName;
	BOOL bChinese=(GetUserDefaultLangID()==0x804);
	sDisplayName=(bChinese?STR_DISPLAY_NAME:STR_DISPLAY_NAME_ENU);
	CGameService MainService(STR_SERVICE_NAME, sDisplayName, 2, 0,
		PRODUCT_LICENSE, PRODUCT_NAME, PRODUCT_VERSION, 
		GAME_PORT, GAME_ID, ENCRYPT_AES, 0);

    if (!MainService.ParseStandardArgs(argc, argv)) {
        // Didn't find any standard args so start the service
        // Uncomment the DebugBreak line below to enter the debugger when the service is started.
        //DebugBreak();
        MainService.StartService();
    }
	// When we get here, the service has been stopped
    int nRetCode = MainService.m_Status.dwWin32ExitCode;
#else
	g_gameServer = new CGameServer(PRODUCT_LICENSE, PRODUCT_NAME, PRODUCT_VERSION, 
		GAME_PORT, GAME_ID, ENCRYPT_AES, 0);
	
	if(FALSE == g_gameServer->Initialize()){
		UwlTrace(_T("server initialize failed!"));
	}

	UwlTrace( "Type 'q' when you want to exit. " );
	TCHAR ch;
	do{
		ch = _getch();
		ch = toupper( ch );
	} while( ch != 'Q' );
	
	g_gameServer->Shutdown();
	
#endif

	
	XygTermNoRes();
	//UwlEndTrace();
	return 1;
}


