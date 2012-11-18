
/**************************************************************************
	created:	2012/11/15	10:56	
	filename: 	Meeting.CPP
	file path:	e:\Documents\Visual Studio 2008\Projects\WinSock\MulticastingMeeting
	author:		Dailiming, en_name: Dicky

	purpose:	
**************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "MulticastingTalk.h"

#include <windows.h>
#include <WinDef.h>

MulticastingTalk *g_pTalk;

#ifdef A2CW
	#undef A2CW
	#undef A2W
	#define A2CW
	#define A2W
#endif // e


BOOL __stdcall DlgProc(HWND, UINT, WPARAM, LPARAM);
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	int nResult = ::DialogBoxParam(
		hInstance,				// instance 
		(LPCTSTR)IDD_MAIN,		// resource id
		NULL,					// parent hand
		DlgProc,				// msg handle function
		NULL); 					// dialog initial value from WM_INITDIALOG's lParam

	return 0;
}


void HandleGroupMsg(HWND hDlg, GT_HDR *pHeader)
{
	USES_CONVERSION;
	switch(pHeader->gt_type)
	{
	case MT_JION:		
		{
			// display join msg to user
			char szText[56];
			sprintf_s((szText), sizeof(szText), (" 用户：《%s》加入！"), pHeader->szUser);
			::SetWindowText(::GetDlgItem(hDlg, IDC_SYSMSG), A2W(szText));

			// add user to list
			int nCurSel = ::SendDlgItemMessage(hDlg, IDC_USERS, CB_GETCURSEL, 0, 0);
			int nIndex = ::SendDlgItemMessage(hDlg, IDC_USERS, CB_ADDSTRING, 0, (long)pHeader->szUser);
			::SendDlgItemMessage(hDlg, IDC_USERS, CB_SETITEMDATA, nIndex, (long)pHeader->dwAddr);
			if(nCurSel == -1)
				nCurSel = nIndex;
			::SendDlgItemMessage(hDlg, IDC_USERS, CB_SETCURSEL, nCurSel, 0);
		}
		break;

	case MT_LEAVE:		
		{
			// display leave msg to user
			char szText[56];
			sprintf_s((szText),sizeof(szText), (" 用户：《%s》离开！"), pHeader->szUser);
			::SetWindowText(::GetDlgItem(hDlg, IDC_SYSMSG), A2W(szText));

			int nCount = ::SendDlgItemMessage(hDlg, IDC_USERS, CB_GETCOUNT, 0, 0);
			for(int i=0; i<nCount; i++)
			{
				int nIndex = ::SendDlgItemMessage(hDlg, IDC_USERS, CB_FINDSTRING, i, (long)pHeader->szUser);
				if((DWORD)::SendDlgItemMessage(hDlg, IDC_USERS, CB_GETITEMDATA, nIndex, 0) == pHeader->dwAddr)
				{
					::SendDlgItemMessage(hDlg, IDC_USERS, CB_DELETESTRING, nIndex, 0);
					break;
				}
			}	
		}
		break;

	case MT_MESG:	
		{
			char *psz = pHeader->data();
			psz[pHeader->nDataLength] = '\0';
			char szText[1024];
			sprintf_s((szText), sizeof(szText), ("【%s 说】"), pHeader->szUser);
			strncat_s(szText, psz, 1024 - strlen(szText));
			::SendDlgItemMessage(hDlg, IDC_RECORD, LB_INSERTSTRING, 0, (long)szText);
		}
		break;
	}
}

BOOL __stdcall DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	USES_CONVERSION;
	switch(uMsg)
	{	
	case WM_INITDIALOG: 
		{
			g_pTalk = new MulticastingTalk(hDlg, ::inet_addr("234.5.6.7"));
			::CheckDlgButton(hDlg, IDC_SELGROUP, 1);
			::SendMessage(hDlg, WM_SETICON, ICON_SMALL, 
				(long)::LoadIcon(::GetModuleHandle(NULL), (LPCTSTR)IDI_MAIN));
		}
		break;

	case WM_GROUPTALK:
		{
			// handle msg from MulticastingTalk
			if(wParam != 0) 
				::MessageBox(hDlg, (LPCTSTR)lParam, _T("出错！"), 0);
			else
				HandleGroupMsg(hDlg, (GT_HDR*)lParam);
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_SEND:		// send button is pressed
			{
				// get send data
				char szText[1024];
				int nLen = ::GetWindowText(::GetDlgItem(hDlg, IDC_SENDMSG), A2W(szText), 1024);
				if(nLen == 0)
					break;

				// group or single user
				BOOL bToAll = ::IsDlgButtonChecked(hDlg, IDC_SELGROUP);
				DWORD dwAddr;
				if(bToAll)
				{
					dwAddr = 0;
				}
				else
				{
					int nIndex = ::SendDlgItemMessage(hDlg, IDC_USERS, CB_GETCURSEL, 0, 0);
					if(nIndex == -1)
					{
						::MessageBox(hDlg, _T("请选择一个用户！"), _T("GroupTalk"), 0);
						break;
					}
					// get user ip addr
					dwAddr = ::SendDlgItemMessage(hDlg, IDC_USERS, CB_GETITEMDATA, nIndex, 0);
				}
				//send data
				if(g_pTalk->SendText(szText, nLen, dwAddr) == nLen)
					::SetWindowText(::GetDlgItem(hDlg, IDC_SENDMSG), _T(""));
			}
			break;

		case IDC_CLEAR:		// clear button is pressed
			::SendDlgItemMessage(hDlg, IDC_RECORD, LB_RESETCONTENT, 0, 0);
			break;

		case IDCANCEL:		// close app
			{
				delete g_pTalk;
				::EndDialog (hDlg, IDCANCEL);
			}
			break;
		}
		break;
	}
	return 0;
}

