
/**************************************************************************
	created:	2012/11/13	16:57	
	filename: 	IOCPUtility.CPP
	file path:	e:\Documents\Visual Studio 2008\Projects\WinSock\WinSock
	author:		Dailiming, en_name: Dicky

	purpose:	
**************************************************************************/

#include "StdAfx.h"
#include "IOCPUtility.h"


CIOCPUtility::CIOCPUtility(void)
{
	// LIST
	m_pFreeBufferList = NULL;
	m_pFreeContextList = NULL;
	m_pConnectionList = NULL;
	m_pPendingAccepts = NULL;
	m_nFreeBufferCount = 0;
	m_nFreeContextCount = 0;
	m_nPendingAcceptCount = 0;
	m_nCurrentConnection = 0;

	::InitializeCriticalSection(&m_PendingAcceptsLock);
	::InitializeCriticalSection(&m_FreeBufferListLock);
	::InitializeCriticalSection(&m_FreeContextListLock);
	::InitializeCriticalSection(&m_ConnectionListLock);

	// ACCEPT request
	m_hAcceptEvent = ::CreateEvent(NULL, FALSE,FALSE,NULL);
	m_hRepostEvent = ::CreateEvent(NULL, FALSE,FALSE,NULL);

	m_nRepostCount = 0;
	m_nPort = 4567;
	m_nInitialAccepts = 10;
	m_nInitialReads = 4;
	m_nMaxAccepts = 100;
	m_nMaxSends = 4;
	m_nMaxFreeBuffers = 200;
	m_nMaxFreeContexts = 100;
	m_nMaxConnections = 2000;
	m_hListenThread = NULL;
	m_hCompletion = NULL;
	m_sListen = INVALID_SOCKET;
	m_lpfnAcceptEx = NULL;
	m_lpfnGetAcceptExSockAddrs = NULL;
	m_bShutDown = FALSE;
	m_bServerStarted = FALSE;
	// initial WS2_32.dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2,2);
	::WSAStartup(sockVersion,&wsaData);
}


CIOCPUtility::~CIOCPUtility(void)
{
	ShutDown();
	if (m_sListen != INVALID_SOCKET)
	{
		::closesocket(m_sListen);
	}
	if (m_hListenThread != NULL)
	{
		::CloseHandle(m_hListenThread);
	}

	::CloseHandle(m_hRepostEvent);
	::CloseHandle(m_hAcceptEvent);

	::DeleteCriticalSection(&m_FreeBufferListLock);
	::DeleteCriticalSection(&m_FreeContextListLock);
	::DeleteCriticalSection(&m_PendingAcceptsLock);
	::DeleteCriticalSection(&m_ConnectionListLock);

	::WSACleanup();
}


bool CIOCPUtility::Start(int nPort, int nMaxConnections, int nMaxFreeBuffer,
						int nMaxFreeContext, int nInitialRead)
{
	// check if it has started
	if (m_bServerStarted)
	{
		goto TEND;
	}
	// store user's param
	m_nPort = nPort;
	m_nMaxFreeBuffers = nMaxFreeBuffer;
	m_nMaxFreeContexts = nMaxFreeContext;
	m_nInitialReads = nInitialRead;
	// initialize state variable
	m_bShutDown = FALSE;
	m_bServerStarted = TRUE;
	// Create listening socket, bind, then listen
	m_sListen = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN si;
	si.sin_family = AF_INET;
	si.sin_port = ::htons(m_nPort);
	si.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(m_sListen,(PSOCKADDR)&si, sizeof(si)) == SOCKET_ERROR)
	{
		m_bServerStarted = FALSE;
		goto FEND;
	}
	::listen(m_sListen,200);
	m_hCompletion = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,0,0,0);
	// load AcceptEX
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes;
	::WSAIoctl(m_sListen, 
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, 
		sizeof(GUID),
		&m_lpfnAcceptEx,
		sizeof(m_lpfnAcceptEx),
		&dwBytes,
		NULL,
		NULL);

	// load GetAcceptExSockAddrs
	GUID GuidAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
	::WSAIoctl(m_sListen, 
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, 
		sizeof(GUID),
		&m_lpfnGetAcceptExSockAddrs,
		sizeof(m_lpfnGetAcceptExSockAddrs),
		&dwBytes,
		NULL,
		NULL);
	
	// associate the listening socket with the complete port
	::CreateIoCompletionPort((HANDLE)m_sListen, m_hCompletion, (DWORD)0, 0);
	// register FD_ACCEPT
	WSAEventSelect(m_sListen, m_hAcceptEvent,FD_ACCEPT);
	// Create listening thread
	m_hListenThread = ::CreateThread(NULL,0,_ListenThreadProc, this, 0, NULL);
