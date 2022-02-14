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

#include "xgpon-onu-omci-engine.h"
#include "xgpon-onu-net-device.h"



NS_LOG_COMPONENT_DEFINE ("XgponOnuOmciEngine");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOnuOmciEngine);

TypeId 
XgponOnuOmciEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuOmciEngine")
    .SetParent<XgponOnuEngine> ()
    .AddConstructor<XgponOnuOmciEngine> ()
  ;
  return tid;
}
TypeId 
XgponOnuOmciEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponOnuOmciEngine::XgponOnuOmciEngine ()
{
}
XgponOnuOmciEngine::~XgponOnuOmciEngine ()
{
}




void 
XgponOnuOmciEngine::ReceiveOmciPacket (const Ptr<Packet>& omciPacket)
{
  NS_LOG_FUNCTION (this);
  return;
}


void
XgponOnuOmciEngine::GenerateOmciPacket4Transmit (uint16_t type, void * info)
{
  NS_LOG_FUNCTION (this);

  //below is just one example.
  Ptr<Packet> pkt = Create<Packet>();

  Ptr<XgponOnuNetDevice> onuDevice = DynamicCast<XgponOnuNetDevice, XgponNetDevice> (m_device);
  Ptr<XgponOnuConnManager> connManager = onuDevice->GetConnManager ( );

  Ptr<XgponConnectionSender> conn=connManager->FindUsOmciConn();
  if(conn!=0) conn->ReceiveUpperLayerSdu(pkt);
  return;
}




}; // namespace ns3

