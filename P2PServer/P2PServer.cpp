// P2PServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../common/SockInit.h"
#include "PeerList.h"
DWORD WINAPI IOThreadProc(LPVOID lpParam);

Socket_NetData theSock;
CPeerList g_PeerList;
CRITICAL_SECTION g_PeerListLock;	// synchronization client list
SOCKET g_s;

int _tmain(int argc, _TCHAR* argv[])
{
	// 
	g_s = ::WSASocket(AF_INET,SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (SOCKET_ERROR == ::bind(g_s,(LPSOCKADDR)&sin,sizeof(sockaddr)))
	{
		printf(" bind() failed %d \n", ::WSAGetLastError());
		return 0;
	}

	//-------------------------------------------
	// display client info
	char szHost[256];
	::gethostname(szHost,256);
	HOSTENT *pHost = ::gethostbyname(szHost);
	in_addr addr;
	for (int i = 0;  ; i++)
	{
		PCHAR p= pHost->h_addr_list[i];
		if (NULL == p)
		{
			break;
		}
		memcpy(&addr.S_un.S_addr, p, pHost->h_length);
		printf(" bind to local address -> %s:%ld \n", ::inet_ntoa(addr), SERVER_PORT);
	}
	printf(" P2P Server starting... \n\n");
	::InitializeCriticalSection(&g_PeerListLock);

	HANDLE hThread = ::CreateThread(NULL, 0, IOThreadProc, NULL, 0, NULL);
	// delete client that don't respond
	while (TRUE)
	{
		int nRet = ::WaitForSingleObject(hThread, 15*1000);
		if (WAIT_TIMEOUT == nRet)
		{
			CP2PMessage queryMsg;
			queryMsg.nMessageType = USERACTIVEQUERY;
			DWORD dwTick = ::GetTickCount();
			for (int i = 0; i < g_PeerList.m_nCurrentSize ; i++)
			{
				if (dwTick - g_PeerList.m_pPeer[i].dwLastActiveTime >= 2*15*1000 + 600)
				{
					printf(" delete a non-active user: %s \n", g_PeerList.m_pPeer[i].szUserName);
					::EnterCriticalSection(&g_PeerListLock);
					g_PeerList.DeleteAPeer(g_PeerList.m_pPeer[i].szUserName);
					::LeaveCriticalSection(&g_PeerListLock);
					// vector likely iterator, delete one, the iter must be decrease one
					i --;
				}
				else
				{
					// public addr at the end of addr list, use it to send data to client
					sockaddr_in peerAddr = {0};
					peerAddr.sin_family = AF_INET;
					peerAddr.sin_addr.S_un.S_addr = 
						g_PeerList.m_pPeer[i].addr[g_PeerList.m_pPeer[i].AddrNum-1].dwIp;
					peerAddr.sin_port = htons(g_PeerList.m_pPeer[i].addr[g_PeerList.m_pPeer[i].AddrNum-1].nPort);
					::sendto(g_s,(PCHAR)&queryMsg, sizeof(queryMsg), 0, (LPSOCKADDR)&peerAddr,sizeof(peerAddr));
				}
			}
		}
		else
		{
			break;
		}
	}

	printf(" P2P Server shutdown. \n");
	::DeleteCriticalSection(&g_PeerListLock);
	::CloseHandle(hThread);
	::closesocket(g_s);
	return 0;
}


DWORD WINAPI IOThreadProc(LPVOID lpParam)
{
	char buf[MAX_PACKET_SIZE];
	CP2PMessage *pMsg = (CP2PMessage*)buf;
	sockaddr_in remoteAddr;
	int nRecv, nAddrLen = sizeof(remoteAddr);
	while (TRUE)
	{
		nRecv = ::recvfrom(g_s,buf,MAX_PACKET_SIZE, 0,(LPSOCKADDR)&remoteAddr, &nAddrLen);
		if (SOCKET_ERROR == nRecv)
		{
			printf(" recvfrom() failed \n");
			continue;
		}
		if (nRecv<sizeof(CP2PMessage))
		{
			continue;
		}

		//-------------------------------------------
		// prevent user to invalid name
		pMsg->peer.szUserName[MAX_USERNAME] = '\0';
		switch (pMsg->nMessageType)
		{
		case USERLOGIN:
			{
				pMsg->peer.addr[pMsg->peer.AddrNum].dwIp = remoteAddr.sin_addr.S_un.S_addr;
				pMsg->peer.addr[pMsg->peer.AddrNum].nPort = ntohs(remoteAddr.sin_port);
				pMsg->peer.AddrNum ++;
				pMsg->peer.dwLastActiveTime = ::GetTickCount();

				//-------------------------------------------
				// store the user info to list
				::EnterCriticalSection(&g_PeerListLock);
				BOOL bOk = g_PeerList.AddAPeer(&pMsg->peer);
				::LeaveCriticalSection(&g_PeerListLock);
				if (bOk)
				{
					//-------------------------------------------
					// send the affirm data
					pMsg->nMessageType = USERLOGACK;
					::sendto(g_s,(PCHAR)pMsg,sizeof(CP2PMessage), 0,(LPSOCKADDR)&remoteAddr, sizeof(remoteAddr));
					printf(" has a user login : %s (%s:%ld) \n", 
						pMsg->peer.szUserName, ::inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port));
				}
			}
			break;
		case USERLOGOUT:
			{
				::EnterCriticalSection(&g_PeerListLock);
				g_PeerList.DeleteAPeer(pMsg->peer.szUserName);
				::LeaveCriticalSection(&g_PeerListLock);
				printf(" has a user logout : %s (%s:%ld) \n", 
					pMsg->peer.szUserName, ::inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port));
			}
			break;
		case GETUSERLIST:
			{
				printf(" sending user list information to %s (%s:%ld)... \n",
					pMsg->peer.szUserName, ::inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port));
				CP2PMessage peerMsg;
				peerMsg.nMessageType = GETUSERLIST;
				for (int i = 0; i < g_PeerList.m_nCurrentSize ; i++)
				{
					memcpy(&peerMsg.peer,&g_PeerList.m_pPeer[i], sizeof(PEER_INFO));
					::sendto(g_s,(PCHAR)&peerMsg, sizeof(CP2PMessage), 0, (PSOCKADDR)&remoteAddr, sizeof(remoteAddr));
				}
				peerMsg.nMessageType = USERLISTCMP;
				::sendto(g_s, (PCHAR)&peerMsg, sizeof(CP2PMessage), 0, (LPSOCKADDR)&remoteAddr, sizeof(remoteAddr));
			}
			break;
		case P2PCONNECT:
			{
				char *pszUser = (PCHAR)(pMsg+1);
				printf(" %s wants to connect to %s \n", pMsg->peer.szUserName, pszUser);
				::EnterCriticalSection(&g_PeerListLock);
				PEER_INFO *pInfo = g_PeerList.GetAPeer(pszUser);
				::LeaveCriticalSection(&g_PeerListLock);

				if (NULL != pInfo)
				{
					remoteAddr.sin_addr.S_un.S_addr = pInfo->addr[pInfo->AddrNum-1].dwIp;
					remoteAddr.sin_port = pInfo->addr[pInfo->AddrNum-1].nPort;
					::sendto(g_s, (PCHAR)pMsg, sizeof(CP2PMessage), 0, (LPSOCKADDR)&remoteAddr, sizeof(remoteAddr));
				}
			}
			break;
		case USERACTIVEQUERYACK:
			{
				printf(" recv active ack message from %s (%s:%ld) \n",
					pMsg->peer.szUserName, ::inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port));
				::EnterCriticalSection(&g_PeerListLock);
				PEER_INFO* pInfo = g_PeerList.GetAPeer(pMsg->peer.szUserName);
				if (NULL != pInfo)
				{
					pInfo->dwLastActiveTime = ::GetTickCount();
				}
				::LeaveCriticalSection(&g_PeerListLock);
			}
			break;
		}
	}
}

/*

//--------------------------------
// Declare and initialize variables.
PCHAR ip = "127.0.0.1";
PCHAR port = "27015";
struct addrinfo aiHints;
struct addrinfo *aiList = NULL;
int retVal;

//--------------------------------
// Setup the hints address info structure
// which is passed to the getaddrinfo() function
memset(&aiHints, 0, sizeof(aiHints));
aiHints.ai_family = AF_INET;
aiHints.ai_socktype = SOCK_STREAM;
aiHints.ai_protocol = IPPROTO_TCP;

//--------------------------------
// Call getaddrinfo(). If the call succeeds,
// the aiList variable will hold a linked list
// of addrinfo structures containing response
// information about the host
if ((retVal = getaddrinfo(ip, port, &aiHints, &aiList)) != 0) {
printf("getaddrinfo() failed.\n");
}

printf("getaddrinfo() aiList=%x.\n",aiList);
getchar();
return 0;
//-------------------------------------------
 */
