/***********************************************************************
 * Module:  Class47.h
 * Author:  dailm
 * Modified: 2012年12月4日 14:57:11
 * Purpose: Declaration of the class Class47
 ***********************************************************************/

#if !defined(__ADOaviation_Class47_h)
#define __ADOaviation_Class47_h

#include "stdafx.h"
#include "../DBCacheMgr.h"

#include "CFlightenvelope.h"
#include "CPortname.h"
#include "CPort.h"
#include "CTelegraph.h"
#include "CDescspdrate.h"
#include "CClimbspdrate.h"
#include "CPlaneInfo.h"
#include "CFlightInfo.h"
#include "CExcisePlane.h"
#include "CAirports.h"
#include "CDeceacccurve.h"
#include "CBelowacccurve.h"
#include "CAboveacccurve.h"
#include "CFlightEvent.h"
#include "CLine.h"
#include "CExcise.h"
#include "CSpeedsimu.h"
#include "CStandposition.h"
#include "CAirways.h"
#include "CDuration.h"
#include "CEvent.h"
#include "CGateposition.h"
#include "CAltsimu.h"
#include "CDvset.h"
#include "CPath.h"
#include "CGroundsimu.h"
#include "CStar.h"
#include "CTaxiway.h"
#include "CPlaneType.h"
#include "CAirportWorld.h"
#include "CRecord.h"
#include "CDuty.h"
#include "CBuilding.h"
#include "CZhuanpan.h"
#include "CRunways.h"
#include "CDegree.h"
#include "CSpeed.h"
#include "CFlightLevel.h"
#include "CParkapron.h"
#include "CPcsituation.h"
#include "CCounterinfo.h"
#include "CNowexcise.h"
#include "CAeroparam.h"
#include "CCombstand.h"
#include "CSid.h"


class Class47 : public DBBaseClass, public CFlightenvelope, public CPortname, public CPort, public CTelegraph, public CDescspdrate, public CClimbspdrate, public CPlaneInfo, public CFlightInfo, public CExcisePlane, public CAirports, public CDeceacccurve, public CBelowacccurve, public CAboveacccurve, public CFlightEvent, public CLine, public CExcise, public CSpeedsimu, public CStandposition, public CAirways, public CDuration, public CEvent, public CGateposition, public CAltsimu, public CDvset, public CPath, public CGroundsimu, public CStar, public CTaxiway, public CPlaneType, public CAirportWorld, public CRecord, public CDuty, public CBuilding, public CZhuanpan, public CRunways, public CDegree, public CSpeed, public CFlightLevel, public CParkapron, public CPcsituation, public CCounterinfo, public CNowexcise, public CAeroparam, public CCombstand, public CSid
{
   BEGIN_ADO_BINDING(Class47)
   END_ADO_BINDING()

public:
   static _bstr_t _m_szString;
   int m_nFirst;  // 用于计算数据的开始位置
   int m_nLast;   // 用于计算数据的结束位置
   inline DBBaseClass * CopyData(DBBaseClass *pDest)
   {
      int datalen = m_nLast - m_nFirst;
      char *pd = (char*)dynamic_cast<Class47*>(pDest);
      char *ps = (char*)this;
      memcpy_s(ps+m_nFirst, datalen, pd+m_nFirst, datalen);
      return pDest;
   }
   inline Class47()
   {      
      m_nFirst = (int)(&((Class47*)0)->m_nFirst);
      m_nLast = (int)(&((Class47*)0)->m_nLast);
      int datalen = m_nLast - m_nFirst;
      char * ps = (char*)this;
      _m_szString = NULL;
      memset(ps+m_nFirst+sizeof(m_nFirst), 0, datalen-sizeof(m_nFirst));
   }
   inline _bstr_t GetTableName()
   {
 //     static _bstr_t tablename("Class_47");
//      static char tableanme[] = "Class_47";
      return _m_szString;
   }
   inline DBBaseClass **AllocInstances(int size)
   {
	   Class47* tmp = new Class47[size];
	   DBBaseClass ** result = new DBBaseClass *[size];
	   for (int iter = 0; iter < size ; iter++)
	   {
	 	   result[iter] = dynamic_cast<DBBaseClass*>(tmp+iter);
	   }
      return result;
   }
   inline void Release(DBBaseClass ** pdst)
   {
      delete[] *pdst;
      delete[] pdst;
   }
   inline DBBaseClass *GetDownEdge(DBBaseClass *cur)
   {
      char *pd = (char*)dynamic_cast<Class47*>(cur);
      return dynamic_cast<Class47*>((DBBaseClass *)(pd + sizeof(Class47)));
   }
   inline char* ToString(const char * tab)
   {
      int size = sizeof(Class47) * 2;
      static char buf[2*sizeof(Class47)];
      memset(buf, 0, size);
      int iter = 0;
      
      *(buf+iter-strlen(tab)) = 0;
      _m_szString = buf;
      return _m_szString;
   }
protected:

private:
};

_bstr_t Class47::_m_szString("Class_47");
DBCacheMgr<Class47> g_vClass47;

#endif