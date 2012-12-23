#pragma once

#include "stdafx.h"
#include "CRstEvent.h"
#include "CConnEvent.h"

// This Class extracts only EmployeeId,FirstName and LastName
// from employees table
class CEmployeeRs : public CADORecordBinding
{
	BEGIN_ADO_BINDING(CEmployeeRs)
	// Column hiredate is the 1st field in the table.
	ADO_VARIABLE_LENGTH_ENTRY2(1, adInteger,m_ie_empid, 
	sizeof(m_ie_empid), le_empidStatus, FALSE)

	// Column LastName is the 2nd field in the table.
	ADO_VARIABLE_LENGTH_ENTRY2(4, adVarChar, m_sze_lname, 
	sizeof(m_sze_lname), le_lnameStatus, FALSE)

	// Column FirstName is the 3rd field in the table.
	ADO_VARIABLE_LENGTH_ENTRY2(2, adVarChar, m_sze_fname, 
	sizeof(m_sze_fname), le_fnameStatus, FALSE)

	ADO_VARIABLE_LENGTH_ENTRY2(8, adVarChar, m_szau_hiredate, 
	sizeof(m_szau_hiredate), lau_hiredateStatus, FALSE)
	END_ADO_BINDING()

public:
	INT    m_ie_empid;
	ULONG  le_empidStatus;
	CHAR    m_sze_fname[11];
	ULONG  le_fnameStatus;
	CHAR    m_sze_lname[21];
	ULONG  le_lnameStatus;    
	CHAR   m_szau_hiredate[40];
	ULONG  lau_hiredateStatus;
};

void PrintProviderError(_ConnectionPtr pConnection);
void PrintComError(_com_error &e);
void DisplaySupport (_RecordsetPtr  pRstTemp);

inline char* mygets(char* strDest, int n) 
{    
	char strExBuff[10];
	char* pstrRet = fgets(strDest, n, stdin);

	if (pstrRet == NULL)
		return NULL;

	if (!strrchr(strDest, '\n'))
		// Exhaust the input buffer.
		do
		{
			fgets(strExBuff, sizeof(strExBuff), stdin);
		}while (!strrchr(strExBuff, '\n'));
	else
		// Replace '\n' with '\0'
		strDest[strrchr(strDest, '\n') - strDest] = '\0';

	return pstrRet;
}

int EventTest(int argc, char* argv[])
{
	HRESULT hr;
	DWORD   dwConnEvt;
	DWORD   dwRstEvt;
	IConnectionPointContainer   *pCPC = NULL;
	IConnectionPoint         *pCP = NULL;
	IUnknown               *pUnk = NULL;
	CRstEvent               *pRstEvent = NULL;
	CConnEvent               *pConnEvent= NULL;
	int                     rc = 0;
	_RecordsetPtr            pRst; 
	_ConnectionPtr            pConn;

	::CoInitialize(NULL);

	hr = pConn.CreateInstance(__uuidof(Connection));
	if (FAILED(hr)) return rc;

	hr = pRst.CreateInstance(__uuidof(Recordset));
	if (FAILED(hr)) return rc;

	// Start using the Connection events

	hr = pConn->QueryInterface(__uuidof(IConnectionPointContainer), 
		(void **)&pCPC);
	if (FAILED(hr)) return rc;
	hr = pCPC->FindConnectionPoint(__uuidof(ConnectionEvents), &pCP);
	pCPC->Release();
	if (FAILED(hr)) return rc;

//	pConnEvent = new CConnEvent();
	hr = pConnEvent->QueryInterface(__uuidof(IUnknown), (void **) &pUnk);
	if (FAILED(hr)) return rc;
	hr = pCP->Advise(pUnk, &dwConnEvt);
	pCP->Release();
	if (FAILED(hr)) return rc;

	// Start using the Recordset events

	hr = pRst->QueryInterface(__uuidof(IConnectionPointContainer), 
		(void **)&pCPC);
	if (FAILED(hr)) return rc;
	hr = pCPC->FindConnectionPoint(__uuidof(RecordsetEvents), &pCP);
	pCPC->Release();
	if (FAILED(hr)) return rc;

//	pRstEvent = new CRstEvent();
	hr = pRstEvent->QueryInterface(__uuidof(IUnknown), (void **) &pUnk);
	if (FAILED(hr)) return rc;
	hr = pCP->Advise(pUnk, &dwRstEvt);
	pCP->Release();
	if (FAILED(hr)) return rc;

	// Do some work

	pConn->Open("dsn=Pubs;", "MyUserName", "MyPassword", adConnectUnspecified);
	pRst->Open("SELECT * FROM authors", (IDispatch *) pConn, 
		adOpenStatic, adLockReadOnly, adCmdText);
	pRst->MoveFirst();
	while (pRst->EndOfFile == FALSE)
	{
		wprintf(L"Name = '%s'\n", (wchar_t*) 
			((_bstr_t) pRst->Fields->GetItem("au_lname")->Value));
		pRst->MoveNext();
	}

	pRst->Close();
	pConn->Close();

	// Stop using the Connection events

	hr = pConn->QueryInterface(__uuidof(IConnectionPointContainer), 
		(void **) &pCPC);
	if (FAILED(hr)) return rc;
	hr = pCPC->FindConnectionPoint(__uuidof(ConnectionEvents), &pCP);
	pCPC->Release();
	if (FAILED(hr)) return rc;
	hr = pCP->Unadvise( dwConnEvt );
	pCP->Release();
	if (FAILED(hr)) return rc;

	// Stop using the Recordset events
	hr = pRst->QueryInterface(__uuidof(IConnectionPointContainer), 
		(void **) &pCPC);
	if (FAILED(hr)) return rc;
	hr = pCPC->FindConnectionPoint(__uuidof(RecordsetEvents), &pCP);
	pCPC->Release();
	if (FAILED(hr)) return rc;
	hr = pCP->Unadvise( dwRstEvt );
	pCP->Release();
	if (FAILED(hr)) return rc;

	CoUninitialize();
	return 1;
}

inline int myscanf(char* strDest, int n) 
{    
	char strExBuff[10];
	char* pstrRet = fgets(strDest, n, stdin);

	if (pstrRet == NULL)
		return 0;

	if (!strrchr(strDest, '\n'))
		// Exhaust the input buffer.
		do
		{
			fgets(strExBuff, sizeof(strExBuff), stdin);
		}while (!strrchr(strExBuff, '\n'));
	else
		// Replace '\n' with '\0'
		strDest[strrchr(strDest, '\n') - strDest] = '\0';

	return strlen(strDest);
}

