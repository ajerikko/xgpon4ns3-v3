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

#include "xgpon-onu-framing-engine.h"
#include "xgpon-onu-net-device.h"




NS_LOG_COMPONENT_DEFINE ("XgponOnuFramingEngine");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOnuFramingEngine);

TypeId 
XgponOnuFramingEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuFramingEngine")
    .SetParent<XgponOnuEngine> ()
    .AddConstructor<XgponOnuFramingEngine> ()
  ;
  return tid;
}
TypeId
XgponOnuFramingEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponOnuFramingEngine::XgponOnuFramingEngine ()
{
}
XgponOnuFramingEngine::~XgponOnuFramingEngine ()
{
}




void
XgponOnuFramingEngine::ParseXgtcDownstreamFrame (XgponXgtcDsFrame& frame)
{
  NS_LOG_FUNCTION(this);

  XgponXgtcDsHeader& xgtcDsHeader = frame.GetHeader();

  //processing BWmap
  const Ptr<XgponOnuDbaEngine>& dbaEngine = m_device->GetDbaEngine ( ); 

  dbaEngine->ProcessBwMap(xgtcDsHeader.GetBwmap( ));

  //processing Ploam messages
  const Ptr<XgponOnuPloamEngine>& ploamEngine = m_device->GetPloamEngine ( );
  int num = xgtcDsHeader.GetPloamCount ();
  for(int i=0; i<num; i++) ploamEngine->ReceivePloamMessage(xgtcDsHeader.GetPloamByIndex(i));  

  //processing all XGEM frames
  const Ptr<XgponOnuXgemEngine>& xgemEngine = m_device->GetXgemEngine ( ); 


  //broadcast traffics
  if(frame.GetNBroadcastXgemFrames() > 0)
  {
    xgemEngine->ProcessXgemFramesFromLowerLayer(frame.GetBroadcastXgemFrames());
  }

  //unicast traffics
  std::vector<uint8_t>& bitmap = frame.GetBitmap ();
  if(bitmap[m_device->GetOnuId()] != 0)
  {
    xgemEngine->ProcessXgemFramesFromLowerLayer(frame.GetUnicastXgemFrames());
  }
}



void 
XgponOnuFramingEngine::ProduceXgtcUsBurst (XgponXgtcUsBurst& usBurst, const Ptr<XgponXgtcBwmap>& map, uint16_t first)
{
  NS_LOG_FUNCTION(this);

  const Ptr<XgponLinkInfo>& linkInfo = (m_device->GetPloamEngine())->GetLinkInfo ();
  Ptr<XgponXgtcBwAllocation> bwAlloc = map->GetBwAllocationByIndex(first);

  //fill XgponXgtcUsHeader
  XgponXgtcUsHeader& xgtcHeader = usBurst.GetHeader();
  xgtcHeader.SetOnuId(m_device->GetOnuId());
  xgtcHeader.SetDgFlag(linkInfo->GetDyingGasp());
  if(bwAlloc->GetPloamuFlag() && linkInfo->HasPloam2Transmit4ONU())
  {
    xgtcHeader.SetPloam(linkInfo->GetPloam2Transmit());
  }
  xgtcHeader.SetPloamQueueStatus(linkInfo->HasPloam2Transmit4ONU());
  xgtcHeader.CalculateHec ();


  //fill payloads
  const Ptr<XgponOnuDbaEngine>& dbaEngine = m_device->GetDbaEngine();
  const Ptr<XgponOnuXgemEngine>& xgemEngine = m_device->GetXgemEngine();


  int i = first;
  int num = map->GetNumberOfBwAllocation();
  do
  {
    int allocSize = bwAlloc->GetGrantSize ();
    uint16_t allocId = bwAlloc->GetAllocId ();
    if(allocSize > 0)
    {
      Ptr<XgponXgtcUsAllocation> allocation = Create<XgponXgtcUsAllocation>();
      usBurst.AddUsAllocation(allocation);
      if(bwAlloc->GetDbruFlag ())  //buffer occupancy report issue
      {
        Ptr<XgponXgtcDbru> dbr = dbaEngine->GenerateStatusReport (allocId);  
        allocation->SetDbru(dbr); 
        allocSize = allocSize - 1;
      }

      //Generate XGEM Frames to be transmitted and put them into UsAllocation
      xgemEngine->GenerateFramesToTransmit(allocation->GetXgemFrames(), allocSize*4, allocId);
    }

    i++;
    if(i<num) { bwAlloc = map->GetBwAllocationByIndex(i); }
  } while(bwAlloc->GetStartTime()==0xFFFF && i<num);



  //calculate and fill trailer;
  usBurst.CalculateTrailer ();
}


}//namespace ns3
