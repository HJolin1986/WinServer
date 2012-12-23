/***********************************************************************
 * Module:  CPublishers.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CPublishers
 ***********************************************************************/

#if !defined(__ADOPubs_CPublishers_h)
#define __ADOPubs_CPublishers_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CPublishers : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CPublishers)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szPubId, sizeof(m_szPubId), m_nPubIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adVarChar, m_szPubName, sizeof(m_szPubName), m_nPubNameStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adVarChar, m_szCity, sizeof(m_szCity), m_nCityStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adVarChar, m_szState, sizeof(m_szState), m_nStateStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(5, DataTypeEnum::adVarChar, m_szCountry, sizeof(m_szCountry), m_nCountryStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // pub_id 
   CHARARRAY m_szPubId;
   ULONG m_nPubIdStatus;
   
   // pub_name 
   CHARARRAY m_szPubName;
   ULONG m_nPubNameStatus;
   
   // city 
   CHARARRAY m_szCity;
   ULONG m_nCityStatus;
   
   // state 
   CHARARRAY m_szState;
   ULONG m_nStateStatus;
   
   // country 
   CHARARRAY m_szCountry;
   ULONG m_nCountryStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CPublishers*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CPublishers()
   {      
      m_nFirst = (int)(&((CPublishers*)0)->m_nFirst);
      m_nLast = (int)(&((CPublishers*)0)->m_nLast);
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
	   CPublishers* tmp = new CPublishers[size];
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
      char *pd = (char*)dynamic_cast<CPublishers*>(cur);
      return dynamic_cast<CPublishers*>((PDBBaseClass)(pd + sizeof(CPublishers)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CPublishers) * 2;
      static char buf[2*sizeof(CPublishers)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"PubId=%s%s",m_nPubIdStatus == S_OK?m_szPubId:"",tab);
      iter += sprintf_s(buf+iter,size,"PubName=%s%s",m_nPubNameStatus == S_OK?m_szPubName:"",tab);
      iter += sprintf_s(buf+iter,size,"City=%s%s",m_nCityStatus == S_OK?m_szCity:"",tab);
      iter += sprintf_s(buf+iter,size,"State=%s%s",m_nStateStatus == S_OK?m_szState:"",tab);
      iter += sprintf_s(buf+iter,size,"Country=%s%s",m_nCountryStatus == S_OK?m_szCountry:"",tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CPublishers::_m_szString("publishers");
DBCacheAdapter<CPublishers> g_vCPublishers;

#endif