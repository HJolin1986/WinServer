
/**************************************************************************
	created:	2012/11/16	20:55
	filename: 	Acl.CPP
	file path:	f:\GitHub\WinSock\PhoenixLSP
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "stdafx.h"

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "../common/debug.h"

#include "Acl.h"
//-------------------------------------------
// share memory
// all app that use WINDOSCK to access net share the .initdata and .uinitdata segment
//-------------------------------------------
// segment .initdata 
#pragma data_seg(".initdata")							// segment .initdata 
HWND			g_hPhoenixWnd = NULL;					// main window handle
UCHAR			g_ucWorkMode = PF_PASS_ALL;				// work mode
#pragma data_seg()

//-------------------------------------------
// segment .uinitdata 
#pragma bss_seg(".uinitdata") 
RULE_ITEM		g_Rule[MAX_RULE_COUNT];					// app rule
ULONG			g_RuleCount;
QUERY_SESSION	g_QuerySession[MAX_QUERY_SESSION];		// ask main app about session
SESSION			g_SessionBuffer[MAX_SESSION_BUFFER];	// send session info to main app
TCHAR			g_szPhoenixFW[MAX_PATH];				// record main app path			
#pragma bss_seg()

extern TCHAR	g_szCurrentApp[MAX_PATH];

CRITICAL_SECTION	g_csGetAccess;

CAcl::CAcl()
{
	m_nSessionCount = 0;

	m_nSessionMaxCount = INIT_SESSION_BUFFER;			// alloc memory for session struct
	m_pSession = new SESSION[m_nSessionMaxCount];

	::InitializeCriticalSection(&g_csGetAccess);
}

CAcl::~CAcl()
{
	ODS(L" CAcl::~CAcl send CODE_APP_EXIT ... ");
	
	int nIndex = CreateSession(0, 0);					// inform main module that current app is exiting
	NotifySession(&m_pSession[nIndex], CODE_APP_EXIT);

	delete[] m_pSession;
	::DeleteCriticalSection(&g_csGetAccess);
}

/*
** modified   by : Dailm(Dicky)
** function param: s(socket), af(family), type(socket type), protocol
** modified  date: 2012/11/16 16:34
** description   : create session for new socket and point out the protocol
*/
void CAcl::CheckSocket(SOCKET s, int af, int type, int protocol)
{
	if (af != AF_INET)									// only support IPv4
		return;

	int nProtocol = RULE_SERVICE_TYPE_ALL;

	if(protocol == 0)
	{
		if(type ==  SOCK_STREAM)
		{
			nProtocol = RULE_SERVICE_TYPE_TCP;
		}
		else if(type == SOCK_DGRAM)
		{
			nProtocol = RULE_SERVICE_TYPE_UDP;
		}
	}
	else if(protocol == IPPROTO_TCP)
	{
		nProtocol = RULE_SERVICE_TYPE_TCP;
	}
	else if(protocol == IPPROTO_UDP)
	{	
		nProtocol = RULE_SERVICE_TYPE_UDP;
	}

	CreateSession(s, nProtocol);					
}


void CAcl::CheckCloseSocket(SOCKET s)
{
	DeleteSession(s);
}

/*
** modified    by: Dailm(Dicky)
** function param: s, addr
** modified  date: 2012/11/16 16:38
**    description: bind the SOCKET to the session
*/
void CAcl::CheckBind(SOCKET s, const PSOCKADDR addr)
{
	int nIndex;
	if((nIndex = FindSession(s)) >= m_nSessionCount)
		return;

	PSOCKADDR_IN pLocal = (PSOCKADDR_IN)addr;			// setting session			
	m_pSession[nIndex].usLocalPort = ntohs(pLocal->sin_port);

	if(pLocal->sin_addr.S_un.S_addr != ADDR_ANY)
		m_pSession[nIndex].ulLocalIP = *((PDWORD)&pLocal->sin_addr);
}

