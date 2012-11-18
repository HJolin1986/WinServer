
/**************************************************************************
	created:	2012/11/18	13:21
	filename: 	MonitorPage.CPP
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "stdafx.h"
#include "PhoenixFW.h"
#include "MonitorPage.h"

#include "RulePage.h"

#include "PhoenixFWDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonitorPage property page

extern CPhoenixApp theApp;

IMPLEMENT_DYNCREATE(CMonitorPage, CPropertyPage)

CMonitorPage::CMonitorPage() : CPropertyPage(CMonitorPage::IDD)
{
	//{{AFX_DATA_INIT(CMonitorPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMonitorPage::~CMonitorPage()
{
}

void CMonitorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonitorPage)
	DDX_Control(pDX, IDC_TREEMONITOR, m_MonitorTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonitorPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMonitorPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonitorPage message handlers

BOOL CMonitorPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CMonitorPage::HandleNotifySession(SESSION *pSession, int nCode)
{
	// get the point of CMonitorPage, HandleNotifySession is a static
	CMonitorPage *pThis = &(((CMainDlg*)theApp.m_pMainWnd)->m_MonitorPage);

	if(nCode == CODE_CHANGE_SESSION)				// session property has changed
		pThis->AddASession(pSession);
	else if(nCode == CODE_DELETE_SESSION)			// delete session
		pThis->DeleteASession(pSession, FALSE);
	else if(nCode == CODE_APP_EXIT)					// a app exit, delete all its session
		pThis->DeleteASession(pSession, TRUE);
}

void CMonitorPage::AddASession(SESSION *pSession)
{
	TRACE(L" AddASession... ");

	// first find the app belong to if there is not, insert it to a new apps
	// add new session to the app
	HTREEITEM hAppItem = FindAppItem(pSession->szPathName);
	if(hAppItem == NULL) 
	{
		hAppItem = m_MonitorTree.InsertItem(pSession->szPathName);
	}

	// build display text according to the session
	CString sText = BuildSessionText(pSession);

	// check the session whether exist
	// if exist, set the text, if not exit, create new session
	HTREEITEM hSessionItem = FindSessionItem(hAppItem, pSession);
	if(hSessionItem != NULL)
	{
		m_MonitorTree.SetItemText(hSessionItem, sText);
	}
	else
	{	
		hSessionItem = m_MonitorTree.InsertItem(sText,hAppItem);
		m_MonitorTree.SetItemData(hSessionItem, pSession->s);
	}
}

HTREEITEM CMonitorPage::FindAppItem(TCHAR *pszPathName)
{
	// find app
	HTREEITEM hAppItem = m_MonitorTree.GetNextItem(TVI_ROOT, TVGN_CHILD);
	while(hAppItem != NULL)
	{
		if(m_MonitorTree.GetItemText(hAppItem).CompareNoCase(pszPathName) == 0)
		{
			return hAppItem;					// exist return handle
		}
		hAppItem = m_MonitorTree.GetNextItem(hAppItem, TVGN_NEXT);
	}
	return NULL;
}

HTREEITEM CMonitorPage::FindSessionItem(HTREEITEM hAppItem, SESSION *pSession)
{
	// find session
	HTREEITEM hSessionItem = m_MonitorTree.GetNextItem(hAppItem, TVGN_CHILD);
	while(hSessionItem != NULL)
	{
		if(pSession->s == m_MonitorTree.GetItemData(hSessionItem))
		{
			return hSessionItem;				// exit return it
		}
		hSessionItem = m_MonitorTree.GetNextItem(hSessionItem, TVGN_NEXT);
	}
	return NULL;
}

CString CMonitorPage::BuildSessionText(SESSION *pSession)
{
	CString sText;

	CString sServType, sLocal, sRemote, sDirection;

	// local ip addr
	BYTE *pByte = (BYTE *)&pSession->ulLocalIP;		// the ip endian is network endian
	sLocal.Format(L"%d.%d.%d.%d：%d", pByte[0], pByte[1], pByte[2], pByte[3], pSession->usLocalPort);

	// remote ip addr
	pByte = (BYTE *)&pSession->ulRemoteIP;
	sRemote.Format(L"%d.%d.%d.%d：%d", pByte[0], pByte[1], pByte[2], pByte[3], pSession->usRemotePort);

	sServType = L"其它";							// service type
	switch(pSession->nProtocol)
	{	
	case RULE_SERVICE_TYPE_ALL:
		sServType.Format(L"所有");
		break;
	case RULE_SERVICE_TYPE_TCP:
		sServType.Format(L"TCP");
		break;
	case RULE_SERVICE_TYPE_UDP:
		sServType.Format(L"UDP");
		break;
	case RULE_SERVICE_TYPE_FTP:
		sServType.Format(L"FTP");
		break;
	case RULE_SERVICE_TYPE_TELNET:
		sServType.Format(L"TELNET");
		break;
	case RULE_SERVICE_TYPE_HTTP:
		sServType.Format(L"HTTP");
		break;
	case RULE_SERVICE_TYPE_NNTP:
		sServType.Format(L"NNTP");
		break;
	case RULE_SERVICE_TYPE_POP3:
		sServType.Format(L"POP3");
		break;
	case RULE_SERVICE_TYPE_SMTP:
		sServType.Format(L"SMTP");
		break;
	}

	switch(pSession->ucDirection)						// direction
	{
	case RULE_DIRECTION_IN:
		sDirection = L"<——";
		break;
	case RULE_DIRECTION_OUT:
		sDirection = L"——>";
		break;
	case RULE_DIRECTION_IN_OUT:
		sDirection = L"<——>";
		break;
	default:
		sDirection = L"——";
	}

	sText.Format(L" %s 协议	    【%s】 %s 【%s】 ", sServType, sLocal, sDirection, sRemote);
	return sText;
}


void CMonitorPage::DeleteASession(SESSION *pSession, BOOL bAppExit)
{
	TRACE(L" DeleteASession... ");

	HTREEITEM hAppItem = FindAppItem(pSession->szPathName);
	if(hAppItem != NULL)
	{
		if(bAppExit)			// delete app, and its all session
		{
			m_MonitorTree.DeleteItem(hAppItem);
		}
		else					// only delete the special session assigned by pSession
		{
			HTREEITEM hSessionItem = FindSessionItem(hAppItem, pSession);
			if(hSessionItem != NULL)
			{	
				m_MonitorTree.DeleteItem(hSessionItem);
			}
			// if there is not session any more ,delete the app
			if(m_MonitorTree.GetNextItem(hAppItem, TVGN_CHILD) == NULL) 
				m_MonitorTree.DeleteItem(hAppItem);
		}
	}
}
