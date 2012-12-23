/***********************************************************************
 * Module:  CSales.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CSales
 ***********************************************************************/

#if !defined(__ADOPubs_CSales_h)
#define __ADOPubs_CSales_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CSales : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CSales)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szStorId, sizeof(m_szStorId), m_nStorIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adVarChar, m_szOrdNum, sizeof(m_szOrdNum), m_nOrdNumStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adDBTimeStamp, m_OrdDate, sizeof(m_OrdDate), m_nOrdDateStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adInteger, m_nQty, sizeof(m_nQty), m_nQtyStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(5, DataTypeEnum::adVarChar, m_szPayterms, sizeof(m_szPayterms), m_nPaytermsStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(6, DataTypeEnum::adVarChar, m_szTitleId, sizeof(m_szTitleId), m_nTitleIdStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // stor_id 
   CHARARRAY m_szStorId;
   ULONG m_nStorIdStatus;
   
   // ord_num 
   CHARARRAY m_szOrdNum;
   ULONG m_nOrdNumStatus;
   
   // ord_date 
   long m_OrdDate;
   ULONG m_nOrdDateStatus;
   
   // qty 
   short m_nQty;
   ULONG m_nQtyStatus;
   
   // payterms 
   CHARARRAY m_szPayterms;
   ULONG m_nPaytermsStatus;
   
   // title_id 
   CHARARRAY m_szTitleId;
   ULONG m_nTitleIdStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CSales*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CSales()
   {      
      m_nFirst = (int)(&((CSales*)0)->m_nFirst);
      m_nLast = (int)(&((CSales*)0)->m_nLast);
      int datalen = m_nLast - m_nFirst;
      char * ps = (char*)this;
      memset(ps+m_nFirst+sizeof(m_nFirst), 0, datalen-sizeof(m_nFirst));
   }
   inline _bstr_t GetTableName()
   {
      return _m_szString;
   }
   inline PDBBaseClass* AllocInstances(int size)
   {
	   CSales* tmp = new CSales[size];
	   PDBBaseClass* result = new PDBBaseClass[size];
	   for (int iter = 0; iter < size ; iter++)
	   {
	 	   result[iter] = dynamic_cast<PDBBaseClass>(tmp+iter);
	   }
      return result;
   }
   inline void Release(PDBBaseClass* pdst)
   {
      delete[] *pdst;
      delete[] pdst;
   }
   inline PDBBaseClass GetDownEdge(PDBBaseClass cur)
   {
      char *pd = (char*)dynamic_cast<CSales*>(cur);
      return dynamic_cast<CSales*>((PDBBaseClass)(pd + sizeof(CSales)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CSales) * 2;
      static char buf[2*sizeof(CSales)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"StorId=%s%s",m_nStorIdStatus == S_OK?m_szStorId:"",tab);
      iter += sprintf_s(buf+iter,size,"OrdNum=%s%s",m_nOrdNumStatus == S_OK?m_szOrdNum:"",tab);
      iter += sprintf_s(buf+iter,size,"OrdDate=%ld%s",m_nOrdDateStatus == S_OK?m_OrdDate:0,tab);
      iter += sprintf_s(buf+iter,size,"Qty=%d%s",m_nQtyStatus == S_OK?m_nQty:0,tab);
      iter += sprintf_s(buf+iter,size,"Payterms=%s%s",m_nPaytermsStatus == S_OK?m_szPayterms:"",tab);
      iter += sprintf_s(buf+iter,size,"TitleId=%s%s",m_nTitleIdStatus == S_OK?m_szTitleId:"",tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CSales::_m_szString("sales");
DBCacheAdapter<CSales> g_vCSales;

#endif