/*
** modified    by: Dailm(Dicky)
** function param: s, sNew, addr
** modified  date: 2012/11/16 16:40
**    description: create session or set by s, return access info
*/
int CAcl::CheckAccept(SOCKET s, SOCKET sNew, sockaddr FAR *addr)
{
	int nIndex;
	if((nIndex = FindSession(s)) >= m_nSessionCount)
		return PF_PASS;

	nIndex = CreateSession(sNew, RULE_SERVICE_TYPE_TCP);

	if(addr != NULL)
	{
		PSOCKADDR_IN pRemote = (PSOCKADDR_IN)addr;
		USHORT usPort = ntohs(pRemote->sin_port);
		DWORD dwIP = *((DWORD*)&pRemote->sin_addr);
		SetSession(&m_pSession[nIndex], usPort, dwIP, RULE_DIRECTION_IN_OUT);
	}

	return GetAccessInfo(&m_pSession[nIndex]);
}

int CAcl::CheckConnect(SOCKET s, const struct sockaddr FAR *addr)
{
	int nIndex;
	if((nIndex = FindSession(s)) >= m_nSessionCount)
		return PF_PASS;

	PSOCKADDR_IN pRemote = (PSOCKADDR_IN)addr;
	USHORT usPort = ntohs(pRemote->sin_port);
	DWORD dwIP = *((DWORD*)&pRemote->sin_addr);
	SetSession(&m_pSession[nIndex], usPort, dwIP, RULE_DIRECTION_IN_OUT);

	return GetAccessInfo(&m_pSession[nIndex]);
}

int CAcl::CheckSendTo(SOCKET s, const SOCKADDR *pTo)
{
	int nIndex;
	if((nIndex = FindSession(s)) >= m_nSessionCount)
		return PF_PASS;

	if(pTo != NULL)
	{
		PSOCKADDR_IN pRemote = (PSOCKADDR_IN)pTo;
		USHORT usPort = ntohs(pRemote->sin_port);
		DWORD dwIP = *((DWORD*)&pRemote->sin_addr);
		SetSession(&m_pSession[nIndex], usPort, dwIP, RULE_DIRECTION_OUT);
	}

	return GetAccessInfo(&m_pSession[nIndex]);
}

int CAcl::CheckRecvFrom(SOCKET s, SOCKADDR *pFrom)
{
	int nIndex;
	if((nIndex = FindSession(s)) >= m_nSessionCount)
		return PF_PASS;

	if(pFrom != NULL)
	{
		PSOCKADDR_IN pRemote = (PSOCKADDR_IN)pFrom;
		USHORT usPort = ntohs(pRemote->sin_port);
		DWORD dwIP = *((DWORD*)&pRemote->sin_addr);
		SetSession(&m_pSession[nIndex], usPort, dwIP, RULE_DIRECTION_IN);
	}
	return GetAccessInfo(&m_pSession[nIndex]);
}

//-------------------------------------------
// see the access privilege
int CAcl::GetAccessInfo(SESSION *pSession)
{
	if(_wcsicmp(g_szCurrentApp, g_szPhoenixFW) == 0)				// let it if it's the main module
	{
		return PF_PASS;
	}

	int nRet;
	if((nRet = GetAccessFromWorkMode(pSession)) != PF_FILTER)	// check work mode
	{
		ODS(L" GetAccessInfo return from WorkMode \n");
		return nRet;
	}

	//-------------------------------------------
	// if work mode is filter, filter by the record of file
	::EnterCriticalSection(&g_csGetAccess);

	RULE_ITEM *pItem = NULL;
	int nIndex = 0;
	nRet = PF_PASS;
	while(TRUE)
	{
		if(nIndex > 0)								// if the check is not 0, nIndex+1, avoid repeating
			nIndex++;

		nIndex = FindRule(g_szCurrentApp, nIndex);
		if(nIndex >= (int)g_RuleCount)
		{
			if(pItem == NULL)						// query if there is no record term
			{
				if(!QueryAccess())					// ask main module how to do
				{
					nRet = PF_DENY;
				}
				break;
			}
			else									// handle according to previous record list
			{
				if(pItem->ucAction != RULE_ACTION_PASS)
				{	
					nRet = PF_DENY;
				}
				break;
			}
		}

		ODS(L" Find a rule in GetAccessInfo ");

		pItem = &g_Rule[nIndex];					// check the rules whether they are equal to session's property

		if(pItem->ucDirection != RULE_DIRECTION_IN_OUT &&
			pItem->ucDirection != pSession->ucDirection)
			continue;

		if(pItem->ucServiceType != RULE_SERVICE_TYPE_ALL &&
			pItem->ucServiceType != pSession->nProtocol)
			continue;

		if(pItem->usServicePort != RULE_SERVICE_PORT_ALL &&
			pItem->usServicePort != pSession->usRemotePort)
			continue;

		if(pItem->ucAction != RULE_ACTION_PASS)		// if run to here, it shows that these rules are equal to the session's property
		{	
			nRet = PF_DENY;
		}	
		break;
	}
	::LeaveCriticalSection(&g_csGetAccess);

	if(nRet == PF_PASS)
		pSession->ucAction = RULE_ACTION_PASS;
	else
		pSession->ucAction =  RULE_ACTION_DENY;
	return nRet;
}

