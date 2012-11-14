/**************************************************************************
	created:	2012/11/14	22:49
	filename: 	GroupTalk.H
	file path:	F:\mycareer\9-终级-源码阅读\Windows网络通信\第5章\GroupTalk
	author:		Dailiming, en_name: Dicky

	purpose:	multicasting	
**************************************************************************/

#pragma once

#include "../common/SockInit.h"

#define WM_GROUPTALK WM_USER+105
#define BUFFER_SIZE 4096L
#define GROUP_PORT 4567L

/*
 * uMsg: WM_GROUPTALK
 * wParam: error num, 0 is no error;
 * lParam: GT_HDR head point
 */

const enum
{
	MT_JION = 1,	// a user joined
	MT_LEAVE,		// a user leave
	MT_MESG,		// a user send msg
	MT_MINE			// tell user his information
};

typedef struct gt_hdr
{
	u_char		gt_type;		// type of msg
	DWORD		dwAddr;			// the user's addr who send the msg
	char		szUser[15];		// name who send the msg
	int			nDataLength;	// data length
	char		*data() {	return (char*)(this+1);	}
}GT_HDR;

DWORD WINAPI _GroupTalkEntry(LPVOID lpParam);	// work thread function

class MulticastingTalk
{
public:
	// create working thread, join in the talk
	MulticastingTalk(HWND hNotifyWnd, DWORD dwMultiAddr, DWORD dwLocalAddr=INADDR_ANY, int nTTL = 64);
	// release sources and leave the talk
	~MulticastingTalk(void);
	BOOL SendText (char *szText, int nLen, DWORD dwRemoteAddr = 0);
protected:
	BOOL JoinGroup();
	BOOL LeaveGroup();
	BOOL Send(char *szText, int nLen, DWORD dwRomoteAddr);	
protected:
	void DispatchMsg(GT_HDR *pHeader, int nLen);			// distpatch the recved data
	friend DWORD WINAPI _GroupTalkEntry(LPVOID lpParam);	// work thread function

	HWND	m_hNotifyWnd;			// main window handle
	DWORD	m_dwMultiAddr;			// the talk multicasting ip
	DWORD	m_dwLocalAddr;			// the user's ip
	int		m_nTTL;					// packet's life cycle
	HANDLE	m_hThread;				// work thread
	HANDLE	m_hEvent;				// event handle, used to recv overlap i/o  data;
	SOCKET	m_sRead;				// recv data
	SOCKET	m_sSend;				// send sock data;
	BOOL	m_bQuit;				// give the work thread a notice that's exit
	char	m_szUser[256];			// user name
	Socket_NetData	sSock;
};
