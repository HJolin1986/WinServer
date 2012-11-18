
/**************************************************************************
	created:	2012/11/18	14:38
	filename: 	RulePage.CPP
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "stdafx.h"
#include "PhoenixFW.h"
#include "RulePage.h"
#include "Ruledlg.h"
#include "PhoenixFWDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRulePage property page
extern CPhoenixApp theApp;


IMPLEMENT_DYNCREATE(CRulePage, CPropertyPage)

CRulePage::CRulePage() : CPropertyPage(CRulePage::IDD)
{
	//{{AFX_DATA_INIT(CRulePage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CRulePage::~CRulePage()
{
}

void CRulePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRulePage)
	DDX_Control(pDX, IDC_RULES, m_rules);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRulePage, CPropertyPage)
	//{{AFX_MSG_MAP(CRulePage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_NOTIFY(NM_CLICK, IDC_RULES, OnClickRules)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RULES, OnItemchangedRules)
	ON_NOTIFY(NM_DBLCLK, IDC_RULES, OnDblclkRules)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRulePage message handlers


BOOL CRulePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_rules.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_rules.InsertColumn(0, L"应用程序", LVCFMT_LEFT, sizeof(L"应用程序")*8, 0);
	m_rules.InsertColumn(1, L"动作", LVCFMT_LEFT, sizeof( L"动作")*8, 1);
	m_rules.InsertColumn(2, L"类型/端口", LVCFMT_LEFT, sizeof(L"类型/端口")*8, 2);
	m_rules.InsertColumn(3, L"应用程序路径", LVCFMT_LEFT, sizeof(L"应用程序路径")*12, 3);
	m_rules.InsertColumn(4, L"说明", LVCFMT_LEFT, sizeof(L"说明")*12, 4);

	UpdateList();

	GetDlgItem(IDC_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRulePage::UpdateList()
{
	m_rules.DeleteAllItems();

	for(int i=0; i<(int)g_RuleFile.m_header.ulLspRuleCount; i++)
		EditARule(&g_RuleFile.m_pLspRules[i]);
}

void CRulePage::EditARule(RULE_ITEM *pItem, int nEditIndex)
{
	// if nEditIndex > 0, set index to nEditIndex, or add a new item
	int nIndex = m_rules.GetItemCount();
	if(nEditIndex >= 0)
		nIndex = nEditIndex;
	else
		m_rules.InsertItem(nIndex, L"", 0);

	// new item's text
	CString sAction, sServType;

	sAction = (pItem->ucAction == 0) ? L"放行" : L"拒绝";

	switch(pItem->ucServiceType)
	{	
	case RULE_SERVICE_TYPE_ALL:
		sServType.Format(L"所有/%d", pItem->usServicePort);
		break;
	case RULE_SERVICE_TYPE_TCP:
		sServType.Format(L"TCP/%d", pItem->usServicePort);
		break;
	case RULE_SERVICE_TYPE_UDP:
		sServType.Format(L"UDP/%d", pItem->usServicePort);
		break;
	case RULE_SERVICE_TYPE_FTP:
		sServType.Format(L"FTP/%d", pItem->usServicePort);
		break;
	case RULE_SERVICE_TYPE_TELNET:
		sServType.Format(L"TELNET/%d", pItem->usServicePort);
		break;
	case RULE_SERVICE_TYPE_HTTP:
		sServType.Format(L"HTTP/%d", pItem->usServicePort);
		break;
	case RULE_SERVICE_TYPE_NNTP:
		sServType.Format(L"NNTP/%d", pItem->usServicePort);
		break;
	case RULE_SERVICE_TYPE_POP3:
		sServType.Format(L"POP3/%d", pItem->usServicePort);
		break;
	}

	// set the new item text
	m_rules.SetItemText(nIndex, 0, GetFileName(pItem->szApplication));
	m_rules.SetItemText(nIndex, 1, sAction);
	m_rules.SetItemText(nIndex, 2, sServType);
	m_rules.SetItemText(nIndex, 3, GetFilePath(pItem->szApplication));
	m_rules.SetItemText(nIndex, 4, pItem->sDemo);
}




int CRulePage::InitAddRule(LPCTSTR szQueryApp)
{
	if(g_RuleFile.m_header.ulLspRuleCount > MAX_RULE_COUNT)
	{
		AfxMessageBox(L" 超过规则的最大数目，不能够再添加");
		return -1;
	}

	// set a default rule
	RULE_ITEM tmpRule;

	_tcscpy(tmpRule.sDemo, L"");
	_tcscpy(tmpRule.szApplication, L"");
	tmpRule.ucAction = RULE_ACTION_PASS;
	tmpRule.ucDirection = RULE_DIRECTION_IN_OUT;
	tmpRule.ucServiceType = RULE_SERVICE_TYPE_ALL;
	tmpRule.usServicePort = RULE_SERVICE_PORT_ALL;

	// set passing param
	CRuleDlg::m_sPathName = szQueryApp;
	CRuleDlg::m_RuleItem = tmpRule;
	CRuleDlg::m_bAppQuery = (szQueryApp == NULL) ? 0 : 1;

	// set current page as activity
	if(CRuleDlg::m_bAppQuery)
		((CMainDlg*)theApp.m_pMainWnd)->m_sheet.SetActivePage(this);

	// pop up add rule window
	CRuleDlg dlg;
	if(dlg.DoModal() == IDCANCEL)
	{
		return -1;
	}

	// add the ruel to file
	if(!g_RuleFile.AddLspRules(&CRuleDlg::m_RuleItem, 1))
	{
		AfxMessageBox(L"添加 ACL 规则错误。");
		return -1;
	}
	// add the rule to the list
	EditARule(&CRuleDlg::m_RuleItem);

	return CRuleDlg::m_RuleItem.ucAction;
}

// static function
BOOL CRulePage::AddQueryRule(LPCTSTR pszQueryApp) 
{
	int nRet = ((CMainDlg*)theApp.m_pMainWnd)->m_RulePage.InitAddRule(pszQueryApp);
	if( nRet == -1 )
		return FALSE;

	// store rules to file
	g_RuleFile.SaveRules();

	// apply rules to DLL
	theApp.ApplyFileData();
	// disable apply buttion
	((CMainDlg*)theApp.m_pMainWnd)->GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);

	return nRet == RULE_ACTION_PASS;
}

// add button callback
void CRulePage::OnAdd()		
{
	if(InitAddRule() != 0)
		return;
	// enable apply button
	GetOwner()->GetOwner()->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


// edit button callback
void CRulePage::OnEdit()									
{
	if(m_nListIndex < 0)
		return;

	CRuleDlg::m_RuleItem = g_RuleFile.m_pLspRules[m_nListIndex];
	CRuleDlg::m_bAppQuery = FALSE;

	CRuleDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		g_RuleFile.m_pLspRules[m_nListIndex] = CRuleDlg::m_RuleItem;
		EditARule(&CRuleDlg::m_RuleItem, m_nListIndex);
		GetOwner()->GetOwner()->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
	}
}

// callback when double click the list
void CRulePage::OnDblclkRules(NMHDR* pNMHDR, LRESULT* pResult) 	
{
	NM_LISTVIEW* pNMList = (NM_LISTVIEW*)pNMHDR;
	if((m_nListIndex = pNMList->iItem) != -1)
	{
		OnEdit();
	}
	*pResult = 0;
}
// delete button callback
void CRulePage::OnDel()	
{	
	if(m_nListIndex < 0)
		return;

	g_RuleFile.DelLspRule(m_nListIndex);
	m_rules.DeleteItem(m_nListIndex);

	// enable apply button
	GetOwner()->GetOwner()->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);

	// disable delete and edit button 
	if(m_rules.GetItemCount() <= 0)
	{
		GetDlgItem(IDC_DEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);	
		return;
	}
	// or chose final one
	if(m_nListIndex == m_rules.GetItemCount())
		m_nListIndex--;
	m_rules.SetItemState(m_nListIndex, LVIS_SELECTED, LVIS_SELECTED);
}

// change rule callback
void CRulePage::OnItemchangedRules(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMList = (NM_LISTVIEW*)pNMHDR;
	// get index and disable del , edit
	if((m_nListIndex = pNMList->iItem) != -1)
	{
		GetDlgItem(IDC_DEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);
	}

	*pResult = 0;
}
// single click list callback
void CRulePage::OnClickRules(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMList = (NM_LISTVIEW*)pNMHDR;
	// get index and disable del, edit
	if((m_nListIndex = pNMList->iItem) == -1)
	{
		GetDlgItem(IDC_DEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
	}
	*pResult = 0;
}

