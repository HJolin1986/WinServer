#pragma once

/**************************************************************************
	created:	2012/11/15	16:23	
	filename: 	P2PClient.H
	file path:	e:\Documents\Visual Studio 2008\Projects\WinSock\P2PClient
	author:		Dailiming, en_name: Dicky

	purpose:	
**************************************************************************/
#include <windows.h>
#include "../common/SockInit.h"
#include "../common/PeerList.h"

#define MAX_USERNAME 15
#define MAX_TRY_NUMBER 5
#define MAX_ADDR_NUMBER 5
#define MAX_PACKET_SIZE 1024
#define SERVER_PORT		8888

class CP2PClient
{
public:
	CP2PClient(void);
	~CP2PClient(void);
	BOOL Init(USHORT usLocalPort = 0);		// init object member
	void LogOut();
	BOOL Login(char *pszUserName, char *pszServerIp);
	BOOL GetUserList();
	BOOL SendText(PCHAR pszUserName, char *pszText, int nTextLen);
	virtual void OnRecv(char *pszUserName, PCHAR pszData, int nDataLen){}
	CPeerList m_PeerList;		// user list
protected:
	void HandleIO(char *pBUf, int nBufLen, PSOCKADDR addr, int nAddrLen);
	static DWORD WINAPI RecvThreadProc(LPVOID lpParam);
	CRITICAL_SECTION m_PeerListLock;
	Socket_NetData snSock;
	SOCKET m_s;
	HANDLE m_hThread;
	WSAOVERLAPPED m_ol;
	PEER_INFO m_localPeer;		// local user info
	DWORD m_dwServerIp;		
	SOCKADDR_IN m_siServer;
	BOOL m_bThreadExit;			// indicate the thread whether it's exit
	BOOL m_bLogin;		
	BOOL m_bUserListCmp;		// symbol that mark the user list transfered completely
	BOOL m_bMessageACK;			// whether recv msg ACK
};
