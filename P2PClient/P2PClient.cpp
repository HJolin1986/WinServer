
/**************************************************************************
	created:	2012/11/15	16:34	
	filename: 	P2PClient.CPP
	file path:	e:\Documents\Visual Studio 2008\Projects\WinSock\P2PClient
	author:		Dailm, en_name: Dicky

	purpose:	
**************************************************************************/

#include "stdafx.h"
#include "P2PClient.h"

CP2PClient::CP2PClient(void)
{
	m_bLogin = FALSE;
	m_hThread = NULL;
	m_s = INVALID_SOCKET;
	memset(&m_ol, 0, sizeof(m_ol));
	::InitializeCriticalSection(&m_PeerListLock);
}

CP2PClient::~CP2PClient(void)
{
	LogOut();
	if (NULL != m_hThread)
	{
		m_bThreadExit = TRUE;
		::WSASetEvent(m_ol.hEvent);
		::WaitForSingleObject(m_hThread,300);
		::CloseHandle(m_hThread);
	}
	if (INVALID_SOCKET != m_s)
	{
		::closesocket(m_s);
	}

	::WSACloseEvent(m_ol.hEvent);
	::DeleteCriticalSection(&m_PeerListLock);
}

BOOL CP2PClient::Init(USHORT usLocalPort)
{
	if (INVALID_SOCKET != m_s)
	{
		return FALSE;
	}
	m_s = ::WSASocket(AF_INET,SOCK_DGRAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN localAddr = {0};
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(usLocalPort);
	localAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (SOCKET_ERROR == ::bind(m_s,(LPSOCKADDR)&localAddr,sizeof(SOCKADDR_IN)))
	{
		::closesocket(m_s);
		m_s = INVALID_SOCKET;
		return FALSE;
	}
	if (0 == usLocalPort)
	{
		int nLen = sizeof(localAddr);
		::getsockname(m_s,(PSOCKADDR)&localAddr,&nLen);
		usLocalPort = ntohs(localAddr.sin_port);
	}
	//-------------------------------------------
	// get local ip and get private terminal
	char szHost[256];
	::gethostname(szHost,256);
	PHOSTENT pHost = ::gethostbyname(szHost);
	for (int i = 0; i < MAX_ADDR_NUMBER-1 ; i++)
	{
		PCHAR p = pHost->h_addr_list[i];
		if (NULL == p)
		{
			break;
		}
		memcpy(&m_localPeer.addr[i].dwIp,&p,pHost->h_length);
		m_localPeer.addr[i].nPort = usLocalPort;
		m_localPeer.AddrNum ++;
	}
	
	m_bThreadExit = FALSE;
	m_hThread = ::CreateThread(NULL,0,RecvThreadProc, this, 0, NULL);
	return TRUE;
}

BOOL CP2PClient::Login(char *pszUserName, char *pszServerIp)
{
	if (m_bLogin || strlen(pszUserName)>MAX_USERNAME-1)
	{
		return FALSE;
	}
	m_dwServerIp = ::inet_addr(pszServerIp);
	strncpy_s(m_localPeer.szUserName,pszUserName,strlen(pszUserName));

	//-------------------------------------------
	// init server socket
	m_siServer.sin_family = AF_INET;
	m_siServer.sin_addr.S_un.S_addr = m_dwServerIp;
	m_siServer.sin_port=htons(SERVER_PORT);

	CP2PMessage loginMsg;
	loginMsg.nMessageType = USERLOGIN;
	memcpy(&loginMsg.peer, &m_localPeer, sizeof(PEER_INFO));
	for (int i=0; i<MAX_TRY_NUMBER; i++)
	{
		::sendto(m_s,(PCHAR)&loginMsg, sizeof(loginMsg), 0, (PSOCKADDR)&m_siServer, sizeof(m_siServer));
		for (int j = 0; j < 10 ; j++)
		{
			if (m_bLogin)
			{
				return TRUE;
			}
			::Sleep(300);
		}
	}
	return FALSE;
}

void CP2PClient::LogOut()
{
	if(m_bLogin)
	{
		CP2PMessage logoutMsg;
		logoutMsg.nMessageType = USERLOGOUT;
		memcpy(&logoutMsg.peer,&m_localPeer,sizeof(m_localPeer));
		::sendto(m_s,(PCHAR)&logoutMsg,sizeof(logoutMsg), 0, (PSOCKADDR)&m_siServer, sizeof(m_siServer));
		m_bLogin = FALSE;
	}
}

BOOL CP2PClient::SendText(char *pszUserName, char *pszText, int nTextLen)
{
	if (!m_bLogin || 
		strlen(pszUserName)>MAX_USERNAME-1 ||
		nTextLen>MAX_PACKET_SIZE - sizeof(CP2PMessage))
	{
		return FALSE;
	}
	char sendBuf[MAX_PACKET_SIZE];
	CP2PMessage *pMsg = (CP2PMessage*)sendBuf;
	pMsg->nMessageType = P2PMESSAGE;
	memcpy(&pMsg->peer,&m_localPeer,sizeof(m_localPeer));
	memcpy((pMsg+1),pszText,nTextLen);

	m_bMessageACK = FALSE;
	for (int i = 0; i < MAX_TRY_NUMBER ; i++)
	{
		PEER_INFO *pInfo = m_PeerList.GetAPeer(pszUserName);
		if (NULL == pInfo)
		{
			return FALSE;
		}
		//-------------------------------------------
		// send data, use pInfo->p2pAddr
		if (0 != pInfo->p2pAddr.dwIp)
		{
			SOCKADDR_IN peerAddr = {0};
			peerAddr.sin_family = AF_INET;
			peerAddr.sin_addr.S_un.S_addr = pInfo->p2pAddr.dwIp;
			peerAddr.sin_port = htons(pInfo->p2pAddr.nPort);
			::sendto(m_s,
					sendBuf, nTextLen + sizeof(CP2PMessage),
					0,
					(PSOCKADDR)&peerAddr,sizeof(peerAddr));
			for (int j = 0; j < 10 ; j++)
			{
				if (m_bMessageACK)
				{
					return TRUE;
				}
				::Sleep(300);
			}
		}
		//-------------------------------------------
		// request target punch
		pInfo->p2pAddr.dwIp = 0;
		char tmpBuf[sizeof(CP2PMessage)+MAX_USERNAME];
		CP2PMessage* p=(CP2PMessage*)tmpBuf;
		p->nMessageType = P2PCONNECT;
		memcpy(&p->peer, &m_localPeer, sizeof(m_localPeer));
		memcpy((PCHAR)(p+1), pszUserName, strlen(pszUserName)+1);

		SOCKADDR_IN peerAddr = {0};
		peerAddr.sin_family = AF_INET;
		for (int j = 0; j < pInfo->AddrNum ; j++)
		{
			peerAddr.sin_addr.S_un.S_addr = pInfo->addr[i].dwIp;
			peerAddr.sin_port = htons(pInfo->addr[i].nPort);
			::sendto(m_s, tmpBuf, sizeof(CP2PMessage), 0, (PSOCKADDR)&peerAddr, sizeof(peerAddr));
		}

		//-------------------------------------------
		// server transmit data to request others punch to me
		sockaddr_in serverAddr = { 0 };
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.S_un.S_addr = m_dwServerIp; 
		serverAddr.sin_port = htons(SERVER_PORT);
		::sendto(m_s, tmpBuf, sizeof(CP2PMessage) + MAX_USERNAME, 0, 
			(PSOCKADDR)&serverAddr, sizeof(serverAddr));
		for (int j = 0; j < 10 ; j++)
		{
			if (0 != pInfo->p2pAddr.dwIp)
			{
				break;
			}
			::Sleep(300);
		}
	}
	return TRUE;
}

BOOL CP2PClient::GetUserList()
{
	CP2PMessage msgList;
	msgList.nMessageType = GETUSERLIST;	
	memcpy(&msgList.peer, &m_localPeer, sizeof(m_localPeer));

	::EnterCriticalSection(&m_PeerListLock);
	m_PeerList.DeleteAllPeers();				// delete all node
	::LeaveCriticalSection(&m_PeerListLock);	

	//-------------------------------------------
	// send GETUSERLIST request and wait it completely
	m_bUserListCmp = FALSE;	
	int nUserCount = 0;
	for(int i=0; i<MAX_TRY_NUMBER; i++)
	{
		::sendto(m_s, (PCHAR)&msgList, 
			sizeof(msgList), 0, (PSOCKADDR)&m_siServer, sizeof(m_siServer));
		do
		{	
			nUserCount = m_PeerList.m_nCurrentSize;
			for(int j=0; j<10; j++)
			{
				if(m_bUserListCmp)
					return TRUE;
				::Sleep(300);
			}
		}while(m_PeerList.m_nCurrentSize > nUserCount);
	}
	return FALSE;
}

DWORD WINAPI CP2PClient::RecvThreadProc(LPVOID lpParam)
{
	CP2PClient *pThis = (CP2PClient *)lpParam;	
	char buff[MAX_PACKET_SIZE];
	sockaddr_in remoteAddr;
	int nAddrLen = sizeof(remoteAddr);
	WSABUF wsaBuf;
	wsaBuf.buf = buff;
	wsaBuf.len = MAX_PACKET_SIZE;

	//-------------------------------------------
	// handle received msg
	while(TRUE)
	{
		DWORD dwRecv, dwFlags = 0;
		int nRet = ::WSARecvFrom(pThis->m_s, &wsaBuf, 
			1, &dwRecv, &dwFlags, (PSOCKADDR)&remoteAddr, &nAddrLen, &pThis->m_ol, NULL);
		if(nRet == SOCKET_ERROR && ::WSAGetLastError() == WSA_IO_PENDING)
		{
			::WSAGetOverlappedResult(pThis->m_s, &pThis->m_ol, &dwRecv, TRUE, &dwFlags);
		}
		
		if(pThis->m_bThreadExit)	// check if it will exit
			break;
		
		pThis->HandleIO(buff, dwRecv, (sockaddr *)&remoteAddr, nAddrLen); // call handleIO to handle the request
	}
	return 0;
}

void CP2PClient::HandleIO(char *pBuf, int nBufLen, sockaddr *addr, int nAddrLen)
{
	CP2PMessage *pMsg = (CP2PMessage*)pBuf;
	if(nBufLen < sizeof(CP2PMessage))
	{
		return;
	}
	switch (pMsg->nMessageType)
	{
	case USERLOGACK:
		{
			memcpy(&m_localPeer, &pMsg->peer, sizeof(PEER_INFO));
			m_bLogin = TRUE;
		}
		break;
	case P2PMESSAGE:
		{
			int nDataLen = nBufLen - sizeof(CP2PMessage);
			if (nDataLen > 0)
			{
				CP2PMessage ackMsg;
				ackMsg.nMessageType = P2PMESSAGEACK;
				memcpy(&ackMsg.peer, &m_localPeer, sizeof(PEER_INFO));
				::sendto(m_s, (PCHAR)&ackMsg, sizeof(ackMsg), 0, addr, nAddrLen);
				OnRecv(pMsg->peer.szUserName,(PCHAR)(pMsg+1), nDataLen);
			}
		}
		break;
	case P2PMESSAGEACK:
		{
			m_bMessageACK = TRUE;
		}
		break;
	case P2PCONNECT:		// a node quest to build connection (punch)
		{
			CP2PMessage ackMsg;
			ackMsg.nMessageType = P2PCONNECTACK;
			memcpy(&ackMsg.peer, &m_localPeer, sizeof(PEER_INFO));

			if(((PSOCKADDR_IN)addr)->sin_addr.S_un.S_addr != m_dwServerIp)	// 节点发来的消息
			{
				::EnterCriticalSection(&m_PeerListLock);
				PEER_INFO *pInfo = m_PeerList.GetAPeer(pMsg->peer.szUserName);
				if(pInfo != NULL)
				{
					if(pInfo->p2pAddr.dwIp == 0)
					{
						pInfo->p2pAddr.dwIp = ((PSOCKADDR_IN)addr)->sin_addr.S_un.S_addr;
						pInfo->p2pAddr.nPort = ntohs(((PSOCKADDR_IN)addr)->sin_port);

						printf(" Set P2P address for %s -> %s:%ld \n", pInfo->szUserName, 
							::inet_ntoa(((PSOCKADDR_IN)addr)->sin_addr), ntohs(((PSOCKADDR_IN)addr)->sin_port));
					}
				}
				::LeaveCriticalSection(&m_PeerListLock);

				::sendto(m_s, (PCHAR)&ackMsg, sizeof(ackMsg), 0, addr, nAddrLen);
			}
			else	// server transmit msg
			{
				// send punch msg to all node's client
				sockaddr_in peerAddr = { 0 };
				peerAddr.sin_family = AF_INET;
				for(int i=0; i<pMsg->peer.AddrNum; i++)
				{
					peerAddr.sin_addr.S_un.S_addr = pMsg->peer.addr[i].dwIp;
					peerAddr.sin_port = htons(pMsg->peer.addr[i].nPort);
					::sendto(m_s, (PCHAR)&ackMsg, sizeof(ackMsg), 0, (PSOCKADDR)&peerAddr, sizeof(peerAddr));
				}
			}
		}
		break;
	case P2PCONNECTACK:
		{
			::EnterCriticalSection(&m_PeerListLock);
			PEER_INFO *pInfo = m_PeerList.GetAPeer(pMsg->peer.szUserName);
			if(pInfo != NULL)
			{		
				if(pInfo->p2pAddr.dwIp == 0)
				{
					pInfo->p2pAddr.dwIp = ((PSOCKADDR_IN)addr)->sin_addr.S_un.S_addr;
					pInfo->p2pAddr.nPort = ntohs(((PSOCKADDR_IN)addr)->sin_port);

					printf(" Set P2P address for %s -> %s:%ld \n", pInfo->szUserName, 
						::inet_ntoa(((PSOCKADDR_IN)addr)->sin_addr), ntohs(((PSOCKADDR_IN)addr)->sin_port));
				}

			}
			::LeaveCriticalSection(&m_PeerListLock);	
		}
		break;
	case USERACTIVEQUERY:	// server ask if i am active
		{
			CP2PMessage ackMsg;
			ackMsg.nMessageType = USERACTIVEQUERYACK;
			memcpy(&ackMsg.peer, &m_localPeer, sizeof(PEER_INFO));
			::sendto(m_s, (PCHAR)&ackMsg, sizeof(ackMsg), 0, addr, nAddrLen);
		}
		break;
	case GETUSERLIST:
		{
			pMsg->peer.p2pAddr.dwIp = 0;
			::EnterCriticalSection(&m_PeerListLock);
			m_PeerList.AddAPeer(&pMsg->peer);
			::LeaveCriticalSection(&m_PeerListLock);
		}
		break;
	case USERLISTCMP:
		{
			m_bUserListCmp = TRUE;
		}
		break;
	}
}