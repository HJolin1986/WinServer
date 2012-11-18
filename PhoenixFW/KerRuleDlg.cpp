
/**************************************************************************
	created:	2012/11/18	12:50
	filename: 	KerRuleDlg.CPP
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/

#include "StdAfx.h"
#include "../common/iocommon.h"
#include "KerRuleDlg.h"
#include "PhoenixFW.h"

#ifdef _DEBUG
#define  new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
PassthruFilter CKerRuleDlg::m_RuleItem;

CKerRuleDlg::CKerRuleDlg(CWnd* pParent/*=NULL*/)
	: CDialog(CKerRuleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKerRuleDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CKerRuleDlg::~CKerRuleDlg(void)
{
}

void CKerRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKerRuleDlg)
	DDX_Control(pDX, IDC_SOURCE_PORT, m_SourcePort);
	DDX_Control(pDX, IDC_SOURCE_MASK, m_SourceMask);
	DDX_Control(pDX, IDC_SOURCE_IP, m_SourceIP);
	DDX_Control(pDX, IDC_RULE_PROTOCOL, m_RuleProtocol);
	DDX_Control(pDX, IDC_RULE_ACTION, m_RuleAction);
	DDX_Control(pDX, IDC_DEST_PORT, m_DestPort);
	DDX_Control(pDX, IDC_DEST_MASK, m_DestMask);
	DDX_Control(pDX, IDC_DEST_IP, m_DestIP);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKerRuleDlg, CDialog)
	//{{AFX_MSG_MAP(CKerRuleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CKerRuleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//--------------------------------------------------
	// initial action Combo Box
	m_RuleAction.SetItemData(m_RuleAction.AddString(L"放行"), 0);
	m_RuleAction.SetItemData(m_RuleAction.AddString(L"拒绝"), 1);
	m_RuleAction.SetCurSel(0);

	//--------------------------------------------------
	// initial protocol Combo Box
	m_RuleProtocol.SetItemData(m_RuleProtocol.AddString(L"全部"), 0);
	m_RuleProtocol.SetItemData(m_RuleProtocol.AddString(L"TCP"), IPPROTO_TCP);
	m_RuleProtocol.SetItemData(m_RuleProtocol.AddString(L"UDP"), IPPROTO_UDP);
	m_RuleProtocol.SetItemData(m_RuleProtocol.AddString(L"ICMP"), IPPROTO_ICMP);
	m_RuleProtocol.SetCurSel(0);

	//--------------------------------------------------
	// set action Combo Box
	m_RuleAction.SetCurSel(m_RuleItem.bDrop);

	// set protocol Combo Box
	for(int i=0; i<m_RuleProtocol.GetCount(); i++)
	{
		if(m_RuleProtocol.GetItemData(i) == m_RuleItem.protocol)
		{
			m_RuleProtocol.SetCurSel(i);
			break;
		}
	}

	m_SourceIP.SetAddress(m_RuleItem.sourceIP);				// set source IP
	m_SourceMask.SetAddress(m_RuleItem.sourceMask);			// set source MASK

	m_DestIP.SetAddress(m_RuleItem.destinationIP);			// set destination's IP
	m_DestMask.SetAddress(m_RuleItem.destinationMask);		// set destination's MASK

	CString tmpStr;
	tmpStr.Format(L"%u", m_RuleItem.sourcePort);
	m_SourcePort.SetWindowText(tmpStr);						// set source port

	tmpStr.Format(L"%u", m_RuleItem.destinationPort);		// set destination's port
	m_DestPort.SetWindowText(tmpStr);

	m_SourcePort.SetLimitText(5);
	m_DestPort.SetLimitText(5);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CKerRuleDlg::OnOK()
{
	PassthruFilter tmpRule = { 0 };
	CString strSourcePort, strDestPort;

	m_SourcePort.GetWindowText(strSourcePort);			//get SourcePort
	int nPort = _ttoi(strSourcePort);
	if(nPort > 65535 || nPort < 0)
	{
		AfxMessageBox(L"源端口号无效，有效范围为 0 - 65535，请重新输入。");
		m_SourcePort.SetFocus();
		return ;
	}
	tmpRule.sourcePort = nPort;

	m_DestPort.GetWindowText(strDestPort);				// get DestPort
	nPort = _ttoi(strDestPort);
	if(nPort > 65535 || nPort < 0)
	{
		AfxMessageBox(L"目的端口号无效，有效范围为 0 - 65535，请重新输入。");
		m_DestPort.SetFocus();
		return ;
	}
	tmpRule.destinationPort = nPort;
	// action
	tmpRule.bDrop = (BOOLEAN)m_RuleAction.GetItemData(m_RuleAction.GetCurSel());
	// protocol
	tmpRule.protocol = (USHORT)m_RuleProtocol.GetItemData(m_RuleProtocol.GetCurSel());
	// return by host endian
	m_SourceIP.GetAddress(tmpRule.sourceIP);
	m_SourceMask.GetAddress(tmpRule.sourceMask);

	m_DestIP.GetAddress(tmpRule.destinationIP);
	m_DestMask.GetAddress(tmpRule.destinationMask);

	m_RuleItem = tmpRule;
	CDialog::OnOK();
}