void AbsolutePositionX(void)
{
	HRESULT hr = S_OK;  

	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace.
	_RecordsetPtr  pRstEmployees  = NULL;

	//Define Other Variables
	//Interface Pointer declared.(VC++ Extensions)
	IADORecordBinding *picRs = NULL;
	CEmployeeRs emprs;                  //C++ class object
	_bstr_t strMessage;
	char chKey;

	//Open a recordset using a Client Cursor
	//For the Employee Table

	_bstr_t strCnn("Provider='sqloledb';Data Source='MySqlServer';"
		"Initial Catalog='pubs';Integrated Security='SSPI';");

	try
	{
		//Open a recordset.
		TESTHR(pRstEmployees.CreateInstance(__uuidof(Recordset)));

		//Use client cursor to enable Absoluteposition property.
		pRstEmployees->CursorLocation = adUseClient;

		//You have to explicitly pass the default Cursor type 
		//and LockType to the Recordset.
		TESTHR(  pRstEmployees->Open("employee", 
			strCnn,adOpenForwardOnly,adLockReadOnly,adCmdTable));

		// Open an IADORecordBinding interface pointer which we'll use 
		// for Binding Recordset to a class.
		TESTHR(pRstEmployees->QueryInterface(
			__uuidof(IADORecordBinding),(LPVOID*)&picRs));

		//Bind the Recordset to a C++ Class here    
		TESTHR(picRs->BindToRecordset(&emprs));

		strMessage= "";

		//Enumerate recordset
		do
		{
			//Display Current Record Information
			printf("Employee : %s \n record %ld of %d", 
				emprs.le_lnameStatus == adFldOK ? 
				emprs.m_sze_lname : "<NULL>",
				pRstEmployees->AbsolutePosition, 
				pRstEmployees->RecordCount);

			printf("\nContinue?(y/n)  :");

			do
			{
				chKey = getch();
			}while(chKey != 'y' && chKey !='n');

			//Clear the Screen for the next output   
			system("cls");

			if(chKey == 'n')
				break;

			strMessage = "";   
			pRstEmployees->MoveNext();   
		}while(!(pRstEmployees->EndOfFile));
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		_variant_t vtConnect = pRstEmployees->GetActiveConnection();

		// GetActiveConnection returns connect _bstr_t if connection
		// is not open, else returns Connection object.
		switch(vtConnect.vt)
		{
		case VT_BSTR:
			printf("Error:\n");
			printf("Code = %08lx\n", e.Error());
			printf("Message = %s\n", e.ErrorMessage());
			printf("Source = %s\n", (LPCSTR) e.Source());
			printf("Description = %s\n", (LPCSTR) e.Description());
			break;
		case VT_DISPATCH:
			PrintProviderError(vtConnect);
			break;
		default:
			printf("Errors occured.");
			break;
		}
	}

	// Clean up objects before exit.
	//Release the IADORecordset Interface here   
	if (picRs)
		picRs->Release();

	if (pRstEmployees)
		if (pRstEmployees->State == adStateOpen)
			pRstEmployees->Close();
}


void AbsolutePageX(void)
{

	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace.
	_RecordsetPtr   pRstEmployees   = NULL;


	//Define Other Variables
	//Interface Pointer declared.(VC++ Extensions) 
	IADORecordBinding   *picRs = NULL;
	CEmployeeRs emprs;          //C++ class object 
	HRESULT hr = S_OK;  
	_bstr_t strMessage; 

	//Open a recordset using a Client Cursor
	//For the Employee Table

	_bstr_t strCnn("Provider='sqloledb';Data Source='MySqlServer';"
		"Initial Catalog='pubs';Integrated Security='SSPI';");

	try
	{
		// Open a recordset.
		TESTHR(hr = pRstEmployees.CreateInstance(__uuidof(Recordset)));

		// Use client cursor to enable Absoluteposition property.
		pRstEmployees->CursorLocation = adUseClient;

		// You have to explicitly pass the default Cursor type 
		// and LockType to the Recordset here 
		TESTHR(hr = pRstEmployees->Open("employee", 
			strCnn,adOpenForwardOnly,adLockReadOnly,adCmdTable));

		//Open an IADORecordBinding interface pointer which we'll use for 
		//Binding Recordset to a class    
		TESTHR(hr = pRstEmployees->QueryInterface
			(__uuidof(IADORecordBinding), (LPVOID*)&picRs));

		//Bind the Recordset to a C++ Class here    
		TESTHR(hr = picRs->BindToRecordset(&emprs));

		//Display Names and hire dates, five records at a time
		pRstEmployees->PageSize = 5;

		int intPageCount = pRstEmployees->PageCount;

		for(int intPage=1;intPage<=intPageCount;intPage++)
		{
			pRstEmployees->put_AbsolutePage((enum PositionEnum)intPage);
			strMessage = "";

			for(int intRecord=1;
				intRecord<=pRstEmployees->PageSize;intRecord++)
			{
				printf("\t%s %s %.10s\n", 
					emprs.le_fnameStatus == adFldOK ? 
					emprs.m_sze_fname : "<NULL>",
					emprs.le_lnameStatus == adFldOK ? 
					emprs.m_sze_lname : "<NULL>",
					emprs.lau_hiredateStatus == adFldOK ? 
					emprs.m_szau_hiredate : "<NULL>");

				pRstEmployees->MoveNext();

				if(pRstEmployees->EndOfFile)
					break;
			}

			printf("\n Press any key to continue...");getch();

			//Clear the Screen for the next Display  
			system("cls");
		}
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		_variant_t vtConnect = pRstEmployees->GetActiveConnection();

		// GetActiveConnection returns connect _bstr_t if connection
		// is not open, else returns Connection object.
		switch(vtConnect.vt)
		{
		case VT_BSTR:
			printf("Error:\n");
			printf("Code = %08lx\n", e.Error());
			printf("Message = %s\n", e.ErrorMessage());
			printf("Source = %s\n", (LPCSTR) e.Source());
			printf("Description = %s\n", (LPCSTR) e.Description());
			break;
		case VT_DISPATCH:
			PrintProviderError(vtConnect);
			break;
		default:
			printf("Errors occured.");
			break;
		}
	}
	// Clean up objects before exit.
	//Release the IADORecordset Interface here
	if (picRs) 
		picRs->Release();

	if (pRstEmployees)
		if (pRstEmployees->State == adStateOpen)
			pRstEmployees->Close();
}