int CAcl::FindRule(TCHAR *szAppName, int nStart)
{
	int nIndex;
	for(nIndex = nStart; nIndex < (int)g_RuleCount; nIndex++)
	{
		if(_wcsicmp(szAppName, g_Rule[nIndex].szApplication) == 0)
			break;
	}
	return nIndex;
}


BOOL CAcl::QueryAccess()
{
	ODS(L" QueryAccess ... ");

	for(int i=0; i<MAX_QUERY_SESSION; i++)
	{
		if(!g_QuerySession[i].bUsed)			// find a QuerySession that's never used
		{
			g_QuerySession[i].bUsed = TRUE;
			wcscpy_s(g_QuerySession[i].szPathName, MAX_PATH, g_szCurrentApp);

			if(!::PostMessage(g_hPhoenixWnd, PM_QUERY_ACL_NOTIFY, i, 0))
			{
				g_QuerySession[i].bUsed = FALSE;
				return TRUE;
			}
			ODS(L"询问发送成功，等待... ");

			int n=0;
			while(g_QuerySession[i].bUsed)
			{
				if(n++ > 3000)					// wait 5m, after that deny it;
					return FALSE;
				::Sleep(100);
			}

			if(g_QuerySession[i].nReturnValue == 0)
				return FALSE;
			return TRUE;
		}
	}
	return FALSE;
}

int CAcl::CreateSession(SOCKET s, int nProtocol)
{
	for(int i=0; i<m_nSessionCount; i++)
	{
		if(m_pSession[i].s == s)
			return i;
	}

	//-------------------------------------------
	// confirm there is enough space
	if(m_nSessionCount >= m_nSessionMaxCount)			// has reached the max session
	{
		SESSION *pTmp = new SESSION[m_nSessionMaxCount];
		memcpy(pTmp, m_pSession, m_nSessionMaxCount);

		delete[] m_pSession;
		m_pSession = new SESSION[m_nSessionMaxCount*2];

		memcpy(m_pSession, pTmp,  m_nSessionMaxCount);
		delete[] pTmp;
		m_nSessionMaxCount = m_nSessionMaxCount*2;
	}

	InitializeSession(&m_pSession[m_nSessionCount]);

	m_pSession[m_nSessionCount].s = s;
	m_pSession[m_nSessionCount].nProtocol = nProtocol;
	wcscpy_s(m_pSession[m_nSessionCount].szPathName, MAX_PATH, g_szCurrentApp);
	m_nSessionCount++;

	ODS1(L" CreateSession m_nSessionCount = %d \n", m_nSessionCount);

	return m_nSessionCount - 1;
}

void CAcl::InitializeSession(SESSION *pSession)
{
	memset(pSession, 0, sizeof(SESSION));
	pSession->ucDirection = RULE_DIRECTION_NOT_SET;
	pSession->ucAction = RULE_ACTION_NOT_SET;
}

void CAcl::DeleteSession(SOCKET s)
{
	for(int i=0; i<m_nSessionCount; i++)
	{
		if(m_pSession[i].s == s)
		{	// infrom the app layer that a session will be destroy
			NotifySession(&m_pSession[i], CODE_DELETE_SESSION);
			memcpy(&m_pSession[i], &m_pSession[i+1], m_nSessionCount - i - 1);
			m_nSessionCount --;
			break;
		}
	}
}


