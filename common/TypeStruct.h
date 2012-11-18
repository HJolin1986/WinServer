#pragma once

/**************************************************************************
	created:	2012/11/16	14:37	
	filename: 	TypeStruct.H
	file path:	e:\Documents\Visual Studio 2008\Projects\WinSock\common
	author:		DAILM, en_name: Dicky

	purpose:	
**************************************************************************/

#include "PMacRes.h"
#include "iocommon.h"

typedef struct						// user layer filter rule
{
	TCHAR szApplication[MAX_PATH];	// app name 
	UCHAR ucAction;					
	UCHAR ucDirection;				// direction 
	UCHAR ucServiceType;			// service type
	USHORT usServicePort;			
	TCHAR sDemo[51];				// description
}RULE_ITEM,  *PRULE_ITEM;


typedef struct						// file header
{
	TCHAR szSignature[12];			
	ULONG ulHeaderLength;			

	UCHAR ucMajorVer;	
	UCHAR ucMinorVer;		
	DWORD dwVersion;				// file header version

	UCHAR ucLspWorkMode;			// work mode
	UCHAR ucKerWorkMode;
	UCHAR bAutoStart;				// whether start app at window startup

	TCHAR szWebURL[MAX_PATH];		// Productor WEB 
	TCHAR szEmail[MAX_PATH];		// Productor Email

	ULONG ulLspRuleCount;			// the amount of filter at user layer
	ULONG ulKerRuleCount;			// amount of filter at kernel
}RULE_FILE_HEADER, *PRULE_FILE_HEADER;

typedef struct						// file structure
{
	RULE_FILE_HEADER header;
	RULE_ITEM LspRules[MAX_RULE_COUNT];
	PassthruFilter KerRules[MAX_RULE_COUNT];
}RULE_FILE, *PRULE_FILE;

//-------------------------------------------
// I/O
typedef struct  
{
	SOCKET s;
	int nProtocol;					// Protocol£¬TCP¡¢UDP¡¢FTP etc.
	UCHAR ucDirection;			
	ULONG ulLocalIP;			
	ULONG ulRemoteIP;			
	USHORT usLocalPort;			
	USHORT usRemotePort;	
	TCHAR szPathName[MAX_PATH];		// app path name
	UCHAR ucAction;					// action for the session
									// others   send data length, startup time etc.
}SESSION, *PSESSION;


typedef struct   
{
	UCHAR bUsed;					// whether this structure can be used
	TCHAR szPathName[MAX_PATH];		// app path name
	int nReturnValue;				// main module returned value
}QUERY_SESSION, *PQUERY_SESSION;

typedef struct						// IO control function's param pattern
{
	UCHAR ucWorkMode;				
	RULE_FILE *pRuleFile;			// rule file

	HWND hPhoenixWnd;				// the window that receive the msg

	TCHAR szPath[MAX_PATH];			// main app's path

	SESSION *pSession;				// get a Session
	int nSessionIndex;
}LSP_IO_CONTROL, *PLSP_IO_CONTROL;

typedef int  (__stdcall * PFNLSPIoControl)(PLSP_IO_CONTROL pIoControl, int nType);