void AbsolutePosition2X(void)
{
	HRESULT hr = S_OK;  

	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace.
	_RecordsetPtr  pRstEmployees  = NULL;

	//Define Other Variables
	//Interface Pointer declared.(VC++ Extensions)
	IADORecordBinding *picRs = NULL;
	CEmployeeRs emprs;                  //C++ class object
	_bstr_t strMessage;

	//Open a recordset using a Client Cursor
	//For the Employee Table

	_bstr_t strCnn("Provider='sqloledb';Data Source='MySqlServer';"
		"Initial Catalog='pubs';Integrated Security='SSPI';");

	try
	{
		//Open a recordset.
		TESTHR(pRstEmployees.CreateInstance(__uuidof(Recordset)));

		//Use client cursor to enable Absoluteposition property.
		pRstEmployees->CursorLocation = adUseClient;

		//You have to explicitly pass the default Cursor type 
		//and LockType to the Recordset.
		TESTHR(pRstEmployees->Open("employee", 
			strCnn,adOpenStatic,adLockReadOnly,adCmdTable));

		// Open an IADORecordBinding interface pointer which we'll use 
		// for Binding Recordset to a class.
		TESTHR(pRstEmployees->QueryInterface(
			__uuidof(IADORecordBinding),(LPVOID*)&picRs));

		//Bind the Recordset to a C++ Class here    
		TESTHR(picRs->BindToRecordset(&emprs));

		long lGoToPos = 21;

		pRstEmployees->AbsolutePosition = (PositionEnum)lGoToPos;

		//Display Current Record Information
		printf("Employee : %s \n record %ld of %d", 
			emprs.le_lnameStatus == adFldOK ? emprs.m_sze_lname : "<NULL>", pRstEmployees->AbsolutePosition, 
			pRstEmployees->RecordCount);

		printf("\nPress any key to continue:");
		getch();
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		_variant_t vtConnect = pRstEmployees->GetActiveConnection();

		// GetActiveConnection returns connect _bstr_t if connection
		// is not open, else returns Connection object.
		switch(vtConnect.vt)
		{
		case VT_BSTR:
			printf("Error:\n");
			printf("Code = %08lx\n", e.Error());
			printf("Message = %s\n", e.ErrorMessage());
			printf("Source = %s\n", (LPCSTR) e.Source());
			printf("Description = %s\n", (LPCSTR) e.Description());
			break;
		case VT_DISPATCH:
			PrintProviderError(vtConnect);
			break;
		default:
			printf("Errors occured.");
			break;
		}
	}

	// Clean up objects before exit.
	//Release the IADORecordset Interface here   
	if (picRs)
		picRs->Release();

	if (pRstEmployees)
		if (pRstEmployees->State == adStateOpen)
			pRstEmployees->Close();
}

void AttributesX()
{
	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace
	_RecordsetPtr pRstEmployee  = NULL;
	_ConnectionPtr pConnection = NULL;
	FieldsPtr fldLoop = NULL;    
	PropertiesPtr proLoop = NULL;

	//Define Other Variables
	HRESULT  hr = S_OK;
	_variant_t Index;
	Index.vt = VT_I2;
	int j=0;      
	//Open a recordset using a Client Cursor
	//For the Employee Table
	_bstr_t strCnn("Provider='sqloledb';Data Source='MySqlServer';"
		"Initial Catalog='pubs';Integrated Security='SSPI';");

	try
	{
		// open connection and record set
		TESTHR(pConnection.CreateInstance(__uuidof(Connection)));
		pConnection->Open(strCnn,"","",adConnectUnspecified);

		TESTHR(pRstEmployee.CreateInstance(__uuidof(Recordset)));
		pRstEmployee->Open("Employee", _variant_t((IDispatch *)pConnection,true), adOpenForwardOnly,
			adLockReadOnly, adCmdTable);

		// Display the attributes of Connection.
		printf("Connection attributes: %d \n", pConnection->Attributes);

		// Display the attribute of the employee table's 
		//fields
		printf("\nFields attributes:\n");
		fldLoop = pRstEmployee->GetFields();

		for (int i = 0; i < (int)fldLoop->GetCount(); i++)
		{
			Index.iVal=i;
			printf ("   %s = %d \n",(LPSTR)fldLoop->GetItem(Index)->GetName(),
				(int)fldLoop->GetItem(Index)->GetAttributes());
		}

		// Display Fields of the Employee table which are NULLBALE
		printf("\nNULLABLE Fields :");

		for (int i1 = 0; i1 < (int)fldLoop->GetCount(); i1++)
		{
			Index.iVal = i1;

			if (fldLoop->GetItem(Index)->GetAttributes()  & adFldIsNullable)
			{
				printf ("%s  \n", (LPSTR)fldLoop->GetItem(Index)->GetName());    
			}
		}

		// Display the attributes of the Employee tables's 
		// properties
		printf("\nProperty attributes:\n");
		proLoop = pRstEmployee->GetProperties();

		for (int i2 = 0; i2 < (int)proLoop->GetCount(); i2++)
		{
			j= j+1;
			Index.iVal=i2;
			printf (" %s = %d \n", (LPSTR)(_bstr_t)proLoop->GetItem(Index)->GetName()
				,(int)proLoop->GetItem(Index)->GetAttributes()); 

			if (((j % 23) == 0) || ( i2==6))
			{
				printf("\nPress any key to continue...");
				getch();

				//Clear the screen for the next display   
				system("cls"); 
				j=0;
			}

		}
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.

		PrintProviderError(pConnection);
		PrintComError(e);
	}

	// Clean up objects before exit.
	if (pRstEmployee)
		if (pRstEmployee->State == adStateOpen)
			pRstEmployee->Close();
	if (pConnection)
		if (pConnection->State == adStateOpen)
			pConnection->Close();
}


void CountX()
{
	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace
	_RecordsetPtr pRstEmployee = NULL;

	//Define Other Variables
	HRESULT hr = S_OK;
	_variant_t Index;
	Index.vt = VT_I2;
	int j = 0;
	int intLoop = 0;
	_bstr_t strCnn("Provider='sqloledb';Data Source='MySqlServer';"
		"Initial Catalog='pubs';Integrated Security='SSPI';");

	try
	{
		// Open recordset with data from Employee table.
		TESTHR(pRstEmployee.CreateInstance(__uuidof(Recordset)));
		pRstEmployee->Open("Employee", strCnn, adOpenForwardOnly,
			adLockReadOnly, adCmdTable);

		// Print information about Fields collection.
		printf("%d Fields in Employee\n", pRstEmployee->Fields->Count);
		for (intLoop = 0;
			intLoop <= (pRstEmployee->Fields->Count-1);
			intLoop++)
		{
			Index.iVal = intLoop;
			printf(" %s\n",(LPSTR) pRstEmployee->Fields->
				GetItem(Index)->Name);
		}

		// Print information about Properties collection.
		printf("\n%d Properties in Employee\n", pRstEmployee->
			Properties->Count);
		for (intLoop = 0; 
			intLoop <= (pRstEmployee->Properties->Count - 1);
			intLoop++)
		{
			j++;
			Index.iVal = intLoop;
			printf(" %s\n" ,(LPSTR)pRstEmployee->Properties->
				GetItem(Index)->Name);
			if (((j % 23) == 0) || (intLoop == 11))
			{
				printf("\nPress any key to continue...");
				getch();

				//Clear the screen for the next display   
				system("cls"); 
				j = 0;
			}
		}
	}
	catch(_com_error &e)
	{
		// Notify user of any errors that result from
		// executing the query.
		// Pass a connection pointer accessed from the Recordset.
		_variant_t vtConnect = pRstEmployee->GetActiveConnection();

		switch(vtConnect.vt)
		{
		case VT_BSTR:
			PrintComError(e);
			break;
		case VT_DISPATCH:
			PrintProviderError(vtConnect);
			break;
		default:
			printf("Errors occured.");
			break;
		}
	}
	// Clean up objects before exit.
	if (pRstEmployee)
		if (pRstEmployee->State == adStateOpen)
			pRstEmployee->Close();
}


