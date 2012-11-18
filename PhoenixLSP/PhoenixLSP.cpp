// PhoenixLSP.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <WinSock2.h>
#include <WS2spi.h>
#include <Windows.h>
#include <WinBase.h>
#include <SpOrder.h>
#include <stdio.h>
#include <tchar.h>

#include "PhoenixLSP.h"

#include "../common/Debug.h"
#include "../common/PMacRes.h"
#include "Acl.h"

#pragma comment(lib, "WS2_32.lib")

CAcl			g_Acl;						// access list, check privilege
WSPUPCALLTABLE	g_pUpCallTable;				// up layer func list, it is called if LSP create its pseudo handle
WSPPROC_TABLE	g_NextProcTable;			// sublayer
TCHAR			g_szCurrentApp[MAX_PATH];	// who call this DLL currently


BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			::GetModuleFileName(NULL, g_szCurrentApp, MAX_PATH);
		}
		break;
	default:
		break;
	}
	return TRUE;
}

int WSPAPI WSPStartup( 
					  IN WORD wVersionRequested, 
					  __in OUT LPWSPDATA lpWSPData, 
					  __in IN LPWSAPROTOCOL_INFOW lpProtocolInfo, 
					  __in IN WSPUPCALLTABLE UpcallTable, 
					  __inout OUT LPWSPPROC_TABLE lpProcTable 
					  )
{
	ODS1(L"  WSPStartup...  %s \n", g_szCurrentApp);
	if (lpProtocolInfo->ProtocolChain.ChainLen <= 1)
	{
		return WSAEPROVIDERFAILEDINIT;
	}

	g_pUpCallTable = UpcallTable;						// store the point that call func table from down to up
	WSAPROTOCOL_INFOW NextProtocolInfo;					// enumeration protocol of kernel layer
	int nTotalProtos;
	LPWSAPROTOCOL_INFOW pProtoInfo = GetProvider(&nTotalProtos);
	DWORD dwBaseEntryId = lpProtocolInfo->ProtocolChain.ChainEntries[1];
	int i = 0;
	for (i = 0; i < nTotalProtos ; i++)
	{
		if (pProtoInfo[i].dwCatalogEntryId == dwBaseEntryId)
		{
			memcpy_s(&NextProtocolInfo, sizeof(WSAPROTOCOL_INFOW), &pProtoInfo[i], sizeof(WSAPROTOCOL_INFOW));
			break;
		}
	}
	if( i >= nTotalProtos)
	{
		ODS(L" WSPStartup:	Can not find underlying protocol \n");
		return WSAEPROVIDERFAILEDINIT;
	}

	//--------------------------------------------------
	// load down protocol DLL, get kernel provider's path, load app of kernel layer
	int nError;
	TCHAR szBaseProviderDll[MAX_PATH];
	int nLen = MAX_PATH;
	if (SOCKET_ERROR == ::WSCGetProviderPath(&NextProtocolInfo.ProviderId,szBaseProviderDll,NULL,&nError))
	{
		ODS1(L" WSPStartup: WSCGetProviderPath() failed %d \n", nError);
		return WSAEPROVIDERFAILEDINIT;
	}
	if (!::ExpandEnvironmentStrings(szBaseProviderDll, szBaseProviderDll,MAX_PATH))
	{
		ODS1(L" WSPStartup:  ExpandEnvironmentStrings() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}
	HMODULE hModule = ::LoadLibrary(szBaseProviderDll);
	if (NULL != hModule)
	{
		ODS1(L" WSPStartup:  LoadLibrary() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}

	//--------------------------------------------------
	// get WSPStartup of sublayer app
	LPWSPSTARTUP pfnWSPStartup = NULL;
	pfnWSPStartup = (LPWSPSTARTUP)::GetProcAddress(hModule, "WSPStartup");
	if (NULL == pfnWSPStartup)
	{
		ODS1(L" WSPStartup:  GetProcAddress() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}
	//--------------------------------------------------
	// call WSPStartup of sublayer
	LPWSAPROTOCOL_INFOW pInfo = lpProtocolInfo;
	if (BASE_PROTOCOL == NextProtocolInfo.ProtocolChain.ChainLen)
	{
		pInfo = &NextProtocolInfo;
	}
	int nRet = pfnWSPStartup(wVersionRequested, lpWSPData, pInfo, UpcallTable,lpProcTable);
	if(ERROR_SUCCESS != nRet)
	{
		ODS1(L" WSPStartup:  underlying provider's WSPStartup() failed %d \n", nRet);
		return nRet;
	}
	
	g_NextProcTable = *lpProcTable;					// sublayer function call
	lpProcTable->lpWSPSocket = WSPSocket;
	lpProcTable->lpWSPCloseSocket = WSPCloseSocket;
	lpProcTable->lpWSPBind = WSPBind;
	lpProcTable->lpWSPAccept = WSPAccept;
	lpProcTable->lpWSPConnect = WSPConnect;
	lpProcTable->lpWSPSendTo = WSPSendTo;	
	lpProcTable->lpWSPRecvFrom = WSPRecvFrom; 

	FreeProvider(pProtoInfo);
	return nRet;
}


SOCKET WSPAPI WSPSocket(
						int			af,                               
						int			type,                             
						int			protocol,                         
						LPWSAPROTOCOL_INFOW lpProtocolInfo,   
						GROUP		g,                              
						DWORD		dwFlags,                        
						LPINT		lpErrno
						)
{
	SOCKET	s = g_NextProcTable.lpWSPSocket(af, type, protocol, lpProtocolInfo, g, dwFlags, lpErrno);
	if(s == INVALID_SOCKET)
		return s;

	if (af == FROM_PROTOCOL_INFO)
		af = lpProtocolInfo->iAddressFamily;
	if (type == FROM_PROTOCOL_INFO)
		type = lpProtocolInfo->iSocketType;
	if (protocol == FROM_PROTOCOL_INFO)
		protocol = lpProtocolInfo->iProtocol;

	g_Acl.CheckSocket(s, af, type, protocol);

	return s;
}


int WSPAPI WSPCloseSocket(
						  SOCKET		s,
						  LPINT		lpErrno
						  )
{
	g_Acl.CheckCloseSocket(s);							// delete session and close socket
	return g_NextProcTable.lpWSPCloseSocket(s, lpErrno);
}

int WSPAPI WSPBind(SOCKET s, const struct sockaddr* name, int namelen, LPINT lpErrno)
{
	g_Acl.CheckBind(s, (const PSOCKADDR)name);							// set new session
	return g_NextProcTable.lpWSPBind(s, name, namelen, lpErrno);
}

int WSPAPI WSPConnect(
					  SOCKET			s,
					  const struct	sockaddr FAR * name,
					  int				namelen,
					  LPWSABUF		lpCallerData,
					  LPWSABUF		lpCalleeData,
					  LPQOS			lpSQOS,
					  LPQOS			lpGQOS,
					  LPINT			lpErrno
					  )
{
	ODS1(L" WSPConnect...	%s", g_szCurrentApp);

	if(g_Acl.CheckConnect(s, name) != PF_PASS)			// check whether it connect to remote host
	{
		*lpErrno = WSAECONNREFUSED;
		ODS1(L" WSPConnect deny a query %s \n", g_szCurrentApp);
		return SOCKET_ERROR;
	} 

	return g_NextProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
}

SOCKET WSPAPI WSPAccept(
						SOCKET			s,
						struct sockaddr FAR *addr,
						LPINT			addrlen,
						LPCONDITIONPROC	lpfnCondition,
						DWORD			dwCallbackData,
						LPINT			lpErrno
						)
{
	ODS1(L"  PhoenixLSP:  WSPAccept  %s \n", g_szCurrentApp);
	// call sublayter to accept new connection
	SOCKET	sNew	= g_NextProcTable.lpWSPAccept(s, addr, addrlen, lpfnCondition, dwCallbackData, lpErrno);

	// check whether the other side allow to connect
	if (sNew != INVALID_SOCKET && g_Acl.CheckAccept(s, sNew, addr) != PF_PASS)
	{
		int iError;
		g_NextProcTable.lpWSPCloseSocket(sNew, &iError);
		*lpErrno = WSAECONNREFUSED;
		return SOCKET_ERROR;
	}

	return sNew;
}

int WSPAPI WSPSendTo(
					SOCKET			s,
					LPWSABUF		lpBuffers,
					DWORD			dwBufferCount,
					LPDWORD			lpNumberOfBytesSent,
					DWORD			dwFlags,
					const struct sockaddr FAR * lpTo,
					int				iTolen,
					LPWSAOVERLAPPED	lpOverlapped,
					LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
					LPWSATHREADID	lpThreadId,
					LPINT			lpErrno
					)
{
	ODS1(L" query send to... %s \n", g_szCurrentApp);

	// check whether allow to send data
	if (g_Acl.CheckSendTo(s, lpTo) != PF_PASS)
	{
		int		iError;
		g_NextProcTable.lpWSPShutdown(s, SD_BOTH, &iError);
		*lpErrno = WSAECONNABORTED;

		ODS1(L" WSPSendTo deny query %s \n", g_szCurrentApp);

		return SOCKET_ERROR;
	}

	// send data
	return g_NextProcTable.lpWSPSendTo(s, lpBuffers, dwBufferCount, 
		lpNumberOfBytesSent, dwFlags, lpTo, iTolen, 
		lpOverlapped, lpCompletionRoutine, lpThreadId, lpErrno);
}

int WSPAPI WSPRecvFrom (
						SOCKET			s,
						LPWSABUF		lpBuffers,
						DWORD			dwBufferCount,
						LPDWORD			lpNumberOfBytesRecvd,
						LPDWORD			lpFlags,
						struct sockaddr FAR * lpFrom,
						LPINT			lpFromlen,
						LPWSAOVERLAPPED lpOverlapped,
						LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
						LPWSATHREADID	lpThreadId,
						LPINT			lpErrno
						)
{
	ODS1(L"  PhoenixLSP:  WSPRecvFrom %s \n", g_szCurrentApp);

	if(g_Acl.CheckRecvFrom(s, lpFrom) != PF_PASS)				// check whether allow recv
	{
		int		iError;
		g_NextProcTable.lpWSPShutdown(s, SD_BOTH, &iError);
		*lpErrno = WSAECONNABORTED;

		ODS1(L" WSPRecvFrom deny query %s \n", g_szCurrentApp);
		return SOCKET_ERROR;
	}

	// receive data
	return g_NextProcTable.lpWSPRecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, 
		lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine, lpThreadId, lpErrno);
}



LPWSAPROTOCOL_INFOW GetProvider(LPINT lpnTotalProtocols)
{
	DWORD dwSize = 0;
	int nError;
	LPWSAPROTOCOL_INFOW pProtoInfo = NULL;

	// get needful length
	if(::WSCEnumProtocols(NULL, pProtoInfo, &dwSize, &nError) == SOCKET_ERROR)
	{
		if(nError != WSAENOBUFS)
			return NULL;
	}

	pProtoInfo = (LPWSAPROTOCOL_INFOW)::GlobalAlloc(GPTR, dwSize);
	*lpnTotalProtocols = ::WSCEnumProtocols(NULL, pProtoInfo, &dwSize, &nError);
	return pProtoInfo;
}

void FreeProvider(LPWSAPROTOCOL_INFOW pProtoInfo)
{
	::GlobalFree(pProtoInfo);
}
