
/**************************************************************************
	created:	2012/11/18	14:18
	filename: 	ptutils.CPP
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "stdafx.h"

#include <winioctl.h>
#include <ntddndis.h>
#include <stdio.h>
#include <tchar.h>

#include "../common/iocommon.h"
#include "ptutils.h"


HANDLE PtOpenControlDevice()
{
	// open driver control handle
	HANDLE hFile = ::CreateFile(
		_T("\\\\.\\PassThru"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	return hFile;
}

HANDLE PtOpenAdapter(PWSTR pszAdapterName)
{
	// open control device handle
	HANDLE hAdapter = PtOpenControlDevice();
	if(hAdapter == INVALID_HANDLE_VALUE)
		return INVALID_HANDLE_VALUE;


	// assume device name len
	int nBufferLength = wcslen((PWSTR)pszAdapterName) * sizeof(WCHAR);

	// send IOCTL_PTUSERIO_OPEN_ADAPTER, open adapter context
	DWORD dwBytesReturn;
	BOOL bOK = ::DeviceIoControl(hAdapter, IOCTL_PTUSERIO_OPEN_ADAPTER, 
		pszAdapterName, nBufferLength, NULL, 0, &dwBytesReturn, NULL);
	// check result
	if(!bOK)
	{
		::CloseHandle(hAdapter);
		return INVALID_HANDLE_VALUE;
	}
	return hAdapter;
}

BOOL PtAdapterRequest(HANDLE hAdapter, PPTUSERIO_OID_DATA pOidData, BOOL bQuery)
{
	if(hAdapter == INVALID_HANDLE_VALUE)
		return FALSE;
	// send IOCTL
	DWORD dw;
	int bRet = ::DeviceIoControl(
		hAdapter, bQuery ? IOCTL_PTUSERIO_QUERY_OID : IOCTL_PTUSERIO_SET_OID,
		pOidData, sizeof(PTUSERIO_OID_DATA) -1 + pOidData->Length,
		pOidData, sizeof(PTUSERIO_OID_DATA) -1 + pOidData->Length, &dw, NULL);

	return bRet;
}

// check net active state
BOOL PtQueryStatistics(HANDLE hAdapter, PPassthruStatistics pStats)
{
	ULONG nStatsLen = sizeof(PassthruStatistics);
	BOOL bRet = ::DeviceIoControl(hAdapter, 
		IOCTL_PTUSERIO_QUERY_STATISTICS, NULL, 0, pStats, nStatsLen, &nStatsLen, NULL);

	return bRet;
}

// reset statistic number
BOOL PtResetStatistics(HANDLE hAdapter)
{
	DWORD dwBytes;
	BOOL bRet = ::DeviceIoControl(hAdapter, 
		IOCTL_PTUSERIO_RESET_STATISTICS, NULL, 0, NULL, 0, &dwBytes, NULL);
	return bRet;
}

// add a filter
BOOL PtAddFilter(HANDLE hAdapter, PPassthruFilter pFilter)
{
	ULONG nFilterLen = sizeof(PassthruFilter);
	BOOL bRet = ::DeviceIoControl(hAdapter, IOCTL_PTUSERIO_ADD_FILTER, 
		pFilter, nFilterLen, NULL, 0, &nFilterLen, NULL);
	return bRet;
}

// clear filter
BOOL PtClearFilter(HANDLE hAdapter)
{
	DWORD dwBytes;
	BOOL bRet = ::DeviceIoControl(hAdapter, 
		IOCTL_PTUSERIO_CLEAR_FILTER, NULL, 0, NULL, 0, &dwBytes, NULL);
	return bRet;
}

BOOL CIMAdapters::EnumAdapters(HANDLE hControlDevice)
{
	DWORD dwBufferLength = sizeof(m_buffer);
	BOOL bRet = ::DeviceIoControl(hControlDevice, IOCTL_PTUSERIO_ENUMERATE, 
		NULL, 0, m_buffer, dwBufferLength, &dwBufferLength, NULL);
	if(!bRet)
		return FALSE;

	// store adapter number
	m_nAdapters = (ULONG)((ULONG*)m_buffer)[0];

	// get adapter name and link name, and point to device name
	WCHAR *pwsz = (WCHAR *)((ULONG *)m_buffer + 1);
	int i = 0;
	m_pwszVirtualName[i] = pwsz;
	while(*(pwsz++) != NULL)
	{
		while(*(pwsz++) != NULL)
		{ ; }

		m_pwszAdapterName[i] = pwsz;

		while(*(pwsz++) != NULL)
		{ ; }

		if(++i >= MAX_ADAPTERS)
			break;	

		m_pwszVirtualName[i] = pwsz;
	}

	return TRUE;
}



// set IM driver filter rule
BOOL IMSetRules(PPassthruFilter pRules, int nRuleCount)
{
	BOOL bRet = TRUE;
	// open PassThruEx control object, enumeration sublayer bound
	HANDLE hControlDevice = PtOpenControlDevice();
	CIMAdapters adapters;
	if(!adapters.EnumAdapters(hControlDevice))
		return FALSE;

	// set filter to sublayer adapter
	HANDLE hAdapter;
	int i, j;
	for(i=0; i<adapters.m_nAdapters; i++)
	{
		// open adapter
		hAdapter = PtOpenAdapter(adapters.m_pwszAdapterName[i]);
		if(hAdapter != INVALID_HANDLE_VALUE)
		{
			for(j=0; j<nRuleCount; j++)
			{
				PassthruFilter rule = pRules[j];
				rule.sourcePort = htons(rule.sourcePort);				
				rule.sourceIP = htonl(rule.sourceIP);
				rule.sourceMask = htonl(rule.sourceMask);

				rule.destinationPort = htons(rule.destinationPort);
				rule.destinationIP = htonl(rule.destinationIP);
				rule.destinationMask = htonl(rule.destinationMask);
				if(!PtAddFilter(hAdapter, &rule))
				{
					bRet = FALSE;
					break;
				}
			}
			::CloseHandle(hAdapter);
		}
		else
		{
			bRet = FALSE;
			break;
		}
	}
	::CloseHandle(hControlDevice);
	return bRet;
}

// clear IM rules
BOOL IMClearRules()
{
	BOOL bRet = TRUE;

	HANDLE hControlDevice = PtOpenControlDevice();
	CIMAdapters adapters;
	if(!adapters.EnumAdapters(hControlDevice))
		return FALSE;

	// set filter to sublayer adapter
	HANDLE hAdapter;
	for(int i=0; i<adapters.m_nAdapters; i++)
	{
		hAdapter = PtOpenAdapter(adapters.m_pwszAdapterName[i]);
		if(hAdapter != INVALID_HANDLE_VALUE)
		{
			PtClearFilter(hAdapter);
			::CloseHandle(hAdapter);
		}
		else
		{
			bRet = FALSE;
			break;
		}
	}
	return bRet;
}