bool GetRowsOK(_RecordsetPtr pRstTemp,int intNumber,
	_variant_t& avarData)
{
	// Store results of GetRows method in array.
	avarData = pRstTemp->GetRows(intNumber);

	// Return False only if fewer than the desired
	// number of rows were returned, but not because the
	// end of the Recordset was reached.
	long lUbound;
	HRESULT hr = SafeArrayGetUBound(avarData.parray,2,&lUbound);   
	if (hr == 0)
	{
		if ((intNumber > lUbound + 1) && (!(pRstTemp->EndOfFile)))
			return false;
		else
			return true;   
	}
	else 
	{
		printf ("\nUnable to Get the Array's Upper Bound\n");
		return false;
	}
}

void OpenSchemaX() 
{
	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace.
	_ConnectionPtr  pConnection    = NULL;
	_RecordsetPtr  pRstSchema  = NULL;

	//Other Variables
	HRESULT  hr = S_OK;

	_bstr_t strCnn("Provider='sqloledb';Data Source='MySqlServer';"
		"Initial Catalog='pubs';Integrated Security='SSPI';");

	try
	{
		// Open connection.
		TESTHR(pConnection.CreateInstance(__uuidof(Connection)));
		pConnection->Open (strCnn, "", "", adConnectUnspecified);

		pRstSchema = pConnection->OpenSchema(adSchemaTables);

		while(!(pRstSchema->EndOfFile))
		{
			_bstr_t table_name = pRstSchema->Fields->
				GetItem("TABLE_NAME")->Value;

			printf("Table Name: %s\n",(LPCSTR) table_name);

			_bstr_t table_type = pRstSchema->Fields->
				GetItem("TABLE_TYPE")->Value;

			printf("Table type: %s\n\n",(LPCSTR) table_type);

			pRstSchema->MoveNext();

			int intLine = intLine + 1;
			if (intLine % 5 == 0)
			{
				printf("\nPress any key to continue...");
				getch();
				//Clear the screen for the next display   
				system("cls"); 
			}
		}
	}
	catch (_com_error &e)
	{
		// Notify the user of errors if any.
		// Pass a connection pointer accessed from the Connection.        
		PrintProviderError(pConnection);
		PrintComError(e);
	}

	// Clean up objects before exit.
	if (pRstSchema)
		if (pRstSchema->State == adStateOpen)
			pRstSchema->Close();
	if (pConnection)
		if (pConnection->State == adStateOpen)
			pConnection->Close();
}

//This class extracts titles and type from Titles table
class CTitleRs : public CADORecordBinding
{
	BEGIN_ADO_BINDING(CTitleRs)
		// Column title_id is the 1st field in the Recordset
		ADO_VARIABLE_LENGTH_ENTRY2(1, adVarChar, m_szau_Title_id,
		sizeof(m_szau_Title_id), lau_Title_idStatus, FALSE)
		// Column title is the 2nd field in the table
		ADO_VARIABLE_LENGTH_ENTRY2(2,adVarChar,m_szt_Title,
		sizeof(m_szt_Title),lt_TitleStatus,FALSE)
		// Column type is the 3rd field in the table
		ADO_VARIABLE_LENGTH_ENTRY2(3,adVarChar,m_szt_Type,
		sizeof(m_szt_Type),lt_TypeStatus,TRUE)
	END_ADO_BINDING()

public:
	CHAR m_szt_Title[81];
	ULONG lt_TitleStatus;
	CHAR m_szt_Type[13];
	ULONG lt_TypeStatus;
	CHAR m_szau_Title_id[7];
	ULONG lau_Title_idStatus;
};
// EndUpdateBatchH

void OptimizeOrderX(void) 
{
	HRESULT   hr = S_OK;

	// Define _bstr_t variables.
	_bstr_t strCnn("Provider='sqloledb';Data Source='MySqlServer';"
		"Initial Catalog='pubs';Integrated Security='SSPI';");
	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace.
	_RecordsetPtr   pRst = NULL;

	try
	{
		TESTHR(pRst.CreateInstance(__uuidof(Recordset)));

		// Enable Index creation.
		pRst->CursorLocation = adUseClient;
		pRst->Open ("SELECT * FROM authors", strCnn,
			adOpenStatic, adLockReadOnly, adCmdText);

		// Create the index
		pRst->Fields->GetItem("zip")->Properties->
			GetItem("Optimize")->PutValue("True");

		// Find Akiko Yokomoto
		pRst->Find("zip = '94595'",1,adSearchForward);
		printf("\n%s %s    %s %s %s\n",
			(LPSTR) (_bstr_t) pRst->Fields->GetItem("au_fname")->Value,
			(LPSTR) (_bstr_t) pRst->Fields->GetItem("au_lname")->Value,
			(LPSTR) (_bstr_t) pRst->Fields->GetItem("address")->Value,
			(LPSTR) (_bstr_t) pRst->Fields->GetItem("city")->Value,
			(LPSTR) (_bstr_t) pRst->Fields->GetItem("state")->Value);

		// Delete the index
		pRst->Fields->GetItem("zip")->Properties->
			GetItem("Optimize")->PutValue("False");
	}
	catch (_com_error &e)
	{
		// Notify the user of errors if any.
		// Pass a connection pointer accessed from the Recordset.
		_variant_t vtConnect = pRst->GetActiveConnection();

		// GetActiveConnection returns connect _bstr_t if connection
		// is not open, else returns Connection object.
		switch(vtConnect.vt)
		{
		case VT_BSTR:
			PrintComError(e);
			break;
		case VT_DISPATCH:
			PrintProviderError(vtConnect);
			break;
		default:
			printf("Errors occured.");
			break;
		}
	}

	// Clean up objects before exit.
	if (pRst)
		if (pRst->State == adStateOpen)
			pRst->Close();
}

