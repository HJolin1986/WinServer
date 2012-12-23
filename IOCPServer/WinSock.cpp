// WinSock.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "IOCPUtility.h"
#include <stdio.h>
#include <windows.h>

class CMyServer : public CIOCPUtility
{
public:

	void OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
	{
		printf(" 接收到一个新的连接（%d）： %s \n", 
			GetCurrentConnection(), ::inet_ntoa(pContext->addrRemote.sin_addr));

		SendText(pContext, pBuffer->buf, pBuffer->nLen);
	}

	void OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
	{
		printf(" 一个连接关闭！ \n" );
	}

	void OnConnectionError(CIOCPContext *pContext, CIOCPBuffer *pBuffer, int nError)
	{
		printf(" 一个连接发生错误： %d \n ", nError);
	}

	void OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
	{
		SendText(pContext, pBuffer->buf, pBuffer->nLen);
	}

	void OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
	{
		printf(" 数据发送成功！\n ");
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	CMyServer *pServer = new CMyServer;

	// 开启服务
	if(pServer->Start())
	{
		printf(" 服务器开启成功... \n");
	}
	else
	{
		printf(" 服务器开启失败！\n");
		return -1;
	}

	// 创建事件对象，让ServerShutdown程序能够关闭自己
	HANDLE hEvent = ::CreateEventA(NULL, FALSE, FALSE, "ShutdownEvent");
	HANDLE hEvent1 = ::CreateEventA(NULL, FALSE, FALSE, "ShutdownEvent");
	::WaitForSingleObject(hEvent, INFINITE);
	::CloseHandle(hEvent);

	// 关闭服务
	pServer->ShutDown();
	delete pServer;

	printf(" 服务器关闭 \n ");
	return 0;
}
