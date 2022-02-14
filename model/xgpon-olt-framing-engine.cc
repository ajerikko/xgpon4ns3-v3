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

#include "xgpon-olt-framing-engine.h"
#include "xgpon-olt-net-device.h"

#include "xgpon-olt-conn-manager.h"
#include "xgpon-olt-ploam-engine.h"
#include "xgpon-olt-dba-engine.h"
#include "xgpon-olt-xgem-engine.h"



NS_LOG_COMPONENT_DEFINE ("XgponOltFramingEngine");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOltFramingEngine);

TypeId 
XgponOltFramingEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltFramingEngine")
    .SetParent<XgponOltEngine> ()
    .AddConstructor<XgponOltFramingEngine> ()
  ;
  return tid;
}
TypeId 
XgponOltFramingEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponOltFramingEngine::XgponOltFramingEngine ()
{
}
XgponOltFramingEngine::~XgponOltFramingEngine ()
{
}




void
XgponOltFramingEngine::ProduceXgtcDsFrame (XgponXgtcDsFrame& xgtcDsFrame)
{
  NS_LOG_FUNCTION(this);

  XgponXgtcDsHeader& header = xgtcDsFrame.GetHeader ();

  //PLOAM messages  
  std::list< Ptr<XgponXgtcPloam> >& ploams = (m_device->GetPloamEngine())->GetPloamsForTransmit( );
  std::list< Ptr<XgponXgtcPloam> >::iterator it; 
  it = ploams.begin();
  while(it!=ploams.end())
  {
    header.AddPloam (*it);
    it++;
  }

  //BWmap
  header.SetBwmap ((m_device->GetDbaEngine( ))->GenerateBwMap ());

  //produce a list of xgem frames
  uint32_t payloadLen = (m_device->GetXgponPhy())->GetXgtcDsFrameSize ( )  - header.GetSerializedSize();
  (m_device->GetXgemEngine( ))->GenerateFramesToTransmit(xgtcDsFrame.GetUnicastXgemFrames(), xgtcDsFrame.GetBroadcastXgemFrames(), xgtcDsFrame.GetBitmap(), payloadLen); 

  return;
}
  


void 
XgponOltFramingEngine::ParseXgtcUpstreamBurst (XgponXgtcUsBurst& burst)
{
  NS_LOG_FUNCTION(this);
  uint64_t nowNano = Simulator::Now().GetNanoSeconds();

  //get header information
  XgponXgtcUsHeader& header = burst.GetHeader();
  uint16_t onuId = header.GetOnuId();
  bool ploamAtOnu = header.GetPloamQueueStatus ();
  bool dgStatus = header.GetDgFlag ();

  //find the corresponding XgponLinkInfo of this ONU and store the new information.
  const Ptr<XgponOltPloamEngine>& ploamEngine = m_device->GetPloamEngine();
  const Ptr<XgponLinkInfo>& linkInfo = ploamEngine->GetLinkInfo (onuId);
  linkInfo->SetPloamExistAtOnu4OLT(ploamAtOnu);
  linkInfo->SetDyingGasp(dgStatus);


  //find the BWmap in which this burst was scheduled
  const Ptr<XgponOltDbaEngine>& dbaEngine = m_device->GetDbaEngine ( );
  Ptr<XgponXgtcBwmap> bwmap = dbaEngine->GetBwMap4CurrentBurst (nowNano);
  NS_ASSERT_MSG((bwmap!=0), "Cannot find the corresponding Bwmap of this upstream burst.");


  //find whether there is PLOAM in the header based on the BWmap stored at the OLT side and process if exists  
  uint32_t first = dbaEngine->GetIndexOfBurstFirstBwAllocation (bwmap, nowNano);
  NS_ASSERT_MSG((first<bwmap->GetNumberOfBwAllocation ( )), "The index is out of range!!!");

  const Ptr<XgponXgtcBwAllocation>& firstBwAlloc = bwmap->GetBwAllocationByIndex(first);
  if(firstBwAlloc->GetPloamuFlag())
  {
    const Ptr<XgponXgtcPloam>& ploam = header.GetPloam ();
    ploamEngine->ReceivePloamMessage (ploam, onuId);  
  }


  //process multiple upstream allocations (for multiple T-CONTs) in the burst.
  //call DBA engine to process queue status report and call XGEM engine to process the payloads.
  const Ptr<XgponOltConnManager>& connManager = m_device->GetConnManager ( );
  const Ptr<XgponOltXgemEngine>& xgemEngine = m_device->GetXgemEngine ( );

  int num = burst.GetUsAllocationCount ();
  for(int i=first, j=0; j<num; i++, j++)
  {
    const Ptr<XgponXgtcBwAllocation>& bwAlloc = bwmap->GetBwAllocationByIndex(i);
    const Ptr<XgponXgtcUsAllocation>& alloc = burst.GetUsAllocationByIndex(j);

    uint16_t allocId = bwAlloc->GetAllocId ( );
    const Ptr<XgponTcontOlt>& tcontOlt = connManager->GetTcontById(allocId);
    NS_ASSERT_MSG(((tcontOlt!=0) && (tcontOlt->GetOnuId()==onuId)), "Cannot find the corresponding Bwmap of this upstream burst.");

    //queue status report
    if(bwAlloc->GetDbruFlag ()) dbaEngine->ReceiveStatusReport(alloc->GetDbru(), onuId, allocId);

    //xgem frames
    xgemEngine->ProcessXgemFramesFromLowerLayer (alloc->GetXgemFrames ( ), onuId, allocId);
  }  

  return;
}


}//namespace ns3