void OriginalValueX(void) 
{
	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace.
	_ConnectionPtr   pConnection    = NULL;
	FieldPtr      pFldType   = NULL;
	_RecordsetPtr   pRstTitles   = NULL;

	// Define _bstr_t variables.
	_bstr_t strSQLChange("UPDATE Titles SET Type = "
		"'sociology' WHERE Type = 'psychology'");
	_bstr_t strSQLRestore("UPDATE Titles SET Type = "
		"'psychology' WHERE Type = 'sociology'");

	// Define Other Variables
	HRESULT    hr = S_OK;
	IADORecordBinding   *picRs  = NULL;  //Interface Pointer declared
	CTitleRs titlers;        // C++ Class object

	try 
	{
		_bstr_t strCnn("Provider='sqloledb';Data Source='MySqlServer';"
			"Initial Catalog='pubs';Integrated Security='SSPI';");

		// Open connection.
		TESTHR(pConnection.CreateInstance(__uuidof(Connection)));
		pConnection->Open (strCnn, "", "", adConnectUnspecified);

		// Open Recordset for batch update.
		TESTHR(pRstTitles.CreateInstance(__uuidof(Recordset)));
		pRstTitles->PutActiveConnection(
			_variant_t((IDispatch *)pConnection,true));
		pRstTitles->CursorType = adOpenKeyset;
		pRstTitles->LockType = adLockBatchOptimistic;

		// Cast Connection pointer to an IDispatch type so converted 
		// to correct type of variant.
		pRstTitles->Open("Titles", 
			_variant_t((IDispatch *)pConnection,true),
			adOpenKeyset, adLockBatchOptimistic, adCmdTable);

		//Open an IADORecordBinding interface pointer which 
		//we'll use for Binding Recordset to a class.
		TESTHR(pRstTitles->QueryInterface(
			__uuidof(IADORecordBinding),(LPVOID*)&picRs));

		//Bind the Recordset to a C++ Class here  
		TESTHR(picRs->BindToRecordset(&titlers));

		// Set field object variable for Type field.
		pFldType = pRstTitles->Fields->GetItem("type");

		// Change the type of psychology titles.
		while(!(pRstTitles->EndOfFile))
		{
			if (!strcmp(strtok((char *)titlers.m_szt_Type," "), 
				"psychology"))
			{
				pFldType->Value = "self_help";
			}
			pRstTitles->MoveNext();
		}

		// Simulate a change by another user by updating data
		// using a command _bstr_t.
		pConnection->Execute(strSQLChange,NULL,0);

		// Check for changes.
		pRstTitles->MoveFirst();
		while(!(pRstTitles->EndOfFile))
		{
			if (strcmp(pFldType->OriginalValue.pcVal,
				pFldType->UnderlyingValue.pcVal))
			{
				printf("\n\nData has changed!");

				printf("\n\nTitle ID: %s",titlers.lau_Title_idStatus == 
					adFldOK ? titlers.m_szau_Title_id : "<NULL>");

				printf("\n\nCurrent Value: %s",
					(LPCSTR) (_bstr_t) pFldType->Value);

				printf("\n\nOriginal Value: %s",
					(LPCSTR) (_bstr_t) pFldType->OriginalValue);

				printf("\n\nUnderlying Value: %s\n\n",
					(LPCSTR) (_bstr_t) pFldType->UnderlyingValue);

				printf("Press any key to continue...");
				getch();

				system("cls");
			}
			pRstTitles->MoveNext();
		}
	}
	catch (_com_error &e)
	{
		// Notify the user of errors if any.
		// Pass a connection pointer accessed from the Connection.
		PrintProviderError(pConnection);
		PrintComError(e);
	}

	// Clean up objects before exit.
	//Release the IADORecordset Interface here   
	if (picRs)
		picRs->Release();

	if (pRstTitles)
		if (pRstTitles->State == adStateOpen)
		{
			// Cancel the update because this is a demonstration.
			pRstTitles->CancelBatch(adAffectAll);
			pRstTitles->Close();
		}
		if (pConnection)
			if (pConnection->State == adStateOpen)
			{
				// Restore Original Values.
				pConnection->Execute(strSQLRestore,NULL,0);
				pConnection->Close();
			}
};
// EndSeekH
void SeekRecordSet()
{
	HRESULT hr = S_OK;

	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace.

	_RecordsetPtr pRstEmp = NULL;

	IADORecordBinding    *picRs = NULL;  // Interface Pointer declared
	CEmployeeRs EmpRs; //C++ class object

	//Definitions of other variables
	_bstr_t strPrompt("Enter an EmployeeID (e.g., 1 to 9)");
	char strEmpId[2];

	try
	{
		TESTHR(pRstEmp.CreateInstance(__uuidof(Recordset)));
		pRstEmp->CursorLocation = adUseServer;
		pRstEmp->Open("employees", "Provider='Microsoft.Jet.OLEDB.4.0';" 
			"Data Source='C:\\Program Files\\Microsoft Office\\Office\\"
			"Samples\\Northwind.mdb';",
			adOpenKeyset,adLockReadOnly,adCmdTableDirect);

		//Open an IADORecordBinding interface pointer which 
		//we'll use for binding Recordset to a Class  
		TESTHR(pRstEmp->QueryInterface(
			__uuidof(IADORecordBinding), (LPVOID*)&picRs));

		//Bind the Recordset to a C++ Class
		TESTHR(picRs->BindToRecordset(&EmpRs));

		//Does this provider support Seek and Index?
		if (pRstEmp->Supports(adIndex) && pRstEmp->Supports(adSeek))
		{
			pRstEmp->Index = "PrimaryKey";

			//Display all the employees.
			pRstEmp->MoveFirst();
			while (!pRstEmp->EndOfFile)
			{
				printf("%d : %s %s\n",  
					EmpRs.le_empidStatus == adFldOK ? 
					EmpRs.m_ie_empid : 0, 
					EmpRs.le_fnameStatus == adFldOK ? 
					EmpRs.m_sze_fname : "<NULL>", 
					EmpRs.le_lnameStatus == adFldOK ? 
					EmpRs.m_sze_lname : "<NULL>");

				pRstEmp->MoveNext();
			}
			//Prompt the user for an EmployeeID between 1 and 9.
			do
			{
				pRstEmp->MoveFirst();
				printf("\n\n%s\t",(LPCSTR) strPrompt);
				mygets(strEmpId, 2);

				//Quit if strEmpID is a zero-length _bstr_t 
				//(CANCEL, null, etc.)
				char *strTemp = strtok(strEmpId," \t");
				if (strTemp == NULL) break;
				if (strlen(strTemp) == 1 && atoi(strTemp) >= 1 && 
					atoi(strTemp) <= 9)
				{
					_variant_t strEmployeeId(strTemp);
					pRstEmp->Seek(strEmployeeId, adSeekAfterEQ);
					if (pRstEmp->EndOfFile)
					{
						printf("Employee not found.\n");
					}
					else
					{
						printf("%d : Employee='%s %s'\n",  
							EmpRs.le_empidStatus == adFldOK ? 
							EmpRs.m_ie_empid : 0,
							EmpRs.le_fnameStatus == adFldOK ? 
							EmpRs.m_sze_fname : "<NULL>", 
							EmpRs.le_lnameStatus == adFldOK ? 
							EmpRs.m_sze_lname : "<NULL>");
					}
				}
			}
			while(true);
		}  
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		// Pass a connection pointer accessed from the Recordset.
		_variant_t vtConnect = pRstEmp->GetActiveConnection();

		// GetActiveConnection returns connect _bstr_t if connection
		// is not open, else returns Connection object.
		switch(vtConnect.vt)
		{
		case VT_BSTR:
			PrintComError(e);
			break;
		case VT_DISPATCH:
			PrintProviderError(vtConnect);
			break;
		default:
			printf("Errors occured.");
			break;
		}
	}

	// Clean up objects before exit.
	//Release the IADORecordset Interface here   
	if (picRs)
		picRs->Release();

	if (pRstEmp)
		if (pRstEmp->State == adStateOpen)
			pRstEmp->Close();
}

