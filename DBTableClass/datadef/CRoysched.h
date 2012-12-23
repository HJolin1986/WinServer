/***********************************************************************
 * Module:  CRoysched.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CRoysched
 ***********************************************************************/

#if !defined(__ADOPubs_CRoysched_h)
#define __ADOPubs_CRoysched_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CRoysched : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CRoysched)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szTitleId, sizeof(m_szTitleId), m_nTitleIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adInteger, m_nLorange, sizeof(m_nLorange), m_nLorangeStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adInteger, m_nHirange, sizeof(m_nHirange), m_nHirangeStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adInteger, m_nRoyalty, sizeof(m_nRoyalty), m_nRoyaltyStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // title_id 
   CHARARRAY m_szTitleId;
   ULONG m_nTitleIdStatus;
   
   // lorange 
   int m_nLorange;
   ULONG m_nLorangeStatus;
   
   // hirange 
   int m_nHirange;
   ULONG m_nHirangeStatus;
   
   // royalty 
   int m_nRoyalty;
   ULONG m_nRoyaltyStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CRoysched*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CRoysched()
   {      
      m_nFirst = (int)(&((CRoysched*)0)->m_nFirst);
      m_nLast = (int)(&((CRoysched*)0)->m_nLast);
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
	   CRoysched* tmp = new CRoysched[size];
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
      char *pd = (char*)dynamic_cast<CRoysched*>(cur);
      return dynamic_cast<CRoysched*>((PDBBaseClass)(pd + sizeof(CRoysched)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CRoysched) * 2;
      static char buf[2*sizeof(CRoysched)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"TitleId=%s%s",m_nTitleIdStatus == S_OK?m_szTitleId:"",tab);
      iter += sprintf_s(buf+iter,size,"Lorange=%d%s",m_nLorangeStatus == S_OK?m_nLorange:0,tab);
      iter += sprintf_s(buf+iter,size,"Hirange=%d%s",m_nHirangeStatus == S_OK?m_nHirange:0,tab);
      iter += sprintf_s(buf+iter,size,"Royalty=%d%s",m_nRoyaltyStatus == S_OK?m_nRoyalty:0,tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CRoysched::_m_szString("roysched");
DBCacheAdapter<CRoysched> g_vCRoysched;

#endif