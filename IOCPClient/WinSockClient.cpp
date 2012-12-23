// WinSockClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "WS2_32")
#define DATA_BUFSIZE 4096
#define SEND_COUNT   10

void _tmain() 
{
	WSADATA wsd;
	struct addrinfo *result = NULL;
	WSAOVERLAPPED SendOverlapped = {0};
	SOCKET RemoteSock = INVALID_SOCKET, ClientSock = INVALID_SOCKET;
	WSABUF DataBuf;
	WSABUF OutBuf;
	DWORD SendBytes = 0, Flags;
	char buffer[DATA_BUFSIZE];
	int err, rc, i;
	
	DataBuf.len = DATA_BUFSIZE;
	DataBuf.buf = buffer;
	OutBuf.len = DATA_BUFSIZE;
	OutBuf.buf = new char[DATA_BUFSIZE];

	// Load Winsock
	rc = WSAStartup(MAKEWORD(2,2), &wsd);
	if (rc != 0) {
		fprintf(stderr, "Unable to load Winsock: %d\n", rc);
		return;
	}

	// create remote sock
	SOCKADDR_IN serveradd;
	serveradd.sin_family = AF_INET;
	serveradd.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	serveradd.sin_port = htons(4567);

	RemoteSock = WSASocket(AF_INET, SOCK_STREAM,IPPROTO_TCP,NULL,0, WSA_FLAG_OVERLAPPED);
	if (RemoteSock == INVALID_SOCKET) {
		fprintf(stderr, "socket failed: %d\n", 
			WSAGetLastError());
		freeaddrinfo(result);
		return;
	}

	SOCKADDR_IN clientadd;
	clientadd.sin_family = AF_INET;
	clientadd.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	clientadd.sin_port = htons(4568);

	ClientSock = WSASocket(AF_INET, SOCK_STREAM,IPPROTO_TCP,NULL,0, WSA_FLAG_OVERLAPPED);
	if (ClientSock == INVALID_SOCKET) {
		fprintf(stderr, "socket failed: %d\n", 
			WSAGetLastError());
		freeaddrinfo(result);
		return;
	}
	::bind(ClientSock,(PSOCKADDR)&clientadd,sizeof(SOCKADDR);
	rc = WSAConnectEx(ClientSock,(PSOCKADDR)&ClientSock,sizeof(SOCKADDR_IN), &DataBuf,&OutBuf,NULL,NULL);
//	rc = listen(ListenSocket, 1);
	if (rc == SOCKET_ERROR) {
		fprintf(stderr, "listen failed: %d\n", 
			WSAGetLastError());
		freeaddrinfo(result);
		closesocket(RemoteSock);
		return;
	}


	printf("Client Sended...\n");
	// Create an event handle and setup an overlapped structure.
	SendOverlapped.hEvent = WSACreateEvent();
	if (SendOverlapped.hEvent == NULL) {
		fprintf(stderr, "WSACreateEvent failed: %d\n", 
			WSAGetLastError());
		freeaddrinfo(result);
		closesocket(RemoteSock);
		return;
	}


	memcpy(DataBuf.buf, "send data\n",11);
	for(i=0; i < SEND_COUNT ;i++) {

		WSARecv(RemoteSock,&OutBuf,1,&SendBytes,0,&SendOverlapped,NULL);

		rc = WSASend(RemoteSock, &DataBuf, 1, 
			&SendBytes, 0, &SendOverlapped, NULL);
		if ( (rc == SOCKET_ERROR) && 
			(WSA_IO_PENDING != (err = WSAGetLastError()))) {
				fprintf(stderr, "WSASend failed: %d\n", err);
				break;
		}

		rc = WSAWaitForMultipleEvents(1, &SendOverlapped.hEvent, TRUE, INFINITE, TRUE);
		if (rc == WSA_WAIT_FAILED) {
			fprintf(stderr, "WSAWaitForMultipleEvents failed: %d\n", WSAGetLastError());
			break;
		}

		rc = WSAGetOverlappedResult(RemoteSock, &SendOverlapped, &SendBytes, FALSE, &Flags);
		if (rc == FALSE) {
			fprintf(stderr, "WSASend operation failed: %d\n", WSAGetLastError());
			break;
		}

		printf("Wrote %d bytes\n", SendBytes);

		WSAResetEvent(SendOverlapped.hEvent);

	}

	WSACloseEvent(SendOverlapped.hEvent);
	closesocket(RemoteSock);
	freeaddrinfo(result);

	WSACleanup();

	return;
}
