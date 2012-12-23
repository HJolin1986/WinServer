/***********************************************************************
 * Module:  CTitleauthor.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CTitleauthor
 ***********************************************************************/

#if !defined(__ADOPubs_CTitleauthor_h)
#define __ADOPubs_CTitleauthor_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CTitleauthor : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CTitleauthor)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szAuId, sizeof(m_szAuId), m_nAuIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adVarChar, m_szTitleId, sizeof(m_szTitleId), m_nTitleIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adInteger, m_nAuOrd, sizeof(m_nAuOrd), m_nAuOrdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adInteger, m_nRoyaltyper, sizeof(m_nRoyaltyper), m_nRoyaltyperStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // au_id 
   CHARARRAY m_szAuId;
   ULONG m_nAuIdStatus;
   
   // title_id 
   CHARARRAY m_szTitleId;
   ULONG m_nTitleIdStatus;
   
   // au_ord 
   short m_nAuOrd;
   ULONG m_nAuOrdStatus;
   
   // royaltyper 
   int m_nRoyaltyper;
   ULONG m_nRoyaltyperStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CTitleauthor*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CTitleauthor()
   {      
      m_nFirst = (int)(&((CTitleauthor*)0)->m_nFirst);
      m_nLast = (int)(&((CTitleauthor*)0)->m_nLast);
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
	   CTitleauthor* tmp = new CTitleauthor[size];
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
      char *pd = (char*)dynamic_cast<CTitleauthor*>(cur);
      return dynamic_cast<CTitleauthor*>((PDBBaseClass)(pd + sizeof(CTitleauthor)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CTitleauthor) * 2;
      static char buf[2*sizeof(CTitleauthor)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"AuId=%s%s",m_nAuIdStatus == S_OK?m_szAuId:"",tab);
      iter += sprintf_s(buf+iter,size,"TitleId=%s%s",m_nTitleIdStatus == S_OK?m_szTitleId:"",tab);
      iter += sprintf_s(buf+iter,size,"AuOrd=%d%s",m_nAuOrdStatus == S_OK?m_nAuOrd:0,tab);
      iter += sprintf_s(buf+iter,size,"Royaltyper=%d%s",m_nRoyaltyperStatus == S_OK?m_nRoyaltyper:0,tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CTitleauthor::_m_szString("titleauthor");
DBCacheAdapter<CTitleauthor> g_vCTitleauthor;

#endif