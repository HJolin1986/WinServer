/***********************************************************************
 * Module:  CStores.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CStores
 ***********************************************************************/

#if !defined(__ADOPubs_CStores_h)
#define __ADOPubs_CStores_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CStores : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CStores)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szStorId, sizeof(m_szStorId), m_nStorIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adVarChar, m_szStorName, sizeof(m_szStorName), m_nStorNameStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adVarChar, m_szStorAddress, sizeof(m_szStorAddress), m_nStorAddressStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adVarChar, m_szCity, sizeof(m_szCity), m_nCityStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(5, DataTypeEnum::adVarChar, m_szState, sizeof(m_szState), m_nStateStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(6, DataTypeEnum::adVarChar, m_szZip, sizeof(m_szZip), m_nZipStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // stor_id 
   CHARARRAY m_szStorId;
   ULONG m_nStorIdStatus;
   
   // stor_name 
   CHARARRAY m_szStorName;
   ULONG m_nStorNameStatus;
   
   // stor_address 
   CHARARRAY m_szStorAddress;
   ULONG m_nStorAddressStatus;
   
   // city 
   CHARARRAY m_szCity;
   ULONG m_nCityStatus;
   
   // state 
   CHARARRAY m_szState;
   ULONG m_nStateStatus;
   
   // zip 
   CHARARRAY m_szZip;
   ULONG m_nZipStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CStores*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CStores()
   {      
      m_nFirst = (int)(&((CStores*)0)->m_nFirst);
      m_nLast = (int)(&((CStores*)0)->m_nLast);
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
	   CStores* tmp = new CStores[size];
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
      char *pd = (char*)dynamic_cast<CStores*>(cur);
      return dynamic_cast<CStores*>((PDBBaseClass)(pd + sizeof(CStores)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CStores) * 2;
      static char buf[2*sizeof(CStores)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"StorId=%s%s",m_nStorIdStatus == S_OK?m_szStorId:"",tab);
      iter += sprintf_s(buf+iter,size,"StorName=%s%s",m_nStorNameStatus == S_OK?m_szStorName:"",tab);
      iter += sprintf_s(buf+iter,size,"StorAddress=%s%s",m_nStorAddressStatus == S_OK?m_szStorAddress:"",tab);
      iter += sprintf_s(buf+iter,size,"City=%s%s",m_nCityStatus == S_OK?m_szCity:"",tab);
      iter += sprintf_s(buf+iter,size,"State=%s%s",m_nStateStatus == S_OK?m_szState:"",tab);
      iter += sprintf_s(buf+iter,size,"Zip=%s%s",m_nZipStatus == S_OK?m_szZip:"",tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CStores::_m_szString("stores");
DBCacheAdapter<CStores> g_vCStores;

#endif