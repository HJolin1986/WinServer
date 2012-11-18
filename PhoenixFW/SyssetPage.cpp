
/**************************************************************************
	created:	2012/11/18	14:50
	filename: 	SyssetPage.CPP
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "stdafx.h"
#include "PhoenixFW.h"
#include "SyssetPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSyssetPage property page

IMPLEMENT_DYNCREATE(CSyssetPage, CPropertyPage)

CSyssetPage::CSyssetPage() : CPropertyPage(CSyssetPage::IDD)
{
	//{{AFX_DATA_INIT(CSyssetPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSyssetPage::~CSyssetPage()
{
}

void CSyssetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyssetPage)
	DDX_Control(pDX, IDC_AUTOSTART, m_AutoStart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSyssetPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSyssetPage)
	ON_BN_CLICKED(IDC_INSTALL, OnInstall)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_AUTOSTART, OnAutostart)
	ON_BN_CLICKED(IDC_PASS_ALL, OnPassAll)
	ON_BN_CLICKED(IDC_QUERY_ALL, OnQueryAll)
	ON_BN_CLICKED(IDC_DENY_ALL, OnDenyAll)
	ON_BN_CLICKED(IDC_KERPASS_ALL, OnKerpassAll)
	ON_BN_CLICKED(IDC_KERSTART_FILTER, OnKerstartFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyssetPage message handlers

BOOL CSyssetPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// set autostart
	if(g_RuleFile.m_header.bAutoStart)
		((CButton*)GetDlgItem(IDC_AUTOSTART))->SetCheck(1);

	// work mode control
	switch(g_RuleFile.m_header.ucLspWorkMode)
	{
	case PF_PASS_ALL:
		((CButton*)GetDlgItem(IDC_PASS_ALL))->SetCheck(1);
		break;
	case PF_QUERY_ALL:
		((CButton*)GetDlgItem(IDC_QUERY_ALL))->SetCheck(1);
		break;
	case PF_DENY_ALL:
		((CButton*)GetDlgItem(IDC_DENY_ALL))->SetCheck(1);
		break;
	}

	// kernel work mode control
	switch(g_RuleFile.m_header.ucKerWorkMode)
	{
	case IM_PASS_ALL:
		((CButton*)GetDlgItem(IDC_KERPASS_ALL))->SetCheck(1);
		break;
	case IM_START_FILTER:
		((CButton*)GetDlgItem(IDC_KERSTART_FILTER))->SetCheck(1);
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
// install callback
void CSyssetPage::OnInstall()
{
	TCHAR szPathName[256];
	TCHAR* p;

	//--------------------------------------------------
	// install LSP need full DLL path,CPIOControl load CPIOControl using full path
	// or the DLL CPIOControl loaded can't share memory with LSP DLL
	// 完整路径，否则CPIOControl类加载的DLL不能和作为LSP的DLL共享内存
	if(::GetFullPathName(PHOENIX_SERVICE_DLL_NAME, 256, szPathName, &p) != 0)
	{
		if(InstallProvider(szPathName))
		{
			MessageBox(L" 应用层过滤安装成功！"); 
			return;
		}
		else
		{
			if(InstallProvider(DLLPATH))
			{
				MessageBox(L" 应用层过滤安装成功！"); 
				return;
			}
		}
	}
	MessageBox(L" 应用层过滤安装失败！"); 
}

// uninstall callback
void CSyssetPage::OnRemove()
{	
	if(RemoveProvider())
		MessageBox(L" 应用层过滤卸载成功！");
	else
		MessageBox(L" 应用层过滤卸载失败！");
}


void CSyssetPage::OnAutostart()				// auto run the app
{
	BOOL bCheck = m_AutoStart.GetCheck();
	g_RuleFile.m_header.bAutoStart = bCheck;

	// enable apply
	GetOwner()->GetOwner()->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CSyssetPage::OnPassAll()				// pass all callback
{
	SetLspWorkMode(PF_PASS_ALL);
}

void CSyssetPage::OnQueryAll()				// query callback
{
	SetLspWorkMode(PF_QUERY_ALL);
}

void CSyssetPage::OnDenyAll()				// deny all callback
{
	SetLspWorkMode(PF_DENY_ALL);
}


void CSyssetPage::OnKerpassAll()			// pass all callback
{
	SetKerWorkMode(IM_PASS_ALL);	
}

void CSyssetPage::OnKerstartFilter()		// open filter callback
{	
	SetKerWorkMode(IM_START_FILTER);	
}


void CSyssetPage::SetKerWorkMode(int nWorkMode)
{
	g_RuleFile.m_header.ucKerWorkMode = nWorkMode;
	GetOwner()->GetOwner()->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CSyssetPage::SetLspWorkMode(int nWorkMode)
{
	g_RuleFile.m_header.ucLspWorkMode = nWorkMode;
	GetOwner()->GetOwner()->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}