void CAcl::SetSession(SESSION *pSession, USHORT usRemotePort, ULONG ulRemoteIP, UCHAR ucDirection)
{
	pSession->ucDirection = ucDirection;
	if((pSession->usRemotePort != usRemotePort) || (pSession->ulRemoteIP != ulRemoteIP))
	{
		if(pSession->nProtocol == RULE_SERVICE_TYPE_TCP)		// set service type in accordance with the protocol
		{
			if(usRemotePort == RULE_SERVICE_PORT_FTP)
				pSession->nProtocol = RULE_SERVICE_TYPE_FTP;
			else if(usRemotePort == RULE_SERVICE_PORT_TELNET)
				pSession->nProtocol = RULE_SERVICE_TYPE_TELNET;
			else if(usRemotePort == RULE_SERVICE_PORT_POP3)
				pSession->nProtocol = RULE_SERVICE_TYPE_POP3;
			else if(usRemotePort == RULE_SERVICE_PORT_SMTP)
				pSession->nProtocol = RULE_SERVICE_TYPE_SMTP;
			else if(usRemotePort  == RULE_SERVICE_PORT_NNTP)
				pSession->nProtocol = RULE_SERVICE_TYPE_NNTP;
			else if(usRemotePort  == RULE_SERVICE_PORT_HTTP)
				pSession->nProtocol = RULE_SERVICE_TYPE_HTTP;
		}

		pSession->usRemotePort = usRemotePort;
		pSession->ulRemoteIP = ulRemoteIP;

		NotifySession(pSession, CODE_CHANGE_SESSION);
	}
}

int CAcl::FindSession(SOCKET s)
{
	int i;
	for(i=0; i<m_nSessionCount; i++)
	{
		if(m_pSession[i].s == s)
		{
			break;
		}
	}
	return i;
}

void CAcl::NotifySession(SESSION *pSession, int nCode)
{	
	ODS(L" NotifySession... ");
	int i;
	if(g_hPhoenixWnd != NULL)
	{
		// find a unused g_SessionBuffer in the g_SessionBuffer
		for(i=0; i<MAX_SESSION_BUFFER; i++)
		{
			if(g_SessionBuffer[i].s == 0)
			{
				g_SessionBuffer[i] = *pSession;
				break;
			}
		}
		// post the session to the main module
		if(i<MAX_SESSION_BUFFER &&	
			!::PostMessage(g_hPhoenixWnd, PM_SESSION_NOTIFY, i, nCode))
		{
			g_SessionBuffer[i].s = 0;	// failed, recovery the value(set to 0);
		}
	}
}

int __stdcall PLSPIoControl(LSP_IO_CONTROL *pIoControl, int nType)
{
	switch(nType)
	{
	case IO_CONTROL_SET_RULE_FILE:			
		{
			if(pIoControl->pRuleFile->header.ulLspRuleCount <= MAX_RULE_COUNT)
			{
				g_RuleCount = pIoControl->pRuleFile->header.ulLspRuleCount;
				memcpy(g_Rule, pIoControl->pRuleFile->LspRules, g_RuleCount * sizeof(RULE_ITEM));
			}
		}
		break; 
	case IO_CONTROL_SET_WORK_MODE:			
		{
			g_ucWorkMode = pIoControl->ucWorkMode;
		}
		break;
	case IO_CONTROL_GET_WORK_MODE:			
		{
			return g_ucWorkMode;
		}
		break;
	case IO_CONTROL_SET_PHOENIX_INSTANCE:
		{
			g_hPhoenixWnd = pIoControl->hPhoenixWnd;
			wcscpy_s(g_szPhoenixFW, MAX_PATH, pIoControl->szPath);
		}
		break;
	case IO_CONTROL_GET_SESSION:			
		{
			*pIoControl->pSession = g_SessionBuffer[pIoControl->nSessionIndex];
			// not the session member now
			g_SessionBuffer[pIoControl->nSessionIndex].s = 0;
		}
		break; 
	case IO_CONTROL_SET_QUERY_SESSION:		
		{
			g_QuerySession[pIoControl->nSessionIndex].nReturnValue = pIoControl->ucWorkMode;
			// not the session member now
			g_QuerySession[pIoControl->nSessionIndex].bUsed = FALSE;
		}
		break;
	case IO_CONTROL_GET_QUERY_SESSION:		
		{	
			wcscpy_s(pIoControl->szPath, MAX_PATH, g_QuerySession[pIoControl->nSessionIndex].szPathName);
		}
		break;
	}
	return 0;
}

int CAcl::GetAccessFromWorkMode(SESSION *pSession)
{
	if(g_ucWorkMode == PF_PASS_ALL)
		return PF_PASS;

	if(g_ucWorkMode == PF_DENY_ALL)
		return PF_DENY;

	if(g_ucWorkMode == PF_QUERY_ALL)
		return PF_FILTER;

	return PF_UNKNOWN;
}