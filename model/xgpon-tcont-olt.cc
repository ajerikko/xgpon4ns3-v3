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
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/enum.h"

#include "xgpon-tcont-olt.h"
#include "xgpon-net-device.h"
#include "xgpon-qos-parameters.h" //jerome, C1


NS_LOG_COMPONENT_DEFINE ("XgponTcontOlt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponTcontOlt);

TypeId 
XgponTcontOlt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponTcontOlt")
    .SetParent<XgponTcont> ()
  ;
  return tid;
}
TypeId 
XgponTcontOlt::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}




XgponTcontOlt::XgponTcontOlt (): XgponTcont (),
  m_lastPollingTime(0),
  m_allocationWords(0),
  m_totalAllocatedRate(0),
  m_variable_word(0),
  m_connections(0),
  m_pkt4Reassemble(0)
{
}

XgponTcontOlt::~XgponTcontOlt ()
{
}

void
XgponTcontOlt::UpdatePIRtimer ()
{
  if (m_pirTimer > TIMER_EXPIRE_VALUE)
    m_pirTimer--;
}
void
XgponTcontOlt::UpdateGIRtimer ()
{
  if (m_girTimer > TIMER_EXPIRE_VALUE)
    m_girTimer--;
}
//sets SImax timer to SImax (fixed or assured) and SImin timer to SImin (non assured)
void
XgponTcontOlt::ResetPIRtimer ()
{
  m_pirTimer = m_pirSI; //Timer counts from SiMaxValue-1 to 0. 
}
void
XgponTcontOlt::ResetGIRtimer ()
{
  m_girTimer = m_pirSI/2; //GirTimer for T3 counts from SiMinValue/2-1 to 0. 
  //std::cout << "At reset, m_girTimer: " << m_girTimer << std::endl;
}
//only 4 tconts are supported in GIANT
void
XgponTcontOlt::CalculateTcontQosParameters(XgponQosParameters::XgponTcontType type)
{
  if (type == XgponQosParameters::XGPON_TCONT_TYPE_1)
  { // allocatedRate = PIR  
    m_allocatedRate = m_qosParameters->GetFixedBw();            
    m_pirTimer = m_pirSI = m_qosParameters->GetMaxInterval();
    //std::cout << "1, m_pirTimer: " << m_pirTimer << std::endl;
  }
  else if (type == XgponQosParameters::XGPON_TCONT_TYPE_2)
  { // allocatedRate = PIR 
    m_allocatedRate = m_qosParameters->GetAssuredBw();          
    m_pirTimer = m_pirSI = m_qosParameters->GetMaxInterval();
    //std::cout << "2, m_pirTimer: " << m_pirTimer << std::endl;
  }
  else if (type == XgponQosParameters::XGPON_TCONT_TYPE_3)
  { // allocatedRate = GIR
    m_allocatedRate = m_qosParameters->GetNonAssuredBw();       
    m_girTimer = m_qosParameters->GetMaxInterval();
    m_pirTimer = m_pirSI = m_qosParameters->GetMinInterval(); 
    //std::cout << "3, m_pirTimer: " << m_pirTimer << ", m_girTimer: " << m_girTimer << std::endl;
  }
  else
  {
    //only 4 tcont types are implemented at the moment. 
    NS_ASSERT_MSG((type==XgponQosParameters::XGPON_TCONT_TYPE_4), "Invalid TCONT Type when receiving data at OLT-side!!!");
    m_allocatedRate = m_qosParameters->GetBestEffortBw();
    m_girTimer = m_qosParameters->GetMaxInterval();
    m_pirTimer = m_pirSI = m_qosParameters->GetMinInterval();
    //std::cout << "4, m_pirTimer: " << m_pirTimer << ", m_girTimer: " << m_girTimer << std::endl;
  }

  m_totalAllocatedRate += m_allocatedRate; 
}

void 
XgponTcontOlt::ReceiveStatusReport (const Ptr<XgponXgtcDbru>& report, uint64_t time)
{
  NS_LOG_FUNCTION(this);

  report->SetReceiveTime(time);
  AddNewBufOccupancyReport (report);

  int64_t timeTh = time - XgponNetDevice::HISTORY_2_MAINTAIN;
  if(timeTh > 0) ClearOldBufOccupancyReports (time - XgponNetDevice::HISTORY_2_MAINTAIN);
}

void 
XgponTcontOlt::AddNewBwAllocation2ServiceHistory (const Ptr<XgponXgtcBwAllocation>& allocation, uint64_t time)
{
  NS_LOG_FUNCTION(this);

  allocation->SetCreateTime(time);
  AddNewBwAllocation (allocation);

  int64_t timeTh = time - XgponNetDevice::HISTORY_2_MAINTAIN;
  if(timeTh > 0) ClearOldBandwidthAllocations(timeTh);

  if(allocation->GetDbruFlag() != 0) { m_lastPollingTime = time; }
}


void 
XgponTcontOlt::AddOneConnection (const Ptr<XgponConnectionReceiver>& conn)
{
  NS_LOG_FUNCTION(this);

  m_connections.push_back(conn);
}



uint32_t 
XgponTcontOlt::CalculateRemainingDataToServe (uint64_t rtt, uint64_t slotSize)
{
  NS_LOG_FUNCTION(this);

  const Ptr<XgponXgtcDbru>& dbru = GetLatestBufOccupancyReport ();
  if(dbru==0)    return 0;

  uint32_t latestOccupancy = dbru->GetBufOcc ();
  uint64_t lastReportTime = dbru->GetReceiveTime ();

  if(m_bwAllocations.size()==0) return latestOccupancy;


  //since m_bwAllocations is maintained according to creation time (ascend order), we should start from the tail to consider the newer bwallocs.
  std::deque < Ptr<XgponXgtcBwAllocation> >::reverse_iterator rit, rend;
  rend = m_bwAllocations.rend();
  rit = m_bwAllocations.rbegin();
  uint32_t assignedSize = 0;
  while(rit!=rend)
  {
    //the status report in one burst includes the data transmitted in that burst. Thus, slotSize / 2 is added to include the corresponding bwalloc.
    uint64_t timeTh = (*rit)->GetCreateTime() + rtt + slotSize / 2;
    if(timeTh > lastReportTime)   
    {
      assignedSize += (*rit)->GetGrantSize();
      if((*rit)->GetDbruFlag()) assignedSize -= 1;   //ocuupancy report occupies one word;    
      
      rit++;  
    } else break;
  } 
  
  int remain = latestOccupancy - assignedSize;
  //std::cout << "Remaining: " << remain << "\t";
  if(remain < 0) remain = 0;

  return remain;
}



void 
XgponTcontOlt::ClearOldBufOccupancyReports (uint64_t time)
{
  NS_LOG_FUNCTION(this);

  while(m_bufOccupancyReports.size() > 0)
  {
    const Ptr<XgponXgtcDbru>& dbru = m_bufOccupancyReports.front();
    if(dbru->GetReceiveTime() < time) m_bufOccupancyReports.pop_front();
    else break;
  }

  return;
}


void 
XgponTcontOlt::ClearOldBandwidthAllocations (uint64_t time)
{
  NS_LOG_FUNCTION(this);

  while(m_bwAllocations.size() > 0)
  {
    const Ptr<XgponXgtcBwAllocation>& bwAlloc = m_bwAllocations.front();
    if(bwAlloc->GetCreateTime () < time) m_bwAllocations.pop_front();
    else break;
  }

  return;
}




}; // namespace ns3

