// MulticastingMeeting.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../common/SockInit.h"

#define WM_GROUPTALK WM_USER+105
#define BUFFER_SIZE 4096
#define GROUP_PORT 4567

/*
 * uMsg: WM_GROUPTALK
 * wParam: error num, 0 is no error;
 * lParam: GT_HDR head point
 */
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

