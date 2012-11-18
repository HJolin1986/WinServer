
/**************************************************************************
	created:	2012/11/16	15:51	
	filename: 	Acl.H
	file path:	e:\Documents\Visual Studio 2008\Projects\WinSock\PhoenixLSP
	author:		DAILM, en_name: Dicky

	purpose:	
**************************************************************************/

#include "../common/PMacRes.h"
#include "../common/TypeStruct.h"

//-------------------------------------------
// create a session for each socket. check access list  CPCheckAccess
class CAcl
{
public:
	CAcl();
	~CAcl();
	//-------------------------------------------
	// socket create, create corresponding session
	void CheckSocket(SOCKET s, int af, int type, int protocol);
	void CheckCloseSocket(SOCKET s);
	void CheckBind(SOCKET s, const PSOCKADDR addr);

	//-------------------------------------------
	// check session whether allow remote service info
	int CheckAccept(SOCKET s, SOCKET sNew, sockaddr FAR *addr);
	int CheckConnect(SOCKET s, const struct sockaddr FAR *addr);

	int	CheckSendTo(SOCKET s, const SOCKADDR *pTo);
	int	CheckRecvFrom(SOCKET s, SOCKADDR *pFrom);

private:
	// session property
	void SetSession(SESSION *pSession, USHORT usRemotePort, ULONG ulRemoteIP, UCHAR ucDirection);
	
	void NotifySession(SESSION *pSession, int nCode);	// notify a session to app

	int GetAccessInfo(SESSION *pSession);
	int GetAccessFromWorkMode(SESSION *pSession);

	int FindRule(TCHAR *szAppName, int nStart);			// find filter rule in rule files

	int CreateSession(SOCKET s, int nProtocol);			// return the index
	int FindSession(SOCKET s);
	void DeleteSession(SOCKET s);
	void InitializeSession(SESSION *pSession);

	SESSION *m_pSession;
	int m_nSessionCount;
	int m_nSessionMaxCount;

	static BOOL QueryAccess();
	static BOOL IsLocalIP(DWORD dwIP);
};