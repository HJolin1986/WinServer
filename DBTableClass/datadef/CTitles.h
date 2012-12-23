/***********************************************************************
 * Module:  CTitles.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CTitles
 ***********************************************************************/

#if !defined(__ADOPubs_CTitles_h)
#define __ADOPubs_CTitles_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CTitles : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CTitles)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szTitleId, sizeof(m_szTitleId), m_nTitleIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adVarChar, m_szTitle, sizeof(m_szTitle), m_nTitleStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adVarChar, m_szType, sizeof(m_szType), m_nTypeStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adVarChar, m_szPubId, sizeof(m_szPubId), m_nPubIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(5, DataTypeEnum::adDouble, m_fPrice, sizeof(m_fPrice), m_nPriceStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(6, DataTypeEnum::adDouble, m_fAdvance, sizeof(m_fAdvance), m_nAdvanceStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(7, DataTypeEnum::adInteger, m_nRoyalty, sizeof(m_nRoyalty), m_nRoyaltyStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(8, DataTypeEnum::adInteger, m_nYtdSales, sizeof(m_nYtdSales), m_nYtdSalesStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(9, DataTypeEnum::adVarChar, m_szNotes, sizeof(m_szNotes), m_nNotesStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(10, DataTypeEnum::adDBTimeStamp, m_Pubdate, sizeof(m_Pubdate), m_nPubdateStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // title_id 
   CHARARRAY m_szTitleId;
   ULONG m_nTitleIdStatus;
   
   // title 
   CHARARRAY m_szTitle;
   ULONG m_nTitleStatus;
   
   // type 
   CHARARRAY m_szType;
   ULONG m_nTypeStatus;
   
   // pub_id 
   CHARARRAY m_szPubId;
   ULONG m_nPubIdStatus;
   
   // price 
   double m_fPrice;
   ULONG m_nPriceStatus;
   
   // advance 
   double m_fAdvance;
   ULONG m_nAdvanceStatus;
   
   // royalty 
   int m_nRoyalty;
   ULONG m_nRoyaltyStatus;
   
   // ytd_sales 
   int m_nYtdSales;
   ULONG m_nYtdSalesStatus;
   
   // notes 
   CHARARRAY m_szNotes;
   ULONG m_nNotesStatus;
   
   // pubdate 
   long m_Pubdate;
   ULONG m_nPubdateStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CTitles*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CTitles()
   {      
      m_nFirst = (int)(&((CTitles*)0)->m_nFirst);
      m_nLast = (int)(&((CTitles*)0)->m_nLast);
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
	   CTitles* tmp = new CTitles[size];
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
      char *pd = (char*)dynamic_cast<CTitles*>(cur);
      return dynamic_cast<CTitles*>((PDBBaseClass)(pd + sizeof(CTitles)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CTitles) * 2;
      static char buf[2*sizeof(CTitles)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"TitleId=%s%s",m_nTitleIdStatus == S_OK?m_szTitleId:"",tab);
      iter += sprintf_s(buf+iter,size,"Title=%s%s",m_nTitleStatus == S_OK?m_szTitle:"",tab);
      iter += sprintf_s(buf+iter,size,"Type=%s%s",m_nTypeStatus == S_OK?m_szType:"",tab);
      iter += sprintf_s(buf+iter,size,"PubId=%s%s",m_nPubIdStatus == S_OK?m_szPubId:"",tab);
      iter += sprintf_s(buf+iter,size,"Price=%lf%s",m_nPriceStatus == S_OK?m_fPrice:0,tab);
      iter += sprintf_s(buf+iter,size,"Advance=%lf%s",m_nAdvanceStatus == S_OK?m_fAdvance:0,tab);
      iter += sprintf_s(buf+iter,size,"Royalty=%d%s",m_nRoyaltyStatus == S_OK?m_nRoyalty:0,tab);
      iter += sprintf_s(buf+iter,size,"YtdSales=%d%s",m_nYtdSalesStatus == S_OK?m_nYtdSales:0,tab);
      iter += sprintf_s(buf+iter,size,"Notes=%s%s",m_nNotesStatus == S_OK?m_szNotes:"",tab);
      iter += sprintf_s(buf+iter,size,"Pubdate=%ld%s",m_nPubdateStatus == S_OK?m_Pubdate:0,tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CTitles::_m_szString("titles");
DBCacheAdapter<CTitles> g_vCTitles;

#endif