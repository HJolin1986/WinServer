#pragma once

#include <WinSock2.h>
#include <string>
using namespace std;

class Socket_NetData
{
public: // structure & destructor
	Socket_NetData(BYTE minorVer=2, BYTE majorVer=2);
	~Socket_NetData(void);
	
public:
	inline void InitSockAddr(sockaddr_in &addr, int port, int ip);
	inline void InitSockAddr(sockaddr_in &addr, int port, const char ip[16]);
	inline void InitSockAddr(sockaddr_in &addr, int port, const string &host);
};

