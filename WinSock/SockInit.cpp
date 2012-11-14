#include "StdAfx.h"
#include "SockInit.h"

#pragma comment(lib,"WS2_32")

Socket_NetData::Socket_NetData(BYTE minorVer, BYTE majorVer)
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer,majorVer);
	if(0 != ::WSAStartup(sockVersion, &wsaData))
	{
		exit(-1);
	}
}

Socket_NetData::~Socket_NetData(void)
{
	::WSACleanup();
}

void Socket_NetData::InitSockAddr(sockaddr_in &addr, int port, int ip)
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = ip;
}

void Socket_NetData::InitSockAddr(sockaddr_in &addr, int port, const char ip[16])
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
}


void Socket_NetData::InitSockAddr(sockaddr_in &addr, int port, const string &host)
{
	HOSTENT * name = gethostbyname(host.c_str());

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(name->h_addr_list[0]);
}