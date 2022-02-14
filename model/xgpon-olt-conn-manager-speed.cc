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
#include <algorithm> 

#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"

#include "xgpon-olt-conn-manager-speed.h"
#include "xgpon-olt-net-device.h"



NS_LOG_COMPONENT_DEFINE ("XgponOltConnManagerSpeed");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOltConnManagerSpeed);

TypeId 
XgponOltConnManagerSpeed::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltConnManagerSpeed")
    .SetParent<XgponOltConnManager> ()
    .AddConstructor<XgponOltConnManagerSpeed> ()
  ;
  return tid;
}
TypeId 
XgponOltConnManagerSpeed::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}




XgponOltConnManagerSpeed::XgponOltConnManagerSpeed ():XgponOltConnManager(),
  m_broadcastConnsAddressIndex()
{
}
XgponOltConnManagerSpeed::~XgponOltConnManagerSpeed ()
{
}







void 
XgponOltConnManagerSpeed::AddOneDsConn (const Ptr<XgponConnectionSender>& conn, bool isBroadcast, uint16_t onuId)
{
  NS_LOG_FUNCTION(this);
  NS_ASSERT_MSG((onuId<1023), "ONU-ID is too large (unlawful)!!!");

  if(isBroadcast)
  {
    AddOneBroadcastDsConnection(conn);
    m_dsConnsPortIndex[conn->GetXgemPort()] = conn;
    m_broadcastConnsAddressIndex.insert ( std::pair<Address, Ptr<XgponConnectionSender> >(conn->GetUpperLayerAddr(),conn) );
  }
  else
  {
    Ptr<XgponOltConnPerOnu> onu  = GetOneOnu4ConnsById (onuId);
    if(onu != 0) 
    { 
      onu->AddOneDsConn(conn); 
      m_dsConnsPortIndex[conn->GetXgemPort()] = conn;
    }
  }
  
}


//Further study may be necessary for CPU efficiency when a lot of broadcast traffics are simulated.
const Ptr<XgponConnectionSender>& 
XgponOltConnManagerSpeed::FindBroadcastConnByAddress (const Address& addr)
{
  return m_broadcastConnsAddressIndex[addr];
}




const Ptr<XgponConnectionSender>& 
XgponOltConnManagerSpeed::FindDsConnByAddress (const Address& addr)
{
  uint16_t xgemPort = CalculateXgemPortFromAddress (addr);
  return m_dsConnsPortIndex[xgemPort];
}


uint16_t 
XgponOltConnManagerSpeed::CalculateXgemPortFromAddress (const Address& addr)
{
  NS_LOG_FUNCTION(this);
  NS_ASSERT_MSG((Ipv4Address::IsMatchingType(addr)), "Address format other than IPv4 isn't supported by the current code!!!");

  //onu-id: 10bits; xgem-port: 16bits
  //Hence, for each ONU, the number of supported xgem-port must be less than 64 (might be enough for many scenarios).
  //TODO: byte-order needs to be investigated further

  Ipv4Address ipv4Addr = Ipv4Address::ConvertFrom (addr);
  uint32_t temp = ipv4Addr.Get();
  uint16_t onuId = (temp >> (32 - m_onuNetmaskLen)) % 1024;
  uint16_t reminder = temp & 0x0000003F;  

  uint16_t port = onuId + reminder * 1024;

  return port;
}

}; // namespace ns3

