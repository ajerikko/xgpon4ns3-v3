/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 University College Cork (UCC), Ireland
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Xiuchao Wu <xw2@cs.ucc.ie>
 */

#ifndef XGPON_TCONT_H
#define XGPON_TCONT_H

#include <deque>

#include "ns3/object.h"

#include "xgpon-xgtc-dbru.h"
#include "xgpon-xgtc-bw-allocation.h"
#include "xgpon-qos-parameters.h" //jerome, C1

namespace ns3 {

/**
 * \ingroup xgpon
 * \brief This class is used to represent one T-CONT, i.e., transmission container (specified by one Alloc-ID). 
 *        It contains the ids & connections and maintains the history (status reports and bandwidth allocation, etc.)
 *
 */
class XgponTcont : public Object
{
public:

  /**
   * \brief Constructor
   */
  XgponTcont ();
  virtual ~XgponTcont ();

  /*buffer occupancy report related operations*/
  void AddNewBufOccupancyReport (const Ptr<XgponXgtcDbru>& report);
  const Ptr<XgponXgtcDbru>& GetOldestBufOccupancyReport () const;  
  const Ptr<XgponXgtcDbru>& GetLatestBufOccupancyReport () const;  
  const std::deque< Ptr<XgponXgtcDbru> >& GetAllBufOccupancyReports ();

  /* bandwidth allocation related operations */
  //this is the actual bandwidth allocation for this TCONT, when served by GIANT MAC
  void AddNewBwAllocation (const Ptr<XgponXgtcBwAllocation>& allocation);
  const Ptr<XgponXgtcBwAllocation>& GetOldestBwAllocation () const;  
  const Ptr<XgponXgtcBwAllocation>& GetLatestBwAllocation () const;  
  const std::deque < Ptr<XgponXgtcBwAllocation> >& GetAllBwAllocations ();

  ////////////////////////////////////////////////Member variable accessors
  void SetAllocId (uint16_t allocId);
  uint16_t GetAllocId ( ) const;

  void SetOnuId (uint16_t onuId);
  uint16_t GetOnuId ( ) const;

  void SetAllocatedRate (uint32_t bw);
  uint32_t GetAllocatedRate ( ) const;

  void SetServiceInterval (uint16_t sInterval);
  uint16_t GetServiceInterval ( ) const;

  void SetTcontType (XgponQosParameters::XgponTcontType tcontType);
  XgponQosParameters::XgponTcontType GetTcontType ( ) const;
  void SetQosParameters(Ptr<XgponQosParameters> qosParameters);
  Ptr<XgponQosParameters> GetQosParameters() const;
  
  /////////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

protected:  //to be accessible by sub-classes

  uint16_t  m_allocId;                                 //alloc-id;
  uint16_t  m_onuId;                                   //onu-id;
  Ptr<XgponQosParameters> m_qosParameters;  // Associated full QosParameters
  XgponQosParameters::XgponTcontType m_tcontType; //tcontType for this allocId/tcont

/* The idea here is to have a single bw value for each alloc ID based on its bandwidth type. 
 * How this bw value is assigned will be a call for the us scheduler mechanism to verify. 
 * However, the serviceInterval cannot be zero at the time of adding connection, 
 * since this will basically be used by the DBA in return, 
 * to calculate bw allocation in the US/DS depeding on which directions are supported by the dba.
 */
  uint32_t m_allocatedRate; //allocated bandwidth (bps) for this tcont
  uint16_t m_pirSI;     //the service interval between services. Unit: nanosecond

  /**
   * Buffer occupancy report from ONU to OLT. It is a list of report (to support multiple thread DBA in the future).
   * It should be maintained at both ONU and OLT side. They may deduce data arrival rate based on change of queue occupancy and adapt correspondingly.
   */  
  std::deque < Ptr<XgponXgtcDbru> >  m_bufOccupancyReports;


  /**
   * Sevice records of this Alloc-ID. It is a list of bandwidth allocation for this Alloc-ID (to support multiple thread DBA in the future).
   * It should be maintained at both ONU and OLT side.
   */
  std::deque < Ptr<XgponXgtcBwAllocation> > m_bwAllocations;

