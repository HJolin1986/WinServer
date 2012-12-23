/***********************************************************************
 * Module:  CEmployee.h
 * Author:  dailm
 * Modified: 2012年12月7日 12:54:36
 * Purpose: Declaration of the class CEmployee
 ***********************************************************************/

#if !defined(__ADOPubs_CEmployee_h)
#define __ADOPubs_CEmployee_h

#include "../stdafx.h"
#include "../DBCacheMgr.h"
#include "../DBDataAdapter.h"


class CEmployee : public DBDataAdapter
{
   BEGIN_ADO_BINDING(CEmployee)
   ADO_VARIABLE_LENGTH_ENTRY2(1, DataTypeEnum::adVarChar, m_szEmpId, sizeof(m_szEmpId), m_nEmpIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(2, DataTypeEnum::adVarChar, m_szFname, sizeof(m_szFname), m_nFnameStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(3, DataTypeEnum::adVarChar, m_szMinit, sizeof(m_szMinit), m_nMinitStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(4, DataTypeEnum::adVarChar, m_szLname, sizeof(m_szLname), m_nLnameStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(5, DataTypeEnum::adInteger, m_nJobId, sizeof(m_nJobId), m_nJobIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(6, DataTypeEnum::adInteger, m_nJobLvl, sizeof(m_nJobLvl), m_nJobLvlStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(7, DataTypeEnum::adVarChar, m_szPubId, sizeof(m_szPubId), m_nPubIdStatus, TRUE)
   ADO_VARIABLE_LENGTH_ENTRY2(8, DataTypeEnum::adDBTimeStamp, m_HireDate, sizeof(m_HireDate), m_nHireDateStatus, TRUE)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   
   // emp_id 
   CHARARRAY m_szEmpId;
   ULONG m_nEmpIdStatus;
   
   // fname 
   CHARARRAY m_szFname;
   ULONG m_nFnameStatus;
   
   // minit 
   CHARARRAY m_szMinit;
   ULONG m_nMinitStatus;
   
   // lname 
   CHARARRAY m_szLname;
   ULONG m_nLnameStatus;
   
   // job_id 
   short m_nJobId;
   ULONG m_nJobIdStatus;
   
   // job_lvl 
   short m_nJobLvl;
   ULONG m_nJobLvlStatus;
   
   // pub_id 
   CHARARRAY m_szPubId;
   ULONG m_nPubIdStatus;
   
   // hire_date 
   long m_HireDate;
   ULONG m_nHireDateStatus;
   int m_nLast;   // 用于计算数据的结束位置
   inline PDBBaseClass CopyData(PDBBaseClass pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<CEmployee*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline CEmployee()
   {      
      m_nFirst = (int)(&((CEmployee*)0)->m_nFirst);
      m_nLast = (int)(&((CEmployee*)0)->m_nLast);
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
	   CEmployee* tmp = new CEmployee[size];
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
      char *pd = (char*)dynamic_cast<CEmployee*>(cur);
      return dynamic_cast<CEmployee*>((PDBBaseClass)(pd + sizeof(CEmployee)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(CEmployee) * 2;
      static char buf[2*sizeof(CEmployee)];
      memset(buf, 0, size);
      int iter = 0;
      iter += sprintf_s(buf+iter,size,"EmpId=%s%s",m_nEmpIdStatus == S_OK?m_szEmpId:"",tab);
      iter += sprintf_s(buf+iter,size,"Fname=%s%s",m_nFnameStatus == S_OK?m_szFname:"",tab);
      iter += sprintf_s(buf+iter,size,"Minit=%s%s",m_nMinitStatus == S_OK?m_szMinit:"",tab);
      iter += sprintf_s(buf+iter,size,"Lname=%s%s",m_nLnameStatus == S_OK?m_szLname:"",tab);
      iter += sprintf_s(buf+iter,size,"JobId=%d%s",m_nJobIdStatus == S_OK?m_nJobId:0,tab);
      iter += sprintf_s(buf+iter,size,"JobLvl=%d%s",m_nJobLvlStatus == S_OK?m_nJobLvl:0,tab);
      iter += sprintf_s(buf+iter,size,"PubId=%s%s",m_nPubIdStatus == S_OK?m_szPubId:"",tab);
      iter += sprintf_s(buf+iter,size,"HireDate=%ld%s",m_nHireDateStatus == S_OK?m_HireDate:0,tab);
      *(buf+iter-strlen(tab)) = 0;
      return buf;
   }
protected:

private:
};

_bstr_t CEmployee::_m_szString("employee");
DBCacheAdapter<CEmployee> g_vCEmployee;

#endif