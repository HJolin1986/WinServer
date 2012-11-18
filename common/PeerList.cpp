#include "StdAfx.h"
#include "PeerList.h"

CPeerList::CPeerList(void)
{
	m_nCurrentSize = 0;
	m_nTatolSize = 100;
	m_pPeer = new PEER_INFO[m_nTatolSize];
}

CPeerList::~CPeerList(void)
{
	delete[] m_pPeer;
}

BOOL CPeerList::AddAPeer(PEER_INFO *pPeer)		// like vector 
{
	if(GetAPeer(pPeer->szUserName) != NULL)
	{
		return FALSE;
	}
	if (m_nCurrentSize >= m_nTatolSize)			// 
	{
		PEER_INFO *pTmp = m_pPeer;
		m_nTatolSize = m_nTatolSize * 2;
		m_pPeer = new PEER_INFO[m_nTatolSize];
		memcpy(m_pPeer, pTmp, m_nCurrentSize);
		delete pTmp; 
	}
	memcpy(&m_pPeer[m_nCurrentSize++], pPeer, sizeof(PEER_INFO));
	return TRUE;
}

PEER_INFO *CPeerList::GetAPeer(PCHAR pszUserName)
{
	for (int i = 0; i < m_nCurrentSize ; i++)
	{
		if (stricmp(m_pPeer[i].szUserName, pszUserName) == 0)
		{
			return &m_pPeer[i];
		}
	}
	return NULL;
}

void CPeerList::DeleteAPeer(PCHAR pszUserName)
{
	for (int i = 0; i < m_nCurrentSize ; i++)
	{
		if (stricmp(m_pPeer[i].szUserName, pszUserName) == 0)
		{
			memcpy(&m_pPeer[i],&m_pPeer[i+1], sizeof(PEER_INFO)*(m_nCurrentSize-i-1));
			 m_nCurrentSize--;
		}
	}
}

void CPeerList::DeleteAllPeers()
{
	m_nCurrentSize = 0;
}
