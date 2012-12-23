/***********************************************************************
 * Module:  CRstEvent.h
 * Author:  HJolin
 * Modified: 2012Äê11ÔÂ27ÈÕ 23:54:16
 * Purpose: Declaration of the class CRstEvent
 ***********************************************************************/

#if !defined(__DB_CRstEvent_h)
#define __DB_CRstEvent_h

#include "stdafx.h"

class CRstEvent
{
public:
   CRstEvent();
   ~CRstEvent();
   
   // Handlers that require access to some of the internal interfaces on the proxy manager have to go through the IInternalUnknown interface. This prevents handlers from blindly delegating and exposing the aggregatee's internal interfaces outside of the aggregate. These interfaces include IProxyManager, IClientSecurity, and IMultiQI. If the handler wants to expose IClientSecurity or IMultiQI, it should implement them itself. 
   // In the case of IClientSecurity, if the client tries to set the security on an interface that the handler has exposed, the handler should set the security on the underlying network interface proxy.
   // In the case of IMultiQI, the handler should fill in the interfaces it knows about and then forward the call to the proxy manager to get the rest of the interfaces filled in.
   STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
   STDMETHODIMP_(ULONG) AddRef(void);
   STDMETHODIMP_(ULONG) Release(void);
   
   // The WillChangeField event is called before a pending operation changes the value of one or more Field objects in the Recordset. The FieldChangeComplete event is called after the value of one or more Field objects has changed.
   STDMETHODIMP raw_WillChangeField(LONG cFields, VARIANT Fields, EventStatusEnum * adStatus, struct _Recordset * pRecordset);
   
   // The WillChangeField event is called before a pending operation changes the value of one or more Field objects in the Recordset. The FieldChangeComplete event is called after the value of one or more Field objects has changed.
   STDMETHODIMP raw_FieldChangeComplete(LONG cFields, VARIANT Fields, struct Error * pError, EventStatusEnum * adStatus, struct _Recordset * pRecordset);
   
   // The WillChangeRecord event is called before one or more records (rows) in the Recordset change. The RecordChangeComplete event is called after one or more records change.
   STDMETHODIMP raw_WillChangeRecord(EventReasonEnum adReason, LONG cRecords, EventStatusEnum * adStatus, struct _Recordset * pRecordset);
   
   // The WillChangeRecord event is called before one or more records (rows) in the Recordset change. The RecordChangeComplete event is called after one or more records change.
   STDMETHODIMP raw_RecordChangeComplete(EventReasonEnum adReason, LONG cRecords, struct Error * pError, EventStatusEnum * adStatus, struct _Recordset * pRecordset);
   
   // The WillChangeRecordset event is called before a pending operation changes the Recordset. The RecordsetChangeComplete event is called after the Recordset has changed.
   STDMETHODIMP raw_WillChangeRecordset(EventReasonEnum adReason, EventStatusEnum * adStatus, struct _Recordset * pRecordset);
   
   // The WillChangeRecordset event is called before a pending operation changes the Recordset. The RecordsetChangeComplete event is called after the Recordset has changed.
   STDMETHODIMP raw_RecordsetChangeComplete(EventReasonEnum adReason, struct Error * pError, EventStatusEnum * adStatus, struct _Recordset * pRecordset);
   
   // The WillMove event is called before a pending operation changes the current position in the Recordset. The MoveComplete event is called after the current position in the Recordset changes.
   STDMETHODIMP raw_WillMove(EventReasonEnum adReason, EventStatusEnum * adStatus, struct _Recordset * pRecordset);
   
   // The WillMove event is called before a pending operation changes the current position in the Recordset. The MoveComplete event is called after the current position in the Recordset changes.
   STDMETHODIMP raw_MoveComplete(EventReasonEnum adReason, struct Error * pError, EventStatusEnum * adStatus, struct _Recordset * pRecordset);
   
   // The EndOfRecordset event is called when there is an attempt to move to a row past the end of the Recordset.
   STDMETHODIMP raw_EndOfRecordset(VARIANT_BOOL * fMoreData, EventStatusEnum * adStatus, struct _Recordset * pRecordset);
   
   // The FetchProgress event is called periodically during a lengthy asynchronous operation to report how many more rows have currently been retrieved into the Recordset.
   STDMETHODIMP raw_FetchProgress(long Progress, long MaxProgress, EventStatusEnum * adStatus, struct _Recordset * pRecordset);
   
   // The FetchComplete event is called after all the records in a lengthy asynchronous operation have been retrieved into the Recordset.
   STDMETHODIMP raw_FetchComplete(struct Error * pError, EventStatusEnum * adStatus, struct _Recordset * pRecordset);

protected:
private:
   ULONG m_nRef;

};

#endif