
/**************************************************************************
	created:	2012/11/14	22:25
	filename: 	SockInit.CPP
	file path:	f:\GitHub\WinSock\common
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "StdAfx.h"
#include "../common/SockInit.h"

#pragma comment(lib,"WS2_32")

inline void Socket_NetData::InitSockAddr(sockaddr_in &addr, int port, int ip)
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = ip;
}

inline void Socket_NetData::InitSockAddr(sockaddr_in &addr, int port, const char ip[16])
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
}


inline void Socket_NetData::InitSockAddr(sockaddr_in &addr, int port, const string &host)
{
	HOSTENT * name = gethostbyname(host.c_str());

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(name->h_addr_list[0]);
}