// connection test
void ShowSupports(_bstr_t strCnn)
{
	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace.
	_RecordsetPtr  pRstTitles  = NULL;

	// Define Other Variables
	HRESULT hr = S_OK;

	try
	{
		// Open a recordset from Titles table
		TESTHR(pRstTitles.CreateInstance(__uuidof(Recordset)));

		// Fill array with CursorType constants.
		int  aintCursorType[4];
		aintCursorType[0] = adOpenForwardOnly;
		aintCursorType[1] = adOpenKeyset;
		aintCursorType[2] = adOpenDynamic;
		aintCursorType[3] = adOpenStatic;

		// Open recordset using each CursorType and optimistic locking.
		// Then call the DisplaySupport procedure to display the
		// supported options.
		for (int intIndex=0; intIndex <= 3; intIndex++)
		{
			pRstTitles->CursorType = 
				(enum CursorTypeEnum)aintCursorType[intIndex];
			pRstTitles->LockType = adLockOptimistic;

			// Pass the Cursor type and LockType to the Recordset.
			pRstTitles->Open ("titles", strCnn, 
				(enum CursorTypeEnum)aintCursorType[intIndex], 
				adLockOptimistic, adCmdTable);

			switch(aintCursorType[intIndex])
			{
			case adOpenForwardOnly:
				printf("\nForwardOnly cursor supports:\n");
				break;

			case adOpenKeyset:
				printf("\nKeyset cursor supports:\n");
				break;

			case adOpenDynamic:
				printf("\nDynamic cursor supports:\n");
				break;

			case adOpenStatic:
				printf("\nStatic cursor supports:\n");
				break;

			default :
				break;
			}

			DisplaySupport(pRstTitles);

			printf("\n\nPress any key to continue...");
			getch();

			//Clear the screen for the next display.
			system("cls");
		}
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		// Pass a connection pointer accessed from the Recordset.
		_variant_t vtConnect = pRstTitles->GetActiveConnection();

		// GetActiveConnection returns connect _bstr_t if connection
		// is not open, else returns Connection object.
		switch(vtConnect.vt)
		{
		case VT_BSTR:
			PrintComError(e);
			break;
		case VT_DISPATCH:
			PrintProviderError(vtConnect);
			break;
		default:
			printf("Errors occured.");
			break;
		}
	}

	// Clean up objects before exit.
	if (pRstTitles)
		if (pRstTitles->State == adStateOpen)
			pRstTitles->Close();
}

void DisplaySupport (_RecordsetPtr  pRstTemp)
{
	// Fill array with cursor option constants.
	long  alngConstants[11];
	alngConstants[0] = adAddNew;
	alngConstants[1] = adApproxPosition;
	alngConstants[2] = adBookmark;
	alngConstants[3] = adDelete;
	alngConstants[4] = adFind;
	alngConstants[5] = adHoldRecords;
	alngConstants[6] = adMovePrevious;
	alngConstants[7] = adNotify;
	alngConstants[8] = adResync;
	alngConstants[9] = adUpdate;
	alngConstants[10] = adUpdateBatch;

	for(int intIndex=0; intIndex <= 10; intIndex++)
	{
		bool booSupports = pRstTemp->
			Supports( (enum CursorOptionEnum)alngConstants[intIndex] );

		if(booSupports)
		{
			switch(alngConstants[intIndex])
			{
			case adAddNew :
				printf("\n  AddNew");
				break;

			case adApproxPosition :
				printf("\n  AbsolutePosition and AbsolutePage");
				break;

			case adBookmark :
				printf("\n  Bookmark");
				break;

			case adDelete :
				printf("\n  Delete");
				break;

			case adFind :
				printf("\n  Find");
				break;

			case adHoldRecords :
				printf("\n  Holding Records");
				break;

			case adMovePrevious :
				printf("\n  MovePrevious and Move");
				break;

			case adNotify :
				printf("\n  Notifications");
				break;

			case adResync :
				printf("\n  Resyncing data");
				break;

			case adUpdate :
				printf("\n  Update");
				break;

			case adUpdateBatch :
				printf("\n  Batch updating");
				break;

			default :
				break;
			}
		}
	}
}

void PrintProviderError(_ConnectionPtr pConnection)
{
	//Define Other Variables
	HRESULT  hr = S_OK;
	_bstr_t  strError;
	ErrorPtr  pErr = NULL;

	try
	{
		// Enumerate Errors collection and display
		// properties of each Error object.
		long nCount = pConnection->Errors->Count;

		// Collection ranges from 0 to nCount - 1.
		for(long i = 0; i < nCount; i++)
		{
			pErr = pConnection->Errors->GetItem(i);
			printf("Error #%d\n", pErr->Number);
			printf(" %s\n",(LPCSTR)pErr->Description);
			printf(" (Source: %s)\n",(LPCSTR)pErr->Source);
			printf(" (SQL State: %s)\n",(LPCSTR)pErr->SQLState);
			printf(" (NativeError: %d)\n",(LPCSTR)pErr->NativeError);
			if ((LPCSTR)pErr->GetHelpFile() == NULL)
			{
				printf("\tNo Help file available\n");
			}
			else
			{
				printf("\t(HelpFile: %s\n)" ,pErr->HelpFile);
				printf("\t(HelpContext: %s\n)" , pErr->HelpContext);
			}
		}
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		PrintComError(e);
	}

}

