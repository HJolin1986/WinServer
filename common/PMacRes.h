#pragma once

/**************************************************************************
	created:	2012/11/16	14:23	
	filename: 	PMacRes.H
	file path:	e:\Documents\Visual Studio 2008\Projects\WinSock\common
	author:		DAILM, en_name: Dicky

	purpose:	
**************************************************************************/

//-------------------------------------------
// LSP send msg
#define PM_SESSION_NOTIFY		WM_USER + 200
#define PM_QUERY_ACL_NOTIFY		WM_USER	+ 201
#define CODE_CHANGE_SESSION		0
#define CODE_DELETE_SESSION		1
#define CODE_APP_EXIT			2

//-------------------------------------------
// operation to special app
#define PF_PASS					0
#define PF_DENY					1
#define PF_QUERY				2
#define PF_FILTER				3
#define PF_UNKNOWN				4

//-------------------------------------------
// LSP and IM work pattern
#define PF_PASS_ALL				0
#define PF_QUERY_ALL			1
#define PF_DENY_ALL				2
#define IM_PASS_ALL				0
#define IM_START_FILTER			1

//-------------------------------------------
// app can use max amount constant
#define MAX_RULE_COUNT			100
#define MAX_QUERY_SESSION		20
#define MAX_SESSION_BUFFER		100
#define INIT_SESSION_BUFFER		50

//-------------------------------------------
// rules file use
#define PHOENIX_PRODUCT_ID			0xFF01
	
#define RULE_HEADER_MAJOR			2
#define RULE_HEADER_MINOR			1
	
#define RULE_HEADER_VERSION			2

#define RULE_HEADER_WEB_URL			_T("http://www.yanping.net/\0")
#define RULE_HEADER_EMAIL			_T("whitegest@sohu.com\0")

#define DLLPATH _T("F:/GitHub/WinSock/Release/PhoenixLSP.dll")
#define PHOENIX_SERVICE_DLL_NAME	_T("PhoenixLSP.dll")
#define RULE_FILE_NAME				_T("prule.fnk")
#define RULE_HEADER_SIGNATURE		_T("PHOENIX/INC\0")

//-------------------------------------------
// DLL IO module control
#define IO_CONTROL_SET_WORK_MODE			0
#define IO_CONTROL_GET_WORK_MODE			1
#define IO_CONTROL_SET_PHOENIX_INSTANCE		2
#define IO_CONTROL_GET_SESSION				3
#define IO_CONTROL_GET_QUERY_SESSION		4
#define IO_CONTROL_SET_QUERY_SESSION		5
#define IO_CONTROL_SET_RULE_FILE			6

//-------------------------------------------
// filter regulation macro
// action + dirction + service type + service port
#define RULE_ACTION_PASS					0
#define RULE_ACTION_DENY					1
#define RULE_ACTION_NOT_SET					-1

#define RULE_DIRECTION_IN					0
#define RULE_DIRECTION_OUT					1
#define RULE_DIRECTION_IN_OUT				2
#define RULE_DIRECTION_NOT_SET				-1

#define RULE_SERVICE_TYPE_ALL				0	// 所有类型
#define RULE_SERVICE_TYPE_TCP				1	// TCP协议
#define RULE_SERVICE_TYPE_UDP				2	// UDP协议
#define RULE_SERVICE_TYPE_FTP				3	// FTP协议
#define RULE_SERVICE_TYPE_TELNET			4
#define RULE_SERVICE_TYPE_HTTP				5
#define RULE_SERVICE_TYPE_NNTP				6
#define RULE_SERVICE_TYPE_POP3				7
#define RULE_SERVICE_TYPE_SMTP				8

#define RULE_SERVICE_PORT_ALL				0
#define RULE_SERVICE_PORT_FTP				21
#define RULE_SERVICE_PORT_TELNET			23
#define RULE_SERVICE_PORT_NNTP				119
#define RULE_SERVICE_PORT_POP3				110
#define RULE_SERVICE_PORT_SMTP				25
#define RULE_SERVICE_PORT_HTTP				80