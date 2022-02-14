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
#include "ns3/simulator.h"

#include "xgpon-onu-dba-engine.h"
#include "xgpon-onu-net-device.h"



NS_LOG_COMPONENT_DEFINE ("XgponOnuDbaEngine");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOnuDbaEngine);

TypeId 
XgponOnuDbaEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuDbaEngine")
    .SetParent<XgponOnuEngine> ()
    .AddConstructor<XgponOnuDbaEngine> ()
  ;
  return tid;
}
TypeId
XgponOnuDbaEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponOnuDbaEngine::XgponOnuDbaEngine ()
{
}
XgponOnuDbaEngine::~XgponOnuDbaEngine ()
{
}







Ptr<XgponXgtcDbru> 
XgponOnuDbaEngine::GenerateStatusReport (uint16_t allocId)
{
  NS_LOG_FUNCTION(this);

  return ((m_device->GetConnManager())->GetTcontById(allocId))->PrepareBufOccupancyReport ( );
}






void 
XgponOnuDbaEngine::ProcessBwMap (const Ptr<XgponXgtcBwmap>& bwmap)
{
  NS_LOG_FUNCTION(this);
  uint64_t nowNano = Simulator::Now().GetNanoSeconds();

  const Ptr<XgponOnuConnManager>& connManager = m_device->GetConnManager();
  const Ptr<XgponPhy>& commonPhy = m_device->GetXgponPhy();
  const Ptr<XgponLinkInfo>& linkInfo = (m_device->GetPloamEngine())->GetLinkInfo ();
	 
  uint16_t bwMapSize=bwmap->GetNumberOfBwAllocation ( );
  for(int i=0; i<bwMapSize; i++)
  {
    const Ptr<XgponXgtcBwAllocation>& bwAlloc=bwmap->GetBwAllocationByIndex(i);
    uint32_t startTime = bwAlloc->GetStartTime();
    uint16_t allocId = bwAlloc->GetAllocId();

    const Ptr<XgponTcontOnu>& tcontOnu = connManager->GetTcontById(allocId);
    if(tcontOnu!=0)
    {
      tcontOnu->ReceiveBwAllocation (bwAlloc, nowNano);

      if(startTime!=0xFFFF) //one upstream burst should be scheduled for this ONU now.
      {
        NS_ASSERT_MSG((startTime<commonPhy->GetUsPhyFrameSizeInWord()), "StartTime is unreasonably large!!!");

        uint16_t burstIndex = bwAlloc->GetBurstProfileIndex ();
        const Ptr<XgponBurstProfile>& profile = linkInfo->GetProfileByIndex (burstIndex);
        NS_ASSERT_MSG((profile!=0), "the corresponding burst profile cannot be found!!!");

        //start_time doesn't consider preamble and delimiter, and its unit is word
        uint64_t tmpLen = profile->GetPreambleLen () + profile->GetDelimiterLen ();
        tmpLen = startTime * 4 - tmpLen; //starttime is the time of transmitting xgtcusheader

        uint64_t waitTime = 2*linkInfo->GetEqualizeDelay();  //different propagation delay.
        uint64_t txTime = waitTime + (tmpLen * 1000000000L) / commonPhy->GetUsLinkRate();
        NS_ASSERT_MSG((txTime<125000), "the scheduled txTime is unreasonably long!!!");

        Simulator::Schedule (NanoSeconds(txTime), &XgponOnuNetDevice::ProduceAndTransmitUsBurst, m_device, bwmap, i);
      }
    }
  }
}








}//namespace ns3
