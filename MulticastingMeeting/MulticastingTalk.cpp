
/**************************************************************************
	created:	2012/11/14	22:49
	filename: 	GroupTalk.CPP
	file path:	F:\mycareer\9-终级-源码阅读\Windows网络通信\第5章\GroupTalk
	author:		Dailiming, en_name: Dicky

	purpose:	multicasting	
**************************************************************************/

#include "StdAfx.h"
#include "MulticastingTalk.h"

#include <Windows.h>
#include <WS2tcpip.h>			//IP_MULTICAST_TTL, IP_MULTICAST_IF are defined in the file

#pragma comment(lib,"WS2_32")
//--------------------------------------------------
// work thread function
DWORD WINAPI _GroupTalkEntry(LPVOID lpParam)
{
	MulticastingTalk *pTalk = (MulticastingTalk*)lpParam;
         
	pTalk->m_sSend = ::socket(AF_INET, SOCK_DGRAM, 0);
	pTalk->m_sRead = ::WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	BOOL bReuse = TRUE;
	::setsockopt(pTalk->m_sRead, SOL_SOCKET, SO_REUSEADDR, (PCHAR)&bReuse, sizeof(BOOL));

	// set multicast TTL
	::setsockopt(pTalk->m_sSend, 
		IPPROTO_IP, IP_MULTICAST_TTL, (PCHAR)&pTalk->m_nTTL, sizeof(pTalk->m_nTTL));

	// set net interface that's send data
	setsockopt(pTalk->m_sSend, 
		IPPROTO_IP, IP_MULTICAST_IF, (PCHAR)&pTalk->m_dwLocalAddr, sizeof(pTalk->m_dwLocalAddr));

	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = ::ntohs(GROUP_PORT);
	si.sin_addr.S_un.S_addr = pTalk->m_dwLocalAddr;
	int nRet = ::bind(pTalk->m_sRead, (PSOCKADDR)&si, sizeof(si));
	if(nRet == SOCKET_ERROR)
	{		
		::closesocket(pTalk->m_sSend);
		::closesocket(pTalk->m_sRead);
		::SendMessage(pTalk->m_hNotifyWnd, WM_GROUPTALK, -1, (long)"bind failed! \n");
		return -1;
	}

	// join in multicasting
	if(!pTalk->JoinGroup())
	{
		::closesocket(pTalk->m_sSend);
		::closesocket(pTalk->m_sRead);
		::SendMessage(pTalk->m_hNotifyWnd, WM_GROUPTALK, -1, (long)"JoinGroup failed! \n");
		return -1;
	}

	// recv data cyclely
	WSAOVERLAPPED ol = { 0 };
	ol.hEvent = pTalk->m_hEvent;
	WSABUF buf;
	buf.buf = new char[BUFFER_SIZE];
	buf.len = BUFFER_SIZE;	
	while(TRUE)
	{
		// post I/O
		DWORD dwRecv;
		DWORD dwFlags = 0;
		sockaddr_in saFrom;
		int nFromLen = sizeof(saFrom);
		int ret = ::WSARecvFrom(pTalk->m_sRead, 
			&buf, 1, &dwRecv, &dwFlags, (PSOCKADDR)&saFrom, &nFromLen, &ol, NULL);
		if(ret == SOCKET_ERROR)
		{
			if(::WSAGetLastError() != WSA_IO_PENDING)
			{
				::SendMessage(pTalk->m_hNotifyWnd, WM_GROUPTALK, -1, (long)"PostRecv failed! \n");
				pTalk->LeaveGroup();	
				::closesocket(pTalk->m_sSend);
				::closesocket(pTalk->m_sRead);
				break;
			}
		}

		// wait I/O complete
		::WSAWaitForMultipleEvents(1, &pTalk->m_hEvent, TRUE, WSA_INFINITE, FALSE);
		if(pTalk->m_bQuit)		// if quit
		{
			pTalk->LeaveGroup();	
			::closesocket(pTalk->m_sSend);
			::closesocket(pTalk->m_sRead);
			break;
		}

		BOOL b = ::WSAGetOverlappedResult(pTalk->m_sRead, &ol, &dwRecv, FALSE, &dwFlags);
		if(b && dwRecv >= sizeof(GT_HDR))
		{	
			GT_HDR *pHeader = (GT_HDR*)buf.buf;	
			// source dest
			pHeader->dwAddr = saFrom.sin_addr.S_un.S_addr;
			pTalk->DispatchMsg(pHeader, dwRecv);
		}
	}

	delete buf.buf;
	return 0;
}

