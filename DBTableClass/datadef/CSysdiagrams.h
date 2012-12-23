/***********************************************************************
 * Module:  CSysdiagrams.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CSysdiagrams
 ***********************************************************************/

#if !defined(__ADOPubs_CSysdiagrams_h)
#define __ADOPubs_CSysdiagrams_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CSysdiagrams : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CSysdiagrams)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szName, sizeof(m_szName), m_nNameStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adInteger, m_nPrincipalId, sizeof(m_nPrincipalId), m_nPrincipalIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adInteger, m_nDiagramId, sizeof(m_nDiagramId), m_nDiagramIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adInteger, m_nVersion, sizeof(m_nVersion), m_nVersionStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(5, DataTypeEnum::adVarChar, m_szDefinition, sizeof(m_szDefinition), m_nDefinitionStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // name 
   char m_szName[NAME];
   ULONG m_nNameStatus;
   
   // principal_id 
   int m_nPrincipalId;
   ULONG m_nPrincipalIdStatus;
   
   // diagram_id 
   long m_nDiagramId;
   ULONG m_nDiagramIdStatus;
   
   // version 
   int m_nVersion;
   ULONG m_nVersionStatus;
   
   // definition 
   CHARARRAY m_szDefinition;
   ULONG m_nDefinitionStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CSysdiagrams*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CSysdiagrams()
   {      
      m_nFirst = (int)(&((CSysdiagrams*)0)->m_nFirst);
      m_nLast = (int)(&((CSysdiagrams*)0)->m_nLast);
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
	   CSysdiagrams* tmp = new CSysdiagrams[size];
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
      char *pd = (char*)dynamic_cast<CSysdiagrams*>(cur);
      return dynamic_cast<CSysdiagrams*>((PDBBaseClass)(pd + sizeof(CSysdiagrams)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CSysdiagrams) * 2;
      static char buf[2*sizeof(CSysdiagrams)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"Name=%s%s",m_nNameStatus == S_OK?m_szName:"",tab);
      iter += sprintf_s(buf+iter,size,"PrincipalId=%d%s",m_nPrincipalIdStatus == S_OK?m_nPrincipalId:0,tab);
      iter += sprintf_s(buf+iter,size,"DiagramId=%d%s",m_nDiagramIdStatus == S_OK?m_nDiagramId:0,tab);
      iter += sprintf_s(buf+iter,size,"Version=%d%s",m_nVersionStatus == S_OK?m_nVersion:0,tab);
      iter += sprintf_s(buf+iter,size,"Definition=%s%s",m_nDefinitionStatus == S_OK?m_szDefinition:"",tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CSysdiagrams::_m_szString("sysdiagrams");
DBCacheAdapter<CSysdiagrams> g_vCSysdiagrams;

#endif