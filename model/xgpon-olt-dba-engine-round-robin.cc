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
 * CoAuthor: Jerome A Arokkiam <jerom2005raj@gmail.com>
 */
#include "ns3/log.h"
#include "ns3/uinteger.h"

#include "xgpon-olt-dba-engine-round-robin.h"




NS_LOG_COMPONENT_DEFINE ("XgponOltDbaEngineRoundRobin");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOltDbaEngineRoundRobin);

TypeId 
XgponOltDbaEngineRoundRobin::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltDbaEngineRoundRobin")
    .SetParent<XgponOltDbaEngine> ()
    .AddConstructor<XgponOltDbaEngineRoundRobin> ()
    .AddAttribute ("MaxPollingInterval", 
                   "The maximal interval to poll one T-CONT (Unit: nano-second).",
                   UintegerValue (XgponOltDbaEngineRoundRobin::XGPON1_MAX_POLLING_INTERVAL),
                   MakeUintegerAccessor (&XgponOltDbaEngineRoundRobin::m_maxPollingInterval),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("MaxServiceSize", 
                   "The maximal number of words that could be allocated to one T-CONT in a bandwidth allocation (Unit: word).",
                   UintegerValue (XgponOltDbaEngineRoundRobin::XGPON1_TCONT_PER_SERVICE_MAX_SIZE),
                   MakeUintegerAccessor (&XgponOltDbaEngineRoundRobin::m_maxServiceSize),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}
TypeId
XgponOltDbaEngineRoundRobin::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}




XgponOltDbaEngineRoundRobin::XgponOltDbaEngineRoundRobin () : XgponOltDbaEngine(),
  m_lastScheduledTcontIndex(0),
  m_getNextTcontAtBeginning(false)
{
  m_usAllTconts.clear();
}
XgponOltDbaEngineRoundRobin::~XgponOltDbaEngineRoundRobin ()
{
}




void 
XgponOltDbaEngineRoundRobin::AddTcontToDbaEngine (Ptr<XgponTcontOlt>& tcont)
{
  NS_LOG_FUNCTION(this);

  m_usAllTconts.push_back(tcont);
  return;
} 



const Ptr<XgponTcontOlt>& 
XgponOltDbaEngineRoundRobin::GetNextTcontOlt ( )
{
  NS_LOG_FUNCTION(this);

  return m_usAllTconts[m_lastScheduledTcontIndex];
}

const Ptr<XgponTcontOlt>& 
XgponOltDbaEngineRoundRobin::GetCurrentTcontOlt ( ) 
{
  NS_LOG_FUNCTION(this);
    
  return m_usAllTconts[m_lastScheduledTcontIndex];
}

const Ptr<XgponTcontOlt>&
XgponOltDbaEngineRoundRobin::GetFirstTcontOlt ( )
{

  m_firstTcontOlt = m_usAllTconts[m_lastScheduledTcontIndex];
  NS_ASSERT_MSG((m_firstTcontOlt!=0), "There is no T-CONT in the network!!!");
  
  return m_firstTcontOlt;
}

bool
XgponOltDbaEngineRoundRobin::CheckAllTcontsServed ( )
{
  NS_LOG_FUNCTION(this);
  
  m_lastScheduledTcontIndex++;
  if(m_lastScheduledTcontIndex >= m_usAllTconts.size()) m_lastScheduledTcontIndex = 0;

  if(m_firstTcontOlt == m_usAllTconts[m_lastScheduledTcontIndex])
  {
    /*******************************************************************
     * In this case, all T-CONTs are processed in the same BWmap.
     * Although the cursor has been moved at the last step, but the tcont isn't served.
     * For starting from the same t-cont in the next round,
     * one flag should be used to notify OLT that GetNextTcontOlt should not
     * be called at the beginning of the next round.
     *******************************************************************/
    return true; //to break the DBA loop
  }
  else
    return false;
}


uint32_t 
XgponOltDbaEngineRoundRobin::CalculateAmountData2Upload (const Ptr<XgponTcontOlt>& tcontOlt,uint32_t allocatedSize, uint64_t nowNano)
{
  uint32_t size2Assign = tcontOlt->CalculateRemainingDataToServe(GetRtt(), GetFrameSlotSize()); 
  if(size2Assign > 0)
  {
    //always allow the t-cont to piggyback queue status report with its upstream data
    size2Assign += 1;      //one word for queue status report
    if(size2Assign > m_maxServiceSize) size2Assign = m_maxServiceSize;
    else if(size2Assign < 4) size2Assign  =4;    //smallest allocation for receiving data from ONU.
  }
  else
  {
    //whether to poll this t-cont for its queue status
    //uint64_t lastPollingTime = tcontOlt->GetLatestPollingTime();
    //if((nowNano - lastPollingTime) > m_maxPollingInterval) 
    //give a reporting opportunity to ONU, when it doesnt have data to upload. 
    //This will be useful to calculate the overhead introduced by DBA engine
    size2Assign = 1;
  }

  return size2Assign;
}


void 
XgponOltDbaEngineRoundRobin::Prepare2ProduceBwmap ( )
{
}

void
XgponOltDbaEngineRoundRobin::FinalizeBwmapProduction ()
{
}


}//namespace ns3
