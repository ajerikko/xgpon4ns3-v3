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
#include "ns3/address.h"
#include "ns3/ipv4-header.h"
#include "ns3/traced-callback.h"

#include "xgpon-onu-net-device.h"
#include "pon-channel.h"

#include "xgpon-ds-frame.h"
#include "xgpon-xgtc-ds-frame.h"



NS_LOG_COMPONENT_DEFINE ("XgponOnuNetDevice");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOnuNetDevice);

TypeId 
XgponOnuNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuNetDevice")
    .SetParent<XgponNetDevice> ()
    .AddConstructor<XgponOnuNetDevice> ()
    //ja:update:ns-3.35 modified AddTraceSource for the new ns-3 version
    .AddTraceSource ("PhyTxEnd",
        "Trace source indicating a packet has been completely transmited by the device",
        MakeTraceSourceAccessor (&XgponOnuNetDevice::m_phyTxEndTrace),
	"ns3::XgponOnuNetDevice::TracedCallBack")
    .AddTraceSource ("PhyRxEnd",
        "Trace source indicating a packet has been completely received by the device",
        MakeTraceSourceAccessor (&XgponOnuNetDevice::m_phyRxEndTrace),
	"ns3::XgponOnuNetDevice::TracedCallBack")
  ;
  return tid;
}
TypeId
XgponOnuNetDevice::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}




XgponOnuNetDevice::XgponOnuNetDevice () : XgponNetDevice (),  m_tcontType (4)	//default Best Effort TCONT type
{	
}
XgponOnuNetDevice::~XgponOnuNetDevice ()
{
}





void 
XgponOnuNetDevice::ReceivePonFrameFromChannel (const Ptr<PonFrame>& frame)
{
  NS_LOG_FUNCTION (this);

  const Ptr<XgponDsFrame>& dsFrame = DynamicCast<XgponDsFrame, PonFrame>(frame);
  
  //PMD layer and PHY Adaptation sub-layer;
  m_onuPhyAdapter->ProcessXgponDsFrameFromChannel (dsFrame);


  //Framing sublayer; It will call XGEM engine to process the XGEM frames.
  m_onuFramingEngine->ParseXgtcDownstreamFrame(dsFrame->GetXgtcDsFrame ());


  //tracesource callback for network device statistics
  m_stat.m_currentTime = Simulator::Now().GetNanoSeconds();
  m_deviceStatisticsTrace (m_stat);


  return;
}







bool 
XgponOnuNetDevice::DoSend (const Ptr<Packet>& packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this<<packet << dest << protocolNumber);

  Ipv4Header ipHeader;
  packet->PeekHeader(ipHeader);
  
  m_tcontType = ipHeader.GetTos(); //tcont type is derived from the TOS field of the header; if a suitable value is set (0 to 4), the appropriate tcont type is used, ja:update:ns-3.35
  NS_ASSERT_MSG(((m_tcontType <= 4) & (m_tcontType >= 0)), "A suitable TOS value (0-4) is not set for the traffic end points"); //ja:update:ns-3.35

  
  const Ptr<XgponConnectionSender>& conn=m_onuConnManager->FindUsConnByTcontType(m_tcontType); //changed from FindUsConnByAddress
  //std::cout << "now at ONU Net Device, allocID: " << conn->GetAllocId() << std::endl;
  
  
  if(conn==0) return false;
  else 
  {
    return conn->ReceiveUpperLayerSdu(packet);
  }
}

bool 
XgponOnuNetDevice::DoSendFrom (const Ptr<Packet>& packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  return Send(packet, dest, protocolNumber);
}







void 
XgponOnuNetDevice::ProduceAndTransmitUsBurst (const Ptr<XgponXgtcBwmap>& map, uint16_t first)
{
  NS_LOG_FUNCTION (this);

  //create the upstream burst to be processed by various engines
  Ptr<XgponUsBurst> usBurst = Create<XgponUsBurst> ();  

  ////////////////////produce the upstream burst with various engines;
  /** Framing sub-layer
   *  Framing engine will call XGEM engine (for Service-Adaptation sub-layer) and DBA engine directly to fill the payloads.
   *  Although we can call Framing, XGEM engine, and upstream scheduler here, it might better to keep XgponOnuNetDevice short.
   */
  m_onuFramingEngine->ProduceXgtcUsBurst(usBurst->GetXgtcUsBurst(), map, first);

  //Get the burst profile that should be used by this burst
  const Ptr<XgponXgtcBwAllocation>& bwAlloc = map->GetBwAllocationByIndex (first);
  NS_ASSERT_MSG((bwAlloc!=0), "Cannot find the XgponXgtcBwAllocation used to produce this burst!!!");
  const Ptr<XgponBurstProfile>& profile = (m_onuPloamEngine->GetLinkInfo ())->GetProfileByIndex (bwAlloc->GetBurstProfileIndex());  

  //PHY and PHY_Adapdation sub-layer  
  m_onuPhyAdapter->ProcessXgtcBurstFromUpperLayer(usBurst, profile);


  //send the burst out
  m_channel->SendUpstream (usBurst, m_channelIndex);

  //m_phyTxEndTrace(usBurst, Simulator::Now());
}





}//namespace ns3
