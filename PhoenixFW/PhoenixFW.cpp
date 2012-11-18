
/**************************************************************************
	created:	2012/11/18	13:37
	filename: 	PhoenixFW.CPP
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "stdafx.h"
#include "PhoenixFW.h"
#include "PhoenixFWDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhoenixApp

BEGIN_MESSAGE_MAP(CPhoenixApp, CWinApp)
	//{{AFX_MSG_MAP(CPhoenixApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhoenixApp construction

CPhoenixApp::CPhoenixApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CPhoenixApp theApp;

CPRuleFile g_RuleFile;				// manage rule file
CPIOControl *g_pIoControl = NULL;	// manage chared DLLs

BOOL CPhoenixApp::InitInstance()
{
	//--------------------------------------------------
	// only run one instance
	TCHAR szModule[] = L"PhoenixFW";
	m_hSemaphore = ::CreateSemaphore(NULL, 0, 1, szModule);
	if(::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(L" 已经有一个实例在运行！");
		return FALSE;
	}

	if(!g_RuleFile.LoadRules())						// load filter rule files
	{
		AfxMessageBox(L" 加载配置文件出错！");
		return FALSE;
	}

	g_pIoControl = new CPIOControl;					// create DLL IO object to load DLL
	ApplyFileData();								// set filter rules

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CMainDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	return FALSE;
}

int CPhoenixApp::ExitInstance() 
{	
	if(g_pIoControl != NULL)
	{
		g_pIoControl->SetWorkMode(PF_PASS_ALL);
		g_pIoControl->SetPhoenixInstance(NULL, L"");
		delete g_pIoControl;
	}
	IMClearRules();
	::CloseHandle(m_hSemaphore);
	return CWinApp::ExitInstance();
}

BOOL CPhoenixApp::SetAutoStart(BOOL bStart)
{
	// root key, subkey name, disposition key
	HKEY hRoot = HKEY_LOCAL_MACHINE;
	TCHAR *szSubKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	HKEY hKey;

	DWORD dwDisposition = REG_OPENED_EXISTING_KEY;	// if not exit, don't create
	LONG lRet = ::RegCreateKeyEx(hRoot, szSubKey, 0, NULL, 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if(lRet != ERROR_SUCCESS)
		return FALSE;

	if(bStart)
	{
		char szModule[MAX_PATH] ;
		::GetModuleFileNameA(NULL, szModule, MAX_PATH);		// get current file name
		// create a new key, set its value as file name
		lRet = ::RegSetValueExA(hKey, "PhoenixFW", 0, REG_SZ, (BYTE*)szModule, strlen(szModule));
	}
	else
	{
		lRet = ::RegDeleteValueA(hKey, "PhoenixFW");
	}

	::RegCloseKey(hKey);								// close subkey hand
	return lRet == ERROR_SUCCESS;
}

BOOL CPhoenixApp::ApplyFileData()
{

	g_pIoControl->SetWorkMode(g_RuleFile.m_header.ucLspWorkMode);	// set work mode
	g_pIoControl->SetRuleFile(&g_RuleFile.m_header, g_RuleFile.m_pLspRules); // set app rule

	IMClearRules();			// set kernel layer rules
	if(g_RuleFile.m_header.ucKerWorkMode == IM_START_FILTER)
	{
		if(!IMSetRules(g_RuleFile.m_pKerRules, g_RuleFile.m_header.ulKerRuleCount))
		{
			AfxMessageBox(L" 设置核心层规则出错！\n");
			return FALSE;
		}
	}
	return TRUE;
}