void PrintComError(_com_error &e)
{
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());

	// Print Com errors.
	printf("Error\n");
	printf("\tCode = %08lx\n", e.Error());
	printf("\tCode meaning = %s\n", e.ErrorMessage());
	printf("\tSource = %s\n", (LPCSTR) bstrSource);
	printf("\tDescription = %s\n", (LPCSTR) bstrDescription);
}
// Fields in Employee a Table:
void ShowTypeInFieldOfRecordSet(_RecordsetPtr pRstEmployees)
{    
	HRESULT  hr = S_OK;

	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace.
	FieldsPtr  pFldLoop      = NULL;

	try
	{  
		// Enumerate the Fields collection of the Employees table.
		pFldLoop = pRstEmployees->GetFields();
		int intLine = 0;
		_bstr_t strType; 
		for (int intFields = 0; intFields < (int)pFldLoop->
			GetCount(); intFields++)
		{
			_variant_t Index;
			Index.vt = VT_I2;
			Index.iVal = intFields;
			printf("  Name: %s\n" ,
				(LPCSTR) pFldLoop->GetItem(Index)->GetName());

			switch(pFldLoop->GetItem(Index)->Type) 
			{
			case adChar:
				strType = "adChar";
				break;
			case adVarChar:
				strType = "adVarChar";
				break;
			case adSmallInt:
				strType = "adSmallInt";
				break;
			case adUnsignedTinyInt:
				strType = "adUnsignedTinyInt";
				break;
			case adDBTimeStamp:
				strType = "adDBTimeStamp";
				break;
			default:
				break;
			}
		
			printf("  Type: %s\n\n", strType);

			intLine++;
			if(intLine % 5 == 0)
			{
				printf("Press any key to continue...");
				getch();
				system("cls");
			}
		}
	}
	catch (_com_error &e)
	{
		// Notify the user of errors if any.
		// Pass a connection pointer accessed from the Recordset.
		_variant_t vtConnect = pRstEmployees->GetActiveConnection();

		// GetActiveConnection returns connect _bstr_t if connection
		// is not open, else returns Connection object.
		switch(vtConnect.vt)
		{
		case VT_BSTR:
			PrintComError(e);
			break;
		case VT_DISPATCH:
			PrintProviderError(vtConnect);
			break;
		default:
			printf("Errors occured.");
			break;
		}
	}

	// Clean up objects before exit.
	if (pRstEmployees)
		if (pRstEmployees->State == adStateOpen)
			pRstEmployees->Close();
}
// EndUpdateBatchCpp
void ShowTypeInPropertyOfRecordSet()
{
	HRESULT  hr = S_OK;

	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace
	_RecordsetPtr  pRst  = NULL;
	PropertyPtr pProperty = NULL;

	//Define Other Variables
	_bstr_t strMsg;
	_variant_t vIndex;
	int intLineCnt = 0;   

	try
	{
		TESTHR(pRst.CreateInstance (__uuidof(Recordset)));

		// Set the Recordset Cursor Location
		pRst->CursorLocation = adUseClient;

		for (short iIndex = 0; iIndex <= (pRst->Properties->
			GetCount() - 1);iIndex++)
		{
			vIndex = iIndex;
			pProperty = pRst->Properties->GetItem(vIndex);

			int propType = (int)pProperty->GetType();
			switch(propType) 
			{
			case adBigInt:
				strMsg = "adBigInt";
				break;
			case adBinary:
				strMsg = "adBinary";
				break;
			case adBoolean:
				strMsg = "adBoolean";
				break;
			case adBSTR:
				strMsg = "adBSTR";
				break;
			case adChapter:
				strMsg = "adChapter";
				break;
			case adChar:
				strMsg = "adChar";
				break;
			case adCurrency:
				strMsg = "adCurrency";
				break;
			case adDate:
				strMsg = "adDate";
				break;
			case adDBDate:
				strMsg = "adDBDate";
				break;
			case adDBTime:
				strMsg = "adDBTime";
				break;
			case adDBTimeStamp:
				strMsg = "adDBTimeStamp";
				break;
			case adDecimal:
				strMsg = "adDecimal";
				break;
			case adDouble:
				strMsg = "adDouble";
				break;
			case adEmpty:
				strMsg = "adEmpty";
				break;
			case adError:
				strMsg = "adError";
				break;
			case adFileTime:
				strMsg = "adFileTime";
				break;
			case adGUID:
				strMsg = "adGUID";
				break;
			case adIDispatch:
				strMsg = "adIDispatch";
				break;
			case adInteger:
				strMsg = "adInteger";
				break;
			case adIUnknown:
				strMsg = "adIUnknown";
				break;
			case adLongVarBinary:
				strMsg = "adLongVarBinary";
				break;
			case adLongVarChar:
				strMsg = "adLongVarChar";
				break;
			case adLongVarWChar:
				strMsg = "adLongVarWChar";
				break;
			case adNumeric:
				strMsg = "adNumeric";
				break;
			case adPropVariant:
				strMsg = "adPropVariant";
				break;
			case adSingle:
				strMsg = "adSingle";
				break;
			case adSmallInt:
				strMsg = "adSmallInt";
				break;
			case adTinyInt:
				strMsg = "adTinyInt";
				break;
			case adUnsignedBigInt:
				strMsg = "adUnsignedBigInt";
				break;
			case adUnsignedInt:
				strMsg = "adUnsignedInt";
				break;
			case adUnsignedSmallInt:
				strMsg = "adUnsignedSmallInt";
				break;
			case adUnsignedTinyInt:
				strMsg = "adUnsignedTinyInt";
				break;
			case adUserDefined:
				strMsg = "adUserDefined";
				break;
			case adVarBinary:
				strMsg = "adVarBinary";
				break;
			case adVarChar:
				strMsg = "adVarChar";
				break;
			case adVariant:
				strMsg = "adVariant";
				break;
			case adVarNumeric:
				strMsg = "adVarNumeric";
				break;
			case adVarWChar:
				strMsg = "adVarWChar";
				break;
			case adWChar:
				strMsg = "adWChar";
				break;
			default:
				strMsg = "*UNKNOWN*";
				break;
			}

			intLineCnt++;
			if (intLineCnt%20 == 0)
			{
				printf("\nPress any key to continue...\n");
				getch();
			}
			printf ("Property %d : %s,Type = %s\n",iIndex,
				(LPCSTR)pProperty->GetName(),(LPCSTR)strMsg);
		}
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		PrintComError(e);
	}
}

