/***********************************************************************
 * Module:  CConnEvent.h
 * Author:  HJolin
 * Modified: 2012Äê11ÔÂ27ÈÕ 23:54:09
 * Purpose: Declaration of the class CConnEvent
 ***********************************************************************/

#if !defined(__DB_CConnEvent_h)
#define __DB_CConnEvent_h

#include "stdafx.h"

class CConnEvent
{
public:
   CConnEvent();
   ~CConnEvent();
   
   // Handlers that require access to some of the internal interfaces on the proxy manager have to go through the IInternalUnknown interface. This prevents handlers from blindly delegating and exposing the aggregatee's internal interfaces outside of the aggregate. These interfaces include IProxyManager, IClientSecurity, and IMultiQI. If the handler wants to expose IClientSecurity or IMultiQI, it should implement them itself. 
   // In the case of IClientSecurity, if the client tries to set the security on an interface that the handler has exposed, the handler should set the security on the underlying network interface proxy.
   // In the case of IMultiQI, the handler should fill in the interfaces it knows about and then forward the call to the proxy manager to get the rest of the interfaces filled in.
   STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
   STDMETHODIMP_(ULONG) AddRef(void);
   STDMETHODIMP_(ULONG) Release(void);
   
   // The InfoMessage event is called whenever a warning occurs during a ConnectionEvent operation
   STDMETHODIMP raw_InfoMessage(struct Error * pError, EventStatusEnum * adStatus, struct _Connection * pConnection);
   
   // These events will be called after the associated operation on the Connection object finishes executing.
   // BeginTransComplete is called after the BeginTrans operation.
   STDMETHODIMP raw_BeginTransComplete(LONG TransactionLevel, struct Error* pError, EventStatusEnum * adStatus, struct _Connection * pConnection);
   
   // These events will be called after the associated operation on the Connection object finishes executing.
   // CommitTransComplete is called after the CommitTrans operation.
   STDMETHODIMP raw_CommitTransComplete(struct Error * pError, EventStatusEnum * adStatus, struct _Connection * pConnection);
   
   // These events will be called after the associated operation on the Connection object finishes executing.
   // RollbackTransComplete is called after the RollbackTrans operation.
   STDMETHODIMP raw_RollbackTransComplete(struct Error * pError, EventStatusEnum* adStatus, struct _Connection * pConnection);
   
   // The WillExecute event is called just before a pending command executes on a connection.
   STDMETHODIMP raw_WillExecute(BSTR * Source, CursorTypeEnum * CursorType, LockTypeEnum * LockType, long * Options, EventStatusEnum * adStatus, struct _Command * pCommand, struct _Recordset * pRecordset, struct _Connection * pConnection);
   
   // The ExecuteComplete event is called after a command has finished executing.
   STDMETHODIMP raw_ExecuteComplete(LONG RecordsAffected, struct Error  * pError, EventStatusEnum * adStatus, struct _Command * pCommand, struct _Recordset * pRecordset, struct _Connection * pConnection);
   
   // The WillConnect event is called before a connection starts.
   STDMETHODIMP raw_WillConnect(BSTR * ConnectionString, BSTR * UserID, BSTR * Password, long * Options, EventStatusEnum * adStatus, struct _Connection * pConnection);
   
   // The ConnectComplete event is called after a connection starts.
   STDMETHODIMP raw_ConnectComplete(struct Error * pError, EventStatusEnum * adStatus, struct _Connection * pConnection);
   
   // An EventStatusEnum value that always returns adStatusOK.
   // When ConnectComplete is called, this parameter is set to adStatusCancel if a WillConnect event has requested cancellation of the pending connection. 
   // The Disconnect event is called after a connection ends
   STDMETHODIMP raw_Disconnect(EventStatusEnum * adStatus, struct _Connection * pConnection);

protected:
private:
   ULONG m_nRef;

};

#endif