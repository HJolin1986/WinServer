#ifndef AFX_KERRULEDLG_H__EC299161_1FC0_448A_91CD_64CB2FCBDBD6__INCLUDED_
#define AFX_KERRULEDLG_H__EC299161_1FC0_448A_91CD_64CB2FCBDBD6__INCLUDED_

/*
 *	_MSC_VER means 'Microsoft compiler version'
 *	MS VC++ 10.0 _MSC_VER = 1600	VS2010
 *	MS VC++ 09.0 _MSC_VER = 1500	VS2008
 *	MS VC++ 08.0 _MSC_VER = 1400	VS2005
 *	MS VC++ 07.1 _MSC_VER = 1310	VS2003
 *	MS VC++ 07.0 _MSC_VER = 1300	VS2002
 *	MS VC++ 06.0 _MSC_VER = 1200	VC++6.0
 *	MS VC++ 05.0 _MSC_VER = 1100	VC++5.0
 */
#if _MSC_VER>100
#pragma once
#endif

/**************************************************************************
	created:	2012/11/18	10:59
	filename: 	KerRuleDlg.H
	file path:	f:\GitHub\WinSock\PhoenixFW
	author:		Dailiming, en_name: Dicky

	purpose:		
**************************************************************************/
#include "stdafx.h"
#include "Resource.h"
#include <afxwin.h>

class CKerRuleDlg : public CDialog
{
public:
	static PassthruFilter m_RuleItem;			// add filter

public:
	CKerRuleDlg(CWnd * pPatent = NULL);
	~CKerRuleDlg(void);

	//Dialog Data
	//({AFX_DATA(CKerRuleDlg)
	enum {IDD = IDD_KERRULE };
	CEdit				m_SourcePort;
	CIPAddressCtrl		m_SourceMask;
	CIPAddressCtrl		m_SourceIP;
	CComboBox			m_RuleProtocol;
	CComboBox			m_RuleAction;
	CEdit				m_DestPort;
	CIPAddressCtrl		m_DestMask;
	CIPAddressCtrl		m_DestIP;
	//}) AFX_DATA
	// Overrides
	// ClassWizard generated wirtual function overrides
	// ({AFX_Virtual(CKerRuleDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);		//	DDX/DDV support
	//}) AFX_VIRTUAL
	// implementation
protected:
	// Generated message map functions
	//({AFX_MSG(CkerRuleDlg
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//})AFX_MSG
	DECLARE_MESSAGE_MAP()
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KERRULEDLG_H__EC299161_1FC0_448A_91CD_64CB2FCBDBD6__INCLUDED_)

