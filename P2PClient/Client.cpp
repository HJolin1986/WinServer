
/**************************************************************************
	created:	2012/11/16	12:46	
	filename: 	Client.CPP
	file path:	e:\Documents\Visual Studio 2008\Projects\WinSock\P2PClient
	author:		DAILM, en_name: Dicky

	purpose:	main
**************************************************************************/
#include "stdafx.h"
#include "P2PClient.h"
#include "../common/PeerList.h"

class CMyP2p : public CP2PClient
{
public:
	void OnRecv(PCHAR pszUserName, PCHAR pszData, int nDataLen)
	{
		pszData[nDataLen] = '\0';
		printf(" Recv a Message from %s :  %s \n", pszUserName, pszData);
	}
};

void _tmain(int argc, char **argv)
{
	CMyP2p client;
	if (!client.Init())
	{
		printf(" CP2PClient::Init() failed \n");
		return ;
	}
	char szServerIp[20];
	char szUserName[MAX_USERNAME];
	printf(" Please input server ip: ");
	gets_s(szServerIp,sizeof(szServerIp));
	printf(" Please input your name: ");
	gets_s(szUserName,sizeof(szUserName));

	if(!client.Login(szUserName, szServerIp))
	{
		printf(" CP2PClient::Login() failed \n");
		return ;
	}

	client.GetUserList();											// first login get list
	printf(" %s has successfully logined server \n", szUserName);	//display user name and usage
	printf("\n Commands are: \"getu\", \"send\", \"exit\" \n");
	//-------------------------------------------
	// handle command recyclely
	char szCommandLine[256]; 
	while (TRUE)
	{
		gets_s(szCommandLine, sizeof(szCommandLine));
		if (strlen(szCommandLine) < 4)
		{
			continue;
		}

		//-------------------------------------------
		// analysis the cmd
		char szCommand[10];
		strncpy_s(szCommand, sizeof(szCommand), szCommandLine, 4);
		szCommand[4] = '\0';
		if(_stricmp(szCommand, "getu") == 0)
		{
			if(client.GetUserList())
			{
				printf(" Have %d users logined server: \n", client.m_PeerList.m_nCurrentSize);
				for(int i=0; i<client.m_PeerList.m_nCurrentSize; i++)
				{
					PEER_INFO *pInfo = &client.m_PeerList.m_pPeer[i];
					printf(" Username: %s(%s:%ld) \n", pInfo->szUserName, 
						::inet_ntoa(*((in_addr*)&pInfo->addr[pInfo->AddrNum -1].dwIp)), pInfo->addr[pInfo->AddrNum - 1].nPort);
				}
			}
			else
			{
				printf(" Get User List Failure !\n");
			}
		}
		else if(_stricmp(szCommand, "send") == 0)
		{
			//-------------------------------------------
			// analysis opposite name
			int i;
			char szPeer[MAX_USERNAME];
			for(i = 5; ; i++)
			{
				if(szCommandLine[i] != ' ')
					szPeer[i-5] = szCommandLine[i];
				else
				{
					szPeer[i-5] = '\0';
					break;
				}	
			}

			// analysis msg
			char szMsg[56];
			strncpy_s(szMsg, sizeof(szMsg), &szCommandLine[i+1],sizeof(szCommandLine)+1);
			if(client.SendText(szPeer, szMsg, strlen(szMsg)))
				printf(" Send OK! \n");
			else
				printf(" Send Failure! \n");
		}
		else if(_stricmp(szCommand, "exit") == 0)
		{
			break;
		}
	}
}
