/***********************************************************************
 * Module:  CPubInfo.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CPubInfo
 ***********************************************************************/

#if !defined(__ADOPubs_CPubInfo_h)
#define __ADOPubs_CPubInfo_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CPubInfo : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CPubInfo)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szPubId, sizeof(m_szPubId), m_nPubIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adVarChar, m_szLogo, sizeof(m_szLogo), m_nLogoStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adVarChar, m_szPrInfo, sizeof(m_szPrInfo), m_nPrInfoStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // pub_id 
   CHARARRAY m_szPubId;
   ULONG m_nPubIdStatus;
   
   // logo 
   CHARARRAY m_szLogo;
   ULONG m_nLogoStatus;
   
   // pr_info 
   CHARARRAY m_szPrInfo;
   ULONG m_nPrInfoStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CPubInfo*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CPubInfo()
   {      
      m_nFirst = (int)(&((CPubInfo*)0)->m_nFirst);
      m_nLast = (int)(&((CPubInfo*)0)->m_nLast);
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
	   CPubInfo* tmp = new CPubInfo[size];
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
      char *pd = (char*)dynamic_cast<CPubInfo*>(cur);
      return dynamic_cast<CPubInfo*>((PDBBaseClass)(pd + sizeof(CPubInfo)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CPubInfo) * 2;
      static char buf[2*sizeof(CPubInfo)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"PubId=%s%s",m_nPubIdStatus == S_OK?m_szPubId:"",tab);
      iter += sprintf_s(buf+iter,size,"Logo=%s%s",m_nLogoStatus == S_OK?m_szLogo:"",tab);
      iter += sprintf_s(buf+iter,size,"PrInfo=%s%s",m_nPrInfoStatus == S_OK?m_szPrInfo:"",tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CPubInfo::_m_szString("pub_info");
DBCacheAdapter<CPubInfo> g_vCPubInfo;

#endif