/***********************************************************************
 * Module:  CJobs.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CJobs
 ***********************************************************************/

#if !defined(__ADOPubs_CJobs_h)
#define __ADOPubs_CJobs_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CJobs : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CJobs)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adInteger, m_nJobId, sizeof(m_nJobId), m_nJobIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adVarChar, m_szJobDesc, sizeof(m_szJobDesc), m_nJobDescStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adInteger, m_nMinLvl, sizeof(m_nMinLvl), m_nMinLvlStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adInteger, m_nMaxLvl, sizeof(m_nMaxLvl), m_nMaxLvlStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // job_id 
   long m_nJobId;
   ULONG m_nJobIdStatus;
   
   // job_desc 
   CHARARRAY m_szJobDesc;
   ULONG m_nJobDescStatus;
   
   // min_lvl 
   short m_nMinLvl;
   ULONG m_nMinLvlStatus;
   
   // max_lvl 
   short m_nMaxLvl;
   ULONG m_nMaxLvlStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CJobs*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CJobs()
   {      
      m_nFirst = (int)(&((CJobs*)0)->m_nFirst);
      m_nLast = (int)(&((CJobs*)0)->m_nLast);
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
	   CJobs* tmp = new CJobs[size];
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
      char *pd = (char*)dynamic_cast<CJobs*>(cur);
      return dynamic_cast<CJobs*>((PDBBaseClass)(pd + sizeof(CJobs)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CJobs) * 2;
      static char buf[2*sizeof(CJobs)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"JobId=%d%s",m_nJobIdStatus == S_OK?m_nJobId:0,tab);
      iter += sprintf_s(buf+iter,size,"JobDesc=%s%s",m_nJobDescStatus == S_OK?m_szJobDesc:"",tab);
      iter += sprintf_s(buf+iter,size,"MinLvl=%d%s",m_nMinLvlStatus == S_OK?m_nMinLvl:0,tab);
      iter += sprintf_s(buf+iter,size,"MaxLvl=%d%s",m_nMaxLvlStatus == S_OK?m_nMaxLvl:0,tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CJobs::_m_szString("jobs");
DBCacheAdapter<CJobs> g_vCJobs;

#endif