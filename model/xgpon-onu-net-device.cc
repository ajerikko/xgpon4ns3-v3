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

  //the below is an example how to use map the DSCP and TCONT type, so that parallel applications on multiple TCONT classess can be realised through XG-PON
  //By default, a Best Effort application is run across XG-PON.
  
	/* First, a user should set different DSCP values at the network layers for different application flows.
	 * In the example code below, the mapping assumes there are four DSCP values as in 0x10, 0x20, 0x40 and 0x60, set at the network layer. 
	 * Here, these four dscp values are assumed to be equivalent to the 1st, 2nd, 3rd and 4th TCONT types. 
	 *
	 *  uint8_t ipDscp = ipHeader.GetDscp(); 
	   //std::cout << "dscp: " << std::hex << (uint16_t)ipDscp << std::dec << std::endl; //undo the >> 4 to see the correct dscp value
	
	    if (ipDscp == 0x00)
		    m_tcontType = 1;
	    else if (ipDscp == 0x20)
		    m_tcontType = 2;
	    else if (ipDscp == 0x40)
		    m_tcontType = 3;
	    else if (ipDscp == 0x60)
		    m_tcontType = 4;
	    else
		    m_tcontType = 4; //Best Effort by default	
	 */
	 
  const Ptr<XgponConnectionSender>& conn=m_onuConnManager->FindUsConnByTcontType(m_tcontType); //changed from FindUsConnByAddress
  //std::cout << "nowat ONU Net Device, allocID: " << conn->GetAllocId() << std::endl;
  
  
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
