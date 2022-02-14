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
#include <iostream>

#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"

#include "xgpon-olt-net-device.h"
#include "pon-channel.h"


//to support pool allocation
#include "xgpon-ds-frame.h"
#include "xgpon-olt-dba-per-burst-info.h"
#include "xgpon-service-record.h"
#include "xgpon-us-burst.h"
#include "xgpon-xgtc-bw-allocation.h"
#include "xgpon-xgtc-bwmap.h"
#include "xgpon-xgtc-dbru.h"
#include "xgpon-xgtc-ploam.h"
#include "xgpon-xgtc-us-allocation.h"
#include "xgpon-xgem-frame.h"



NS_LOG_COMPONENT_DEFINE ("XgponOltNetDevice");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOltNetDevice);

TypeId 
XgponOltNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltNetDevice")
    .SetParent<XgponNetDevice> ()
    .AddConstructor<XgponOltNetDevice> ()
    //ja:update:ns-3.35 modified AddTraceSource for the new ns-3 version
    .AddTraceSource ("PhyTxEnd",
        "Trace source indicating a packet has been completely transmitted by the device",
        MakeTraceSourceAccessor (&XgponOltNetDevice::m_phyTxEndTrace),
	"ns3::XgponOltNetDevice::TracedCallBack")
    .AddTraceSource ("PhyRxEnd",
        "Trace source indicating a packet has been completely received by the device",
        MakeTraceSourceAccessor (&XgponOltNetDevice::m_phyRxEndTrace),
	"ns3::XgponOltNetDevice::TracedCallBack")
  ;
  return tid;
}

TypeId 
XgponOltNetDevice::GetInstanceTypeId (void) const
{
  return GetTypeId();
}



XgponOltNetDevice::XgponOltNetDevice () : XgponNetDevice()
{
  //The engines should be configured with the helper.
}
XgponOltNetDevice::~XgponOltNetDevice ()
{
  //release the memory in the pools
  XgponDsFrame::DisablePoolAllocation();
  XgponUsBurst::DisablePoolAllocation();
  XgponXgtcUsAllocation::DisablePoolAllocation();
  XgponXgemFrame::DisablePoolAllocation();

  XgponOltDbaPerBurstInfo::DisablePoolAllocation();
  XgponXgtcBwmap::DisablePoolAllocation();
  XgponXgtcBwAllocation::DisablePoolAllocation();
  XgponXgtcDbru::DisablePoolAllocation();

  XgponXgtcPloam::DisablePoolAllocation();
  XgponServiceRecord::DisablePoolAllocation();
}







void 
XgponOltNetDevice::ReceivePonFrameFromChannel (const Ptr<PonFrame>& frame)
{
  NS_LOG_FUNCTION(this);

  Time now = Simulator::Now();

  const Ptr<XgponUsBurst>& usBurst = DynamicCast<XgponUsBurst, PonFrame>(frame);

  //get the burst profile used by this burst based on the bwmap history maintained by dba engine and the time that this burst is received
  const Ptr<XgponBurstProfile>& profile = m_oltDbaEngine->GetProfile4BurstFromChannel(now.GetNanoSeconds());

  //PHY+PHY-Adaptation sub-layer  
  m_oltPhyAdapter->ProcessXgponUsBurstFromChannel(usBurst, profile);


  //Framing sub-layer
  //Since the burst may contain multiple lists of packets for different T-CONTs, 
  //framing engine will call XGEM engine for Service-Adaptation sub-layer multiple times to process these payloads.
  //Thus, XgponOltNetDevice does not call XGEM engine here. 
  //If we let framing engine put all payloads into a list and return back, we will lose the boundary of T-CONTs.
  m_oltFramingEngine->ParseXgtcUpstreamBurst(usBurst->GetXgtcUsBurst());

  return;
}








bool 
XgponOltNetDevice::DoSend (const Ptr<Packet>& packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION(this);

  //NS_LOG_FUNCTION ("In Olt Netdevice Send: "<<packet << dest << protocolNumber);
  //NS_ASSERT_MSG(protocolNumber==2048,"Only supported protocol is ipv4");

  Ipv4Header ipHeader;
  Ipv4Address dstAddress;
  packet->PeekHeader(ipHeader);
  dstAddress=ipHeader.GetDestination();  

  const Ptr<XgponConnectionSender>& conn=m_oltConnManager->FindDsConnByAddress(dstAddress);	
  if(conn == 0) return false;
  else
  {
    return conn->ReceiveUpperLayerSdu(packet);
  }	
}

bool 
XgponOltNetDevice::DoSendFrom (const Ptr<Packet>& packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  bool sendRst;
  sendRst = Send (packet, dest, protocolNumber);
  return sendRst;
}









void
XgponOltNetDevice::DoInitialize (void)
{
  NS_LOG_FUNCTION(this);

  Simulator::ScheduleNow(&XgponOltNetDevice::SendDownstreamFrameToChannelPeriodically, this );
  return;
}


void 
XgponOltNetDevice::SendDownstreamFrameToChannelPeriodically ()
{
  NS_LOG_FUNCTION(this);

  Ptr<XgponDsFrame> dsFrame = Create<XgponDsFrame> ();  //create the downstream frame to be processed by various engines


  //Framing sub-layer
  //Note that framing engine will call XGEM engine (for Service-Adaptation sub-layer) directly to fill the payloads.
  //Although we can call XGEM engine and downstream scheduler here, it might be better to keep XgponOltNetDevice short.
  m_oltFramingEngine->ProduceXgtcDsFrame(dsFrame->GetXgtcDsFrame ());

  //phy and ohy-adaptation layer
  m_oltPhyAdapter->ProcessXgtcDsFrameFromUpperLayer(dsFrame);

  //send to the channel
  m_channel->SendDownstream (dsFrame);

  m_phyTxEndTrace(dsFrame, Simulator::Now());

  //schedule for the next downstream frame
  Simulator::Schedule (NanoSeconds(m_commonPhy->GetDsFrameSlotSize()), &XgponOltNetDevice::SendDownstreamFrameToChannelPeriodically, this); 


  //tracesource callback for network device statistics
  m_stat.m_currentTime = Simulator::Now().GetNanoSeconds();
  m_deviceStatisticsTrace (m_stat);
}







}// namespace ns3
