
/**************************************************************************
	created:	2012/11/18	14:48
	filename: 	stdafx.CPP
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:	stdafx.cpp : source file that includes just the standard includes
				PhoenixFW.pch will be the pre-compiled header
				stdafx.obj will contain the pre-compiled type information
**************************************************************************/

#include "stdafx.h"

CString GetFilePath(TCHAR *sFilename) 
{
	TCHAR sDrive[_MAX_DRIVE];
	TCHAR sDir[_MAX_DIR];
	TCHAR sFname[_MAX_FNAME];
	TCHAR sExt[_MAX_EXT];

	_tsplitpath(sFilename, sDrive, sDir, sFname, sExt);

	CString rVal(CString(sDrive) + CString(sDir));
	int nLen = rVal.GetLength();

	if (rVal.GetAt(nLen-1) != _T('\\'))
		rVal += _T("\\");

	return rVal;
}  

CString GetFileName(TCHAR *sFilename) 
{
	TCHAR sDrive[_MAX_DRIVE];
	TCHAR sDir[_MAX_DIR];
	TCHAR sFname[_MAX_FNAME];
	TCHAR sExt[_MAX_EXT];

	_tsplitpath(sFilename, sDrive, sDir, sFname, sExt);

	CString rVal;
	rVal.Format(_T("%s%s"), sFname, sExt);

	return rVal;
}  