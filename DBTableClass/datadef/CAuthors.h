/***********************************************************************
 * Module:  CAuthors.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CAuthors
 ***********************************************************************/

#if !defined(__ADOPubs_CAuthors_h)
#define __ADOPubs_CAuthors_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CAuthors : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CAuthors)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szAuId, sizeof(m_szAuId), m_nAuIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adVarChar, m_szAuLname, sizeof(m_szAuLname), m_nAuLnameStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adVarChar, m_szAuFname, sizeof(m_szAuFname), m_nAuFnameStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adVarChar, m_szPhone, sizeof(m_szPhone), m_nPhoneStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(5, DataTypeEnum::adVarChar, m_szAddress, sizeof(m_szAddress), m_nAddressStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(6, DataTypeEnum::adVarChar, m_szCity, sizeof(m_szCity), m_nCityStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(7, DataTypeEnum::adVarChar, m_szState, sizeof(m_szState), m_nStateStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(8, DataTypeEnum::adVarChar, m_szZip, sizeof(m_szZip), m_nZipStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(9, DataTypeEnum::adInteger, m_bContract, sizeof(m_bContract), m_nContractStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // au_id 
   CHARARRAY m_szAuId;
   ULONG m_nAuIdStatus;
   
   // au_lname 
   CHARARRAY m_szAuLname;
   ULONG m_nAuLnameStatus;
   
   // au_fname 
   CHARARRAY m_szAuFname;
   ULONG m_nAuFnameStatus;
   
   // phone 
   CHARARRAY m_szPhone;
   ULONG m_nPhoneStatus;
   
   // address 
   CHARARRAY m_szAddress;
   ULONG m_nAddressStatus;
   
   // city 
   CHARARRAY m_szCity;
   ULONG m_nCityStatus;
   
   // state 
   CHARARRAY m_szState;
   ULONG m_nStateStatus;
   
   // zip 
   CHARARRAY m_szZip;
   ULONG m_nZipStatus;
   
   // contract 
   bool m_bContract;
   ULONG m_nContractStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CAuthors*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CAuthors()
   {      
      m_nFirst = (int)(&((CAuthors*)0)->m_nFirst);
      m_nLast = (int)(&((CAuthors*)0)->m_nLast);
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
	   CAuthors* tmp = new CAuthors[size];
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
      char *pd = (char*)dynamic_cast<CAuthors*>(cur);
      return dynamic_cast<CAuthors*>((PDBBaseClass)(pd + sizeof(CAuthors)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CAuthors) * 2;
      static char buf[2*sizeof(CAuthors)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"AuId=%s%s",m_nAuIdStatus == S_OK?m_szAuId:"",tab);
      iter += sprintf_s(buf+iter,size,"AuLname=%s%s",m_nAuLnameStatus == S_OK?m_szAuLname:"",tab);
      iter += sprintf_s(buf+iter,size,"AuFname=%s%s",m_nAuFnameStatus == S_OK?m_szAuFname:"",tab);
      iter += sprintf_s(buf+iter,size,"Phone=%s%s",m_nPhoneStatus == S_OK?m_szPhone:"",tab);
      iter += sprintf_s(buf+iter,size,"Address=%s%s",m_nAddressStatus == S_OK?m_szAddress:"",tab);
      iter += sprintf_s(buf+iter,size,"City=%s%s",m_nCityStatus == S_OK?m_szCity:"",tab);
      iter += sprintf_s(buf+iter,size,"State=%s%s",m_nStateStatus == S_OK?m_szState:"",tab);
      iter += sprintf_s(buf+iter,size,"Zip=%s%s",m_nZipStatus == S_OK?m_szZip:"",tab);
      iter += sprintf_s(buf+iter,size,"Contract=%c%s",m_nContractStatus == S_OK?m_bContract:0,tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CAuthors::_m_szString("authors");
DBCacheAdapter<CAuthors> g_vCAuthors;

#endif