TEND:
	return TRUE;
FEND:
	return FALSE;
}


void CIOCPUtility::ShutDown(void)
{
	if (!m_bServerStarted)
	{
		return;
	}
	// annotation give all thread notice
	m_bShutDown = TRUE;
	::SetEvent(m_hAcceptEvent);
	// wait for threads
	::CloseHandle(m_hListenThread);
	m_hListenThread = NULL;
	m_bServerStarted = FALSE;
}


void CIOCPUtility::CloseAConnection(CIOCPContext* pContext)
{
	// remove from m_ConnectionList
	::EnterCriticalSection(&m_ConnectionListLock);
	CIOCPContext * pTest = m_pConnectionList;
	if (pTest == m_pConnectionList)
	{
		m_pConnectionList = pContext->pNext;
		m_nCurrentConnection --;
	} 
	else
	{
		while(pTest != NULL && pTest->pNext !=pContext)
		{
			pTest = pTest->pNext;
		}
		if (pTest != NULL)
		{
			pTest->pNext = pContext->pNext;
			m_nCurrentConnection --;
		}
	}
	::LeaveCriticalSection(&m_ConnectionListLock);
	// close socket
	::EnterCriticalSection(&pTest->lock);
	if (pContext->s != INVALID_SOCKET)
	{
		::closesocket(pContext->s);
		pContext->s = INVALID_SOCKET;
	}
	pContext->bClosing = TRUE;
	::LeaveCriticalSection(&pContext->lock);
}


void CIOCPUtility::CloseAllConnections(void)
{
	// traverse all ConnectionList
	CIOCPContext *pContext = m_pConnectionList;
	::EnterCriticalSection(&m_ConnectionListLock);
	while (pContext != NULL)
	{
		::EnterCriticalSection(&pContext->lock);
		if (pContext->s != INVALID_SOCKET)
		{
			::closesocket(pContext->s);
			pContext->s != INVALID_SOCKET;
		}

		pContext->bClosing = TRUE;
		pContext = pContext->pNext;
		::LeaveCriticalSection(&pContext->lock);
	}

	m_pConnectionList = NULL;
	m_nCurrentConnection = 0;
	::LeaveCriticalSection(&m_ConnectionListLock);
}

