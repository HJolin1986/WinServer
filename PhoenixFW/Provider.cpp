
/**************************************************************************
	created:	2012/11/18	13:58
	filename: 	Provider.CPP
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "stdafx.h"

#include <Ws2spi.h>
#include <Sporder.h>


// install LSP hard encode, it will be used when uninstall
GUID  ProviderGuid = {0xd3c21122, 0x85e1, 0x48f3, 
{0x9a,0xb6,0x23,0xd9,0x0c,0x73,0x07,0xef}};



LPWSAPROTOCOL_INFOW GetProvider(LPINT lpnTotalProtocols)
{
	DWORD dwSize = 0;
	int nError;
	LPWSAPROTOCOL_INFOW pProtoInfo = NULL;

	// get needful
	if(::WSCEnumProtocols(NULL, pProtoInfo, &dwSize, &nError) == SOCKET_ERROR)
	{
		if(nError != WSAENOBUFS)
			return NULL;
	}

	pProtoInfo = (LPWSAPROTOCOL_INFOW)::GlobalAlloc(GPTR, dwSize);
	*lpnTotalProtocols = ::WSCEnumProtocols(NULL, pProtoInfo, &dwSize, &nError);
	return pProtoInfo;
}

void FreeProvider(LPWSAPROTOCOL_INFOW pProtoInfo)
{
	::GlobalFree(pProtoInfo);
}



BOOL InstallProvider(WCHAR *pwszPathName)
{
	int i = 0;
	WCHAR wszLSPName[] = L"PhoenixLSP";
	LPWSAPROTOCOL_INFOW pProtoInfo;
	int nProtocols;
	WSAPROTOCOL_INFOW OriginalProtocolInfo[3];
	DWORD			 dwOrigCatalogId[3];
	int nArrayCount = 0;

	DWORD dwLayeredCatalogId;		// layered protocol catalog ID

	int nError;

	//--------------------------------------------------
	// find our sublayer protocol and store the infomation to the array
	// enumeration all service provider
	pProtoInfo = GetProvider(&nProtocols);
	BOOL bFindUdp = FALSE;
	BOOL bFindTcp = FALSE;
	BOOL bFindRaw = FALSE;
	for(i=0; i<nProtocols; i++)
	{
		if(pProtoInfo[i].iAddressFamily == AF_INET)
		{
			if(!bFindUdp && pProtoInfo[i].iProtocol == IPPROTO_UDP)
			{
				memcpy(&OriginalProtocolInfo[nArrayCount], &pProtoInfo[i], sizeof(WSAPROTOCOL_INFOW));
				OriginalProtocolInfo[nArrayCount].dwServiceFlags1 = 
					OriginalProtocolInfo[nArrayCount].dwServiceFlags1 & (~XP1_IFS_HANDLES); 

				dwOrigCatalogId[nArrayCount++] = pProtoInfo[i].dwCatalogEntryId;

				bFindUdp = TRUE;
			}

			if(!bFindTcp && pProtoInfo[i].iProtocol == IPPROTO_TCP)
			{
				memcpy(&OriginalProtocolInfo[nArrayCount], &pProtoInfo[i], sizeof(WSAPROTOCOL_INFOW));
				OriginalProtocolInfo[nArrayCount].dwServiceFlags1 = 
					OriginalProtocolInfo[nArrayCount].dwServiceFlags1 & (~XP1_IFS_HANDLES); 

				dwOrigCatalogId[nArrayCount++] = pProtoInfo[i].dwCatalogEntryId;

				bFindTcp = TRUE;
			} 
			if(!bFindRaw && pProtoInfo[i].iProtocol == IPPROTO_IP)
			{
				memcpy(&OriginalProtocolInfo[nArrayCount], &pProtoInfo[i], sizeof(WSAPROTOCOL_INFOW));
				OriginalProtocolInfo[nArrayCount].dwServiceFlags1 = 
					OriginalProtocolInfo[nArrayCount].dwServiceFlags1 & (~XP1_IFS_HANDLES); 

				dwOrigCatalogId[nArrayCount++] = pProtoInfo[i].dwCatalogEntryId;

				bFindRaw = TRUE;
			}
		}
	}  

	//--------------------------------------------------
	// install our layered protocol and get a dwLayeredCatalogId
	// can copy any a layered protocol struct
	WSAPROTOCOL_INFOW LayeredProtocolInfo;
	memcpy(&LayeredProtocolInfo, &OriginalProtocolInfo[0], sizeof(WSAPROTOCOL_INFOW));
	// change protocol name , type and set PFL_HIDDEN
	wcscpy_s(LayeredProtocolInfo.szProtocol, WSAPROTOCOL_LEN+1, wszLSPName);
	LayeredProtocolInfo.ProtocolChain.ChainLen = LAYERED_PROTOCOL; // 0;
	LayeredProtocolInfo.dwProviderFlags |= PFL_HIDDEN;
	// install
	if(::WSCInstallProvider(&ProviderGuid, 
		pwszPathName, &LayeredProtocolInfo, 1, &nError) == SOCKET_ERROR)
	{
		return FALSE;
	}
	// reenumeration protocol to get catalog ID
	FreeProvider(pProtoInfo);
	pProtoInfo = GetProvider(&nProtocols);
	for(i=0; i<nProtocols; i++)
	{
		if(memcmp(&pProtoInfo[i].ProviderId, &ProviderGuid, sizeof(ProviderGuid)) == 0)
		{
			dwLayeredCatalogId = pProtoInfo[i].dwCatalogEntryId;
			break;
		}
	}

	// install protocol chain. change protocol name and service type
	WCHAR wszChainName[WSAPROTOCOL_LEN + 1];
	for(i=0; i<nArrayCount; i++)
	{
		swprintf_s(wszChainName, sizeof(wszChainName), L"%ws over %ws", wszLSPName, OriginalProtocolInfo[i].szProtocol);
		wcscpy_s(OriginalProtocolInfo[i].szProtocol, WSAPROTOCOL_LEN+1, wszChainName);
		if(OriginalProtocolInfo[i].ProtocolChain.ChainLen == 1)
		{
			OriginalProtocolInfo[i].ProtocolChain.ChainEntries[1] = dwOrigCatalogId[i];
		}
		else
		{
			for(int j = OriginalProtocolInfo[i].ProtocolChain.ChainLen; j>0; j--)
			{
				OriginalProtocolInfo[i].ProtocolChain.ChainEntries[j] 
				= OriginalProtocolInfo[i].ProtocolChain.ChainEntries[j-1];
			}
		}
		OriginalProtocolInfo[i].ProtocolChain.ChainLen ++;
		OriginalProtocolInfo[i].ProtocolChain.ChainEntries[0] = dwLayeredCatalogId;	
	}
	// get a GUID
	GUID ProviderChainGuid;
	if(::UuidCreate(&ProviderChainGuid) == RPC_S_OK)
	{
		if(::WSCInstallProvider(&ProviderChainGuid, 
			pwszPathName, OriginalProtocolInfo, nArrayCount, &nError) == SOCKET_ERROR)
		{
			return FALSE;	
		}
	}
	else
		return FALSE;

	// reorder winosck directory
	FreeProvider(pProtoInfo);
	pProtoInfo = GetProvider(&nProtocols);

	DWORD dwIds[20];
	int nIndex = 0;
	// add our protocol chain
	for(i=0; i<nProtocols; i++)
	{
		if((pProtoInfo[i].ProtocolChain.ChainLen > 1) &&
			(pProtoInfo[i].ProtocolChain.ChainEntries[0] == dwLayeredCatalogId))
			dwIds[nIndex++] = pProtoInfo[i].dwCatalogEntryId;
	}
	// add other protocols
	for(i=0; i<nProtocols; i++)
	{
		if((pProtoInfo[i].ProtocolChain.ChainLen <= 1) ||
			(pProtoInfo[i].ProtocolChain.ChainEntries[0] != dwLayeredCatalogId))
			dwIds[nIndex++] = pProtoInfo[i].dwCatalogEntryId;
	}
	// reorder winsock directory
	if(nError = ::WSCWriteProviderOrder(dwIds, nIndex) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	FreeProvider(pProtoInfo);

	return TRUE;
}

BOOL RemoveProvider()
{
	LPWSAPROTOCOL_INFOW pProtoInfo;
	int nProtocols;
	DWORD dwLayeredCatalogId;
	int i = 0;
	// get catalog ID by protocol
	pProtoInfo = GetProvider(&nProtocols);
	int nError;
	for(i=0; i<nProtocols; i++)
	{
		if(memcmp(&ProviderGuid, &pProtoInfo[i].ProviderId, sizeof(ProviderGuid)) == 0)
		{
			dwLayeredCatalogId = pProtoInfo[i].dwCatalogEntryId;
			break;
		}
	}

	if(i < nProtocols)
	{
		// remove protocol chain
		for(i=0; i<nProtocols; i++)
		{
			if((pProtoInfo[i].ProtocolChain.ChainLen > 1) &&
				(pProtoInfo[i].ProtocolChain.ChainEntries[0] == dwLayeredCatalogId))
			{
				::WSCDeinstallProvider(&pProtoInfo[i].ProviderId, &nError);
			}
		}
		// remove provider
		::WSCDeinstallProvider(&ProviderGuid, &nError);
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////
void RemoveAllLayeredEntries()
{
	BOOL    bLayer;
	int     ErrorCode,
		i;

	int TotalProtocols;

	LPWSAPROTOCOL_INFOW ProtocolInfo;

	while (1)
	{
		bLayer = FALSE;
		ProtocolInfo = GetProvider(&TotalProtocols);
		if (!ProtocolInfo)
		{
			printf("Unable to enumerate Winsock catalog!\n");
			return;
		}
		for(i=0; i < TotalProtocols ;i++)
		{
			if (ProtocolInfo[i].ProtocolChain.ChainLen != BASE_PROTOCOL)
			{
				bLayer = TRUE;
				printf("Removing '%S'\n", ProtocolInfo[i].szProtocol);
				if (WSCDeinstallProvider(&ProtocolInfo[i].ProviderId, &ErrorCode) == SOCKET_ERROR)
				{
					printf("Failed to remove [%s]: Error %d\n", ProtocolInfo[i].szProtocol, ErrorCode);
				}
				break;
			}
		}
		FreeProvider(ProtocolInfo);
		if (bLayer == FALSE)
		{
			break;
		}

	}
}