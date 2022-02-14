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

#include "xgpon-onu-engine.h"
#include "xgpon-onu-net-device.h"


NS_LOG_COMPONENT_DEFINE ("XgponOnuEngine");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOnuEngine);

TypeId 
XgponOnuEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuEngine")
    .SetParent<Object> ()
  ;
  return tid;
}
TypeId 
XgponOnuEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponOnuEngine::XgponOnuEngine ()
{
}
XgponOnuEngine::~XgponOnuEngine ()
{
}




void 
XgponOnuEngine::SetXgponOnuNetDevice (const Ptr<XgponOnuNetDevice>& device)
{
  m_device = device;
}
const Ptr<XgponOnuNetDevice>& 
XgponOnuEngine::GetXgponOnuNetDevice ( )
{
  NS_ASSERT_MSG((m_device!=0), "Network device has not been set yet.");
  return m_device;
}




}; // namespace ns3

