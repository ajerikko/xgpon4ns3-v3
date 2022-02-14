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

#include "ns3/xgpon-connection-sender.h"



NS_LOG_COMPONENT_DEFINE ("XgponConnectionSender");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponConnectionSender);

TypeId
XgponConnectionSender::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponConnectionSender")
    .SetParent<XgponConnection> ()
    .AddConstructor<XgponConnectionSender> ()
  ;
  return tid;
}
TypeId
XgponConnectionSender::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponConnectionSender::XgponConnectionSender ()  : XgponConnection(),
  m_txQueue (0), m_serviceRecords(0)
{
}
XgponConnectionSender::~XgponConnectionSender ()
{
}


}; // namespace ns3
