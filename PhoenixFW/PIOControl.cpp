
/**************************************************************************
	created:	2012/11/18	13:55
	filename: 	PIOControl.CPP
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "stdafx.h"

#include "PIOControl.h"

CPIOControl::CPIOControl()
{
	m_fnIoControl = NULL;

	//--------------------------------------------------
	// load DLL and get PLSPIoControl function point
	TCHAR szPathName[256];
	TCHAR* p;
	if(::GetFullPathName(PHOENIX_SERVICE_DLL_NAME, 256, szPathName, &p) != 0)
	{
		m_hLSPModule = ::LoadLibrary(szPathName);
		if(m_hLSPModule != NULL)
		{
			m_fnIoControl = (PFNLSPIoControl)::GetProcAddress(m_hLSPModule, "PLSPIoControl");
		}
		else
		{
			m_hLSPModule = ::LoadLibrary(DLLPATH);
			if(m_hLSPModule != NULL)
			{
				m_fnIoControl = (PFNLSPIoControl)::GetProcAddress(m_hLSPModule, "PLSPIoControl");
			}
		}
	}	
	if(m_fnIoControl == NULL)
	{
		::MessageBox(NULL, _T("Can not find LSP module"), _T("error"), 0);
		exit(0);
	}
}

CPIOControl::~CPIOControl()
{
	if(m_hLSPModule != NULL)
		::FreeLibrary(m_hLSPModule);
}

void CPIOControl::SetWorkMode(int nWorkMode)
{
	m_IoControl.ucWorkMode = nWorkMode;
	m_fnIoControl(&m_IoControl, IO_CONTROL_SET_WORK_MODE);
}

int CPIOControl::GetWorkMode()
{
	return m_fnIoControl(&m_IoControl, IO_CONTROL_GET_WORK_MODE);
}

void CPIOControl::SetRuleFile(RULE_FILE_HEADER *pHeader, RULE_ITEM *pRules)
{
	// apply for a temp rule object
	RULE_FILE RuleFile;
	memcpy(&RuleFile.header, pHeader, sizeof(RULE_FILE_HEADER));
	memcpy(&RuleFile.LspRules, pRules, sizeof(RULE_ITEM) * pHeader->ulLspRuleCount);

	// set the filter rule
	m_IoControl.pRuleFile = &RuleFile;
	m_fnIoControl(&m_IoControl, IO_CONTROL_SET_RULE_FILE);
}

void CPIOControl::SetPhoenixInstance(HWND hWnd, TCHAR *pszPathName)
{
	// set instance handle
	m_IoControl.hPhoenixWnd = hWnd;
	wcscpy(m_IoControl.szPath, pszPathName);
	m_fnIoControl(&m_IoControl, IO_CONTROL_SET_PHOENIX_INSTANCE);
}

void CPIOControl::GetSession(SESSION *pSession, int nIndex)
{
	// get session that happened event
	m_IoControl.pSession = pSession;
	m_IoControl.nSessionIndex = nIndex;
	m_fnIoControl(&m_IoControl, IO_CONTROL_GET_SESSION);
}

void CPIOControl::SetQueryApp(int nIndex, BOOL bPass)
{
	m_IoControl.nSessionIndex = nIndex;
	m_IoControl.ucWorkMode = bPass;
	m_fnIoControl(&m_IoControl, IO_CONTROL_SET_QUERY_SESSION);
}

LPCTSTR CPIOControl::GetQueryApp(int nIndex)
{
	m_IoControl.nSessionIndex = nIndex;
	m_fnIoControl(&m_IoControl, IO_CONTROL_GET_QUERY_SESSION);
	return m_IoControl.szPath;
}

