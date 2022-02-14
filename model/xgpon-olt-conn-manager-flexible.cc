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

#include "xgpon-olt-conn-manager-flexible.h"



NS_LOG_COMPONENT_DEFINE ("XgponOltConnManagerFlexible");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOltConnManagerFlexible);

TypeId 
XgponOltConnManagerFlexible::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltConnManagerFlexible")
    .SetParent<XgponOltConnManager> ()
    .AddConstructor<XgponOltConnManagerFlexible> ()
  ;
  return tid;
}
TypeId 
XgponOltConnManagerFlexible::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}




XgponOltConnManagerFlexible::XgponOltConnManagerFlexible ():XgponOltConnManager(), m_dsConnsAddressIndex()
{
}
XgponOltConnManagerFlexible::~XgponOltConnManagerFlexible ()
{
}




void 
XgponOltConnManagerFlexible::AddOneDsConn (const Ptr<XgponConnectionSender>& conn, bool isBroadcast, uint16_t onuId)
{
  NS_LOG_FUNCTION(this);
  NS_ASSERT_MSG((onuId<1023), "ONU-ID is too large (unlawful)!!!");

  if(isBroadcast)
  {
    AddOneBroadcastDsConnection(conn);
    m_dsConnsPortIndex[conn->GetXgemPort()] = conn;
    m_dsConnsAddressIndex.insert ( std::pair<Address, Ptr<XgponConnectionSender> >(conn->GetUpperLayerAddr(),conn) );
  }
  else
  {
    Ptr<XgponOltConnPerOnu> onu  = GetOneOnu4ConnsById (onuId);
    if(onu != 0) 
    { 
      onu->AddOneDsConn(conn); 
      m_dsConnsPortIndex[conn->GetXgemPort()] = conn;
      m_dsConnsAddressIndex.insert ( std::pair<Address, Ptr<XgponConnectionSender> >(conn->GetUpperLayerAddr(),conn) );
    }
  }
  
}




const Ptr<XgponConnectionSender>& 
XgponOltConnManagerFlexible::FindDsConnByAddress (const Address& addr)
{
  return m_dsConnsAddressIndex[addr];
}

const Ptr<XgponConnectionSender>& 
XgponOltConnManagerFlexible::FindBroadcastConnByAddress (const Address& addr)
{
  return m_dsConnsAddressIndex[addr];
}


}; // namespace ns3

