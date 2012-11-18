
/**************************************************************************
	created:	2012/11/14	22:26
	filename: 	SockInit.H
	file path:	f:\GitHub\WinSock\common
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/
#pragma once

#include <WinSock2.h>
#include <windows.h>
#include <string>
#include <ws2tcpip.h>
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

public:
	static USHORT CheckSum(USHORT*buf, int size);
	inline static BOOL SetTTL(SOCKET s, int nValue)
	{
		int ret = ::setsockopt(s, IPPROTO_IP, IP_TTL, (PCHAR)nValue,sizeof(nValue));
		return ret != SOCKET_ERROR;
	}

	inline static BOOL SetTimeOut(SOCKET s, int nTime, BOOL bRecv)
	{
		int ret = ::setsockopt(s,SOL_SOCKET, bRecv?SO_RCVTIMEO:SO_SNDTIMEO, (PCHAR)&nTime, sizeof(nTime));
		return ret != SOCKET_ERROR;
	}
};