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
#include "ns3/log.h"
#include "ns3/uinteger.h"

#include "xgpon-onu-us-scheduler-round-robin.h"



NS_LOG_COMPONENT_DEFINE ("XgponOnuUsSchedulerRoundRobin");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOnuUsSchedulerRoundRobin);

TypeId 
XgponOnuUsSchedulerRoundRobin::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuUsSchedulerRoundRobin")
    .SetParent<XgponOnuUsScheduler> ()
    .AddConstructor<XgponOnuUsSchedulerRoundRobin> ()
    .AddAttribute ("MaxServiceSize", 
                   "The maximal number of bytes that could be allocated to one xgem-port in upstream burst (Unit: byte).",
                   UintegerValue (XGPON1_US_PER_SERVICE_MAX_SIZE),
                   MakeUintegerAccessor (&XgponOnuUsSchedulerRoundRobin::m_maxServiceSize),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}
TypeId 
XgponOnuUsSchedulerRoundRobin::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


XgponOnuUsSchedulerRoundRobin::XgponOnuUsSchedulerRoundRobin() : XgponOnuUsScheduler (),
  m_lastServedConnIndex(0)
{
}
XgponOnuUsSchedulerRoundRobin::~XgponOnuUsSchedulerRoundRobin ()
{
}




const Ptr<XgponConnectionSender>  
XgponOnuUsSchedulerRoundRobin::SelectConnToServe (uint32_t* amountToServe)
{
  NS_LOG_FUNCTION(this);

  NS_ASSERT_MSG((m_tcontOnu->GetConnNumber()>0), "There is no connection in this Alloc-ID!!!");
  NS_ASSERT_MSG(((m_lastServedConnIndex >= 0) && (m_lastServedConnIndex < m_tcontOnu->GetConnNumber())), "The index of the last served connection is strange!!!");


  const Ptr<XgponConnectionSender> lastConn = m_tcontOnu->GetConnByIndex(m_lastServedConnIndex);
  if(lastConn->IsSegmentationRunning()) 
  { //segmentation is running for the last connection served in the last burst
    *amountToServe = lastConn->GetFragBufOccupancy4Scheduling() * 4; 
    return lastConn;
  }
  else
  {//get one connection that has data to send
    uint32_t num = m_tcontOnu->GetConnNumber();

    m_lastServedConnIndex++;
    if(m_lastServedConnIndex >= num) m_lastServedConnIndex = 0;
    uint16_t orgServedIndex = m_lastServedConnIndex;

    do
    {
      const Ptr<XgponConnectionSender> conn = m_tcontOnu->GetConnByIndex(m_lastServedConnIndex);
      uint32_t dataInQueue = conn->GetBufOccupancy4Scheduling() * 4; 
      if(dataInQueue > 0)
      {
        if(dataInQueue < m_maxServiceSize)  *amountToServe = dataInQueue;
        else *amountToServe = m_maxServiceSize;
        return conn;
      }      
      m_lastServedConnIndex++;
      if(m_lastServedConnIndex >= num) m_lastServedConnIndex = 0;
    } while(m_lastServedConnIndex != orgServedIndex);
  }

  return m_nullConn;
}



}; // namespace ns3

