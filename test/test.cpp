// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib,"WS2_32")

void main() {

	//---------------------------------------
	// Declare variables
	WSADATA wsaData;
	SOCKET ListenSocket;
	sockaddr_in service;

	//---------------------------------------
	// Initialize Winsock
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if( iResult != NO_ERROR )
		printf("Error at WSAStartup\n");

	//---------------------------------------
	// Create a listening socket
	ListenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket()\n");
		WSACleanup();
		return;
	}
	
	//---------------------------------------
	// Bind the socket to the local IP address
	// and port 27015
	hostent* thisHost;
	PCHAR ip;
	u_short port;
	port = 27015;
	thisHost = gethostbyname("");
	ip = inet_ntoa (*(struct in_addr *)*thisHost->h_addr_list);

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(ip);
	service.sin_port = htons(port);

	if ( bind( ListenSocket,(SOCKADDR*) &service, sizeof(service) )  == SOCKET_ERROR ) {
		printf("bind failed\n");
		closesocket(ListenSocket);
		return;
	}

	//---------------------------------------
	// Initialize variables and call setsockopt. 
	// The SO_KEEPALIVE parameter is a socket option 
	// that makes the socket send keepalive messages
	// on the session. The SO_KEEPALIVE socket option
	// requires a boolean value to be passed to the
	// setsockopt function. If TRUE, the socket is
	// configured to send keepalive messages, if FALSE
	// the socket configured to NOT send keepalive messages.
	// This section of code tests the setsockopt function
	// by checking the status of SO_KEEPALIVE on the socket
	// using the getsockopt function.
	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	int iOptVal;
	int iOptLen = sizeof(int);

	if (getsockopt(ListenSocket, SOL_SOCKET, SO_KEEPALIVE, (PCHAR)&iOptVal, &iOptLen) != SOCKET_ERROR) {
		printf("SO_KEEPALIVE Value: %ld\n", iOptVal);
	}

	if (setsockopt(ListenSocket, SOL_SOCKET, SO_KEEPALIVE, (PCHAR)&bOptVal, bOptLen) != SOCKET_ERROR) {
		printf("Set SO_KEEPALIVE: ON\n");
	}

	if (getsockopt(ListenSocket, SOL_SOCKET, SO_KEEPALIVE, (PCHAR)&iOptVal, &iOptLen) != SOCKET_ERROR) {
		printf("SO_KEEPALIVE Value: %ld\n", iOptVal);
	}

	WSACleanup();
	getchar();
	return;

}