
/**************************************************************************
	created:	2012/11/15	14:00	
	filename: 	PeerList.H
	file path:	e:\Documents\Visual Studio 2008\Projects\WinSock\P2PServer
	author:		Dailiming, en_name: Dicky

	purpose:	p2p
**************************************************************************/
#pragma once

#define MAX_USERNAME 15
#define MAX_TRY_NUMBER 5
#define MAX_ADDR_NUMBER 5
#define MAX_PACKET_SIZE 1024
#define SERVER_PORT		8888


typedef struct _ADDR_INFO					// one client info
{
	DWORD		dwIp;
	USHORT		nPort;
}ADDR_INFO, * PADDR_INFO;


typedef struct _PEER_INFO					// one peer info
{
	char		szUserName[MAX_USERNAME];	// user name
	ADDR_INFO	addr[MAX_ADDR_NUMBER];		// private client and public client compose the array
	UCHAR		AddrNum;
	ADDR_INFO	p2pAddr;					// used to p2p communication
	DWORD		dwLastActiveTime;			// record the user active time(server use)
}PEER_INFO, *PPEER_INFO;

typedef struct CP2PMessage
{
	int			nMessageType;		// msg type
	PEER_INFO	peer;				// peer info
};

//-------------------------------------------
// msg type def
#define USERLOGIN			101		// login in 
#define USERLOGOUT			102		// login out
#define USERLOGACK			103

#define GETUSERLIST			104		// request user list
#define USERLISTCMP			105		// list transfered completely

#define USERACTIVEQUERY		106		// server query a user whether it is active
#define USERACTIVEQUERYACK	107		// response to server query

#define P2PCONNECT			108		// build connection with user
#define P2PCONNECTACK		109		// response to connection

#define P2PMESSAGE			110		// send msg
#define P2PMESSAGEACK		111		// recv response of msg

class CPeerList
{
public:
	CPeerList(void);
	~CPeerList(void);

	BOOL AddAPeer(PEER_INFO *pPeer);			// add a peer
	PEER_INFO *GetAPeer(char * pszUserName);	// get a peer;
	void DeleteAPeer(PCHAR pszUserName);			// delete a peer;
	void DeleteAllPeers();						// delete all peer;
	PEER_INFO *m_pPeer;							// list header
	int m_nCurrentSize;							// current list size

protected:
	int m_nTatolSize;							// total list size
};
