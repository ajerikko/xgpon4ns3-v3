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
#include "ns3/ipv4-address.h"

#include "xgpon-onu-conn-manager-flexible.h"



NS_LOG_COMPONENT_DEFINE ("XgponOnuConnManagerFlexible");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOnuConnManagerFlexible);


TypeId 
XgponOnuConnManagerFlexible::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuConnManagerFlexible")
    .SetParent<XgponOnuConnManager> ()
    .AddConstructor<XgponOnuConnManagerFlexible> ()
  ;
  return tid;
}
TypeId 
XgponOnuConnManagerFlexible::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


XgponOnuConnManagerFlexible::XgponOnuConnManagerFlexible () 
  :XgponOnuConnManager(),
  m_tconts(0), m_usConnsAddressIndex(),
  m_dsConns(0), m_broadcastConns(0)
{
}
XgponOnuConnManagerFlexible::~XgponOnuConnManagerFlexible ()
{
}







void 
XgponOnuConnManagerFlexible::AddOneUsTcont (const Ptr<XgponTcontOnu>& tcont)
{
  NS_LOG_FUNCTION(this);
  m_tconts.push_back(tcont);
}
const Ptr<XgponTcontOnu>& 
XgponOnuConnManagerFlexible::GetTcontById (uint16_t allocId)
{
  NS_LOG_FUNCTION(this);

  uint16_t num = m_tconts.size();
  for(int i=0; i<num; i++)
  {
    if(m_tconts[i]->GetAllocId() == allocId) return m_tconts[i];
  }
  return m_nullTcont; 
}



void 
XgponOnuConnManagerFlexible::AddOneUsConn (const Ptr<XgponConnectionSender>& conn, uint16_t allocId)
{
  NS_LOG_FUNCTION(this);

  const Ptr<XgponTcontOnu>& tcont = GetTcontById (allocId);
  if(tcont != 0)
  {
    tcont->AddOneConnection (conn);    
    m_usConnsAddressIndex.insert ( std::pair<Address, Ptr<XgponConnectionSender> >(conn->GetUpperLayerAddr(),conn) );
    if(conn->GetOnuId() == conn->GetXgemPort()) m_usOmciConn = conn;
    
  }
}
const Ptr<XgponConnectionSender>& 
XgponOnuConnManagerFlexible::FindUsConnByAddress (const Address& addr)
{
  NS_LOG_FUNCTION(this);
  return m_usConnsAddressIndex[addr]; 
}


//jerome, X2, dummy implementation
const Ptr<XgponConnectionSender>& 
XgponOnuConnManagerFlexible::FindUsConnByTcontType (const uint16_t& type)
{
  NS_LOG_FUNCTION(this);
  return m_nullConnSender;
}



void 
XgponOnuConnManagerFlexible::AddOneDsConn (const Ptr<XgponConnectionReceiver>& conn)
{
  NS_LOG_FUNCTION(this);
  
  if(conn->IsBroadcast()) m_broadcastConns.push_back(conn);
  else m_dsConns.push_back(conn);
}
const Ptr<XgponConnectionReceiver>& 
XgponOnuConnManagerFlexible::FindDsConnByXgemPort (uint16_t port)
{
  NS_LOG_FUNCTION(this);

  uint16_t num = m_dsConns.size();
  for(int i=0; i<num; i++)
  {
    if(m_dsConns[i]->GetXgemPort()==port) return m_dsConns[i];
  }

  num = m_broadcastConns.size();
  for(int i=0; i<num; i++)
  {
    if(m_broadcastConns[i]->GetXgemPort()==port) return m_broadcastConns[i];
  }
  return m_nullConnReceiver; 
}




}; // namespace ns3