  ////////just used to return one empty dbru or bwalloc
  Ptr<XgponXgtcDbru> m_nullDbru;
  Ptr<XgponXgtcBwAllocation> m_nullBwAlloc;

private:  
  //////////////////////////////////////////////////////Clear history: operations are different at ONU and OLT.
  //remove based on create_time/receive_time
  virtual void ClearOldBufOccupancyReports (uint64_t time)=0;

  //remove based on create_time/receive_time
  virtual void ClearOldBandwidthAllocations (uint64_t time)=0;

  /* more variables may be needed */

};



///////////////////////////////////////////////////////////INLINE Functions
inline void 
XgponTcont::SetAllocId (uint16_t allocId)
{
  m_allocId = allocId;
}
inline uint16_t 
XgponTcont::GetAllocId ( ) const
{
  return m_allocId;
}
inline void 
XgponTcont::SetOnuId (uint16_t onuId)
{
  m_onuId = onuId;
}
inline uint16_t 
XgponTcont::GetOnuId ( ) const
{
  return m_onuId;
}
inline void
XgponTcont::SetAllocatedRate (uint32_t bw)
{
  m_allocatedRate = bw;
}
inline uint32_t
XgponTcont::GetAllocatedRate ( ) const
{
  return m_allocatedRate;
}

inline void 
XgponTcont::SetTcontType (XgponQosParameters::XgponTcontType tcontType)
{
  //NS_ASSERT_MSG( (((int)tcontType > 0) && ((int)tcontType < 6)), "TCONT type should be an enum value of [XGPON_TCONT_TYPE_1,XGPON_TCONT_TYPE_5] or [1,5]");
  m_tcontType = tcontType;
}
inline XgponQosParameters::XgponTcontType
XgponTcont::GetTcontType () const
{
  return m_tcontType;
}
inline void
XgponTcont::SetServiceInterval (uint16_t sInterval)
{
  m_pirSI = sInterval;
}
inline uint16_t
XgponTcont::GetServiceInterval ( ) const
{
  return m_pirSI;
}
inline  void
XgponTcont::SetQosParameters (Ptr<XgponQosParameters> qosParameters)
{
  m_qosParameters = qosParameters;
}
inline Ptr<XgponQosParameters>
XgponTcont::GetQosParameters () const
{
  return m_qosParameters;
}



inline void 
XgponTcont::AddNewBufOccupancyReport (const Ptr<XgponXgtcDbru>& report)
{
  m_bufOccupancyReports.push_back(report);
}

inline const Ptr<XgponXgtcDbru>& 
XgponTcont::GetOldestBufOccupancyReport () const
{
  if(m_bufOccupancyReports.size() > 0) return m_bufOccupancyReports.front();
  else return m_nullDbru;
}
inline const Ptr<XgponXgtcDbru>& 
XgponTcont::GetLatestBufOccupancyReport () const
{
  if(m_bufOccupancyReports.size() > 0) return m_bufOccupancyReports.back();
  else return m_nullDbru;
}

inline const std::deque< Ptr<XgponXgtcDbru> >& 
XgponTcont::GetAllBufOccupancyReports () 
{
  return m_bufOccupancyReports;
}






inline void 
XgponTcont::AddNewBwAllocation (const Ptr<XgponXgtcBwAllocation>& allocation)
{
  m_bwAllocations.push_back(allocation);
}

inline const Ptr<XgponXgtcBwAllocation>& 
XgponTcont::GetOldestBwAllocation () const
{
  if(m_bwAllocations.size() > 0) return m_bwAllocations.front();
  else return m_nullBwAlloc;
}
inline const Ptr<XgponXgtcBwAllocation>& 
XgponTcont::GetLatestBwAllocation () const
{
  if(m_bwAllocations.size() > 0) return m_bwAllocations.back();
  else return m_nullBwAlloc;
}

inline const std::deque < Ptr<XgponXgtcBwAllocation> >& 
XgponTcont::GetAllBwAllocations ()
{
  return m_bwAllocations;
}






}; // namespace ns3



#endif // XGPON_TCONT_H
