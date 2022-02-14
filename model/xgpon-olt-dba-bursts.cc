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

#include "xgpon-olt-dba-bursts.h"



NS_LOG_COMPONENT_DEFINE ("XgponOltDbaBursts");

namespace ns3{


XgponOltDbaBursts::XgponOltDbaBursts (): m_dbaPerBurstInfos(0), m_servedTconts(16384, false), m_nullPerBurstInfo(0)
{
}
XgponOltDbaBursts::~XgponOltDbaBursts ()
{
}



void 
XgponOltDbaBursts::ClearBurstInfoList( )
{
  NS_LOG_FUNCTION(this);
  m_dbaPerBurstInfos.clear();
  std::fill(m_servedTconts.begin(), m_servedTconts.end(), false);
}


bool
XgponOltDbaBursts::IsNewBurstNecessary(const Ptr<XgponTcontOlt>& tcont)
{
  NS_LOG_FUNCTION(this);

  uint16_t onuId = tcont->GetOnuId();

  std::list< Ptr<XgponOltDbaPerBurstInfo> >::iterator it, end;
  it = m_dbaPerBurstInfos.begin();
  end = m_dbaPerBurstInfos.end();
  while(it!=end)
  {
    Ptr<XgponOltDbaPerBurstInfo> perBurstInfo = *it;
    if(perBurstInfo->GetOnuId()==onuId)
    {
      if(perBurstInfo->GetBwAllocNumber() < XgponOltDbaPerBurstInfo::MAX_TCONT_PER_BURST)
      {
        return false;
      }
    }
    it++;
  }
  return true;
}





const Ptr<XgponOltDbaPerBurstInfo> 
XgponOltDbaBursts::GetBurstInfo4TcontOlt(const Ptr<XgponTcontOlt>& tcont)
{
  NS_LOG_FUNCTION(this);

  uint16_t onuId = tcont->GetOnuId();
  uint32_t num4Onu=0;

  std::list< Ptr<XgponOltDbaPerBurstInfo> >::iterator it, end;
  it = m_dbaPerBurstInfos.begin();
  end = m_dbaPerBurstInfos.end();
  while(it!=end)
  {
    Ptr<XgponOltDbaPerBurstInfo> perBurstInfo = *it;
    if(perBurstInfo->GetOnuId()==onuId)
    {
      if(perBurstInfo->GetBwAllocNumber() < XgponOltDbaPerBurstInfo::MAX_TCONT_PER_BURST)
      {
        m_dbaPerBurstInfos.erase(it);
        m_dbaPerBurstInfos.push_front(perBurstInfo);
        return perBurstInfo;
      }
      else num4Onu += perBurstInfo->GetBwAllocNumber();
    }
    it++;
  }

  if(num4Onu < MAX_TCONT_PER_ONU)
  {
    Ptr<XgponOltDbaPerBurstInfo> perBurstInfo = Create<XgponOltDbaPerBurstInfo> ();
    m_dbaPerBurstInfos.push_front(perBurstInfo);
    return perBurstInfo;
  } else return m_nullPerBurstInfo;  //no more bandwidth allocation for this ONU
}





const Ptr<XgponXgtcBwmap>
XgponOltDbaBursts::ProduceBwmapFromBursts(uint64_t now, uint16_t extraAllocationInLastBwmap, uint16_t usFrameSize)
{
  NS_LOG_FUNCTION(this);
  NS_ASSERT_MSG((extraAllocationInLastBwmap<(usFrameSize-10)), "the last BWMAP over-allocated too much!!!");

  Ptr<XgponXgtcBwmap> map = Create<XgponXgtcBwmap>();

  if(m_dbaPerBurstInfos.size()==0) return map;
  else if(m_dbaPerBurstInfos.size()==1)
  {
    uint16_t startTime = extraAllocationInLastBwmap;

    const Ptr<XgponOltDbaPerBurstInfo>& perBurstInfo = m_dbaPerBurstInfos.front();
    startTime += perBurstInfo->GetGapPhyOverhead( );
    if(startTime>=usFrameSize)
    {//The last bwmap has over-allocated too much. return one empty bwmap.
      return map;
    } 
    else
    {   
      perBurstInfo->PutAllBwAllocIntoBwmap(map, startTime, now);
      return map;
    }
  } else
  {
    uint16_t startTime = extraAllocationInLastBwmap;

    //When added bwalloc into the bursts, we maintain the last modified burst as the head of the list.
    //Thus, we should process the list from the tail to the head so that the last modified burst will be the last one in the BWMAP.
    //Otherwise, if the last modified one has over-allocated the upstream frame, 
    //some short bursts that are behind the last modified one in bwmap may be in trouble.
    //their starttime might be larger than usFrameSize and the corresponding bursts cannot be received correctly by the OLT.
    std::list< Ptr<XgponOltDbaPerBurstInfo> >::reverse_iterator rit, rend;
    rit = m_dbaPerBurstInfos.rbegin();
    rend = m_dbaPerBurstInfos.rend();
    while(rit!=rend)
    {
      startTime += (*rit)->GetGapPhyOverhead( );
      NS_ASSERT_MSG((startTime<usFrameSize), "StartTime in the BWMAP to be generated is large than the upstream frame size!!!");
    
      (*rit)->PutAllBwAllocIntoBwmap(map, startTime, now);
      startTime -= (*rit)->GetGapPhyOverhead( );
      startTime += (*rit)->GetFinalBurstSize( );

      rit++;
    }

    return map;
  }
}

bool
XgponOltDbaBursts::CheckServedTcont(uint64_t allocId)
{
  return m_servedTconts[allocId];
}

void
XgponOltDbaBursts::SetServedTcont(uint64_t allocId)
{
  m_servedTconts[allocId]=true;
}



}//namespace ns3