bool CIOCPUtility::SendText(CIOCPContext* pContext, char *pText, int nLen)
{
	CIOCPBuffer *pBuffer = AllocateBuffer(nLen);
	if(pBuffer != NULL)
	{
		::memcpy(pBuffer,pText,nLen);
		return PostSend(pContext,pBuffer);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
// user-defined help function
CIOCPBuffer *CIOCPUtility::AllocateBuffer(int nLen)
{
	if (nLen > BUFFER_SIZE)
	{
		return NULL;
	}

	CIOCPBuffer *pBuffer = NULL;
	::EnterCriticalSection(&m_FreeBufferListLock);
	if (NULL == m_pFreeBufferList)
	{
		pBuffer = (CIOCPBuffer*)::HeapAlloc(GetProcessHeap(), NULL,sizeof(CIOCPBuffer)+BUFFER_SIZE);
		pBuffer->ol.InternalHigh = 4040328;
		pBuffer->ol.Offset = 0;
		pBuffer->ol.OffsetHigh = 0;
		pBuffer->ol.Pointer = 0;
		pBuffer->nSequenceNumber = 0;
		pBuffer->ToString();
	}
	else
	{
		pBuffer = m_pFreeBufferList;
		m_pFreeBufferList = m_pFreeBufferList->pNext;
		pBuffer->pNext = NULL;
		m_nFreeBufferCount --;
	}

	if (NULL != pBuffer)
	{
		pBuffer->buf = (char *)(pBuffer+1);
		pBuffer->nLen = nLen;
	}

	return pBuffer;
}
void CIOCPUtility::ReleaseBuffer(CIOCPBuffer *pBuffer)
{
	// if m_nFreeBufferCount >= m_nMaxFreeBuffers free it, otherwise add it to list;
	::EnterCriticalSection(&m_FreeBufferListLock);
	if (m_nFreeBufferCount >= m_nMaxFreeBuffers)
	{
		::HeapFree(::GetProcessHeap(), 0 , pBuffer);
	}
	else
	{
		::memset(pBuffer, 0, sizeof(CIOCPBuffer)+BUFFER_SIZE);
		pBuffer->pNext = m_pFreeBufferList;
		m_pFreeBufferList = pBuffer;
		m_nFreeBufferCount ++;
	}
	::LeaveCriticalSection(&m_FreeBufferListLock);
}

void CIOCPUtility::FreeBuffers()
{
	// 遍历m_pFreeBufferList空闲列表，释放缓冲区池内存
	::EnterCriticalSection(&m_FreeBufferListLock);

	CIOCPBuffer *pFreeBuffer = m_pFreeBufferList;
	CIOCPBuffer *pNextBuffer;
	while(pFreeBuffer != NULL)
	{
		pNextBuffer = pFreeBuffer->pNext;
		if(!::HeapFree(::GetProcessHeap(), 0, pFreeBuffer))
		{
#ifdef _DEBUG
			::OutputDebugStringA("  FreeBuffers error！");
#endif // _DEBUG
			break;
		}
		pFreeBuffer = pNextBuffer;
	}
	m_pFreeBufferList = NULL;
	m_nFreeBufferCount = 0;

	::LeaveCriticalSection(&m_FreeBufferListLock);
}

CIOCPContext* CIOCPUtility::AllocateContext(SOCKET s)
{
	CIOCPContext* pContext = NULL;
	::EnterCriticalSection(&m_FreeContextListLock);
	if (NULL != m_pFreeBufferList)
	{
		pContext = m_pFreeContextList;
		m_pFreeContextList = m_pFreeContextList->pNext;

		pContext->s = s;
		pContext->pNext = NULL;
		m_nFreeContextCount --;
	} 
	else
	{
		pContext = (CIOCPContext*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CIOCPContext));
		::InitializeCriticalSection(&pContext->lock);
	}
	::LeaveCriticalSection(&m_FreeContextListLock);

	if (NULL != pContext)
	{
		pContext->s = s;

	}
	return NULL;
}
void CIOCPUtility::ReleaseContext(CIOCPContext * pContext)
{
	// firt reales buufers
	CIOCPBuffer* pNext;
	while (NULL != pContext->pOutOfOrderReads)
	{
		pNext = pContext->pOutOfOrderReads;
		pContext->pOutOfOrderReads = pNext->pNext;
		ReleaseBuffer(pNext);
	}

	::EnterCriticalSection(&m_FreeContextListLock);
	if (m_nFreeContextCount >= m_nMaxFreeContexts)
	{
		CRITICAL_SECTION cstmp = pContext->lock;
		::memset(pContext,0,sizeof(CIOCPContext));
		pContext->lock = cstmp;
		pContext->pNext = m_pFreeContextList;
		m_pFreeContextList = pContext;
		m_nFreeContextCount ++;
	} 
	else
	{
		DeleteCriticalSection(&pContext->lock);
		::HeapFree(::GetProcessHeap(), 0, pContext);
	}
	::LeaveCriticalSection(&m_FreeContextListLock);
}
void CIOCPUtility::FreeContexts()
{
	// traverse to free memory pool
	::EnterCriticalSection(&m_FreeContextListLock);

	CIOCPContext *pFreeContext = m_pFreeContextList;
	CIOCPContext *pNextContext;
	while(pFreeContext != NULL)
	{
		pNextContext = pFreeContext->pNext;

		::DeleteCriticalSection(&pFreeContext->lock);
		if(!::HeapFree(::GetProcessHeap(), 0, pFreeContext))
		{
#ifdef _DEBUG
			::OutputDebugStringA("  FreeBuffers释放内存出错！");
#endif // _DEBUG
			break;
		}
		pFreeContext = pNextContext;
	}
	m_pFreeContextList = NULL;
	m_nFreeContextCount = 0;

	::LeaveCriticalSection(&m_FreeContextListLock);
}

bool CIOCPUtility::AddAConnection(CIOCPContext* pContext)
{
	::EnterCriticalSection(&m_ConnectionListLock);
	if (m_nCurrentConnection < m_nMaxConnections)
	{
		pContext->pNext = m_pConnectionList;
		m_pConnectionList = pContext;
		m_nMaxConnections ++;
		::LeaveCriticalSection(&m_ConnectionListLock);
		return TRUE;
	}
	::LeaveCriticalSection(&m_ConnectionListLock);
	return FALSE;
}
bool CIOCPUtility::InsertPendingAccept(CIOCPBuffer *pBuffer)
{
	::EnterCriticalSection(&m_PendingAcceptsLock);
	if (NULL == m_pPendingAccepts)
	{
		m_pPendingAccepts = pBuffer;
	} 
	else
	{
		pBuffer->pNext = m_pPendingAccepts;
		m_pPendingAccepts = pBuffer;
	}
	m_nPendingAcceptCount ++;
	::LeaveCriticalSection(&m_PendingAcceptsLock);
	return TRUE;
}
bool CIOCPUtility::RemovePendingAccept(CIOCPBuffer *pBuffer)
{
	bool bResult = FALSE;
	// traverse m_pPendingAccepts
	::EnterCriticalSection(&m_PendingAcceptsLock);

	CIOCPBuffer* pTmp = m_pPendingAccepts;
	if (pTmp == pBuffer)
	{
		m_pPendingAccepts = m_pPendingAccepts->pNext;
		bResult = TRUE;
	} 
	else
	{
		while(pTmp != NULL && pTmp->pNext != pBuffer)
		{
			pTmp = pTmp->pNext;
		}
		if (NULL != pTmp)
		{
			pTmp->pNext = pBuffer->pNext;
			bResult = TRUE;
		}
	}

	if (bResult)
	{
		m_nPendingAcceptCount --;
	}
	::LeaveCriticalSection(&m_PendingAcceptsLock);
	return bResult;
}

/********************************************************************************/
/* pBuffer->nSequenceNumber == pContext->nSequenceNumber						*/
/* | >-> true --> return  pBuffer												*/
/* | >-> false --> insert into ascending order pContext->pOutOfOrderReads		*/
/* V																			*/
/* pContext->pOutOfOrderReads->pOutOfOrderReads == pContext->nSequenceNumber	*/
/* | >-> true return the first of pContext->pOutOfOrderReads					*/
/* V																			*/
/* return NULL																	*/
/********************************************************************************/
CIOCPBuffer *CIOCPUtility::GetNextReadBuffer(CIOCPContext* pContext, CIOCPBuffer* pBuffer)
{
	if (NULL != pBuffer)
	{
		// if the Sequence number that will be read is equal to the pBuffer->nSequenceNumber
		if (pBuffer->nSequenceNumber == pContext->nCurrentReadSequence)
		{
			return pBuffer;
		}
		// if not equal to the pBuffer->nSequenceNumber, that's to say send in disorder
		// so we'll store them into pContext->pOutOfOrderReads
		pBuffer->pNext = NULL;
		CIOCPBuffer* ptr = pContext->pOutOfOrderReads;
		CIOCPBuffer* pPre = NULL;
		while(NULL != ptr)
		{
			if (pBuffer->nSequenceNumber < ptr->nSequenceNumber)
			{
				break;
			}
			pPre = ptr;
			ptr = ptr->pNext;
		}

		if (NULL == pPre)								// insert into the head of List
		{
			pBuffer->pNext = pContext->pOutOfOrderReads;
			pContext->pOutOfOrderReads = pBuffer;
		}
		else											// insert into the medium of List
		{
			pBuffer->pNext = pPre->pNext;
			pPre->pNext = pBuffer;
		}
	}
	CIOCPBuffer* ptr = pContext->pOutOfOrderReads;
	if (NULL != ptr && (ptr->nSequenceNumber == pContext->nCurrentReadSequence))
	{
		pContext->pOutOfOrderReads = ptr->pNext;
		return ptr;
	}
	return NULL;
}

// Accept
bool CIOCPUtility::PostAccept(CIOCPBuffer *pBuffer)
{
	// I/O type
	pBuffer->nOperation = OP_ACCEPT;
	DWORD dwBytes;
	pBuffer->sClient = ::WSASocket(AF_INET, SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);

	pBuffer->ToString();
	BOOL b = m_lpfnAcceptEx(m_sListen,
		pBuffer->sClient,
		pBuffer->buf,
		pBuffer->nLen-((sizeof(sockaddr_in) + 16)*2),
		sizeof(sockaddr_in)+16,
		sizeof(sockaddr_in)+16,
		&dwBytes,
		&pBuffer->ol);
	pBuffer->ToString();
	if (!b && ::WSAGetLastError() != WSA_IO_PENDING)
	{
		fprintf(stderr, "PostAccept failed: %d\n", WSAGetLastError());
		return FALSE;
	}
	puts("PostAccept sucess");
	return TRUE;
}
bool CIOCPUtility::PostSend(CIOCPContext* pContext, CIOCPBuffer*pBuffer)
{
	if (pContext->nOutstandingSend > m_nMaxSends)
	{
		return FALSE;
	}
	pBuffer->nOperation = OP_WRITE;
	DWORD dwBytes;
	DWORD dwFlags = 0;
	WSABUF buf;
	buf.buf = pBuffer->buf;
	buf.len = pBuffer->nLen;
	if (::WSASend(pContext->s,&buf,1,&dwBytes,dwFlags,&pBuffer->ol,NULL) != NO_ERROR)
	{
		if (::WSAGetLastError() != WSA_IO_PENDING)
		{
			return FALSE;
		}
	
	}
	// increase the amount of overlap I/O on socket
	::EnterCriticalSection(&pContext->lock);
	pContext->nOutstandingSend ++;
	::LeaveCriticalSection(&pContext->lock);
	return TRUE;
}
bool CIOCPUtility::PostRecv(CIOCPContext* pContext, CIOCPBuffer*pBuffer)
{
	pBuffer->nOperation = OP_READ;
	::EnterCriticalSection(&pContext->lock);
	pBuffer->nSequenceNumber = pContext->nCurrentReadSequence;
	// post overlap I/O
	DWORD dwBytes;
	DWORD dwFlags;
	WSABUF buf;
	buf.buf = pBuffer->buf;
	buf.len = pBuffer->nLen;
	if (::WSARecv(pContext->s,&buf,1,&dwBytes,&dwFlags,&pContext->ol,NULL) != NO_ERROR)
	{
		if (::WSAGetLastError() != WSA_IO_PENDING)
		{
			::LeaveCriticalSection(&pContext->lock);
			return FALSE;
		}
	}
	// increase the amount of overlap I/O on socket
	pContext->nOutstandingRecv ++;
	pContext->nReadSeuence ++;
	puts("PostRecv");
	::LeaveCriticalSection(&pContext->lock);
}
void CIOCPUtility::HandleIO(DWORD dwKey, CIOCPBuffer *pBuffer, DWORD dwTrans, int nError)
{
	CIOCPContext *pContext = (CIOCPContext *)dwKey;

	puts("IO");
#ifdef _DEBUG
	::OutputDebugStringA("	HandleIO... \n");
#endif // _DEBUG

	// 1）first decrease nOutstandingRecv
	if(pContext != NULL)
	{

		puts("pContext != NULL");
		::EnterCriticalSection(&pContext->lock);

		if(pBuffer->nOperation == OP_READ)
			pContext->nOutstandingRecv --;
		else if(pBuffer->nOperation == OP_WRITE)
			pContext->nOutstandingSend --;

		::LeaveCriticalSection(&pContext->lock);

		// 2）check the socket 
		if(pContext->bClosing) 
		{
#ifdef _DEBUG
			::OutputDebugStringA("	检查到套节字已经被我们关闭 \n");
#endif // _DEBUG
			if(pContext->nOutstandingRecv == 0 && pContext->nOutstandingSend == 0)
			{		
				ReleaseContext(pContext);
			}
			// release buff when the socket had closed
			ReleaseBuffer(pBuffer);	
			return;
		}
	}
	else
	{
		puts("pContext == NULL");
		RemovePendingAccept(pBuffer);
	}

	// 3）check the error on socket, if there is error, will inform user, then close it
	if(nError != NO_ERROR)
	{
		if(pBuffer->nOperation != OP_ACCEPT)
		{
			OnConnectionError(pContext, pBuffer, nError);
			CloseAConnection(pContext);
			if(pContext->nOutstandingRecv == 0 && pContext->nOutstandingSend == 0)
			{		
				ReleaseContext(pContext);
			}
#ifdef _DEBUG
			::OutputDebugStringA("	检查到客户套节字上发生错误 \n");
#endif // _DEBUG
		}
		else 
		{
			// when client has error, that's there is erro on socket, we will close it
			if(pBuffer->sClient != INVALID_SOCKET)
			{
				::closesocket(pBuffer->sClient);
				pBuffer->sClient = INVALID_SOCKET;
			}
#ifdef _DEBUG
			::OutputDebugStringA("	检查到监听套节字上发生错误 \n");
#endif // _DEBUG
		}

		ReleaseBuffer(pBuffer);
		return;
	}


	// begin handle
	if(pBuffer->nOperation == OP_ACCEPT)
	{
		if(dwTrans == 0)
		{
#ifdef _DEBUG
			::OutputDebugStringA("	监听套节字上客户端关闭 \n");
#endif // _DEBUG

			if(pBuffer->sClient != INVALID_SOCKET)
			{
				::closesocket(pBuffer->sClient);
				pBuffer->sClient = INVALID_SOCKET;
			}
		}
		else
		{
			// alloc context for new connection
			CIOCPContext *pClient = AllocateContext(pBuffer->sClient);
			if(pClient != NULL)
			{
				if(AddAConnection(pClient))
				{	
					// achieve the client's socket
					int nLocalLen, nRmoteLen;
					LPSOCKADDR pLocalAddr, pRemoteAddr;
					m_lpfnGetAcceptExSockAddrs(
						pBuffer->buf,
						pBuffer->nLen - ((sizeof(sockaddr_in) + 16) * 2),
						sizeof(sockaddr_in) + 16,
						sizeof(sockaddr_in) + 16,
						(SOCKADDR **)&pLocalAddr,
						&nLocalLen,
						(SOCKADDR **)&pRemoteAddr,
						&nRmoteLen);
					memcpy(&pClient->addrLocal, pLocalAddr, nLocalLen);
					memcpy(&pClient->addrRemote, pRemoteAddr, nRmoteLen);

					// association connection with complete port
					::CreateIoCompletionPort((HANDLE)pClient->s, m_hCompletion, (DWORD)pClient, 0);

					// inform user
					pBuffer->nLen = dwTrans;
					OnConnectionEstablished(pClient, pBuffer);

					// post recv to new connection，these space will be free when the socket is closed or error
					for(int i=0; i<5; i++)
					{
						CIOCPBuffer *p = AllocateBuffer(BUFFER_SIZE);
						if(p != NULL)
						{
							if(!PostRecv(pClient, p))
							{
								CloseAConnection(pClient);
								break;
							}
						}
					}
				}
				else	// the connection num has reach the max
				{
					CloseAConnection(pClient);
					ReleaseContext(pClient);
				}
			}
			else
			{
				// when lack resource, close connection
				::closesocket(pBuffer->sClient);
				pBuffer->sClient = INVALID_SOCKET;
			}
		}
		ReleaseBuffer(pBuffer);	

		// give the listen thread to post a accept request again
		::InterlockedIncrement(&m_nRepostCount);
		::SetEvent(m_hRepostEvent);
	}
	else if(pBuffer->nOperation == OP_READ)
	{
		if(dwTrans == 0)	// close by opposite side
		{
			// inform user
			pBuffer->nLen = 0;
			OnConnectionClosing(pContext, pBuffer);	

			CloseAConnection(pContext);
			if(pContext->nOutstandingRecv == 0 && pContext->nOutstandingSend == 0)
			{		
				ReleaseContext(pContext);
			}
			ReleaseBuffer(pBuffer);	
		}
		else
		{
			pBuffer->nLen = dwTrans;
			// recv data by the post order
			CIOCPBuffer *p = GetNextReadBuffer(pContext, pBuffer);
			while(p != NULL)
			{
				// inform user
				OnReadCompleted(pContext, p);
				// increase nCurrentReadSequence
				::InterlockedIncrement((LONG*)&pContext->nCurrentReadSequence);
				ReleaseBuffer(p);
				p = GetNextReadBuffer(pContext, NULL);
			}

			// continue post Recv
			pBuffer = AllocateBuffer(BUFFER_SIZE);
			if(pBuffer == NULL || !PostRecv(pContext, pBuffer))
			{
				CloseAConnection(pContext);
			}
		}
	}
	else if(pBuffer->nOperation == OP_WRITE)
	{

		if(dwTrans == 0)	// closed by the opposite side
		{
			// inform user firstly
			pBuffer->nLen = 0;
			OnConnectionClosing(pContext, pBuffer);	

			// reclose connections
			CloseAConnection(pContext);

			// release context
			if(pContext->nOutstandingRecv == 0 && pContext->nOutstandingSend == 0)
			{		
				ReleaseContext(pContext);
			}
			ReleaseBuffer(pBuffer);	
		}
		else
		{
			// complete writing, then inform user
			pBuffer->nLen = dwTrans;
			OnWriteCompleted(pContext, pBuffer);
			// release SendText's buf
			ReleaseBuffer(pBuffer);
		}
	}
}

DWORD WINAPI CIOCPUtility::_ListenThreadProc(LPVOID lpParam)
{
	CIOCPUtility *pThis = (CIOCPUtility*)lpParam;

	// first post accept
	CIOCPBuffer *pBuffer;
	Sleep(1000);
	for(int i=0; i<pThis->m_nInitialAccepts; i++)
	{
		pBuffer = pThis->AllocateBuffer(BUFFER_SIZE);
		if(pBuffer == NULL)
			return -1;
		pThis->InsertPendingAccept(pBuffer);
		pThis->PostAccept(pBuffer);
	}

	// create event array, 
	HANDLE hWaitEvents[2 + MAX_THREAD];
	int nEventCount = 0;
	hWaitEvents[nEventCount ++] = pThis->m_hAcceptEvent;
	hWaitEvents[nEventCount ++] = pThis->m_hRepostEvent;

	for(int i=0; i<MAX_THREAD; i++)
	{
		hWaitEvents[nEventCount ++] = ::CreateThread(NULL, 0, _WorkerThreadProc, pThis, 0, NULL);
	}

	// infinite cycle, handle envent
	while(TRUE)
	{
		puts("_ListenThreadProc");
		int nIndex = ::WSAWaitForMultipleEvents(nEventCount, hWaitEvents, FALSE, 60*1000, FALSE);

		printf("_ListenThreadProc index=%d\n", nIndex);
		// first check if the serve is shutdown
		if(pThis->m_bShutDown || nIndex == WSA_WAIT_FAILED)
		{
			pThis->CloseAllConnections();
			::Sleep(0);										// give io thread chance to run
			
			::closesocket(pThis->m_sListen);				// close listen socket
			pThis->m_sListen = INVALID_SOCKET;
			::Sleep(0);										// give io thread chance to run

			// notice all io thread to exit
			for(int i=2; i<MAX_THREAD + 2; i++)
			{	
				::PostQueuedCompletionStatus(pThis->m_hCompletion, -1, 0, NULL);
			}

			// wait for all io thread to exit
			::WaitForMultipleObjects(MAX_THREAD, &hWaitEvents[2], TRUE, 5*1000);

			for(int i=2; i<MAX_THREAD + 2; i++)
			{	
				::CloseHandle(hWaitEvents[i]);
			}

			::CloseHandle(pThis->m_hCompletion);

			pThis->FreeBuffers();
			pThis->FreeContexts();
			::ExitThread(0);
		}	

		// 1）timing to check accept
		if(nIndex == WSA_WAIT_TIMEOUT)
		{
			pBuffer = pThis->m_pPendingAccepts;
			while(pBuffer != NULL)
			{
				int nSeconds;
				int nLen = sizeof(nSeconds);
				// how long connected
				::getsockopt(pBuffer->sClient, 
					SOL_SOCKET, SO_CONNECT_TIME, (char *)&nSeconds, &nLen);	
				// after 2m since connected, let client go away;
				if(nSeconds != -1 && nSeconds > 2*60)
				{   
					closesocket(pBuffer->sClient);
					pBuffer->sClient = INVALID_SOCKET;
				}

				pBuffer = pBuffer->pNext;
			}
		}
		else
		{
			nIndex = nIndex - WAIT_OBJECT_0;
			WSANETWORKEVENTS ne;
			int nLimit=0;
			// 2）m_hAcceptEvent happened, so we should increase post accept amount that is less;
			if(nIndex == 0)			
			{
				::WSAEnumNetworkEvents(pThis->m_sListen, hWaitEvents[nIndex], &ne);
				if(ne.lNetworkEvents & FD_ACCEPT)
				{
					nLimit = 50;  // increase amount
				}
			}
			// 3）when accepted m_hRepostEvent, new client coming
			else if(nIndex == 1)	
			{
				nLimit = InterlockedExchange(&pThis->m_nRepostCount, 0);
			}
			// I/O service thread exit, that's there is error
			else if(nIndex > 1)		
			{
				pThis->m_bShutDown = TRUE;
				continue;
			}

			// post nLimit accept
			int i = 0;
			while(i++ < nLimit && pThis->m_nPendingAcceptCount < pThis->m_nMaxAccepts)
			{
				pBuffer = pThis->AllocateBuffer(BUFFER_SIZE);
				if(pBuffer != NULL)
				{
					pThis->InsertPendingAccept(pBuffer);
					pThis->PostAccept(pBuffer);
				}
			}
		}
	}
	return 0;
}
DWORD WINAPI CIOCPUtility::_WorkerThreadProc(LPVOID lpParam)
{
#ifdef _DEBUG
	::OutputDebugStringA("	WorkerThread start... \n");
#endif // _DEBUG

	CIOCPUtility *pThis = (CIOCPUtility*)lpParam;

	CIOCPBuffer *pBuffer;
	DWORD dwKey;
	DWORD dwTrans;
	LPOVERLAPPED lpol;
	puts("WorkThreadProc start");
	while(TRUE)
	{
		// wait all complete on the complete port
		BOOL bOK = ::GetQueuedCompletionStatus(pThis->m_hCompletion, 
			&dwTrans, (LPDWORD)&dwKey, (LPOVERLAPPED*)&lpol, WSA_INFINITE);
		printf("DWKEY=%d, handle=%d\n", dwKey,pThis->m_hCompletion);
		if (FALSE == bOK)
		{
			int error = GetLastError();
			if (WAIT_TIMEOUT != error)
			{
				::OutputDebugStringA("	GetQueuedCompletionStatus error\n");
				::exit(0);
			}

			printf("WorkThreadProc error = %d\n", error);
			continue;
		}
		if(dwTrans == -1) // exited notification announced by user
		{
#ifdef _DEBUG
			::OutputDebugStringA("	WorkerThread exit \n");
#endif // _DEBUG
			::ExitThread(0);
		}

		pBuffer = CONTAINING_RECORD(lpol, CIOCPBuffer, ol);
		int nError = NO_ERROR;
		if(!bOK)						// there is error on the socket
		{
			SOCKET s;
			if(pBuffer->nOperation == OP_ACCEPT)
			{
				s = pThis->m_sListen;
			}
			else
			{
				if(dwKey == 0)
					break;
				s = ((CIOCPContext*)dwKey)->s;
			}
			DWORD dwFlags = 0;
			if(!::WSAGetOverlappedResult(s, &pBuffer->ol, &dwTrans, FALSE, &dwFlags))
			{
				nError = ::WSAGetLastError();
			}
		}
		puts("handio");
		pThis->HandleIO(dwKey, pBuffer, dwTrans, nError);
	}

#ifdef _DEBUG
	::OutputDebugStringA("	WorkerThread 退出 \n");
#endif // _DEBUG
	return 0;

}

void CIOCPUtility::OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer* pBuffer)
{}
void CIOCPUtility::OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer* pBuffer)
{}
void CIOCPUtility::OnConnectionError(CIOCPContext *pContext, CIOCPBuffer* pBuffer, int nError)
{}
void CIOCPUtility::OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer* pBuffer)
{}
void CIOCPUtility::OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer* pBuffer)
{}