void UpdateBatchByBinding(_RecordsetPtr pRstTitles)
{   
	IADORecordBinding *picRs    = NULL;    // Interface Pointer Declared
	CTitleRs titlers;             // C++ Class Object
	try{
		// Binding the Recordset to a C++ Class
		TESTHR(pRstTitles->QueryInterface(__uuidof(IADORecordBinding),(LPVOID*)&picRs));
		TESTHR(picRs->BindToRecordset(&titlers));
		pRstTitles->MoveFirst();

		// Loop through recordset and ask user if she wants,
		// to change the type for a specified title.
		while (!(pRstTitles->EndOfFile))
		{
			// Compare type with psychology
			if (!strcmp( (char *)strtok(titlers.m_szt_Type," "),
				"psychology" ))
			{ 
				printf("\n\nTitle: %s \nChange type to self_help?(y/n):",
					titlers.m_szt_Title);
				char chKey;
				chKey = getch();
				if(toupper(chKey) == 'Y')
				{
					// Change type to self_help.
					pRstTitles->Fields->GetItem("type")->Value = 
						(_bstr_t)("self_help");
				}
			}
			pRstTitles->MoveNext();
		}

		// Ask the user if she wants to commit to all the 
		// changes made above.
		printf("\n\nSave all changes?");
		char chKey;
		chKey = getch();
		if(toupper(chKey) == 'Y')
		{
			pRstTitles->UpdateBatch(adAffectAll);
		}
		else
		{
			pRstTitles->CancelBatch(adAffectAll);
		}

		// Print current data in recordset.
		pRstTitles->Requery(adOptionUnspecified);

		// Open IADORecordBinding interface pointer for Binding Recordset to a class    
		TESTHR(pRstTitles->QueryInterface(
			__uuidof(IADORecordBinding),(LPVOID*)&picRs));

		// ReBinding the Recordset to a C++ Class.
		TESTHR(picRs->BindToRecordset(&titlers));

		// Move to the first record of the title table
		pRstTitles->MoveFirst();

		//Clear the screen for the next display.
		system("cls");

		while (!pRstTitles->EndOfFile)
		{
			printf("%s -  %s\n",
				titlers.lt_TitleStatus == adFldOK ? 
				titlers.m_szt_Title :"<NULL>",
				titlers.lt_TypeStatus == adFldOK ? 
				titlers.m_szt_Type :"<NULL>");
			pRstTitles->MoveNext();
		}

		pRstTitles->MoveFirst();

		// Restore original data because this is demonstration.
		while (!(pRstTitles->EndOfFile))
		{
			// Compare type with psychology
			if(!strcmp( (char *)strtok(titlers.m_szt_Type," "),
				"self_help" ))
			{
				// Change type to psychology.
				pRstTitles->Fields->GetItem("type")->Value = 
					(_bstr_t)("psychology");
			}
			pRstTitles->MoveNext();
		}
	}
	catch (_com_error &e)
	{
		// Notify the user of errors if any.
		// Pass a connection pointer accessed from the Recordset.
		_variant_t vtConnect = pRstTitles->GetActiveConnection();

		// GetActiveConnection returns connect _bstr_t if connection
		// is not open, else returns Connection object.
		switch(vtConnect.vt)
		{
		case VT_BSTR:
			PrintComError(e);
			break;
		case VT_DISPATCH:
			PrintProviderError(vtConnect);
			break;
		default:
			printf("Errors occured.");
			break;
		}
	}

	if (pRstTitles)
		if (pRstTitles->State == adStateOpen)
		{
			pRstTitles->UpdateBatch(adAffectAll);
			pRstTitles->Close();
		}
}
void ShowVersion(_ConnectionPtr pConnection)
{
	printf("ADO Version   : %s\n\n",(LPCSTR) pConnection->Version);
	printf("DBMS Name   : %s\n\n",(LPCSTR) (_bstr_t) 
		pConnection->Properties->GetItem("DBMS Name")->Value);
	printf("DBMS Version   : %s\n\n",(LPCSTR) (_bstr_t)
		pConnection->Properties->GetItem("DBMS Version")->Value);
	printf("OLE DB Version   : %s\n\n",(LPCSTR) (_bstr_t) 
		pConnection->Properties->GetItem("OLE DB Version")->Value);
	printf("Provider Name   : %s\n\n",(LPCSTR) (_bstr_t) 
		pConnection->Properties->GetItem("Provider Name")->Value);
	printf("Provider Version   : %s\n\n",(LPCSTR) (_bstr_t) 
		pConnection->Properties->GetItem("Provider Version")->Value);
	printf("Driver Name   : %s\n\n",(LPCSTR) (_bstr_t) 
		pConnection->Properties->GetItem("Driver Name")->Value);
	printf("Driver Version   : %s\n\n",(LPCSTR) (_bstr_t) 
		pConnection->Properties->GetItem("Driver Version")->Value);
	printf("Driver ODBC Version   : %s\n\n",(LPCSTR) (_bstr_t) 
		pConnection->Properties->GetItem("Driver ODBC Version")->Value);

}
void ShowFileds(_ConnectionPtr rst)
{
	// Enumerate the Properties collection of the Recordset object.
	FieldsPtr pPrpLoop = rst->GetProperties();
	int intLine = 0;
	_variant_t vtIndex;

	for (int intProperties = 0; intProperties < (int)pPrpLoop->
		GetCount(); intProperties++)
	{
		vtIndex.iVal = intProperties;

		// Because Value is the default property of a
		// Property object,the use of the actual keyword
		// here is optional.
		_variant_t propValue = pPrpLoop->GetItem(vtIndex)->Value;
		switch(propValue.vt)
		{

		case (VT_BOOL):
			if(propValue.boolVal)
			{
				printf(" %s = True\n\n",(LPCSTR) pPrpLoop->
					GetItem(vtIndex)->GetName());
			}
			else
			{
				printf(" %s = False\n\n",(LPCSTR) pPrpLoop->
					GetItem(vtIndex)->GetName());
			}
			break;

		case (VT_I4):
			printf(" %s = %d\n\n",(LPCSTR) pPrpLoop->
				GetItem(vtIndex)->GetName(),
				pPrpLoop->GetItem(vtIndex)->Value.lVal);
			break;

		case (VT_EMPTY):
			printf(" %s = \n\n",(LPCSTR) pPrpLoop->
				GetItem(vtIndex)->GetName());
			break;

		default:
			break;
		}

		intLine++;
		if (intLine % 10 == 0)
		{
			printf("\nPress any key to continue...");
			getch();

			//Clear the screen for the next display   
			system("cls"); 
		}
	}

}