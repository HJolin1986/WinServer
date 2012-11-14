
/**************************************************************************
	created:	2012/11/14	22:26
	filename: 	SockInit.H
	file path:	f:\GitHub\WinSock\common
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/
#pragma once

#include <WinSock2.h>
#include <string>
using namespace std;

class Socket_NetData
{
public: // structure & destructor
	Socket_NetData(BYTE minorVer=2, BYTE majorVer=2)
	{
		WSADATA wsaData;
		WORD sockVersion = MAKEWORD(minorVer,majorVer);
		if(0 != ::WSAStartup(sockVersion, &wsaData))
		{
			exit(-1);
		}
	}
	~Socket_NetData(void)
	{
		::WSACleanup();
	}
	
public:
	inline void InitSockAddr(sockaddr_in &addr, int port, int ip);
	inline void InitSockAddr(sockaddr_in &addr, int port, const char ip[16]);
	inline void InitSockAddr(sockaddr_in &addr, int port, const string &host);
};