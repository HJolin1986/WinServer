/***********************************************************************
 * Module:  CDiscounts.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CDiscounts
 ***********************************************************************/

#if !defined(__ADOPubs_CDiscounts_h)
#define __ADOPubs_CDiscounts_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CDiscounts : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CDiscounts)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szDiscounttype, sizeof(m_szDiscounttype), m_nDiscounttypeStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adVarChar, m_szStorId, sizeof(m_szStorId), m_nStorIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adInteger, m_nLowqty, sizeof(m_nLowqty), m_nLowqtyStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adInteger, m_nHighqty, sizeof(m_nHighqty), m_nHighqtyStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(5, DataTypeEnum::adDouble, m_fDiscount, sizeof(m_fDiscount), m_nDiscountStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // discounttype 
   CHARARRAY m_szDiscounttype;
   ULONG m_nDiscounttypeStatus;
   
   // stor_id 
   CHARARRAY m_szStorId;
   ULONG m_nStorIdStatus;
   
   // lowqty 
   short m_nLowqty;
   ULONG m_nLowqtyStatus;
   
   // highqty 
   short m_nHighqty;
   ULONG m_nHighqtyStatus;
   
   // discount 
   double m_fDiscount;
   ULONG m_nDiscountStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CDiscounts*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CDiscounts()
   {      
      m_nFirst = (int)(&((CDiscounts*)0)->m_nFirst);
      m_nLast = (int)(&((CDiscounts*)0)->m_nLast);
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
	   CDiscounts* tmp = new CDiscounts[size];
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
      char *pd = (char*)dynamic_cast<CDiscounts*>(cur);
      return dynamic_cast<CDiscounts*>((PDBBaseClass)(pd + sizeof(CDiscounts)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CDiscounts) * 2;
      static char buf[2*sizeof(CDiscounts)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"Discounttype=%s%s",m_nDiscounttypeStatus == S_OK?m_szDiscounttype:"",tab);
      iter += sprintf_s(buf+iter,size,"StorId=%s%s",m_nStorIdStatus == S_OK?m_szStorId:"",tab);
      iter += sprintf_s(buf+iter,size,"Lowqty=%d%s",m_nLowqtyStatus == S_OK?m_nLowqty:0,tab);
      iter += sprintf_s(buf+iter,size,"Highqty=%d%s",m_nHighqtyStatus == S_OK?m_nHighqty:0,tab);
      iter += sprintf_s(buf+iter,size,"Discount=%lf%s",m_nDiscountStatus == S_OK?m_fDiscount:0,tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CDiscounts::_m_szString("discounts");
DBCacheAdapter<CDiscounts> g_vCDiscounts;

#endif