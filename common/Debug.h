#pragma once 

/**************************************************************************
	created:	2012/11/16	16:02	
	filename: 	Debug.H
	file path:	e:\Documents\Visual Studio 2008\Projects\WinSock\common
	author:		DAILM, en_name: Dicky

	purpose:	
**************************************************************************/


#ifdef _DEBUG

	#define ODS(szOut)					\
	{									\
		OutputDebugString(szOut);		\
	}

	#define ODS1(szOut, var)			\
	{									\
		TCHAR sz[1024];				    \
		swprintf_s(sz, szOut, var);		\
		OutputDebugString(sz);			\
	}

#else

	#define ODS(szOut)
	#define ODS1(szOut, var)	

#endif	// _DEBUG
