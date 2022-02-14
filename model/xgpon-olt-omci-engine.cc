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

#include "xgpon-olt-omci-engine.h"
#include "xgpon-olt-net-device.h"

#include "xgpon-olt-conn-manager.h"
#include "xgpon-connection-sender.h"



NS_LOG_COMPONENT_DEFINE ("XgponOltOmciEngine");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOltOmciEngine);

TypeId 
XgponOltOmciEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltOmciEngine")
    .SetParent<XgponOltEngine> ()
    .AddConstructor<XgponOltOmciEngine> ()
  ;
  return tid;
}
TypeId 
XgponOltOmciEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}





XgponOltOmciEngine::XgponOltOmciEngine (): XgponOltEngine ()
{
}
XgponOltOmciEngine::~XgponOltOmciEngine ()
{
}




void 
XgponOltOmciEngine::ReceiveOmciPacket (const Ptr<Packet>& omciPacket)
{
  NS_LOG_FUNCTION(this);

  return;
}


void
XgponOltOmciEngine::GenerateOmciPacket4Transmit (uint16_t type, void * info)
{
  NS_LOG_FUNCTION(this);

  //below is just one example.
  Ptr<Packet> pkt = Create<Packet>();

  Ptr<XgponOltConnManager> connManager = m_device->GetConnManager ( );

  uint16_t onuId = 11;

  Ptr<XgponConnectionSender> conn=connManager->FindDsOmciConnByOnuId(onuId);
  if(conn!=0) conn->ReceiveUpperLayerSdu(pkt);
  return;
}





}; // namespace ns3

