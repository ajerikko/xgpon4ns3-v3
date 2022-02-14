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

#include "xgpon-olt-ds-scheduler-round-robin.h"


NS_LOG_COMPONENT_DEFINE ("XgponOltDsSchedulerRoundRobin");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOltDsSchedulerRoundRobin);

TypeId 
XgponOltDsSchedulerRoundRobin::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltDsSchedulerRoundRobin")
    .SetParent<XgponOltDsScheduler> ()
    .AddConstructor<XgponOltDsSchedulerRoundRobin> ()
    .AddAttribute ("MaxServiceSize", 
                   "The maximal number of bytes that could be allocated to one xgem-port in a downstream frame (Unit: byte).",
                   UintegerValue (XGPON1_DS_PER_SERVICE_MAX_SIZE),
                   MakeUintegerAccessor (&XgponOltDsSchedulerRoundRobin::m_maxServiceSize),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}
TypeId
XgponOltDsSchedulerRoundRobin::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}





XgponOltDsSchedulerRoundRobin::XgponOltDsSchedulerRoundRobin ():XgponOltDsScheduler(),
  m_lastServedConnIndex(0)
{
  m_dsAllConns.clear();
}
XgponOltDsSchedulerRoundRobin::~XgponOltDsSchedulerRoundRobin ()
{
}






const Ptr<XgponConnectionSender>
XgponOltDsSchedulerRoundRobin::SelectConnToServe (uint32_t* amountToServe)
{
  NS_LOG_FUNCTION(this);
  
  NS_ASSERT_MSG((m_dsAllConns.size() > 0), "There is no downstream connections in the network!!!");

  if(m_startFrame)
  {
    m_startFrame = false;
    const Ptr<XgponConnectionSender> lastConn = GetTheLastServedConnection ();
    if(lastConn->IsSegmentationRunning ( ))  //the connection in segmentation has the highest priority.
    {
      *amountToServe = lastConn->GetFragBufOccupancy4Scheduling () * 4;
      return lastConn;
    }
  }

  //get one connection who has data to transmit
  uint16_t numEmpty = 0;
  Ptr<XgponConnectionSender> conn = GetNextConnection2Serve ();
  while(conn->GetQueueStatus()==0)
  {
    numEmpty++;
    if(numEmpty > m_dsAllConns.size()) //all connections are empty. There is no data in the OLT.
    { 
      *amountToServe = 0; 
      return m_nullConn; 
    } else { conn = GetNextConnection2Serve (); }
  }

  //calculate the amount of data to be served
  uint32_t dataInQueue = conn->GetBufOccupancy4Scheduling () * 4;
  if(dataInQueue < m_maxServiceSize)  *amountToServe = dataInQueue;
  else *amountToServe = m_maxServiceSize;

  return conn;
}






void
XgponOltDsSchedulerRoundRobin::AddConnToScheduler (const Ptr<XgponConnectionSender>& conn)
{
  m_dsAllConns.push_back(conn);

  return;
}






}//namespace ns3
