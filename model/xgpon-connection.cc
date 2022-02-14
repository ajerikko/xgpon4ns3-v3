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

#include "xgpon-connection.h"



NS_LOG_COMPONENT_DEFINE ("XgponConnection");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponConnection);

TypeId
XgponConnection::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponConnection")
    .SetParent<Object> ()
  ;
  return tid;
}
TypeId
XgponConnection::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}




XgponConnection::XgponConnection () 
{
}

XgponConnection::~XgponConnection ()
{
}




void 
XgponConnection::SetUpperLayerAddr (const Address& addr)
{
  m_upperLayerAddr = addr;
}
const Address& 
XgponConnection::GetUpperLayerAddr () const
{
  return m_upperLayerAddr;
}

}; // namespace ns3
