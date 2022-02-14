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

#include "ns3/simulator.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/enum.h"

#include "xgpon-net-device.h"

#include "xgpon-tcont-onu.h"
#include "xgpon-connection-sender.h"

#include "xgpon-onu-us-scheduler.h"

NS_LOG_COMPONENT_DEFINE ("XgponTcontOnu");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponTcontOnu);

TypeId 
XgponTcontOnu::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponTcontOnu")
    .SetParent<XgponTcont> ()
    .AddConstructor<XgponTcontOnu> ()
  ;
  return tid;
}

TypeId 
XgponTcontOnu::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


XgponTcontOnu::XgponTcontOnu (): XgponTcont (), m_connections(0), m_usScheduler(0)
{
}
XgponTcontOnu::~XgponTcontOnu ()
{
}





Ptr<XgponXgtcDbru> 
XgponTcontOnu::PrepareBufOccupancyReport ( )
{
  NS_LOG_FUNCTION(this);

  uint64_t nanoNow = Simulator::Now().GetNanoSeconds();
  uint32_t bufOccupancy = 0;

  int num = GetConnNumber();
  for(int i=0; i<num; i++)
  {
    Ptr<XgponConnectionSender> conn = GetConnByIndex(i);
    bufOccupancy = bufOccupancy + conn->GetBufOccupancy4Scheduling ();
  }

  Ptr<XgponXgtcDbru> dbr = Create<XgponXgtcDbru> ();
  dbr->SetBufOcc (bufOccupancy);
  dbr->CalculateCrc ();

  dbr->SetCreateTime(nanoNow);
  AddNewBufOccupancyReport(dbr);

  int64_t timeTh = nanoNow - XgponNetDevice::HISTORY_2_MAINTAIN;
  if(timeTh > 0) ClearOldBufOccupancyReports (timeTh);

  return dbr;
}



void 
XgponTcontOnu::ReceiveBwAllocation (const Ptr<XgponXgtcBwAllocation>& allocation, uint64_t time)
{
  NS_LOG_FUNCTION(this);

  allocation->SetReceiveTime(time);
  AddNewBwAllocation (allocation);

  int64_t timeTh = time - XgponNetDevice::HISTORY_2_MAINTAIN;
  if(timeTh > 0) ClearOldBandwidthAllocations(timeTh);

  return;
}




void 
XgponTcontOnu::AddOneConnection (const Ptr<XgponConnectionSender>& conn)
{
  m_connections.push_back(conn);
}











void 
XgponTcontOnu::ClearOldBufOccupancyReports (uint64_t time)
{
  NS_LOG_FUNCTION(this);

  while(m_bufOccupancyReports.size() > 0)
  {
    const Ptr<XgponXgtcDbru>& dbru = m_bufOccupancyReports.front();
    if(dbru->GetCreateTime() < time) m_bufOccupancyReports.pop_front();
    else break;
  }

  return;
}


void 
XgponTcontOnu::ClearOldBandwidthAllocations (uint64_t time)
{
  NS_LOG_FUNCTION(this);

  while(m_bwAllocations.size() > 0)
  {
    const Ptr<XgponXgtcBwAllocation>& bwAlloc = m_bwAllocations.front();
    if(bwAlloc->GetReceiveTime () < time) m_bwAllocations.pop_front();
    else break;
  }

  return;
}


const Ptr<XgponOnuUsScheduler>& 
XgponTcontOnu::GetOnuUsScheduler ( ) const
{
  NS_ASSERT_MSG((m_usScheduler!=0), "Upstream scheduler is not set for this ALLOC-ID!!!");
  return m_usScheduler;
}
void 
XgponTcontOnu::SetOnuUsScheduler (const Ptr<XgponOnuUsScheduler>& usScheduler)
{
  m_usScheduler = usScheduler;
}


}; // namespace ns3

