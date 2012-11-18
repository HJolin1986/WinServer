
/**************************************************************************
	created:	2012/11/18	14:28
	filename: 	RuleDlg.CPP
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "stdafx.h"
#include "PhoenixFW.h"
#include "RuleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

RULE_ITEM	CRuleDlg::m_RuleItem;
BOOL		CRuleDlg::m_bAppQuery = FALSE;
CString		CRuleDlg::m_sPathName;


CRuleDlg::CRuleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRuleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRuleDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRuleDlg)
	DDX_Control(pDX, IDC_TYPE, m_ComboType);
	DDX_Control(pDX, IDC_RULE_TITLE, m_RuleTitle);
	DDX_Control(pDX, IDC_PORT, m_EditPort);
	DDX_Control(pDX, IDC_MEMO, m_EditMemo);
	DDX_Control(pDX, IDC_DIRECTION, m_ComboDirection);
	DDX_Control(pDX, IDC_ACTION, m_ComboAction);
	DDX_Control(pDX, IDC__APPLICATION, m_ComboApp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRuleDlg, CDialog)
	//{{AFX_MSG_MAP(CRuleDlg)
	ON_CBN_SELCHANGE(IDC_TYPE, OnSelchangeType)
	ON_CBN_SELCHANGE(IDC__APPLICATION, OnSelchangeApplication)
	ON_BN_CLICKED(IDC_BROWSER, OnAppBrowser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRuleDlg message handlers

BOOL CRuleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ComboAction.InsertString(0, L"放行");
	m_ComboAction.InsertString(1, L"拒绝");

	m_ComboDirection.InsertString(0, L"进");
	m_ComboDirection.InsertString(1, L"出");
	m_ComboDirection.InsertString(2, L"双向");

	m_ComboType.InsertString(0, L"全部");
	m_ComboType.InsertString(1, L"TCP");
	m_ComboType.InsertString(2, L"UDP");
	m_ComboType.InsertString(3, L"FTP");
	m_ComboType.InsertString(4, L"TELNET");
	m_ComboType.InsertString(5, L"HTTP");
	m_ComboType.InsertString(6, L"NNTP");
	m_ComboType.InsertString(7, L"POP3");
	m_ComboType.InsertString(8, L"SMTP");

	// app name
	for(int i=0; i< (int)g_RuleFile.m_header.ulLspRuleCount; i++)
	{
		if(m_ComboApp.FindString(0, g_RuleFile.m_pLspRules[i].szApplication) == CB_ERR)
			m_ComboApp.AddString(g_RuleFile.m_pLspRules[i].szApplication);
	}

	// set app name and initial state
	if(m_bAppQuery)
	{
		CString s;
		s.Format(L"%s 要访问网络，放行吗？", m_sPathName); 
		m_RuleTitle.SetWindowText(s);
		m_ComboApp.SetWindowText(m_sPathName);
	}
	else
	{
		if(_tcscmp(m_RuleItem.szApplication, L"") != 0)
		{
			m_RuleTitle.SetWindowText(m_RuleItem.szApplication);
			m_ComboApp.SetWindowText(m_RuleItem.szApplication);
		}
	}
	// if query from DLL, disable browser and combo box
	GetDlgItem(IDC_BROWSER)->EnableWindow(!m_bAppQuery);
	m_ComboApp.EnableWindow(!m_bAppQuery);

	// initial combo box
	m_ComboAction.SetCurSel(m_RuleItem.ucAction);
	m_ComboDirection.SetCurSel(m_RuleItem.ucDirection);
	m_ComboType.SetCurSel(m_RuleItem.ucServiceType);

	OnSelchangeType() ;
	// set port
	CString s;
	s.Format(L"%d", m_RuleItem.usServicePort);
	m_EditPort.SetLimitText(5);
	m_EditPort.SetWindowText(s);

	// set remark
	m_EditMemo.SetLimitText(50);
	m_EditMemo.SetWindowText(m_RuleItem.sDemo);

	// let the window front of ohers if popup from DLL
	if(m_bAppQuery)
	{
		ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW); 
		::SetWindowPos(m_hWnd,HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRuleDlg::OnSelchangeType()	// combo change event callback
{
	int nIndex = m_ComboType.GetCurSel();

	//--------------------------------------------------
	// port and check service type
	BOOL bEnable = TRUE;
	USHORT usPort = RULE_SERVICE_PORT_ALL;
	switch(nIndex)
	{
	case RULE_SERVICE_TYPE_ALL:
		bEnable = FALSE;
		break;
	case RULE_SERVICE_TYPE_TCP:
		break;
	case RULE_SERVICE_TYPE_UDP:
		break;
	case RULE_SERVICE_TYPE_FTP:
		usPort = RULE_SERVICE_PORT_FTP;
		bEnable = FALSE;
		break;
	case RULE_SERVICE_TYPE_TELNET:
		usPort = RULE_SERVICE_PORT_TELNET;
		bEnable = FALSE;
		break;
	case RULE_SERVICE_TYPE_HTTP:
		usPort = RULE_SERVICE_PORT_HTTP;
		bEnable = FALSE;
		break;
	case RULE_SERVICE_TYPE_NNTP:
		usPort = RULE_SERVICE_PORT_NNTP;
		bEnable = FALSE;
		break;
	case RULE_SERVICE_TYPE_POP3:
		usPort = RULE_SERVICE_PORT_POP3;
		bEnable = FALSE;
		break;
	}
	CString s;
	s.Format(L"%d", usPort);
	m_EditPort.SetWindowText(s);
	m_EditPort.EnableWindow(bEnable);
}

void CRuleDlg::OnSelchangeApplication() 
{
	CString s;
	m_ComboApp.GetLBText(m_ComboApp.GetCurSel(), s);
	m_RuleTitle.SetWindowText(s);
}

void CRuleDlg::OnAppBrowser() 
{
	TCHAR szFilter[] = L"(*.exe)|*.exe||";
	// pop up file browser window
	CFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,szFilter);
	if(dlg.DoModal() == IDOK)
	{	
		m_ComboApp.SetWindowText(dlg.GetPathName());
	}
}

void CRuleDlg::OnOK() 
{	
	CString s;

	// get port
	m_EditPort.GetWindowText(s);
	int nPort = _ttoi(s);
	if(nPort > 65535 || nPort < 0)
	{
		MessageBox(L"端口值无效，有效范围为 0 - 65535，请重新输入。");
		m_EditPort.SetFocus();
		return;
	}

	// get app name
	CString sApp;
	m_ComboApp.GetWindowText(sApp);
	if(sApp.IsEmpty() || ::GetFileAttributes(sApp) == -1)
	{
		MessageBox(L"应用程序不存在，请检查路径和名称。");
		m_ComboApp.SetFocus();
		return;
	}

	// get remark
	CString sMemo;
	m_EditMemo.GetWindowText(sMemo);

	// if user doesn't change any value, cancel it;
	if(sApp == m_RuleItem.szApplication && 
		m_RuleItem.usServicePort == nPort &&
		m_RuleItem.ucAction == m_ComboAction.GetCurSel() &&
		m_RuleItem.ucDirection == m_ComboDirection.GetCurSel() &&
		m_RuleItem.ucServiceType == m_ComboType.GetCurSel() &&
		m_RuleItem.sDemo == sMemo)
	{
		CDialog::OnCancel();
		return;
	}

	// return result to user
	_tcscpy(m_RuleItem.szApplication, sApp);
	m_RuleItem.usServicePort = nPort;
	m_RuleItem.ucAction = m_ComboAction.GetCurSel();
	m_RuleItem.ucDirection = m_ComboDirection.GetCurSel();
	m_RuleItem.ucServiceType = m_ComboType.GetCurSel();
	if(!sMemo.IsEmpty())
	{
		_tcscpy(m_RuleItem.sDemo, sMemo);
	}
	CDialog::OnOK();
}