MulticastingTalk::MulticastingTalk(HWND hNotifyWnd,DWORD dwMultiAddr, DWORD dwLocalAddr,int nTTL)
{
	m_hNotifyWnd = hNotifyWnd;
	m_dwMultiAddr = dwMultiAddr;
	m_dwLocalAddr = dwLocalAddr;
	m_nTTL = nTTL;
	m_bQuit = FALSE;

	// get host name, use it as username
	DWORD dw = 256;
	::gethostname(m_szUser, dw);
	//create event, and thread
	m_hEvent = ::WSACreateEvent();
	m_hThread = ::CreateThread(NULL, 0, _GroupTalkEntry, this, 0, NULL);
}

MulticastingTalk::~MulticastingTalk(void)
{
	//--------------------------------------------------
	// inform all thread exiting and wait, release sources
	m_bQuit = TRUE;
	::SetEvent(m_hEvent);
	::WaitForSingleObject(m_hThread, INFINITE);
	::CloseHandle(m_hThread);
	::CloseHandle(m_hEvent);
}

BOOL MulticastingTalk::JoinGroup()
{
	ip_mreq mcast;
	mcast.imr_interface.S_un.S_addr = INADDR_ANY;
	mcast.imr_multiaddr.S_un.S_addr = m_dwMultiAddr;
	int nRet = ::setsockopt(m_sRead, 
		IPPROTO_IP, 
		IP_ADD_MEMBERSHIP, 
		(PCHAR)&mcast, 
		sizeof(mcast));

	//--------------------------------------------------
	// join in talk and send
	if (nRet != SOCKET_ERROR)
	{
		char buf[sizeof(GT_HDR)] = {0};
		GT_HDR* pHeader = (GT_HDR*) buf;
		pHeader->gt_type = MT_JION;
		::strncpy_s(pHeader->szUser, m_szUser,15);
		Send(buf,sizeof(GT_HDR),m_dwMultiAddr);
		return TRUE;
	}
	return FALSE;
}
BOOL MulticastingTalk::LeaveGroup()
{
	ip_mreq mcast;
	mcast.imr_multiaddr.S_un.S_addr = m_dwMultiAddr;
	mcast.imr_interface.S_un.S_addr = INADDR_ANY;
	int nRet = ::setsockopt(m_sRead,
		IPPROTO_IP, 
		IP_DROP_MEMBERSHIP,
		(PCHAR)&mcast,
		sizeof(mcast));
	if (nRet != SOCKET_ERROR)
	{
		char buf[sizeof(GT_HDR)] = {0};
		GT_HDR *pHeader = (GT_HDR*)buf;
		pHeader->gt_type = MT_LEAVE;
		strncpy_s(pHeader->szUser,m_szUser,15);
		Send(buf,sizeof(GT_HDR), m_dwMultiAddr);
		return TRUE;
	}
	return FALSE;
}
BOOL MulticastingTalk::Send(char *szText, int nLen, DWORD dwRomoteAddr)
{
	sockaddr_in dest;
	dest.sin_addr.S_un.S_addr = dwRomoteAddr;
	dest.sin_family = AF_INET;
	dest.sin_port = ::ntohs(GROUP_PORT);
	return ::sendto(m_sRead,szText,nLen,0,(PSOCKADDR)&dest,sizeof(dest));
}

BOOL MulticastingTalk::SendText (char *szText, int nLen, DWORD dwRemoteAddr)
{
	char buf[sizeof(GT_HDR)+1024] = {0};
	GT_HDR *pHeader = (GT_HDR*)buf;
	pHeader->gt_type = MT_MESG;
	pHeader->nDataLength = nLen < 1024? nLen:1024;
	strncpy_s(pHeader->data(),pHeader->nDataLength, szText,pHeader->nDataLength);
	strncpy_s(pHeader->szUser,m_szUser,15);
	int nSends = Send(buf,pHeader->nDataLength+sizeof(GT_HDR), dwRemoteAddr==0?m_dwMultiAddr:dwRemoteAddr);
	return nSends-sizeof(GT_HDR);
}

//--------------------------------------------------
// dispatch the received data
void MulticastingTalk::DispatchMsg(GT_HDR *pHeader, int nLen)	
{
	if (pHeader->gt_type == MT_JION)
	{
		char buf[sizeof(GT_HDR)] = {0};
		GT_HDR* pSend = (GT_HDR*)buf;
		strncpy_s(pSend->szUser,m_szUser,15);
		pSend->gt_type = MT_MINE;
		pSend->nDataLength = 0;
		Send(buf,sizeof(GT_HDR),pHeader->dwAddr);
	}
	else if (pHeader->gt_type == MT_MINE)
	{
		if(strcmp(pHeader->szUser,m_szUser) == 0)
		{
			return;
		}
		pHeader->gt_type = MT_JION;
	}
	::SendMessage(m_hNotifyWnd, WM_GROUPTALK,0,(LPARAM)pHeader);
}

